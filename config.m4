dnl $Id: config.m4,v 1.2 2002-06-12 15:57:14 oops Exp $
dnl config.m4 for extension korean

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_ENABLE(korean, whether to enable korean support,
[  --enable-korean           Enable korean support])

if test "$PHP_KOREAN" != "no"; then
	AC_DEFINE(HAVE_KOREAN,1,[ ])
  PHP_EXTENSION(korean, $ext_shared)

  if test "$PHP_GD" != "no"; then

    if test "$PHP_GD" = "yes"; then
      GD_SEARCH_PATHS="/usr/local /usr"
    else
      GD_SEARCH_PATHS=$PHP_GD
    fi

    for j in $GD_SEARCH_PATHS; do
      for i in include/gd1.3 include/gd include gd1.3 gd ""; do
        test -f $j/$i/gd.h && KOREAN_INCLUDE=$j/$i
      done

      for i in lib/gd1.3 lib/gd lib gd1.3 gd ""; do
        test -f $j/$i/libgd.$SHLIB_SUFFIX_NAME -o -f $j/$i/libgd.a && KOREAN_LIB=$j/$i
      done
    done

    if test -n "$KOREAN_INCLUDE" -a -n "$KOREAN_LIB" ; then
      PHP_ADD_LIBRARY_WITH_PATH(korean, $KOREAN_LIB, KOREAN_SHARED_LIBADD)
    else
      AC_MSG_ERROR([Unable to find libgd.(a|so) anywhere under $GD_SEARCH_PATHS])
    fi 

    if [ "${KOREAN_INCLUDE}" != "" ]; then
      PHP_EXPAND_PATH($KOREAN_INCLUDE, KOREAN_INCLUDE)
      PHP_ADD_INCLUDE($KOREAN_INCLUDE)
    fi

  fi
fi
