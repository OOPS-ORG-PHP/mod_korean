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
$Id: php_krparse.h,v 1.12 2004-09-14 06:52:22 oops Exp $
*/

#ifndef PHP_KRPARSE_H
#define PHP_KRPARSE_H

PHP_FUNCTION(agentinfo_lib);
PHP_FUNCTION(autolink_lib);
PHP_FUNCTION(substr_lib);
PHP_FUNCTION(postposition_lib);

unsigned char *autoLink (unsigned char *str_o);
unsigned char *get_useragent(void);
unsigned char *get_serverenv(unsigned char *para);
int get_postposition (unsigned char *str);
unsigned char * strtrim(unsigned char *str);

#define b_NAME br
#define b_OS os
#define b_VERSION vr
#define b_LANG ln

#endif /* PHP_KRPARSE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
