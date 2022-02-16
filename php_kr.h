/*
  +----------------------------------------------------------------------+
  | Copyright 2022. JoungKyun.Kim All rights reserved.                   |
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

#ifndef PHP_KR_H
#define PHP_KR_H

#if PHP_VERSION_ID < 70000
#error "************ PHP version dependency problems *******************"
#error "This package requires over php 7.0.0 !!"
#error "If you build with php under 7.0.0, use mod_korean 0.1.x version"
#error "You can download mod_korean 0.1.x at https://github.com/OOPS-ORG-PHP/mod_korean/releases"
#endif

#if PHP_VERSION_ID >= 80000
#define TSRMLS_CC
#define TSRMLS_C
#define TSRMLS_FETCH()
#endif

#if PHP_VERSION_ID < 80000
#if PHP_VERSION_ID < 70200
#undef ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX
#define ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(name, return_reference, required_num_args, type, allow_null) \
	static const zend_internal_arg_info name[] = { \
		{ (const char*)(zend_uintptr_t)(required_num_args), NULL, type, return_reference, allow_null, 0 },
#endif
#define ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(pass_by_ref, name, type_hint, allow_null, default_value) \
		ZEND_ARG_TYPE_INFO(pass_by_ref, name, type_hint, allow_null)
#endif

/* open_basedir and safe_mode checks */
#define PHP_KR_CHECK_OPEN_BASEDIR(x)                                               \
	do {                                                                           \
		char *p = NULL;                                                            \
		if ( strlen((char *)x) > 0 ) { p = estrdup((char *) x); }                  \
		if (!p || php_check_open_basedir(p TSRMLS_CC))                             \
		{                                                                          \
			php_error (E_ERROR, "restriction OPEN_BASE_DIR(%s)", p);               \
			kr_safe_efree(p); RETURN_FALSE;                                        \
		}                                                                          \
		kr_safe_efree(p);                                                          \
	} while (0)

#define kr_parameters(...) \
	zend_parse_parameters (ZEND_NUM_ARGS() TSRMLS_CC, __VA_ARGS__)

#ifndef UChar
#define UChar unsigned char
#endif

#define STRLEN(x) strlen((char *) x)
#define ze_string_init(x,z) zend_string_init((char *)x, STRLEN(x), z);

#endif	/* PHP_KR_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
