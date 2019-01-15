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

int checkAddr (char * addr, int type);
int chkMetaChar (char * str, int type);
int check_table (char * str);
int multibyte_check (char * str_o, int point);
int check_windows (int type);

#endif  /* end PHP_KRCHECK_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
