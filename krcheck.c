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
 
  $Id: krcheck.c,v 1.5 2002-08-11 20:08:23 oops Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "SAPI.h"

#include "php_krcheck.h"


unsigned int checkAddr(unsigned char *addr, int type)
{
	regex_t preg;
	int regRet;
	unsigned char regex[95];
	unsigned char regex_e[] = "^[[:alnum:]\xA1-\xFE._-]+@[[:alnum:]\xA1-\xFE_-]+\\.[[:alnum:]._-]+$";
	unsigned char regex_u[] = "^(http|https|ftp|telnet|news)://[[:alnum:]\xA1-\xFE_-]+\\.[[:alnum:]\xA1-\xFE:&#@=_~%?/.+-]+$";

	if (type == 1) { strcpy(regex, regex_u); }
    else { strcpy (regex, regex_e); }	

	if (regcomp(&preg, regex, REG_EXTENDED|REG_ICASE) != 0)
	{
		php_error(E_WARNING, "Problem REGEX compile in PHP");
		return 0;
	}

	regRet = regexec(&preg,addr, 0, NULL, 0);
	regfree(&preg);

	if(regRet == 0) { return 1; }
	else { return 0; }
}

unsigned int chkMetaChar (unsigned char *str, int type)
{
	regex_t preg;
	int regRet;
	unsigned char regex[34];
	unsigned char regex_ur[] = "[^[:alnum:]\xA1-\xFE_-]";
	unsigned char regex_up[] = "[^[:alnum:]\xA1-\xFE \\._%-]|\\.\\.";

	if (type == 1) { strcpy (regex, regex_up); }
	else { strcpy (regex, regex_ur); }

	if (regcomp(&preg,regex,REG_EXTENDED|REG_ICASE) != 0)
   	{
		php_error(E_WARNING, "Problem REGEX compile in PHP");
		return 0;
	}

	regRet = regexec(&preg,str, 0, NULL, 0);
	regfree(&preg);

	if(regRet == 0) { return 1; }
	else { return 0; }
}

unsigned int check_table (unsigned char *str)
{
	const char delimiters[] = "\n";
	unsigned int res;
	unsigned int table_o = 0, tr_o = 0, td_o = 0, th_o = 0, iframe_o = 0;
	unsigned int table_p = 0, tr_p = 0, td_p = 0, th_p = 0, iframe_p = 0;
	unsigned char *buf, *token;
	unsigned char *regex[9] =
					{
						"/>[^<]*</i",
						"/#|@/i",
						"/<(\\/?(TABLE|TR|TD|TH))[^>]*>/i",
						"/^[^#]*/i",
						"/(TABLEEMARK@)[^#]*(#TABLESMARK)/i",
						"/<[^>]*>/i",
						"/#TABLESMARK#/i",
						"/@TABLEEMARK@/i",
						"/(\r?\n)+/i"
					};
	unsigned char *replace[9] =
					{
						">\n<",
						"",
						"\n#TABLESMARK#\\1@TABLEEMARK@\n",
						"",
						"\\1\\2",
						"",
						"\n<",
						">\n",
						"\n"
					};

	buf = (unsigned char *) kr_regex_replace_arr (regex,replace,str, (sizeof (regex) / sizeof (regex[0])));

	token = strtok(buf, delimiters);
	while(token != NULL)
	{
		if(!strcasecmp(token,"<TABLE>")) { table_o++; }
		else if(!strcasecmp(token,"</TABLE>")) { table_p++; }
		else if(!strcasecmp(token,"<TH>")) { th_o++; }
		else if(!strcasecmp(token,"</TH>")) { th_p++; }
		else if(!strcasecmp(token,"<TR>")) { tr_o++; }
		else if(!strcasecmp(token,"</TR>")) { tr_p++; }
		else if(!strcasecmp(token,"<TD>")) { td_o++; }
		else if(!strcasecmp(token,"</TD>")) { td_p++; }
		else if(!strcasecmp(token,"<IFRAME>")) { iframe_o++; }
		else if(!strcasecmp(token,"</IFRAME>")) { iframe_p++; }

		token = strtok (NULL, delimiters);
	}

	if (table_o != table_p) { res = 1; }
	else if (tr_o != tr_p) { res = 1; }
	else if (td_o != td_p) { res = 1; }
	else if (th_o != th_p) { res = 1; }
	else if (iframe_o != iframe_p) { res = 1; }
	else { res = 0; }

	return res;
}

unsigned int multibyte_check(unsigned char *str_o, unsigned int p)
{
	unsigned char *start_p, *test;
	unsigned int i, l, twobyte = 0;

	/* return 0 if point is 1st byte in string */
	if ( p == 0 ) { return 0; }

	if (str_o[p] & 0x80 ||
		/* check of 2byte charactor except KSX 1001 range */
		(str_o[p-1] >= 0x81 && str_o[p-1] <= 0xa0 && str_o[p] >= 0x41 && str_o[p] <=0xfe) ||
		(str_o[p-1] >= 0xa1 && str_o[p-1] <= 0xc6 && str_o[p] >= 0x41 && str_o[p] <=0xa0))
   	{
		start_p = strchr(&str_o[p], ' ');

		/* if don't exist ' ' charactor */
		if(start_p == NULL) { l = strlen(str_o); }
		else { l = start_p-str_o; }

		for (i=p ; i<l ; i++)
	   	{
			if (str_o[i] & 0x80) { twobyte++; }
			/* 2th byte of 2 byte charactor is not KSX 1001 range */
			else if ((str_o[i-1] >= 0x81 && str_o[i-1] <= 0xa0 && str_o[i] >= 0x41 && str_o[i] <=0xfe) ||
					 (str_o[i-1] >= 0xa1 && str_o[i-1] <= 0xc6 && str_o[i] >= 0x41 && str_o[i] <=0xa0))
		   	{ twobyte++; }
		}

		if (twobyte % 2 != 0)
	   	{
			return 1;
		}
	}

	return 0;
}

unsigned int result_except_ksx_1001 (unsigned char *kstr, unsigned int s, unsigned int p)
{
	unsigned int i = 0, rest = 1;
	unsigned int point = s + p;
	unsigned int last = s+p;
	unsigned int slen = strlen(kstr);

	/* if exists NCR code except range of KSX 1001, this retards 2byte charactors.
	   not completed */
	while(1)
	{
		if ( rest == 0) { break; }
		rest = 0;
		for (i=s; i<last; i++) {
			if (i > slen) { break; }
			if (kstr[i] == '&' && kstr[i+1] == '#' && kstr[i+7] == ';')
			{
				i += 7;
				point += 6;
				rest = 1;
			}
		}
		s = last;
		last += point;
	}

	if (kstr[point-1] == '&' && kstr[point] == '#' && kstr[point+6] == ';') { point += 7; }
	else if (kstr[point-2] == '&' && kstr[point-1] == '#' && kstr[point+5] == ';') { point += 6; }
	else if (kstr[point-3] == '&' && kstr[point-2] == '#' && kstr[point+4] == ';') { point += 5; }
	else if (kstr[point-4] == '&' && kstr[point-3] == '#' && kstr[point+3] == ';') { point += 4; }
	else if (kstr[point-5] == '&' && kstr[point-4] == '#' && kstr[point+2] == ';') { point += 3; }
	else if (kstr[point-6] == '&' && kstr[point-5] == '#' && kstr[point+1] == ';') { point += 2; }
	else if (kstr[point-7] == '&' && kstr[point-6] == '#' && kstr[point] == ';') { point += 1; }
	else { point += 0; }

	return point;
}

/* type 1 => check of webserver. if iis, return 1. if not return 0
 * type 0 => check of os. if windows, return 1. if not return 0 */
unsigned int check_windows(unsigned int type)
{
	switch(type)
   	{
		case 1:
			if (sapi_module.name && !strcasecmp(sapi_module.name, "isapi")) { return 1; }
			else { return 0; }
			break;
		default:
#ifdef PHP_WIN32
			return 1;
#else
			return 0;
#endif

	}
}


/* {{{ proto string check_uristr_lib(string str)
 *    check uri value that include meta charactors. if include, return 1 nor return 0 */
PHP_FUNCTION(check_uristr_lib)
{
	pval **arg1;
	int ret;
	unsigned char *str;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
			{
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	str = Z_STRVAL_PP(arg1);

	if ( strlen(str) < 1 ) { RETURN_LONG(0); }
	else { ret = chkMetaChar(str, 0); }

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto string check_filename_lib(string str)
 *    check filename that include meta charactors. if include, return 1 nor return 0 */
PHP_FUNCTION(check_filename_lib)
{
	pval **arg1;
	int ret;
	unsigned char *filename;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	filename = Z_STRVAL_PP(arg1);

	if ( strlen(filename) < 1 ) { RETURN_LONG (0); }
	else { ret = chkMetaChar(filename, 1); }

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto string is_email_lib(string mailaddr)
 *    check mail address. if mail address is regular, return mail address or return null */
PHP_FUNCTION(is_email_lib)
{
	pval **arg1;
	unsigned char *email;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	email = Z_STRVAL_PP(arg1);

	if ( strlen(email) < 1 ) { RETURN_EMPTY_STRING(); }
	else
   	{
		if ( checkAddr (email, 0) == 1 ) { RETURN_STRING (email, 1); }
		else { RETURN_EMPTY_STRING(); }
	}
}
/* }}} */

/* {{{ proto string is_url_lib(string url)
 *    check url. if url is regular, return url address or return null */
PHP_FUNCTION(is_url_lib)
{
	pval **arg1;
	unsigned char *url;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	url = Z_STRVAL_PP(arg1);

	if ( strlen(url) < 1 ) { RETURN_EMPTY_STRING(); }
	else
   	{
		if ( checkAddr (url, 1) == 1 ) { RETURN_STRING (url, 1); }
		else { RETURN_EMPTY_STRING(); }
	}
}
/* }}} */

/* {{{ proto string is_hangul_lib(char charactor)
 *    check first 1byte is hangul or not. if it is hangul, return 1 nor return 0 */
PHP_FUNCTION(is_hangul_lib)
{
	pval **arg1;
	unsigned char *str;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	str = Z_STRVAL_PP(arg1);

	if ( strlen(str) < 1 ) { RETURN_LONG (0); }
	else
   	{
      if( str[0] >= 0xa1 && str[0] <= 0xfe ) { RETURN_LONG (1); }
	  else { RETURN_LONG (0); }
	}
}
/* }}} */

/* {{{ proto string check_htmltable_lib(string str)
 *   check of table structure. if uncomplete structure, return 1 not return 0 */
PHP_FUNCTION(check_htmltable_lib)
{
	pval **arg1;
	unsigned int ret;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	ret = check_table(Z_STRVAL_PP(arg1));

	RETURN_LONG(ret);
}
/* }}} */

/* {{{ proto string is_iis_lib(viod)
 *   check of web server. if web server is iis, return 1 or isn't iis return 0 */
PHP_FUNCTION(is_iis_lib) {
	if (ZEND_NUM_ARGS() != 0)
   	{
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(check_windows(1));
}
/* }}} */

/* {{{ proto string is_windows_lib(viod)
 *  check of os. if os is windows, return 1 or isn't windows, return 0 */
PHP_FUNCTION(is_windows_lib) {
	if (ZEND_NUM_ARGS() != 0)
   	{
		WRONG_PARAM_COUNT;
	}

	RETURN_LONG(check_windows(0));
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
