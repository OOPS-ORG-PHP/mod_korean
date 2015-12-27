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

UChar * kr_regex_replace (UChar * regex_o, UChar * replace_o, UChar * str_o) // {{{
{
	zend_string * buf;
	zend_string * regex;
	zend_string * subject;
	zval        * replaces;

	TSRMLS_FETCH ();

	regex = zend_string_init (regex_o, strlen (regex_o), 0);
	subject = zend_string_init (str_o, strlen (str_o), 0);

	replaces = NULL;
	ZVAL_STRING (replaces, replace_o);

	buf = php_pcre_replace (
			regex, subject, ZSTR_VAL (subject), (int) ZSTR_LEN (subject), replaces, 0, -1, 0
	);

	zend_string_free (regex);
	zend_string_free (subject);

	return (UChar *) ZSTR_VAL (buf);
} // }}}

UChar * kr_regex_replace_arr (UChar * regex_o[], UChar * replace_o[], UChar * str_o, unsigned int regex_no) // {{{
{
	zend_string * buf;
	zend_string * regex;
	zend_string * subject;

	unsigned int i;
#ifdef PHP_WIN32
	zval * replaces[100];
#else
	zval * replaces[regex_no];
#endif

	TSRMLS_FETCH ();

	subject = zend_string_init (str_o, strlen (str_o), 0);

	for ( i=0; i<regex_no ; i++ ) {
		regex = zend_string_init (regex_o[i], strlen (regex_o[i]), 0);
		if ( i != 0 ) {
			subject = zend_string_dup (buf, 0);
			zend_string_free (buf);
		}

		replaces[i] = NULL;
		ZVAL_STRING (replaces[i], replace_o[i]);

		buf = php_pcre_replace(
				regex,
		  		subject,
				ZSTR_VAL (subject),
				ZSTR_LEN (subject),
				replaces[i],
				0, -1, 0
		);

		zend_string_free (regex);
		zend_string_free (subject);
	}

	return (UChar *) ZSTR_VAL (buf);
} // }}}

unsigned int checkReg (UChar * str, UChar * regex_o) // {{{
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

int pcre_match (UChar * regex, UChar * str) // {{{
{
	pcre * re = NULL;
	pcre_extra * extra = NULL;
	int preg_options = 0, * offsets, val = 0;
	unsigned int size_offsets;
	int num_subpats;
	zend_string * regex_string = NULL;

	regex_string = zend_string_init (regex, strlen (regex), 0);

	/* Compile regex or get it from cache. */
	if ( (re = pcre_get_compiled_regex (regex_string, &extra, &preg_options)) == NULL ) {
		zend_string_free (regex_string);
		return -1;
	}

	zend_string_free (regex_string);

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
