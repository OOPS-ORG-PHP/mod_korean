/**
 * Project: mod_korean::
 * File:    korean.c
 *
 * Copyright 2022. JoungKyun.Kim All rights reserved.
 *
 * LICENSE: GPL
 *
 * @category    Text
 * @package     mod_korean
 * @author      JoungKyun.Kim <http://oops.org>
 * @copyright   2013 OOPS.org
 * @license     GPL
 * @since       File available since release 0.0.1
 */

/*
 * PHP5 Korean String modue "korean" - only korean
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "ext/standard/info.h"

#include "php_kr.h"
#include "krregex.h"
#include "php_korean.h"
#include "php_krparse.h"
#include "php_krcheck.h"
#include "php_krerror.h"
#include "php_krfile.h"
#include "php_krnetwork.h"
#include "php_krimage.h"
#include "php_krmail.h"
#include "php_krcharset.h"

/* If you declare any globals in php_korean.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(korean)
*/

/* True global resources - no need for thread safety here */
// don't use
//static int le_korean;

/* {{{ korean_functions[]
 *
 * Every user visible function must have an entry in korean_functions[].
 */
#include "korean_arginfo.h"
/* }}} */

/* {{{ korean_module_entry
 */
zend_module_entry korean_module_entry = {
#if ZEND_MODULE_API_NO >= 20010901
	STANDARD_MODULE_HEADER,
#endif
	"korean",
	korean_functions,
	NULL,
	NULL,
	NULL,
	NULL,
	PHP_MINFO(korean),
#if ZEND_MODULE_API_NO >= 20010901
	BUILDVER, /* Replace with version number for your extension */
#endif
	STANDARD_MODULE_PROPERTIES
};
/* }}} */

#ifdef COMPILE_DL_KOREAN
ZEND_GET_MODULE(korean)
#endif

/* {{{ PHP_MINFO_FUNCTION
 */
PHP_MINFO_FUNCTION(korean)
{
	php_info_print_table_start();
	php_info_print_table_colspan_header(2, "Korean extension support");
	php_info_print_table_row(2, "Build No", BUILDNO);
	php_info_print_table_row(2, "Build version", BUILDVER);
	php_info_print_table_row(2, "URL", "http://oops.org/");
	php_info_print_table_header(2, "Function", "Support");
	php_info_print_table_row(2, "Check function", "enabled");
	php_info_print_table_row(2, "Charset function", "NCR code, Native Unicode 2.0, UTF-8, EUC-KR, CP949");
	php_info_print_table_row(2, "Filesystem function", "enabled");
	php_info_print_table_row(2, "HTML function", "enabled");
#if KRGD_BUILTIN
	php_info_print_table_row(2, "Image function", "enabled, builtin gd library 2.0 compatible");
#elif HAVE_GD2
	php_info_print_table_row(2, "Image function", "enabled, gd library 2.0 or over");
#else
	php_info_print_table_row(2, "Image function", "disabled, gd library between 1.2 or higher");
#endif
	php_info_print_table_row(2, "Mail function", "enabled");
	php_info_print_table_row(2, "Parse function", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto char buildno_lib(void)
 *  print korean extension build number */
PHP_FUNCTION(buildno_lib)
{
	RETURN_STRING (BUILDNO);
}
/* }}} */

/* {{{ proto char version_lib(void)
 *  print korean extension version */
PHP_FUNCTION(version_lib)
{
	RETURN_STRING (BUILDVER);
}
/* }}} */

/* {{{ proto bool movepage_lib(string url, int timeout)
 *  print move action to url */
PHP_FUNCTION(movepage_lib)
{
	zend_string * url  = NULL;
    zend_long     time = 0;

	if ( kr_parameters ("S|l", &url, &time) == FAILURE )
		return;

	if ( ZSTR_LEN (url) == 0 ) {
		php_error (E_WARNING, "1st argument is missing.");
		RETURN_FALSE;
	}

	php_printf ("<meta http-equiv=\"refresh\" content=\"%ld; url=%s\">", time, ZSTR_VAL (url));
	RETURN_TRUE;
	//zend_bailout();
}
/* }}} */

/* {{{ proto float get_microtime_lib(int old, int new) */
PHP_FUNCTION(get_microtime_lib)
{
	zend_string * old;
	zend_string * new;

	char * start_sec;
	char * start_mil;
	char * end_sec;
	char * end_mil;
	char * buf;
	char   ret[10] = { 0, };

	if ( kr_parameters ("SS", &old, &new) == FAILURE )
		return;

	if ( ZSTR_LEN (old) == 0 || ZSTR_LEN (new) == 0 )
		RETURN_FALSE;

	if ( (buf = strchr (ZSTR_VAL (old), ' ')) == NULL )
		RETURN_FALSE;

	*buf = 0;
	start_sec = buf + 1;
	start_mil = ZSTR_VAL (old);

	if ( (buf = strchr (ZSTR_VAL (new), ' ')) == NULL )
		RETURN_FALSE;

	*buf = 0;
	end_sec = buf + 1;
	end_mil = ZSTR_VAL (new);

	sprintf(ret,"%.2f", ((atoi (end_sec) + atof (end_mil)) - (atoi (start_sec) + atof (start_mil))));

	RETURN_STRING(ret);
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
