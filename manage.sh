#!/bin/bash

source /usr/share/annyung-release/functions.d/bash/functions

errmsg () {
	echo "$*" 1>&2
}

usage () {
	echo "Usage: $0 [clean|pack|test [php-version]]"
	exit 1
}

ctrlModule () {
	local mode=$1
	local ver=$2
	local m

	case "${mode}" in
		add)
			for m in ${NEED_MODULES}
			do
				if [[ -f /opt/php-qa/php${ver}/modules/${m}.so ]]; then
					if [[ ! -L ./modules/${m}.so ]]; then
						ln -sf /opt/php-qa/php${ver}/modules/${m}.so ./modules/${m}.so
					fi
				fi
			done
			;;
		*)
			for m in ${NEED_MODULES}
			do
				[[ -L ./modules/${m}.so ]] && rm -f ./modules/${m}.so
			done
	esac
}

opts=$(getopt -u -o h -l help -- "$@")
[ $? != 0 ] && usage

set -- ${opts}
for i
do
	case "$i" in
		-h|--help)
			usage
			shift
			;;
		--)
			shift
			break
			;;
	esac
done

mode="${1}"

case "${mode}" in
	clean)
		cat <<-EOL
			[ -f Makefile ] && make distclean
			rm -rf autom4te.cache build include modules
			rm -f .deps Makefile* ac*.m4 compile
			rm -f config.h* config.nice configure* config.sub config.guess
			rm -f install-sh ltmain.sh missing mkinstalldirs run-tests.php

			rm -f package.xml
			find ./tests ! -name '*.phpt' -a ! -name '*.txt' -a -type f
			----->
		EOL

		[ -f Makefile ] && make distclean
		rm -rf autom4te.cache build include modules
		rm -f .deps Makefile* ac*.m4 compile tags
		rm -f config.h* config.nice configure* config.sub config.guess
		rm -f install-sh ltmain.sh missing mkinstalldirs run-tests.php
		rm -f tests/*.{diff,exp,log,out,php,sh,mem}
		;;
	pack)
		rel="$( awk '/^#define BUILDVER / { print gensub(/"/, "", "g", $NF); }' php_korean.h )"

		[[ -d ../mod_korean-${rel} ]] && rm -rf ../mod_korean-${rel}
		mkdir -p ../mod_korean-${rel}
		cp -af charset/ libgd/ tests/ ../mod_korean-${rel}
		cp -af CREDITS Changelog README.md ../mod_korean-${rel}
		cp -af *.m4 *.dsp *.c *.h ../mod_korean-${rel}
		cd ..
		tar cvfpJ mod_korean-${rel}.tar.xz mod_korean-${rel}
		cd -
		mv ../mod_korean-${rel}.tar.xz ./
		[[ -d ../mod_korean-${rel} ]] && rm -rf ../mod_korean-${rel}

		;;
	test)
		PHPBIN=/opt/php-qa/php${2}/bin/php
		PHPIZE=/opt/php-qa/php${2}/bin/phpize
		PHPCONFIG=/opt/php-qa/php${2}/bin/php-config
		PHP_OPT="-n"
		NEED_MODULES="iconv openssl"

		if [[ $# == 2 ]]; then
			./manage.sh clean
			echo "${PHPIZE} && ./configure"
			${PHPIZE} && ./configure && make -j8 || exit 0
		fi

		ctrlModule add ${2}

		if (( $2 > 71 )); then
			PHP_OPT+=" -d 'extension_dir=./modules/' -d 'extension=korean.so'"
		else
			PHP_OPT+=" -d 'track_errors=1' -d 'extension_dir=./modules/' -d 'extension=korean.so'"
		fi

		if [[ -f /opt/php-qa/php${2}/modules/iconv.so ]];then
			PHP_OPT+=" -d 'extension=/opt/php-qa/php${2}/modules/iconv.so'"
		fi

		if [[ -f tests/${3}.php ]]; then
			cat <<-EOL
				${bgreen}------------------------------------------------------------------------
				Sample code execution:

				${bcyan}${PHPBIN} ${PHP_OPT} test/${3}.php
				${bgreen}------------------------------------------------------------------------${normal}

			EOL
			${PHPBIN} ${PHP_OPT} test/${3}.php
			#ctrlModule remove ${2}
			exit $?
		elif [[ -f ${3} ]]; then
			cat <<-EOL
				${bgreen}------------------------------------------------------------------------
				Sample code execution:

				${bcyan}${PHPBIN} ${PHP_OPT} ${3}
				${bgreen}------------------------------------------------------------------------${normal}

			EOL
			eval "${PHPBIN} ${PHP_OPT} ${3}"
			#ctrlModule remove ${2}
			exit $?
		fi

        cat <<-EOL
			${bwhite}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
			${tcolor}** MAKE test::                                                       ${normal}
			${bwhite}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

			${bcyan}make test PHP_EXECUTABLE=${PHPBIN}
			${bwhite}~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~${normal}

		EOL
		make test PHP_EXECUTABLE=${PHPBIN} <<< n
		#ctrlModule remove ${2}
		;;
	stub)
		# stub tagging
		# /** @generate-function-entries **/ build with function entryies
		# /** @generate-legacy-arginfo **/   build with legacy style
		if [[ ! -f build/gen_stub.php ]]; then
			cat <<-EOL
				ERROR: execute before PHP 8 build environment or before execute phpize
			EOL
			exit 1
		fi
		phpcmd="/usr/bin/php80"
		perl -pi -e 's/ext_functions/korean_functions/g' build/gen_stub.php
		${phpcmd} build/gen_stub.php -f *.stub.php
		;;
	*)
		errmsg "Unsupport mode '${1}'"
		exit 1
esac

exit 0
