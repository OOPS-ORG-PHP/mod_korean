/**
 * Project: mod_korean ::
 * File:    krcharset.c
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

#define HAVE_XUINITTABLE

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "php_kr.h"

#include "php_krcharset.h"
#include "php_krcheck.h"
#include "krregex.h"
#include "php_krparse.h"

/* include charset */
#include "charset/iso8859.h"
#include "charset/koi8r.h"
#include "charset/ksc5601.h"
#include "charset/jis0208.h"
#include "charset/gb2312.h"
#include "charset/big5.h"

/* {{{ [[ Global Varivales ]]
 */
static XUChar table_rev_latin2 [ 1024] = { 0, };
static XUChar table_rev_koi8r_1[ 1024] = { 0, };
static XUChar table_rev_koi8r_2[ 1024] = { 0, };
static XUChar table_rev_iso8859[65536] = { 0, };
static XUChar table_rev_jis0208[65536] = { 0, };
static XUChar table_rev_ksc5601[65536] = { 0, };
static XUChar table_rev_gb2312 [65536] = { 0, };
static XUChar table_rev_big5   [65536] = { 0, };
//static const char* xu_lang = NULL;
//static char xu_language[3] = {0, 0, 0};
static int  xu_locale_encoding = XU_CONV_NONE;
static int  XUINITTABLE_CHECK = 0;
/* }}} */

/* {{{ proto string ncrencode_lib (string str [, int type])
   Return ncr code from euc-kr */
PHP_FUNCTION(ncrencode_lib)
{
	zend_string * input = NULL;
	char        * string = NULL;
	int           type   = 0;
	char        * buf;

	if ( kr_parameters ("S|b", &input, &type) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	buf = estrdup (ZSTR_VAL(input));

	string = krNcrEncode (buf, type);

	RETVAL_STRING (string);
	kr_safe_efree (buf);
	kr_safe_efree (string);
}

/* }}} */

/* {{{ proto string ncrdecode_lib (string str)
   Return ncr code from euc-kr */
PHP_FUNCTION(ncrdecode_lib)
{
	zend_string * input = NULL;
	char        * string = NULL;

	if ( kr_parameters ("S", &input) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	string = krNcrDecode (ZSTR_VAL (input));
	RETVAL_STRING (string);
	kr_safe_efree (string);
}

/* }}} */

/* {{{ proto string uniencode_lib (string str [, string start, string end] )
   Return string that convert to unicode from euc-kr or cp949 */
PHP_FUNCTION(uniencode_lib)
{
	zend_string * input = NULL;
	zend_string * Zprefix = NULL;
	zend_string * Zpostfix = NULL;
	char        * prefix = "\\u";
	char        * postfix = "";
	char        * buf = NULL;
	char        * string  = NULL;

	if ( kr_parameters ("S|SS", &input, &Zprefix, &Zpostfix) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	buf = estrdup (ZSTR_VAL (input));

	if ( Zprefix )
		prefix = ZSTR_LEN (Zprefix) ? ZSTR_VAL (Zprefix) : prefix;

	if ( Zpostfix )
		postfix = ZSTR_LEN (Zpostfix) ? ZSTR_VAL (Zpostfix) : postfix;

	if ( (string = uniConv (buf, 0, 0, prefix, postfix)) == NULL ) {
		kr_safe_efree (buf);
		RETURN_FALSE;
	}

	RETVAL_STRING (string);
	kr_safe_efree (buf);
	kr_safe_efree (string);
}

/* }}} */

/* {{{ proto string unidecode_lib (string str , string to_charset [, string start, string end] )
   Return string to convert to cp949 or euc-kr from unicode */
PHP_FUNCTION(unidecode_lib)
{
	zend_string * input   = NULL;
	zend_string * cset    = NULL;
	zend_string * Zprefix  = NULL;
	zend_string * Zpostfix = NULL;
	char        * prefix = "\\u";
	char        * postfix = "";
	int           type    = 0;
	char        * string  = NULL;

	if ( kr_parameters ("SS|SS",
		&input, &cset, &Zprefix, &Zpostfix) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	if ( ZSTR_LEN (cset) == 0 ) {
		php_error(E_ERROR, "missing 2st argument.");
		RETURN_FALSE;
	}

	if ( Zprefix )
		prefix = ZSTR_LEN (Zprefix) ? ZSTR_VAL (Zprefix) : prefix;

	if ( Zpostfix )
		postfix = ZSTR_LEN (Zpostfix) ? ZSTR_VAL (Zpostfix) : postfix;

	if (
			strcasecmp (ZSTR_VAL (cset), "euc-kr") &&
			strcasecmp (ZSTR_VAL (cset), "euc_kr") && 
			strcasecmp (ZSTR_VAL (cset), "cp949")
		)
	{
		php_error(E_ERROR, "Unknown encoding \"%s\"", ZSTR_VAL (cset));
		RETURN_FALSE;
	}

	type = ( ! strcasecmp (ZSTR_VAL (cset), "euc-kr") || ! strcasecmp (ZSTR_VAL (cset), "euc_kr")) ? 1 : 0;

	if ( (string = uniConv(ZSTR_VAL (input), 1, type, prefix, postfix)) == NULL )
		RETURN_FALSE;

	RETVAL_STRING (string);
	kr_safe_efree (string);
}

/* }}} */

/* {{{ proto string utf8encode_lib(string str[, string from_charset)
   Return utf8 string from euc-kr or cp949 */
PHP_FUNCTION(utf8encode_lib)
{
	zend_string * input = NULL;
	zend_string * Zcset = NULL;
	char * cset;
	int    ccode = 0;
	int    ulen  = 0;

	char * utf8  = NULL;

	if ( kr_parameters ("S|S", &input, &Zcset) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	cset = (Zcset && ZSTR_LEN (Zcset)) ? ZSTR_VAL (Zcset) : "EUC-KR";

	utf8 = (char *) emalloc (ZSTR_LEN (input) * 6);

	if ( ZEND_NUM_ARGS() == 1 )
		ccode = XU_CONV_CP949;
	else {
		if      ( ! strcasecmp (cset, "EUC-KR") ) ccode = XU_CONV_CP949;
		else if ( ! strcasecmp (cset, "CP949") )  ccode = XU_CONV_CP949;
		else if ( ! strcasecmp (cset, "BIG5") )   ccode = XU_CONV_BIG5;
		else if ( ! strcasecmp (cset, "CHI") )    ccode = XU_CONV_BIG5;
		else if ( ! strcasecmp (cset, "SJIS") )   ccode = XU_CONV_SJIS;
		else if ( ! strcasecmp (cset, "JPN") )    ccode = XU_CONV_SJIS;
		else                                      ccode = XU_CONV_CP949;
	}

	XUCodeConv (utf8, ZSTR_LEN (input) * 6, XU_CONV_UTF8, ZSTR_VAL (input), ZSTR_LEN(input), ccode);
	ulen = strlen (utf8);

	RETVAL_STRINGL (utf8, ulen);
	kr_safe_efree (utf8);
}
/* }}} */

/* {{{ proto string utf8decode_lib(string str[, string to_charset])
   Return euc-kr or cp949 or unicode string from utf8 */
PHP_FUNCTION(utf8decode_lib)
{
	zend_string * input = NULL;
	zend_string * Zcset = NULL;
	char * cset;
	char * str   = NULL;
	int    slen  = 0;
	int    ccode = 0;
	char * ncr   = NULL;

	if ( kr_parameters ("S|S", &input, &Zcset) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	cset = (Zcset && ZSTR_LEN (Zcset)) ? ZSTR_VAL (Zcset) : "EUC-KR";
	str = emalloc (ZSTR_LEN (input) * 6);

	if      ( ! strcasecmp (cset, "EUC-KR") ) ccode = XU_CONV_CP949;
	else if ( ! strcasecmp (cset, "CP949") )  ccode = XU_CONV_CP949;
	else if ( ! strcasecmp (cset, "CHI") )    ccode = XU_CONV_BIG5;
	else if ( ! strcasecmp (cset, "BIG5") )   ccode = XU_CONV_BIG5;
	else if ( ! strcasecmp (cset, "JPN") )    ccode = XU_CONV_SJIS;
	else if ( ! strcasecmp (cset, "SJIS") )   ccode = XU_CONV_SJIS;
	else                                      ccode = XU_CONV_CP949;

	XUCodeConv (str, ZSTR_LEN (input) * 6, ccode, ZSTR_VAL (input), ZSTR_LEN (input), XU_CONV_UTF8);
	slen = strlen (str);

	if ( ccode == XU_CONV_CP949 && strcasecmp (cset, "CP949") ) {
		// if charset is euc-kr, convert ncr code about out of ranges
		ncr = krNcrEncode (str, 1);
		RETVAL_STRING (ncr);
		kr_safe_efree (ncr);
	} else
		RETVAL_STRINGL(str, slen);

	kr_safe_efree (str);
}
/* }}} */

/* {{{ char * krNcrEncode (char * str_o, int type)
 * convert euc-kr to ncr code, or convert outside EUC-KR range to ncr code
 * chart * str_o => EUC-KR/CP949 string
 * int type      => convert whole string(0) or outside EUC-KR range(1)
 */
char * krNcrEncode (char * str_o, int type)
{
	long i;
	int ncr;
	size_t len;
	char rc[9] = { 0, };
	char * ret = NULL;
	UChar c1, c2;

	if ( str_o == NULL ) { return NULL; }
	else { len = strlen (str_o); }

	ret = emalloc (sizeof(char) * strlen (str_o) * 8);
	memset (ret, 0, sizeof (char) * strlen (str_o) * 8);

	for( i=0; i<len; i++ ) {
		memset (rc, 0, 9);

		c1 = (UChar) str_o[i];
		c2 = (UChar) str_o[i + 1];

		/* if 2byte charactor */
		if ( c1 & 0x80 ) {
			switch (type) {
				/* if type 1, check range of KSX 1001 */
				case 1:
					if ( (c1 >= 0x81 && c1 <= 0xa0 && c2 >= 0x41 && c2 <=0xfe) ||
					   (c1 >= 0xa1 && c1 <= 0xc6 && c2 >= 0x41 && c2 <=0xa0) )
				   	{
						if ( c2 < 0x41 || c2 > 0xfe ) { memmove (rc, "?", 1); }
						else if ( 0x5a < c2 && c2 < 0x61 ) { memmove (rc, "?", 1); }
						else if ( 0x7a < c2 && c2 < 0x81 ) { memmove (rc, "?", 1); }
						else
						{
							if ( c2 > 0x7a ) c2 -= 6;
							if ( c2 > 0x5a ) c2 -= 6;
							ncr = (c1 - 0x81) * 178 + (c2 - 0x41);
							sprintf(rc, "&#%d;", table_ksc5601[ncr]);
						}
						i++;
					} else {
						memcpy (rc, str_o + i, 2);
						memset (rc + 2, '\0', 1);
						i++;
					}

					break;
				/* range of whole string */
				default:
					if ( 0x81 <= c1 && c1 <= 0xc8 ) {
						if ( c2 < 0x41 || c2 > 0xfe ) { memmove (rc, "?", 1); }
						else if ( 0x5a < c2 && c2 < 0x61 ) { memmove (rc, "?", 1); }
						else if ( 0x7a < c2 && c2 < 0x81 ) { memmove (rc, "?", 1); }
						else {
							if ( c2 > 0x7a ) c2 -= 6;
							if ( c2 > 0x5a ) c2 -= 6;
							ncr = (c1 - 0x81) * 178 + (c2 - 0x41);
							sprintf(rc, "&#%d;", table_ksc5601[ncr]);
						}
					} else if ( 0xca <= c1 && c1 <= 0xfd ) {
						if ( c2 < 0xa1 || c2 > 0xfe ) { memmove (rc, "?", 1); }
						else {
							ncr = (c1 - 0xca) * 94 + (c2 - 0xa1);
							sprintf(rc, "&#%d;", table_ksc5601_hanja[ncr]);
						}
					} else {
						memset (rc, c1, 1);
						memset (rc + 1, '\0', 1);
					}

					i++;
			}
		}
		/* 1 byte charactor */
		else
	   	{
			memset (rc, c1, 1);
			memset (rc + 1, '\0', 1);
	   	}

		if ( strlen (rc) != 0 ) {
			int rc_len = strlen (rc);
			if ( ret != NULL ) {
				int ret_len = strlen (ret);
				memmove (ret + ret_len, rc, rc_len);
				memset (ret + ret_len + rc_len, '\0', 1);
			} else {
				memmove (ret, rc, rc_len);
				memset (ret + rc_len, '\0', 1);
			}
		}
	}

	return ret;
}
/* }}} */

/* {{{ char * krNcrDecode (char * str_o) */
char * krNcrDecode (char * str_o)
{
	unsigned int first, second, tmp;
	int slen, i = 0;
	char rc[3] = { 0, }, tmpstr[8] = { 0, };
	char * ret = NULL;

	ret = emalloc (sizeof (char) * strlen (str_o) * 2);
	memset (ret, 0, sizeof (char) * strlen (str_o) * 2);

	if ( str_o == NULL ) { return NULL; }
	else { slen = strlen (str_o); }

	for ( i=0; i<slen; i++ ) {
		if ( str_o[i] == '&' && str_o[i+1] == '#' && str_o[i+7] == ';' )
		{
			if ( XUINITTABLE_CHECK == 0 )
			{
				XUInitTable();
				XUINITTABLE_CHECK = 1;
			}

			memmove (tmpstr, str_o + i + 2, 5);

			tmp = atoi (tmpstr);
			tmp = table_rev_ksc5601[tmp];

			first = tmp >> 8;
			second = tmp & 0x00FF;

			/* if converted charactor is first byte of 2byte charactor */
			if ( first & 0x80 ) {
				memset (rc, first, 1);
				memset (rc + 1, second, 1);
				memset (rc + 2, '\0', 1);
				i += 7;
			} else {
				memset (rc, str_o[i], 1);
				memset (rc + 1, '\0', 1);
			}
		} else {
			memset (rc, str_o[i], 1);
			memset (rc + 1, '\0', 1);
		}

		if ( strlen (rc) != 0 ) {
			int rc_len = strlen (rc);
			if ( ret != NULL ) {
				int ret_len = strlen (ret);
				memmove (ret + ret_len, rc, rc_len);
				memset (ret + ret_len + rc_len, '\0', 1);
			} else {
				memmove (ret, rc, rc_len);
				memset (ret + rc_len, '\0', 1);
			}
		}
	}

	return ret;
}
/* }}} */

/* {{{ char * uniConv (char * str_o, int type, int subtype, char * start, char * end)
 * Convert EUC-KR/CP940 to unicode
 * char * str_o => convert string (euc-kr, cp949, unicode)
 * int type     => 0: convert euc-kr,cp949 -> unicode
 *                1: convert unicode -> ecu-kr,cp949
 * int subtype  => if value of type set 1, 1 is euc-kr and 0 is cp949
 * char * start => front string of hex value of unicode (ex U+AC60; => U+)
 * char * end   => after string of hex value of unicode (ex U+AC60; => ; )
 */
char * uniConv (char * str_o, int type, int subtype, char * start, char * end)
{
	unsigned long i;
	unsigned int aryno;
	size_t len;
	char rc[256] = { 0, };
	char * ret = NULL;

	int regno,hexv,firsti,secondi;
	long slen = strlen (start);
	long elen = strlen (end);
	regex_t preg;
	char regex[12] = "[0-9a-f]{4}";
	char chkReg[5], conv[5], first[3], second[3];
	UChar c1, c2;

	if ( str_o == NULL ) { return NULL; }
	else { len = strlen (str_o); }

	if (slen > 10 || elen > 10) {
		php_error(E_ERROR,"Can't use string over 10 charactors <br />\n" \
				          "on unicode start string or end string");
	}

	if (type == 1)
   	{
		regno = regcomp(&preg, regex, REG_EXTENDED|REG_ICASE);

		if (regno != 0) {
			php_error(E_WARNING, "Problem in Unicode start charactors or end charactocs");
			return str_o;
		}
	}

	i = sizeof(char) * strlen (str_o) * (5 + strlen (start) + strlen (end));
	ret = emalloc (i);
	memset (ret, '\0', i);

	for ( i=0; i<len; i++ )
   	{
		switch (type)
	   	{
			/* convert to euc-kr/cp949 from unicode */
			/* unicode is constructed start charactors and hex code and end charactors */
			case 1:
				/* make regex check value */
				memset (chkReg, str_o[i+slen], 1);
				memset (chkReg+1, str_o[i+slen+1], 1);
				memset (chkReg+2, str_o[i+slen+2], 1);
				memset (chkReg+3, str_o[i+slen+3], 1);
				memset (chkReg+4, '\0', 1);

				if ( XUINITTABLE_CHECK == 0 ) {
					XUInitTable();
					XUINITTABLE_CHECK = 1;
				}

				if ( ! strncmp (&str_o[i], start, slen) &&
				     regexec (&preg, chkReg, 0, NULL, 0) == 0 &&
				     ! strncmp (&str_o[i+slen+4], end, elen) )
			   	{
					hexv = hex2dec (chkReg, 0);
					sprintf (conv, "%x", table_rev_ksc5601[hexv]);

					/* make first byte */
					memset (first, conv[0], 1);
					memset (first + 1, conv[1], 1);
					memset (first + 2, '\0', 1);
					firsti = hex2dec (first, 1);

					/* make second byte */
					memset (second, conv[2], 1);
					memset (second + 1, conv[3], 1);
					memset (second + 2, '\0', 1);
					secondi = hex2dec (second, 1);

					/* make complete 2byte charactor */
					memset (rc, firsti, 1);
					memset (rc + 1, secondi, 1);
					memset (rc + 2, '\0', 1);

					/* convert ncr code with outsize of EUC-KR range */
					if ( subtype == 1 ) {
						c1 = (UChar) rc[0];
						c2 = (UChar) rc[1];
						if (
							(c1 >= 0x81 && c1 <= 0xa0 && c2 >= 0x41 && c2 <=0xfe) ||
							(c1 >= 0xa1 && c1 <= 0xc6 && c2 >= 0x41 && c2 <=0xa0)
						) {
							sprintf (rc, "&#%d;", hexv);
						}
					}

					i = i + 3 + slen+elen;
				} else {
					memset (rc, str_o[i], 1);
					memset (rc + 1, '\0', 1);
				}

				break;
			/* convert to unicode from euc-kr/cp949 */
			default:
				c1 = (UChar) str_o[i];
				c2 = (UChar) str_o[i + 1];

				/* if 2byte charactor */
				if ( 0x81 <= c1 && c1 <= 0xc8 )
				{
					if ( c2 < 0x41 || c2 > 0xfe ) { memmove (rc, "?", 1); }
					else if ( 0x5a < c2 && c2 < 0x61 ) { memmove (rc, "?", 1); }
					else if ( 0x7a < c2 && c2 < 0x81 ) { memmove (rc, "?", 1); }
					else
					{
						if ( c2 > 0x7a ) c2 -= 6;
						if ( c2 > 0x5a ) c2 -= 6;
						aryno = (c1 - 0x81) * 178 + (c2 - 0x41);
						sprintf (rc, "%s%X%s", start, table_ksc5601[aryno], end);
						i++;
					}
				}
				else if (0xca <= c1 && c1 <= 0xfd ) {
					if ( c2 < 0xa1 || c2 > 0xfe ) { memmove (rc, "?", 1); }
					else {
						aryno = (c1 - 0xca) * 94 + (c2 - 0xa1);
						sprintf (rc, "%s%X%s", start, table_ksc5601_hanja[aryno], end);
						i++;
					}
				}
				/* if 1byte charactor */
				else
				{
					memset (rc, c1, 1);
					memset (rc + 1, '\0', 1);
				}
		}

		if ( strlen (rc) != 0 ) {
			int rclen = strlen (rc);
			if ( ret != NULL ) {
				int retlen = strlen (ret);
				memmove (ret + retlen, rc, rclen);
				memset (ret + retlen + rclen, '\0', 1);
			} else {
				memmove (ret, rc, rclen);
				memset (ret + rclen, '\0', 1);
			}
		}
	}

	if ( type == 1 ) { regfree(&preg); }
	return ret;
}
/* }}} */

/* {{{ int hex2dec (char * str_o,int type) */
int hex2dec (const char * str_o, int type) {
	int i,buf[4],len = strlen (str_o);

	for ( i=0; i<len; i++ ) {
		/* range of alphabat a -> f */
		if ( (str_o[i] >= 0x61 && str_o[i] <= 0x66) || (str_o[i] >= 0x41 && str_o[i] <= 0x46) ) {
			switch (str_o[i]) {
				case 'a' : buf[i] = 10; break;
				case 'b' : buf[i] = 11; break;
				case 'c' : buf[i] = 12; break;
				case 'd' : buf[i] = 13; break;
				case 'e' : buf[i] = 14; break;
				case 'f' : buf[i] = 15; break;
				case 'A' : buf[i] = 10; break;
				case 'B' : buf[i] = 11; break;
				case 'C' : buf[i] = 12; break;
				case 'D' : buf[i] = 13; break;
				case 'E' : buf[i] = 14; break;
				case 'F' : buf[i] = 15; break;
			}
		} else {
			switch (str_o[i]) {
				case '0' : buf[i] = 0; break;
				case '1' : buf[i] = 1; break;
				case '2' : buf[i] = 2; break;
				case '3' : buf[i] = 3; break;
				case '4' : buf[i] = 4; break;
				case '5' : buf[i] = 5; break;
				case '6' : buf[i] = 6; break;
				case '7' : buf[i] = 7; break;
				case '8' : buf[i] = 8; break;
				case '9' : buf[i] = 9; break;
				default: buf[i] = 0;
			}
		}
	}

	switch (type) {
		case 1 :
			return ((buf[0] * 16) + buf[1]);
			break;
		default :
			return ((buf[0] * 16 * 16 * 16) + (buf[1] * 16 * 16) + (buf[2] * 16) + buf[3]);
			
	}
}
/* }}} */

/* {{{ int is_utf8 (char * s) */
int is_utf8 (char * s)
{
	int len, i, check = 0, j;
	int ascii_status = 0;

	// utf-8 bomb character
	if ( s[0] == 0xef && s[1] == 0xbb && s[2] == 0xbf )
		return 0;

	// 적어도 utf8 1글자의 길이는 3byte여야 함
	if ( (len = strlen (s)) < 3 )
		return 1;

	for ( i=0; i<len; i++ ) {
		unsigned char c = (unsigned char) s[i];

		// ASCII 영역은 건너띈다.
		if ( ! (c & 0x80) )
			continue;

		ascii_status = 1;

		// utf8 is must 110xxxxxxx
		// 0x40 -> 01000000
		if ( ! (c & 0x40) )
			return 1;

		if ( (c >> 5) == 0x06 )      // 1st of 2byte utf8
			check = 1;
		else if ( (c >> 4) == 0x0e ) // 1st of 3byte utf8
			check = 2;

		if ( (i + check) >= len )
			return 1;

		i++;
		for ( j=i; j<(i + check); j++ ) {
			if ( ((unsigned char) s[j] >> 6) != 0x02 )
				return 1;
		}
		break;
	}

	if ( ascii_status == 0 )
		return 1;

	return 0;
}
/* }}} */

/* new utf8 */
/*****************************************************************************************/
/* Code A -> Code B  Start  */
/*****************************************************************************************/
// UTF-8 -> locale code OR locale code -> UTF-8

/* {{{ int XUCodeConv(char* dest, int max, int codeTo, const char* text, int length, int codeFrom) */
int XUCodeConv(char* dest, int max, int codeTo, const char* text, int length, int codeFrom)
{
	XUChar* buf;
	int len1;
	int len2;

	if ( XUINITTABLE_CHECK == 0 )
	{
		XUInitTable();
		XUINITTABLE_CHECK = 1;
	}

	if (length < 0) length = strlen (text);
	buf = (XUChar*) emalloc(sizeof(XUChar) * length);
	len1 = XUEncode(buf, length, text, length, codeFrom);	 // other -> unicode
	len2 = XUDecode(dest, max, buf, len1, codeTo);			// unicode -> other

	kr_safe_efree(buf);
	return len2;
}
/* }}} */

/* {{{ static void XUInitTable() */
static void XUInitTable()
{
	int i, j;
	XUChar c;
	XUChar c1;
	XUChar c2; 

	/*
	for(i = 0; i < 1024; i++) 
	{
	   table_rev_latin2 [i] = 0;
	   table_rev_koi8r_1[i] = 0;
	   table_rev_koi8r_2[i] = 0;
	} 
	*/

	for(i = 0; i < 128; i++) 
	{
	   c = table_iso8859[1].data[i];
	   if (c) table_rev_latin2[c] = i + 0x0080;
	} 
	
	for(i = 0; i < 128; i++) 
	{
		c = table_koi8r[i];
		if (0x0080 <= c && c < 0x0480) 
			table_rev_koi8r_1[c - 0x0080] = i + 0x0080;
		else if (0x2200 <= c && c < 0x2600) 
			table_rev_koi8r_2[c - 0x2200] = i + 0x0080;
	} 

	/*
	for(i = 0; i < 65536; i++) table_rev_iso8859[i] = 0;
	for(i = 0; i < 65536; i++) table_rev_jis0208[i] = 0;
	for(i = 0; i < 65536; i++) table_rev_ksc5601[i] = 0;
	for(i = 0; i < 65536; i++) table_rev_gb2312 [i] = 0;
	for(i = 0; i < 65536; i++) table_rev_big5   [i] = 0; 
	*/

	for(i = 9; i >= 1; i--) 
	{
		c = (i << 8) + 128;
		for(j = 0; j < 128; j++, c++) 
			table_rev_iso8859[table_iso8859[i - 1].data[j]] = c;
	} 

	c1 = 0x21;
	c2 = 0x21;
	for(i = 0; i < 7896; i++) 
	{
		c = table_jis0208[i];
		if (c) table_rev_jis0208[c] = (c1 << 8) + c2;
		c2++;
		if (c2 == 0x7f) 
		{
		   c2 = 0x21;
		   c1++;
		}
	} 

	c1 = 0x21;
	c2 = 0x21;
	for(i = 0; i < 8178; i++) 
	{
	   c = table_gb2312[i];
	   if (c) table_rev_gb2312[c] = (c1 << 8) + c2;
	   c2++;
	   if (c2 == 0x7f) 
	   {
		   c2 = 0x21;
		   c1++;
	   }
	} 

	c1 = 0xa1;
	c2 = 0x40;
	for(i = 0; i < 13973; i++) 
	{
	   c = table_big5[i];
	   if (c) table_rev_big5[c] = (c1 << 8) + c2;
	   c2++;
	   if (c2 == 0x7f) 
		   c2 = 0xa1;
	   else if (c2 == 0xff) 
	   {
		   c2 = 0x40;
		   c1++;
	   }
	} 

	c1 = 0x81;
	c2 = 0x41;
	for(i = 0; i < 12816; i++) 
	{
	   c = table_ksc5601[i];
	   if (c) table_rev_ksc5601[c] = (c1 << 8) + c2;
	   c2++;
	   if (c2 == 0x5b) 
		  c2 = 0x61;
	   else if (c2 == 0x7b) 
		  c2 = 0x81;
	   else if (c2 == 0xff) 
	   {
		   c2 = 0x41;
		   c1++;
	   }
	} 

	c1 = 0xca;
	c2 = 0xa1;
	for(i = 0; i < 4888; i++) 
	{
	   c = table_ksc5601_hanja[i];
	   if (c) table_rev_ksc5601[c] = (c1 << 8) + c2;
	   c2++;
	   if (c2 == 0xff) 
	   {
		   c2 = 0xa1;
		   c1++;
	   }
	}
}
/* }}} */

/* Other -> Unicode(UCS-2) */
/* {{{ int XUEncode(XUChar* dest, int max, const char* text, int length, int code) */
int XUEncode(XUChar* dest, int max, const char* text, int length, int code)
{
	int ret = 0;
	int chlen; 
	
	if (length == 0) 
	{
		if (max > 0) *dest = 0;
		return 0;
	}

	for(;;) 
	{
		if (length < 0 && !*text) break;
		if (ret < max) 
		{
		   *dest = XUCharEncode(text, length, code);
		   dest++;
		}
		ret++;
		chlen = XUCharLen(text, length, code);
		text += chlen;
		if (length > 0) 
		{
			length -= chlen;
			if (length < 1) break;
		}
	} 

	if (ret < max) *dest = 0;
	return ret;
}
/* }}} */

/* Other -> Unicode */
/* {{{ XUChar XUCharEncode(const char* text, int max, int code) */
XUChar XUCharEncode(const char* text, int max, int code)
{
	XUChar ret = 0;
	XUChar c1;
	XUChar c2;
	XUChar num; 

	if (max == 0 || !*text) return 0; 

	if (code == XU_CONV_LOCALE) code = xu_locale_encoding;

	if (code <= XU_CONV_NONE) return '?';
		   if (code == XU_CONV_UTF8) return XUutf8CharEncode(text, max); 

	c1 = (XUChar)(unsigned char)*text;
		   if (c1 < 0x80) return c1;

	if (code <= XU_CONV_ISO8859(15)) 
	{
		ret = table_iso8859[code - XU_CONV_ISO8859(1)].data[c1 - 0x80];
		if (ret) return ret;
		return '?';
	} 
	else if (code == XU_CONV_KOI8R) 
	{
		ret = table_koi8r[c1 - 0x80];
		if (ret) return ret;
		return '?';
	}

	if (code == XU_CONV_SJIS) 
	{
		if (0xa1 <= c1 && c1 <= 0xdf) return c1 + 0xfec0;
	}
	if (max == 1) return '?';
	text++; 

	c2 = (XUChar)(unsigned char)*text;
	if (!c2) return '?';
	if (code == XU_CONV_EUCJP) 
	{
		if (c1 == 0x8e) return c2 + 0xfec0;
		if (c1 < 0xa1 || c1 > 0xf4) return '?';
		if (c2 < 0xa1 || c2 > 0xfe) return '?';
		ret = table_jis0208[(c1 - 0xa1) * 94 + (c2 - 0xa1)];
	} 
	else if (code == XU_CONV_SJIS) 
	{
		if (c1 == 0x80 || c1 == 0xa0 || c1 > 0xfc) return '?';
		if (c2  < 0x40 || c2 == 0x7f || c2 > 0xfc) return '?';
		if (c1 >= 0xe0) c1 -= 0x40;
		if (c2 > 0x7e) c2--;
		num = (c1 - 0x81) * 188 + (c2 - 0x40);
		if (num < 7896) ret = table_jis0208[num];
	} 
	else if (code == XU_CONV_CP949) 
	{
		if (0x81 <= c1 && c1 <= 0xc8) 
		{
			if (c2 < 0x41 || c2 > 0xfe) return '?';
			if (0x5a < c2 && c2 < 0x61) return '?';
			if (0x7a < c2 && c2 < 0x81) return '?';
			if (c2 > 0x7a) c2 -= 6;
			if (c2 > 0x5a) c2 -= 6;
			ret = table_ksc5601[(c1 - 0x81) * 178 + (c2 - 0x41)];
		} 
		else if (0xca <= c1 && c1 <= 0xfd) 
		{
			if (c2 < 0xa1 || c2 > 0xfe) return '?';
			ret = table_ksc5601_hanja[(c1 - 0xca) * 94 + (c2 - 0xa1)];
		}
	} 
	else if (code == XU_CONV_EUCCN) 
	{
		if (c1 < 0xa1 || c1 > 0xf7) return '?';
		if (c2 < 0xa1 || c2 > 0xfe) return '?';
		ret = table_gb2312[(c1 - 0xa1) * 94 + (c2 - 0xa1)];
	} 
	else if (code == XU_CONV_BIG5) 
	{
		if (c1 < 0xa1 || c1 > 0xf9) return '?';
		if (c2 < 0x40 || c2 > 0xfe) return '?';
		if (0x7e < c2 && c2 < 0xa1) return '?';
		if (c2 > 0x7e) c2 -= 0x22;
		ret = table_big5[(c1 - 0xa1) * 157 + (c2 - 0x40)];
	}
	if (ret) return ret; 

	return '?';
}
/* }}} */

/* {{{ int XUCharLen(const char* text, int max, int code) */
int XUCharLen(const char* text, int max, int code)
{
	XUChar c1;
	XUChar c2; 

	if (max == 0 || !*text) return 1; 

	if (code == XU_CONV_LOCALE) code = xu_locale_encoding;
	if (code == XU_CONV_UTF8  ) return XUutf8CharLen(text, max);
	if (code <= XU_CONV_KOI8R ) return 1; 

	c1 = (XUChar)(unsigned char)*text;
	if (code == XU_CONV_SJIS) 
	{
		if (c1 < 0x80 || (0xa1 <= c1 && c1 <= 0xdf)) return 1;
	}
	if (max == 1) return 1;
	text++; 

	c2 = (XUChar)(unsigned char)*text;
	if (!c2) return 1;
	if (code == XU_CONV_EUCJP) 
	{
		if (c1 == 0x8e) return 2;
		if (c1 < 0xa1 || c1 > 0xf4) return 1;
		if (c2 < 0xa1 || c2 > 0xfe) return 1;
		return 2;
	} 
	else if (code == XU_CONV_SJIS) 
	{
		if (c1 == 0x80 || c1 == 0xa0 || c1 > 0xfc) return 1;
		if (c2  < 0x40 || c2 == 0x7f || c2 > 0xfc) return 1;
		return 2;
	} 
	else if (code == XU_CONV_CP949) 
	{
		if (0x81 <= c1 && c1 <= 0xc8) 
		{
			if (c2 < 0x41 || c2 > 0xfe) return 1;
			if (0x5a < c2 && c2 < 0x61) return 1;
			if (0x7a < c2 && c2 < 0x81) return 1;
			return 2;
		} 
		else if (0xca <= c1 && c1 <= 0xfd) 
		{
			if (c2 < 0xa1 || c2 > 0xfe) return 1;
			return 2;
		}
	} 
	else if (code == XU_CONV_EUCCN) 
	{
		if (c1 < 0xa1 || c1 > 0xf7) return 1;
		if (c2 < 0xa1 || c2 > 0xfe) return 1;
		return 2;
	} 
	else if (code == XU_CONV_BIG5) 
	{
		if (c1 < 0xa1 || c1 > 0xf9) return 1;
		if (c2 < 0x40 || c2 > 0xfe) return 1;
		if (0x7e < c2 && c2 < 0xa1) return 1;
		return 2;
	} 

	return 1;
}
/* }}} */

/* Unicode(UCS-2) -> Other */
/* {{{ int XUDecode(char* dest, int max, const XUChar* text, int length, int code) */
int XUDecode(char* dest, int max, const XUChar* text, int length, int code)
{
	int ret = 0;
	int chlen; 

	if (length == 0) 
	{
		if (max > 0) *dest = 0;
		return 0;
	}

	for(;;) 
	{
		if (length < 0 && !*text) break;
		chlen = XUCharDecode(dest, max, *text, code);
		ret += chlen;
		if (max > 0) 
		{
			dest += chlen;
			max -= chlen;
		}
		text++;
		if (length > 0) 
		{
			length--;
			if (length < 1) break;
		}
	} 

	if (max > 0) *dest = 0;
	return ret;
}
/* }}} */

/* Unicode -> Other */
/* {{{ int XUCharDecode(char* dest, int max, XUChar ch, int code) */
int XUCharDecode(char* dest, int max, XUChar ch, int code)
{
	XUChar ret = 0;
	XUChar r1;
	XUChar r2;
	XUChar i;
	XUChar* table; 

	if (code == XU_CONV_LOCALE) 
	{
		code = xu_locale_encoding;
	} 
	else if (code == XU_CONV_NONE) 
	{
		if (max >= 1) dest[0] = '?';
		return 1;
	} 
	else if (code == XU_CONV_UTF8) 
	{
		return XUutf8CharDecode(dest, max, ch);
	}	 
	if (ch < 0x0080) 
	{
		if (max >= 1) dest[0] = (char)ch;
		return 1;
	}
	if (code <= XU_CONV_ISO8859(15)) 
	{
		if (ch < 0x00a1) 
		{
			ret = ch;
		} 
		else 
		{
			table = table_iso8859[code - XU_CONV_ISO8859(1)].data;
			for(i = 0x21; i < 0x80; i++) 
			{
				if (ch == table[i]) 
				{
					ret = i + 0x80;
					break;
				}
			}
		}
		if (!ret) ret = '?';
		if (max >= 1) dest[0] = (char)ret;
		return 1;
	} 
	else if (code == XU_CONV_KOI8R) 
	{
		if (0x0080 <= ch && ch < 0x0480) 
		{
			 ret = table_rev_koi8r_1[ch - 0x0080];
		} 
		else if (0x2200 <= ch && ch < 0x2600) 
		{
			 ret = table_rev_koi8r_2[ch - 0x2200];
		}
		if (!ret) ret = '?';
		if (max >= 1) dest[0] = (char)ret;
		return 1;
	} 

	if (code == XU_CONV_EUCJP) 
	{
		if (0xff61 <= ch && ch <= 0xff9f) 
		{
			ret = (ch - 0xfec0) + 0x8e00;
		} 
		else 
		{
			ret = table_rev_jis0208[ch];
			if (ret) ret += 0x8080;
		}
	} 
	else if (code == XU_CONV_SJIS) 
	{
		if (0xff61 <= ch && ch <= 0xff9f) 
		{
			if (max >= 1) dest[0] = (char)(ch - 0xfec0);
			return 1;
		} 
		else 
		{
			 ret = table_rev_jis0208[ch];
			 r1 = (ret >> 8  ) - 0x21;
			 r2 = (ret & 0xff) - 0x21;
			 if (r1 & 1) r2 += 94;
			 r1 = (r1 >> 1) + 0x81;
			 if (r1 > 0x9f) r1 += 0x40;
			 r2 += 0x40;
			 if (r2 > 0x7e) r2++;
			 ret = (r1 << 8) + r2;
		 }
	} 
	else if (code == XU_CONV_CP949) 
	{
		 ret = table_rev_ksc5601[ch];
	} 
	else if (code == XU_CONV_EUCCN) 
	{
		 ret = table_rev_gb2312[ch];
		 if (ret) ret += 0x8080;
	} 
	else if (code == XU_CONV_BIG5) 
	{
		 ret = table_rev_big5[ch];
	}
	if (!ret) 
	{
		 if (max >= 1) dest[0] = '?';
		 return 1;
	} 
	
	if (max >= 2) 
	{
		dest[0] = (char)(ret >> 8);
		dest[1] = (char)(ret & 0xff);
	}
	return 2;
}
/* }}} */

/********************************************/
/* Code A -> Code B  End  */
/********************************************/

/********************************************/
/* UTF8 -> Unicode  Start  */
/********************************************/

/* {{{ int XUutf8Encode(XUChar* dest, int max, const char* text, int length)
 * utf-8 -> ucs
 */
int XUutf8Encode(XUChar* dest, int max, const char* text, int length)
{
	int ret = 0;
	int chlen; 

	if (length == 0) 
	{
		if (max > 0) *dest = 0;
		return 0;
	} 

	for(;;) 
	{
		if (length < 0 && !*text) break;
		if (ret < max) 
		{
			 *dest = XUutf8CharEncode(text, length);
			  dest++;
		}
		ret++;
		chlen = XUutf8CharLen(text, length);
		text += chlen;
		if (length > 0) 
		{
			length -= chlen;
			if (length < 1) break;
		}
	} 

	if (ret < max) *dest = 0;
	return ret;
}
/* }}} */

/* {{{ int XUutf8CharLen(const char* text, int max) */
int XUutf8CharLen(const char* text, int max)
{
	XUChar c1;
	XUChar c2;
	XUChar ch;

	if (max == 0 || !*text) return 1;

	c1 = (XUChar)(unsigned char)*text;
	if (max == 1 || c1 < 0xc0 || c1 > 0xfd) return 1;
	text++; 

	c2 = (XUChar)(unsigned char)*text;
	if ((c2 & 0xc0) != 0x80) return 1;
	c2 = c2 & 0x3f;
	if ((c1 & 0xe0) == 0xc0) 
	{
		if (c1 < 0xc2) return 1;
		return 2;
	}
	if (max == 2) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if ((ch & 0xc0) != 0x80) return 1;
	if ((c1 & 0xf0) == 0xe0) 
	{
		 if (c2 < 0x20 && c1 < 0xe1) return 1;
		 return 3;
	}
	if (max == 3) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if ((ch & 0xc0) != 0x80) return 1;
	if ((c1 & 0xf8) == 0xf0) 
	{
		 if (c2 < 0x10 && c1 < 0xf1) return 1;
		 return 4;
	}
	if (max == 4) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if ((ch & 0xc0) != 0x80) return 1;
	if ((c1 & 0xfc) == 0xf8) 
	{
		if (c2 < 0x08 && c1 < 0xf9) return 1;
		return 5;
	}
	if (max == 5) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if ((ch & 0xc0) != 0x80) return 1;
	if (c2 < 0x04 && c1 < 0xfd) return 1;
	return 6;
}
/* }}} */

/* {{{ XUChar XUutf8CharEncode(const char* text, int max) */
XUChar XUutf8CharEncode(const char* text, int max)
{
	XUChar c[6];

	if (max == 0 || !*text) return 0;

	c[0] = (XUChar)(unsigned char)*text;
	if (max == 1 || c[0] < 0xc0 || c[0] > 0xfd) return c[0];
	text++; 

	c[1] = (XUChar)(unsigned char)*text;
	if ((c[1] & 0xc0) != 0x80) return c[0];
	c[1] = c[1] & 0x3f;
	if ((c[0] & 0xe0) == 0xc0) 
	{
		if (c[0] < 0xc2) return c[0];
		return ((c[0] & 0x1f) << 6) + c[1];
	}
	if (max == 2) return c[0];
	text++; 

	c[2] = (XUChar)(unsigned char)*text;
	if ((c[2] & 0xc0) != 0x80) return c[0];
	c[2] = c[2] & 0x3f;
	if ((c[0] & 0xf0) == 0xe0) 
	{
		if (c[1] < 0x20 && c[0] < 0xe1) return c[0];
		return ((c[0] & 0x0f) << 12) + (c[1] << 6) + c[2];
	}
	if (max == 3) return c[0];
	text++; 

	c[3] = (XUChar)(unsigned char)*text;
	if ((c[3] & 0xc0) != 0x80) return c[0];
	c[3] = c[3] & 0x3f;
	if ((c[0] & 0xf8) == 0xf0) 
	{
		if (c[1] < 0x10 && c[0] < 0xf1) return c[0];
		return ((c[1] & 0x0f) << 12) + (c[2] << 6) + c[3];
	}
	if (max == 4) return c[0];
	text++; 

	c[4] = (XUChar)(unsigned char)*text;
	if ((c[4] & 0xc0) != 0x80) return c[0];
	c[4] = c[4] & 0x3f;
	if ((c[0] & 0xfc) == 0xf8) 
	{
		if (c[1] < 0x08 && c[0] < 0xf9) return c[0];
		return ((c[2] & 0x0f) << 12) + (c[3] << 6) + c[4];
	}
	if (max == 5) return c[0];
	text++; 

	c[5] = (XUChar)(unsigned char)*text;
	if ((c[5] & 0xc0) != 0x80) return c[0];
	c[5] = c[5] & 0x3f;
	if (c[1] < 0x04 && c[0] < 0xfd) return c[0];
	return ((c[3] & 0x0f) << 12) + (c[4] << 6) + c[5];
}
/* }}} */

/********************************************/
/* UTF8 -> Unicode  End  */
/********************************************/

/********************************************/
/* Unicode -> UTF8 Start   */
/********************************************/

/* {{{ int XUutf8Decode(char* dest, int max, const XUChar* text, int length)
 * Unicode -> UTF8
 */
int XUutf8Decode(char* dest, int max, const XUChar* text, int length)
{
	int ret = 0;
	int chlen; 

	if (length == 0) 
	{
	   if (max > 0) *dest = 0;
	   return 0;
	} 

	for(;;) 
	{
	   if (length < 0 && !*text) break;
	   chlen = XUutf8CharDecode(dest, max, *text);
	   ret += chlen;
	   if (max > 0) 
	   {
		   dest += chlen;
		   max -= chlen;
	   }
	   text++;
	   if (length > 0) 
	   {
		   length--;
		   if (length < 1) break;
	   }
	} 

	if (max > 0) *dest = 0;
	return ret;
}
/* }}} */

/* {{{ int XUutf8CharDecode(char* dest, int max, XUChar ch) */
int XUutf8CharDecode(char* dest, int max, XUChar ch)
{
	if (ch < 0x0080) 
	{
	   if (max >= 1) dest[0] = (char)ch;
	   if (max >= 2) dest[1] = '\0';
	   return 1;
	} 

	if (ch < 0x0800) 
	{
	   if (max >= 1) dest[0] = (char)(0xc0 + ((ch >> 6) & 0x001f));
	   if (max >= 2) dest[1] = (char)(0x80 + (ch & 0x003f));
	   if (max >= 3) dest[2] = '\0';
	   return 2;
	} 

	if (max >= 1) dest[0] = (char)(0xe0 + ((ch >> 12) & 0x000f));
	if (max >= 2) dest[1] = (char)(0x80 + ((ch >>  6) & 0x003f));
	if (max >= 3) dest[2] = (char)(0x80 + (ch & 0x003f));
	if (max >= 4) dest[3] = '\0';
	return 3;
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
