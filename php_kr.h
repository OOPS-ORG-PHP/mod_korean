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

  $Id$
*/

#ifndef PHP_KR_H
#define PHP_KR_H

/* open_basedir and safe_mode checks */
#if PHP_MAJOR_VERSION < 5 || (PHP_MAJOR_VERSION == 5 && PHP_MINOR_VERSION < 4)
#define PHP_KR_CHECK_OPEN_BASEDIR(x)                                               \
	do {                                                                           \
		char *p = NULL;                                                            \
		if ( strlen((char *)x) > 0 ) { p = estrdup(x); }                           \
		if (!p || php_check_open_basedir(p TSRMLS_CC) ||                           \
			(PG(safe_mode) && !php_checkuid(p, NULL, CHECKUID_CHECK_FILE_AND_DIR)) \
		) {                                                                        \
			php_error (E_ERROR, "restriction OPEN_BASE_DIR(%s)", p);               \
			safe_efree(p); RETURN_FALSE;                                           \
		}                                                                          \
		safe_efree(p);                                                             \
	} while (0)
#else
#define PHP_KR_CHECK_OPEN_BASEDIR(x)                                               \
	do {                                                                           \
		char *p = NULL;                                                            \
		if ( strlen((char *)x) > 0 ) { p = estrdup(x); }                           \
		if (!p || php_check_open_basedir(p TSRMLS_CC))                             \
		{                                                                          \
			php_error (E_ERROR, "restriction OPEN_BASE_DIR(%s)", p);               \
			safe_efree(p); RETURN_FALSE;                                           \
		}                                                                          \
		safe_efree(p);                                                             \
	} while (0)
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
