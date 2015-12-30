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
  | Author: JoungKyun Kim <http://oops.org>                              |
  +----------------------------------------------------------------------+

  $Id: php_kr.h 378 2015-12-30 02:17:19Z oops $
*/

#ifndef PHP_KR_H
#define PHP_KR_H

/* open_basedir and safe_mode checks */
#if PHP_MAJOR_VERSION < 5 || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4)
#define PHP_KR_CHECK_OPEN_BASEDIR(filename)                                             \
	if (!filename || php_check_open_basedir(filename TSRMLS_CC) ||                      \
		(PG(safe_mode) && !php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))   \
	) {                                                                                 \
		RETURN_FALSE;                                                                   \
	}
#else
#define PHP_KR_CHECK_OPEN_BASEDIR(filename)                                             \
	if (!filename || php_check_open_basedir(filename TSRMLS_CC))                        \
	{                                                                                   \
		RETURN_FALSE;                                                                   \
	}
#endif

#define kr_parameters(...) \
	zend_parse_parameters (ZEND_NUM_ARGS() TSRMLS_CC, __VA_ARGS__)

#ifndef UChar
#define UChar unsigned char
#endif

#endif	/* PHP_KR_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
