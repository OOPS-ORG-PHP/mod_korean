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
 
  $Id$ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_kr.h"
#include "krregex.h"
#include "php_krparse.h"

char * kr_regex_replace (char * regex_o, char * replace_o, char * str_o) // {{{
{
	size_t   str_len = 0;
	zval   * replaces;
	char  * buf_o;

	TSRMLS_FETCH ();

	if ( str_o != NULL )
		str_len = strlen (str_o);

	MAKE_STD_ZVAL (replaces);
	ZVAL_STRING (replaces, replace_o, 1);
		
	buf_o = php_pcre_replace (
			regex_o, strlen(regex_o), str_o, str_len, replaces, 0, (int *) &str_len, -1, 0 TSRMLS_CC
	);

	return buf_o;
} // }}}

char * kr_regex_replace_arr (char * regex_o[], char * replace_o[], char * str_o, int regex_no) // {{{
{
	int i;
	size_t str_len = 0;
#ifdef PHP_WIN32
	zval * replaces[100];
#else
	zval * replaces[regex_no];
#endif
	char * o_str = NULL;
	char * c_str = NULL;

	TSRMLS_FETCH ();

	if ( str_o != NULL )
		str_len = strlen (str_o);

	for ( i=0; i<regex_no ; i++ ) {
		MAKE_STD_ZVAL (replaces[i]);
		ZVAL_STRING (replaces[i], replace_o[i], 1);

		if( i == 0 ) {
			o_str = php_pcre_replace (
						regex_o[i],
				   		strlen(regex_o[i]),
						str_o,
						str_len,
						replaces[i],
						0,
						(int *) &str_len,
						-1, 0 TSRMLS_CC
					);
			c_str = emalloc (sizeof (char *) * (str_len + 1));
			strcpy (c_str, o_str);
		} else {
			o_str = NULL;
			o_str = php_pcre_replace(
						regex_o[i],
				  		strlen(regex_o[i]),
						c_str,
						str_len,
						replaces[i],
						0,
						(int *) &str_len,
						-1, 0 TSRMLS_CC
					);
			efree (c_str);
			c_str = emalloc (sizeof (char *) * (str_len + 1));
			strcpy (c_str, o_str);
		}

		str_len = strlen(o_str);
	}

	if ( c_str != NULL )
		efree (c_str);

	return o_str;
} // }}}

int checkReg (char * str, char * regex_o) // {{{
{
	regex_t preg;

	if ( regcomp (&preg, regex_o, REG_EXTENDED) != 0 ) {
		php_error (E_WARNING," Problem REGEX compile in PHP");
		return 0;
	}

	if ( regexec (&preg, str, 0, NULL, 0) == 0 ) {
		regfree (&preg);
		return 1;
	} else {
		regfree (&preg);
		return 0;
	}
} // }}}

int pcre_match (char * regex, char * str) // {{{
{
	pcre * re = NULL;
	pcre_extra * extra = NULL;
	int preg_options = 0, * offsets, val = 0;
	int size_offsets;
	int num_subpats;

	/* Compile regex or get it from cache. */
	if ( (re = pcre_get_compiled_regex (regex, &extra, &preg_options TSRMLS_CC)) == NULL )
		return -1;

	pcre_fullinfo (re, extra, PCRE_INFO_CAPTURECOUNT, &num_subpats);
	num_subpats++;
	size_offsets = num_subpats * 3;
	offsets = (int *) safe_emalloc (size_offsets, sizeof (int), 0);

	/* Execute the regular expression. */
	if ( (pcre_exec (re, extra, str, strlen (str), 0, 0, offsets, size_offsets)) > 0 )
		val = 1;

	safe_efree (offsets);

	return val;
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
