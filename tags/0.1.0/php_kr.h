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
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id: php_kr.h,v 1.1 2006-02-12 06:43:14 oops Exp $
*/

#ifndef PHP_KR_H
#define PHP_KR_H

/* open_basedir and safe_mode checks */
#define PHP_KR_CHECK_OPEN_BASEDIR(filename)                                             \
	if (!filename || php_check_open_basedir(filename TSRMLS_CC) ||                      \
		(PG(safe_mode) && !php_checkuid(filename, NULL, CHECKUID_CHECK_FILE_AND_DIR))   \
	) {                                                                                 \
		RETURN_FALSE;                                                                   \
	}

#endif	/* PHP_KR_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
