/*
+----------------------------------------------------------------------+
| PHP Version 4                                                        |
+----------------------------------------------------------------------+
| Copyright (c) 1997-2002 The PHP Group                                |
+----------------------------------------------------------------------+
| This source file is subject to version 2.02 of the PHP license,      |
| that is bundled with this package in the file LICENSE, and is        |
| available at through the world-wide-web at                           |
| http://www.php.net/license/2_02.txt.                                 |
| If you did not receive a copy of the PHP license and are unable to   |
| obtain it through the world-wide-web, please send a note to          |
| license@php.net so we can mail you a copy immediately.               |
+----------------------------------------------------------------------+
| Author: JoungKyun Kim <http://www.oops.org>                          |
+----------------------------------------------------------------------+
$Id: php_krcheck.h,v 1.3 2002-08-21 16:15:27 oops Exp $
*/

#ifndef PHP_KRCHECK_H
#define PHP_KRCHECK_H

PHP_FUNCTION(check_uristr_lib);
PHP_FUNCTION(check_filename_lib);
PHP_FUNCTION(check_htmltable_lib);
PHP_FUNCTION(is_iis_lib);
PHP_FUNCTION(is_windows_lib);
PHP_FUNCTION(is_email_lib);
PHP_FUNCTION(is_url_lib);
PHP_FUNCTION(is_hangul_lib);

unsigned int checkAddr(unsigned char *addr, int type);
unsigned int chkMetaChar (unsigned char *str, int type);
unsigned int check_table (unsigned char *str);
unsigned int multibyte_check(unsigned char *str_o, unsigned int point);
unsigned int check_windows(unsigned int type);

#endif  /* end PHP_KRCHECK_H */