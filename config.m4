dnl $Id: config.m4,v 1.10 2002-12-28 14:53:15 oops Exp $
dnl config.m4 for extension korean

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_ENABLE(korean, whether to enable korean support,
[  --enable-korean           Enable korean support])

if test "$PHP_KOREAN" != "no"; then
        AC_DEFINE(HAVE_KOREAN,1,[ ])
  dnl if php version is under 4.2.x, use PHP_EXTENSION
  dnl bug over php 4.2.x, use PHP_NEW_EXTENSION
  PHP_EXTENSION(korean, $ext_shared)
  dnl PHP_NEW_EXTENSION(korean, conftest.c krcharset.c krerror.c krimage.c krmath.c krparse.c korean.c krcheck.c krfile.c krmail.c krnetwork.c krregex.c, $ext_shared)
  PHP_SUBST(KOREAN_SHARED_LIBADD)

  CPPFLAGS=$CFLAGS
  PHP_SUBST(CPPFLAGS)
fi

AC_MSG_CHECKING(whether to enable gd functoin)
AC_ARG_ENABLE(korean-gd, [ --enable-korean-gd         Enable gd functoin ],[
  if test "$enable_korean_gd" = "yes" ; then
    AC_DEFINE(KOEAN_GD, 1, [ ])
    AC_MSG_RESULT(yes)

    if test "$enable_korean_gd" = "yes"; then
      GD_SEARCH_PATHS="/usr/local /usr"
    else
      GD_SEARCH_PATHS=$enable_korean_gd
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
      PHP_ADD_LIBRARY_WITH_PATH(gd, $KOREAN_LIB, KOREAN_SHARED_LIBADD)
      AC_DEFINE(HAVE_KRLIBGD,1,[ ])
    else
      AC_MSG_ERROR([Unable to find libgd.(a|so) anywhere under $GD_SEARCH_PATHS])
    fi 

    PHP_EXPAND_PATH($KOREAN_INCLUDE, KOREAN_INCLUDE)
    PHP_ADD_INCLUDE($KOREAN_INCLUDE)
  else
    AC_MSG_RESULT(no)
  fi
],[
  AC_MSG_RESULT(no)
])
