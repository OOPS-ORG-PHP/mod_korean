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

  $Id: korean.c,v 1.20 2002-09-01 06:23:32 oops Exp $
*/

/*
 * PHP4 Korean String modue "korean" - only korean
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"

#include "krregex.h"
#include "php_korean.h"
#include "php_krparse.h"
#include "php_krcheck.h"
#include "php_krerror.h"
#include "php_krfile.h"
#include "php_krnetwork.h"
#include "php_krimage.h"
#include "php_krmail.h"

/* If you declare any globals in php_korean.h uncomment this:
ZEND_DECLARE_MODULE_GLOBALS(korean)
*/

/* True global resources - no need for thread safety here */
static int le_korean;

/* {{{ korean_functions[]
 *
 * Every user visible function must have an entry in korean_functions[].
 */
function_entry korean_functions[] = {
	PHP_FE(buildno_lib,	NULL)
	PHP_FE(version_lib,	NULL)
	PHP_FE(ncrencode_lib,	NULL)
	PHP_FE(ncrdecode_lib,	NULL)
	PHP_FE(uniencode_lib,	NULL)
	PHP_FE(unidecode_lib,	NULL)
	PHP_FE(utf8encode_lib,	NULL)
	PHP_FE(utf8decode_lib,	NULL)
	PHP_FE(check_uristr_lib,	NULL)
	PHP_FE(check_filename_lib,	NULL)
	PHP_FE(check_htmltable_lib,	NULL)
	PHP_FE(is_iis_lib,	NULL)
	PHP_FE(is_windows_lib,	NULL)
	PHP_FE(is_email_lib,	NULL)
	PHP_FE(is_url_lib,	NULL)
	PHP_FE(is_hangul_lib,	NULL)
	PHP_FE(autolink_lib,	NULL)
	PHP_FE(substr_lib,	NULL)
	PHP_FE(perror_lib,	NULL)
	PHP_FE(pnotice_lib,	NULL)
	PHP_FE(movepage_lib,	NULL)
	PHP_FE(human_fsize_lib,	NULL)
	PHP_FE(get_microtime_lib,	NULL)
	PHP_FE(getfiletype_lib,	NULL)
	PHP_FE(getfile_lib,	NULL)
	PHP_FE(putfile_lib,	NULL)
	PHP_FE(filelist_lib,	NULL)
	PHP_FE(agentinfo_lib,	NULL)
	PHP_FE(get_hostname_lib,	NULL)
	PHP_FE(readfile_lib,	NULL)
#if HAVE_KRLIBGD
	PHP_FE(imgresize_lib,	NULL)
#endif
	PHP_FE(mailsource_lib,	NULL)
	PHP_FE(sockmail_lib,	NULL)
	{NULL, NULL, NULL}
};
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
	php_info_print_table_row(2, "URL", "http://cvs.oops.org/index.cgi/korean/");
	php_info_print_table_header(2, "Function", "Support");
	php_info_print_table_row(2, "Check function", "enabled");
	php_info_print_table_row(2, "Charset function", "NCR code, Unicode 2.0, UTF-8, EUC-KR, CP949");
	php_info_print_table_row(2, "Filesystem function", "enabled");
	php_info_print_table_row(2, "HTML function", "enabled");
#if HAVE_KRLIBGD
	php_info_print_table_row(2, "Image function", "enabled");
#else
	php_info_print_table_row(2, "Image function", "disabled, required --enable-korean-gd option");
#endif
	php_info_print_table_row(2, "Mail function", "enabled");
	php_info_print_table_row(2, "Parse function", "enabled");
	php_info_print_table_end();
}
/* }}} */

/* {{{ proto unsigned char buildno_lib(void)
 *  print korean extension build number */
PHP_FUNCTION(buildno_lib)
{
	RETURN_STRING (BUILDNO,1);
}
/* }}} */

/* {{{ proto unsigned char version_lib(void)
 *  print korean extension version */
PHP_FUNCTION(version_lib)
{
	RETURN_STRING (BUILDVER,1);
}
/* }}} */

/* {{{ proto void movepage_lib(string url, int timeout)
 *  print move action to url */
PHP_FUNCTION(movepage_lib)
{
	pval **url, **timeout;
	unsigned int time = 0;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &url) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &url, &timeout) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(timeout);
			time = Z_LVAL_PP(timeout);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(url);

	if (Z_STRLEN_PP(url) == 0)
   	{
		php_error(E_ERROR,"Can't permit NULL value of url address\n");
	}

	php_printf("<META http-equiv=\"refresh\" content=\"%d;URL=%s\">\n", time, Z_STRVAL_PP(url));
	zend_bailout();
}
/* }}} */

/* {{{ proto float get_microtime_lib(int old, int new) */
PHP_FUNCTION(get_microtime_lib)
{
	pval **old, **new;
	unsigned char *old_o, *new_o, *old_f, *old_t, *new_f, *new_t;
	unsigned char *pt_o, *pt_n, ret[10];

	switch(ZEND_NUM_ARGS())
	{
		case 2:
			if(zend_get_parameters_ex(2, &old, &new) == FAILURE)
			{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(old);
			convert_to_string_ex(new);
			old_o = Z_STRVAL_PP(old);
			new_o = Z_STRVAL_PP(new);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if ( strlen(old_o) == 0 || strlen(new_o) == 0 )
	{
		RETURN_FALSE;
	}

	pt_o = strchr(old_o,' ');

	if (pt_o != NULL)
	{
		old_f = estrdup(old_o);
		old_f[pt_o-old_o] = '\0';
		old_t = estrdup(&old_o[pt_o-old_o+1]);
	} else {
		RETURN_FALSE;
	}

	pt_n = strchr(new_o, ' ');

	if (pt_n != NULL)
	{
		new_f = estrdup(new_o);
		new_f[pt_n-new_o] = '\0';
		new_t = estrdup(&new_o[pt_n-new_o+1]);
	} else {
		RETURN_FALSE;
	}

	sprintf(ret,"%.2f", ((atof(new_t) + atof(new_f)) - (atof(old_t) + atof(old_f))));
	RETURN_STRING(ret, 1);
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
