dnl $Id: config.m4,v 1.1.1.1 2002-05-14 09:50:50 oops Exp $
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
fi
