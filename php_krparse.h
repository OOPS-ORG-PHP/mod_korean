/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2019 The PHP Group                                |
  +----------------------------------------------------------------------+
  | This source file is subject to version 3.01 of the PHP license,      |
  | that is bundled with this package in the file LICENSE, and is        |
  | available through the world-wide-web at the following url:           |
  | http://www.php.net/license/3_01.txt                                  |
  | If you did not receive a copy of the PHP license and are unable to   |
  | obtain it through the world-wide-web, please send a note to          |
  | license@php.net so we can mail you a copy immediately.               |
  +----------------------------------------------------------------------+
  | Author: JoungKyun Kim <hostmaster@oops.org>                          |
  +----------------------------------------------------------------------+
*/

#ifndef PHP_KRPARSE_H
#define PHP_KRPARSE_H

PHP_FUNCTION(agentinfo_lib);
PHP_FUNCTION(autolink_lib);
PHP_FUNCTION(substr_lib);
PHP_FUNCTION(postposition_lib);

unsigned char *autoLink (unsigned char *str_o);
unsigned char *get_useragent (void);
unsigned char *get_serverenv (unsigned char *para);
int get_postposition (unsigned char *str);
unsigned char * strtrim (unsigned char *str);
void safe_efree (void * str);

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
