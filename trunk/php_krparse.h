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
$Id: php_krparse.h,v 1.4 2002-08-14 10:28:28 oops Exp $
*/

#ifndef PHP_KRPARSE_H
#define PHP_KRPARSE_H

PHP_FUNCTION(ncrencode_lib);
PHP_FUNCTION(uniencode_lib);
PHP_FUNCTION(unidecode_lib);
PHP_FUNCTION(utf8encode_lib);
PHP_FUNCTION(utf8decode_lib);
PHP_FUNCTION(agentinfo_lib);
PHP_FUNCTION(autolink_lib);
PHP_FUNCTION(substr_lib);

unsigned char *krNcrConv (unsigned char *str_o, int type);
unsigned char *uniConv (unsigned char *str_o, int type, int subtype, unsigned char *start, unsigned char *end);
unsigned int getNcrIDX (unsigned char str1, unsigned char str2);
unsigned int getUniIDX (unsigned int key);
unsigned int hex2dec (unsigned char *str_o, unsigned int type);
int comp (const void *s1, const void *s2);
unsigned char *convUTF8 (unsigned char *str_o, int type);
unsigned int bin2dec (unsigned char *str_o);
unsigned char *autoLink (unsigned char *str_o);
unsigned char *get_useragent(void);
unsigned char *get_serverenv(unsigned char *para);

#define b_NAME br
#define b_OS os
#define b_VERSION vr
#define b_LANG ln

#endif /* PHP_KRPARSE_H */
