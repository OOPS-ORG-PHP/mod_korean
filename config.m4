dnl $Id: config.m4,v 1.5 2002-08-16 01:03:54 oops Exp $
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
  PHP_SUBST(KOREAN_SHARED_LIBADD)

fi
