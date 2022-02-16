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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_kr.h"
#include "krregex.h"
#include "php_krparse.h"

// need efree
char * kr_regex_replace (char * regex_o, char * replace_o, char * str_o) // {{{
{
	zend_string * buf;
	zend_string * regex;
	zend_string * subject;
#if PHP_VERSION_ID < 70200
	zval          replaces;
#else
	zend_string * replaces;
#endif
#if PHP_VERSION_ID < 70300
	int           repc = 0;
#else
	size_t        repc = 0;
#endif

	char        * sval;

	regex = ze_string_init (regex_o, 0);
	subject = ze_string_init (str_o, 0);
#if PHP_VERSION_ID < 70200
	ZVAL_STRINGL (&replaces, replace_o, STRLEN (replace_o));
#else
	replaces = ze_string_init (replace_o, 0);
#endif

	buf = php_pcre_replace (
#if PHP_VERSION_ID < 70200
			regex, subject, ZSTR_VAL (subject), (int) ZSTR_LEN (subject), &replaces, 0, -1, &repc
#else
			regex, subject, ZSTR_VAL (subject), ZSTR_LEN (subject), replaces, -1, &repc
#endif
	);

#if PHP_VERSION_ID < 70200
	zval_ptr_dtor (&replaces);
#else
	zend_string_release (replaces);
#endif
	zend_string_release (regex);
	zend_string_release (subject);

	sval = estrdup (ZSTR_VAL (buf));
	zend_string_release (buf);

	return sval;
} // }}}

// need efree
char * kr_regex_replace_arr (char * regex_o[], char * replace_o[], char * str_o, int regex_no) // {{{
{
	zend_string * buf = NULL;
	zend_string * regex;
	zend_string * subject;

	int           i;
#if PHP_VERSION_ID < 70200
	zval          rep;
#else
	zend_string * rep;
#endif
#if PHP_VERSION_ID < 70300
	int           repc = 0;
#else
	size_t        repc = 0;
#endif

	char        * sval;

	subject = ze_string_init (str_o, 0);

	for ( i=0; i<regex_no ; i++ ) {
		regex = ze_string_init (regex_o[i], 0);
#if PHP_VERSION_ID < 70200
		ZVAL_STRINGL (&rep, replace_o[i], STRLEN (replace_o[i]));
#else
		rep = ze_string_init (replace_o[i], 0);
#endif

		if ( i != 0 ) {
			subject = zend_string_dup (buf, 0);
			zend_string_release(buf);
			buf = NULL;
		}

		/*
		php_printf ("regex ########### %s\n", ZSTR_VAL (regex));
		php_printf ("subject ######### %s\n", ZSTR_VAL (subject));
		php_printf ("subjlen ######### %d\n", ZSTR_LEN (subject));
		php_printf ("replace ######### %s\n", ZSTR_VAL (rep.value.str));
		php_printf ("replace ######### %s\n", Z_STRVAL (rep.value.str));
		*/

		buf = php_pcre_replace (
				regex,
		  		subject,
				ZSTR_VAL (subject),
				ZSTR_LEN (subject),
#if PHP_VERSION_ID < 70200
				&rep, 0,
#else
				rep,
#endif
				-1, &repc
		);

#if PHP_VERSION_ID < 70200
		zval_ptr_dtor (&rep);
#else
		zend_string_release (rep);
#endif
		zend_string_release (regex);
		zend_string_release (subject);
	}


	sval = estrdup (ZSTR_VAL (buf));
	zend_string_release (buf);

	return sval;
} // }}}

int checkReg (char * str, char * regex_o) // {{{
{
	regex_t preg;

	if ( regcomp (&preg, (const char *) regex_o, REG_EXTENDED) != 0 ) {
		php_error (E_WARNING," Problem REGEX compile in PHP");
		return 0;
	}

	if ( regexec (&preg, (const char *) str, 0, NULL, 0) == 0 ) {
		regfree (&preg);
		return 1;
	} else {
		regfree (&preg);
		return 0;
	}
} // }}}

/*
 * get php_do_pcre_match
 */
int pcre_match (char * regex, char * subject) // {{{
{
	/* parameters */
	pcre_cache_entry * pce;              /* Compiled regular expression */
	zend_string      * regex_string;     /* Regular expression */
	zval             * subpats = NULL;   /* Array for subpatterns */
	zval             * matches;         /* match counter */
	zend_long          start_offset = 0; /* Where the new search starts */
	int                return_val = -1;

	regex_string = ze_string_init (regex, 0);

#if PHP_VERSION_ID < 70300
	if ( ZEND_SIZE_T_INT_OVFL (STRLEN (subject))) {
		php_error_docref (NULL, E_WARNING, "Subject is too long");
		return -1;
	}
#endif

	/* Compile regex or get it from cache. */
	if ( (pce = pcre_get_compiled_regex_cache (regex_string) ) == NULL) {
		zend_string_release (regex_string);
		return -1;
	}

	zend_string_release (regex_string);
	matches = safe_emalloc (pce->capture_count + 1, sizeof (zval), 0);

	pce->refcount++;
#if PHP_VERSION_ID >= 70400
	{
		zend_string * zsubject = ze_string_init (subject, 0);
		php_pcre_match_impl (
			pce, zsubject, matches, subpats, 0, 0, 0, start_offset
		);
		zend_string_release (zsubject);
	}
#else
	php_pcre_match_impl (
		pce, subject, STRLEN (subject), matches, subpats, 0, 0, 0, start_offset
	);
#endif
	pce->refcount--;

	if ( Z_TYPE_P (matches) != IS_LONG ) {
		kr_safe_efree (matches);
		return -1;
	}

	return_val = (int) Z_LVAL_P (matches);
	kr_safe_efree (matches);

	return return_val;
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
