/**
 * Project: mod_korean ::
 * File:    krcheck.c
 *
 * Copyright (c) 2019 JoungKyun.Kim
 *
 * LICENSE: GPL
 *
 * @category    Text
 * @package     mod_korean
 * @author      JoungKyun.Kim <http://oops.org>
 * @copyright   2019 OOPS.org
 * @license     GPL
 * @since       File available since release 0.0.1
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"

#include "php_kr.h"
#include "php_krcheck.h"
#include "krregex.h"

/* {{{ Static functions
 */
/* {{{ int numberOfchar(char *str) */
int numberOfchar (char *str, char chk) {
	int no, i, len;

	no = 0;
	len = strlen (str);

	for ( i = 0; i < len; i++ )
		if ( str[i] == chk ) no++;

	return no;
}
/* }}} */

/* {{{ unsigned int checkAddr (UChar * addr, int type) */
unsigned int checkAddr (UChar * addr, int type) {
	UChar * regex;
	UChar regex_e[] = "!^[[:alnum:]\xA1-\xFE._-]+@[[:alnum:]\xA1-\xFE-]+\\.[[:alnum:].-]+$!i";
	UChar regex_u[] = "!^(http|https|ftp|telnet|news)://[[:alnum:]\xA1-\xFE-]+\\.[[:alnum:]\xA1-\xFE:&#@=_~%?/.+-]+$!i";
	UChar u_regex_e[] = "!^[[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}._-]+@[[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}-]+\\.[[:alnum:].-]+$!ui";
	UChar u_regex_u[] = "!^(http|https|ftp|telnet|news)://[[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}-]+\\.[[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}:&#@=_~%?/.+-]+$!ui";
	int ret = 0;

	if ( is_utf8 (addr) )
		regex = estrdup (type ? regex_u : regex_e);
	else
		regex = estrdup (type ? u_regex_u : u_regex_e);

	ret = pcre_match (regex, addr);
	efree (regex);

	return (ret > 0) ? 1 : 0;
}
/* }}} */

/* {{{ unsigned int chkMetaChar (UChar * str, int type) */
unsigned int chkMetaChar (UChar * str, int type) {
	int ret;
	UChar * regex;
	UChar regex_ur[] = "![^[:alnum:]\xA1-\xFE_-]!i";
	UChar regex_up[] = "![^[:alnum:]\xA1-\xFE \\._%-]|\\.\\.!i";
	UChar u_regex_ur[] = "![^[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}_-]!ui";
	UChar u_regex_up[] = "![^[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF} \\._%-]|\\.\\.!ui";

	if ( is_utf8 (str) )
		regex = estrdup (type ? regex_up : regex_ur);
	else
		regex = estrdup (type ? u_regex_up : u_regex_ur);

	ret = pcre_match (regex, str);
	efree (regex);

	return (ret > 0) ? 1 : 0;
}
/* }}} */

/* {{{ unsigned int check_table (UChar *str) */
unsigned int check_table (UChar *str) {
	UChar * buf;
	UChar * regex[] =
	{
		";[\\d]+;",
		";<(/?)(TABLE|TH|TR|TD)[^>]*>;i",
		";<TABLE>;i",
		";<TR>;i",
		";<TH>;i",
		";<TD>;i",
		";</TD>;i",
		";</TH>;i",
		";</TR>;i",
		";</TABLE>;i",
		";[\\D]+;"
	};
	UChar * replace[] = { "", "<\\1\\2>", "1", "2", "3", "4", "94", "93", "92", "91", "" };
	int     result;

	if ( ! checkReg (str, "</?[tT][aA][bB][lL][eE][^>]*>") )
		return 0;

	buf = (UChar *) kr_regex_replace_arr (
		regex, replace, str, (sizeof (regex) / sizeof (regex[0]))
	);


	if ( (strlen(buf) % 3) != 0 ) {
		safe_efree (buf);
		return 1;
	}
	if ( ! checkReg (buf, "^12(3|4).+9291$") )  {
		safe_efree (buf);
		return 2;
	}

	while ( checkReg (buf, "([1-4])9\\1") ) {
		UChar * tbuf;
		tbuf = estrdup (buf);
		safe_efree (buf);
		buf = kr_regex_replace ("/([1-4])9\\1/", "", tbuf);

		safe_efree (tbuf);
	}

	result = (strlen (buf) > 0) ? 3 : 0;
	safe_efree (buf);

	return result;
}
/* }}} */

/* {{{ unsigned int multibyte_check(UChar *str_o, unsigned int p) */
unsigned int multibyte_check (UChar *str_o, unsigned int p) {
	UChar * start_p;
	unsigned int    i,
					l,
					twobyte = 0;

	/* return 0 if point is 1st byte in string */
	if ( p == 0 )
		return 0;

	if ( str_o[p] & 0x80 ||
		/* check of 2byte charactor except KSX 1001 range */
		(str_o[p-1] >= 0x81 && str_o[p-1] <= 0xa0 && str_o[p] >= 0x41 && str_o[p] <=0xfe) ||
		(str_o[p-1] >= 0xa1 && str_o[p-1] <= 0xc6 && str_o[p] >= 0x41 && str_o[p] <=0xa0) )
   	{
		/* if don't exist ' ' charactor */
		if ( (start_p = strchr (&str_o[p], ' ')) == NULL )
			l = strlen (str_o);
		else
			l = start_p - str_o;

		for ( i=p ; i<l ; i++ ) {
			if ( str_o[i] & 0x80 ) {
				twobyte++;
			/* 2th byte of 2 byte charactor is not KSX 1001 range */
			} else if (
				(str_o[i-1] >= 0x81 && str_o[i-1] <= 0xa0 && str_o[i] >= 0x41 && str_o[i] <= 0xfe) ||
				(str_o[i-1] >= 0xa1 && str_o[i-1] <= 0xc6 && str_o[i] >= 0x41 && str_o[i] <= 0xa0)
			) {
				twobyte++;
			}
		}

		if ( (twobyte % 2) != 0)
			return 1;
	}

	return 0;
}
/* }}} */

/* {{{ unsigned int check_windows(unsigned int type)
 * type 1 => check of webserver. if iis, return 1. if not return 0
 * type 0 => check of os. if windows, return 1. if not return 0
 */
unsigned int check_windows (unsigned int type) {
	switch (type) {
		case 1:
			if ( sapi_module.name && ! strcasecmp (sapi_module.name, "isapi") )
				return 1;
			else
				return 0;
			break;
		default:
#ifdef PHP_WIN32
			return 1;
#else
			return 0;
#endif

	}
}
/* }}} */
/* }}} */

/* {{{ proto string check_uristr_lib(string str)
 *    check uri value that include meta charactors. if include, return 1 nor return 0 */
PHP_FUNCTION(check_uristr_lib)
{
	zend_string * input = NULL;
	int           ret   = 0;

	if ( kr_parameters ("S", &input) == FAILURE )
		return;

	if ( ! ZSTR_LEN (input) )
		RETURN_LONG(0);

	ret = chkMetaChar (ZSTR_VAL (input), 0);
	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto string is_email_lib(string mailaddr)
 *    check mail address. if mail address is regular, return mail address or return empty string */
PHP_FUNCTION(is_email_lib)
{
	zend_string  * input = NULL;
	int            ret   = 0;

	if ( kr_parameters ("S", &input) == FAILURE )
		return;

	if ( ! ZSTR_LEN (input) || checkAddr (ZSTR_VAL (input), 0) != 1 )
		RETURN_EMPTY_STRING ();
	
	RETURN_STRING (ZSTR_VAL (input));
}
/* }}} */

/* {{{ proto string is_url_lib(string url)
 *    check url. if url is regular, return url address or return empty string */
PHP_FUNCTION(is_url_lib)
{
	zend_string * input = NULL;
	int           ret   = 0;

	if ( kr_parameters ("S", &input) == FAILURE )
		return;

	if ( ! ZSTR_LEN (input) || checkAddr (ZSTR_VAL (input), 1) != 1 )
		RETURN_EMPTY_STRING ();
	
	RETURN_STRING (ZSTR_VAL (input));
}
/* }}} */

/* {{{ proto string is_hangul_lib(char charactor)
 *    check first 1byte is hangul or not. if it is hangul, return true nor return false */
PHP_FUNCTION(is_hangul_lib)
{
	zend_string * input = NULL;
	char        * buf;

	if ( kr_parameters ("S", &input) == FAILURE )
		return;

	if ( ! ZSTR_LEN (input) )
		RETURN_FALSE;

	if ( ZSTR_VAL (input)[0] >= 0xffffffa1 && ZSTR_VAL (input)[0] <= 0xfffffffe )
		RETURN_LONG (1);

	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string check_htmltable_lib(string str)
 *   check of table structure. if uncomplete structure, return 1 not return 0 */
PHP_FUNCTION(check_htmltable_lib)
{
	zend_string * input = NULL;

	if ( kr_parameters ("S", &input) == FAILURE )
		return;

	if ( ! ZSTR_LEN (input) )
		RETURN_LONG (1);

	RETURN_LONG (check_table (ZSTR_VAL (input)));
}
/* }}} */

/* {{{ proto string is_iis_lib(viod)
 *   check of web server. if web server is iis, return 1 or isn't iis return 0 */
PHP_FUNCTION(is_iis_lib) {
	RETURN_LONG (check_windows (1));
}
/* }}} */

/* {{{ proto string is_windows_lib(viod)
 *  check of os. if os is windows, return 1 or isn't windows, return 0 */
PHP_FUNCTION(is_windows_lib) {
	RETURN_LONG (check_windows (0));
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
