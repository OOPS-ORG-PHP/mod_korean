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

  $Id: krcharset.c,v 1.1 2002-11-26 08:49:23 oops Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <ctype.h>
#include <math.h>

#include "php.h"
#include "php_ini.h"
#include "php_krcharset.h"
#include "php_krcheck.h"
#include "krregex.h"
#include "SAPI.h"

/* include charset */
#include "charset/iso8859.h"
#include "charset/koi8r.h"
#include "charset/ksc5601.h"
#include "charset/jis0208.h"
#include "charset/gb2312.h"
#include "charset/big5.h"
#include "cp949_table.h"
#include "unicode_cp949_ncr_table.h"

static XUChar table_rev_latin2 [ 1024];
static XUChar table_rev_koi8r_1[ 1024];
static XUChar table_rev_koi8r_2[ 1024];
static XUChar table_rev_iso8859[65536];
static XUChar table_rev_jis0208[65536];
static XUChar table_rev_ksc5601[65536];
static XUChar table_rev_gb2312 [65536];
static XUChar table_rev_big5   [65536];
static const char* xu_lang = NULL;
static char xu_language[3] = {0, 0, 0};
static int  xu_locale_encoding = XU_CONV_NONE;
static int  XUINITTABLE_CHECK = 0;

/* {{{ proto string ncrencode_lib (string str [, int type])
   Return ncr code from euc-kr */
PHP_FUNCTION(ncrencode_lib)
{
	pval **arg1, **arg2;
	int argc;
	unsigned int type;
	unsigned char *string;

	argc = ZEND_NUM_ARGS();

	switch(argc)
   	{
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(arg2);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (argc > 1 && Z_LVAL_PP(arg2) == 1) { type = 1; }
	else { type = 0; }

	convert_to_string_ex(arg1);

	if (strlen(Z_STRVAL_PP(arg1)) > 0)
   	{
		string = krNcrEncode(Z_STRVAL_PP(arg1), type);
		RETURN_STRING(string, 1);
	}
   	else { RETURN_EMPTY_STRING(); }
}

/* }}} */

/* {{{ proto string ncrencode_lib (string str [, int type])
   Return ncr code from euc-kr */
PHP_FUNCTION(ncrdecode_lib)
{
	pval **arg1;
	int argc;

	argc = ZEND_NUM_ARGS();

	switch(argc)
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

	if (strlen(Z_STRVAL_PP(arg1)) > 0)
   	{
		RETURN_STRING(krNcrDecode(Z_STRVAL_PP(arg1)), 1);
	}
   	else { RETURN_EMPTY_STRING(); }
}

/* }}} */

/* {{{ proto string uniencode_lib (string str [, string start, string end] )
   Return string that convert to unicode from euc-kr or cp949 */
PHP_FUNCTION(uniencode_lib)
{
	pval **arg1, **arg2, **arg3;
	int argc;
	unsigned char *str, *start, *end, *string;

	argc = ZEND_NUM_ARGS();

	switch(argc)
   	{
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg2);
			break;
		case 3:
			if(zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg2);
			convert_to_string_ex(arg3);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	str  = Z_STRVAL_PP(arg1);

	if (strlen(str) == 0)
   	{
		php_error(E_ERROR, "Can't use null value of argument 1");
		RETURN_FALSE;
	}

	if (argc < 2)
   	{
		start = "\\U";
		end   = "";
	}
   	else if (argc == 2)
   	{
		start = Z_STRVAL_PP(arg2);
		end   = "";
	}
   	else
   	{
		start = Z_STRVAL_PP(arg2);
		end   = Z_STRVAL_PP(arg3);
	}

	string = uniConv(str, 0, 0, start, end);

	if ( string == NULL ) { RETURN_FALSE; }
	RETURN_STRING(string,1);
}

/* }}} */

/* {{{ proto string unidecode_lib (string str , string to_charset [, string start, string end] )
   Return string to convert to cp949 or euc-kr from unicode */
PHP_FUNCTION(unidecode_lib)
{
	pval **arg1, **arg2, **arg3, **arg4;
	int argc;
	unsigned int type, subtype;
	unsigned char *str, *to, *start, *end, *string;

	argc = ZEND_NUM_ARGS();

	switch(argc)
   	{
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg3);
			break;
		case 4:
			if(zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg3);
			convert_to_string_ex(arg4);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);
	convert_to_string_ex(arg2);

	str  = Z_STRVAL_PP(arg1);
	to   = Z_STRVAL_PP(arg2);

	if (strlen(str) == 0 || strlen(to) == 0)
   	{
		php_error(E_ERROR, "Can't use null value of argument 1");
		RETURN_FALSE;
	}

	if (strcasecmp(to, "euc-kr") && strcasecmp(to, "euc_kr") && strcasecmp(to, "cp949"))
   	{
		php_error(E_ERROR, "Unknown encoding \"%s\"", to);
		RETURN_FALSE;
	}

	if (!strcasecmp(to, "euc-kr") || !strcasecmp(to, "euc_kr"))
   	{
		type = 1;
		subtype = 1;
	}
   	else if (!strcasecmp(to,"cp949"))
   	{
		type = 1;
		subtype = 0;
	}
   	else
   	{
		type = 1;
		subtype = 0;
	}

	if (argc < 3)
   	{
		start = "\\U";
		end   = "";
	}
   	else if (argc == 3)
   	{
		start = Z_STRVAL_PP(arg3);
		end   = "";
	}
   	else
   	{
		start = Z_STRVAL_PP(arg3);
		end   = Z_STRVAL_PP(arg4);
	}
	string = uniConv(str, type, subtype, start, end);

	if ( string == NULL ) { RETURN_FALSE; }
	RETURN_STRING(string,1);
}

/* }}} */

/* {{{ proto string utf8encode_lib(string str)
   Return utf8 string from euc-kr or cp949 */
PHP_FUNCTION(utf8encode_lib)
{
	pval **str;
	unsigned char *string;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &str) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(str);

			if(Z_STRLEN_PP(str) < 1)
		   	{
				php_error(E_ERROR, "Can't use null value of argument");
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	string = convUTF8 (Z_STRVAL_PP(str), 0);

	if ( string == NULL ) { RETURN_FALSE; }
	RETURN_STRING(string,1);
}
/* }}} */

/* {{{ proto string utf8decode_lib(string str, string type)
   Return euc-kr or cp949 or unicode string from utf8 */
PHP_FUNCTION(utf8decode_lib)
{
	pval **arg1, **arg2;
	int chk;
	unsigned char *str, *type, *string;

	switch(ZEND_NUM_ARGS())
   	{
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg1);
			convert_to_string_ex(arg2);
			str  = Z_STRVAL_PP(arg1);
			type = Z_STRVAL_PP(arg2);

			if (strlen(str) == 0 || strlen(type) == 0)
		   	{
				php_error(E_ERROR, "Can't use null value of argument 1 or 2");
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if ( strcasecmp(type, "euc-kr") == 0 ) { chk = 1; }
   	else if (strcasecmp(type, "euc_kr") == 0 ) { chk = 1; }
   	else if (strcasecmp(type, "cp949") == 0 ) { chk = 2; }
   	else if (strcasecmp(type, "ncr") == 0 ) { chk = 3; }
   	else if (strcasecmp(type, "unicode") == 0 ) { chk = 4; }
   	else
   	{
		php_error(E_ERROR, "Unsupported charactor set : %s", type);
		RETURN_FALSE;
	}

	string = convUTF8 (str, chk);

	if ( string != NULL ) { RETURN_STRING(string, 1); }
	RETURN_FALSE;
}
/* }}} */

/* {{{ unsigned char *krNcrEncode (unsigned char *str_o, int type)
 * convert euc-kr to ncr code, or convert outside EUC-KR range to ncr code
 * unsigned chart *str_o => EUC-KR/CP949 string
 * int type              => convert whole string(0) or outside EUC-KR range(1)
 */
unsigned char *krNcrEncode (unsigned char *str_o, int type)
{
	unsigned long i;
	unsigned int ncr;
	size_t len;
	unsigned char rc[9], *strs;
	unsigned char *ret = NULL;

	if ( str_o == NULL ) { return NULL; }
	else { len = strlen(str_o); }

	for(i=0;i<len;i++)
   	{
		/* if 2byte charactor */
		if (str_o[i] & 0x80)
	   	{
			switch(type)
		   	{
				/* if type 1, check range of KSX 1001 */
				case 1:
					if((str_o[i] >= 0x81 && str_o[i] <= 0xa0 && str_o[i+1] >= 0x41 && str_o[i+1] <=0xfe) ||
					   (str_o[i] >= 0xa1 && str_o[i] <= 0xc6 && str_o[i+1] >= 0x41 && str_o[i+1] <=0xa0))
				   	{
						ncr = getNcrIDX(str_o[i], str_o[i+1]);
						sprintf(rc, "&#%d;\0", uni_cp949_ncr_table[ncr]);
						i++;
					}
				   	else
				   	{
						memset(rc, str_o[i], 1);
						memset(rc + 1, '\0', 1);
					}

					break;
				/* range of whole string */
				default:
					ncr = getNcrIDX(str_o[i], str_o[i+1]);
					sprintf(rc, "&#%d;\0", uni_cp949_ncr_table[ncr]);
					i++;
			}
		}
		/* 1 byte charactor */
		else
	   	{
			memset(rc, str_o[i], 1);
			memset(rc + 1, '\0', 1);
	   	}

		if (strlen(rc) != 0)
	   	{
			unsigned int rc_len = strlen(rc);
			if (ret != NULL)
		   	{
				unsigned ret_len = strlen(ret);
				ret = erealloc(ret,sizeof(char) * (ret_len + rc_len + 1));
				memmove(ret + ret_len, rc, rc_len);
				memset(ret + ret_len + rc_len, '\0', 1);
			}
		   	else
		   	{
				ret = erealloc(NULL,sizeof(char) * (rc_len + 1));
				memmove(ret, rc, rc_len);
				memset(ret + rc_len, '\0', 1);
			}
		}
	}

	strs = (unsigned char *) estrndup(ret, strlen(ret));
	efree(ret);
	return strs;
}
/* }}} */

/* {{{ unsigned char *krNcrDecode (unsigned char *str_o) */
unsigned char *krNcrDecode (unsigned char *str_o)
{
	unsigned int slen, i = 0, tmp, first, second;
	unsigned char *ret = NULL, rc[3], tmpstr[8], *strs;

	if ( str_o == NULL ) { return NULL; }
	else { slen = strlen(str_o); }

	for (i=0; i<slen; i++)
	{
		if (str_o[i] == '&' && str_o[i+1] == '#' && str_o[i+7] == ';')
		{
			sprintf(tmpstr, "%c%c%c%c%c", str_o[i+2], str_o[i+3], str_o[i+4], str_o[i+5], str_o[i+6]);
			tmp = atoi(tmpstr);

			tmp = getNcrArrayNo(tmp);

			first = tmp >> 8;
			second = tmp & 0x00FF;

			/* if converted charactor is first byte of 2byte charactor */
			if ( first & 0x80 )
			{
				memset(rc, first, 1);
				memset(rc + 1, second, 1);
				memset(rc + 2, '\0', 1);
				i += 7;
			}
			else
			{
				memset(rc, str_o[i], 1);
				memset(rc + 1, '\0', 1);
			}
		}
		else
		{
			memset(rc, str_o[i], 1);
			memset(rc + 1, '\0', 1);
		}

		if (strlen(rc) != 0)
		{
			unsigned int rc_len = strlen(rc);
			if (ret != NULL)
			{
				unsigned ret_len = strlen(ret);
				ret = erealloc(ret,sizeof(char) * (ret_len + rc_len + 1));
				memmove(ret + ret_len, rc, rc_len);
				memset(ret + ret_len + rc_len, '\0', 1);
			}
			else
			{
				ret = erealloc(NULL,sizeof(char) * (rc_len + 1));
				memmove(ret, rc, rc_len);
				memset(ret + rc_len, '\0', 1);
			}
		}
	}

	strs = (unsigned char *) estrndup(ret, strlen(ret));
	efree(ret);
	return strs;
}
/* }}} */

/* {{{ unsigned char *uniConv (unsigned char *str_o, int type, int subtype, unsigned char *start, unsigned char *end)
 * Convert EUC-KR/CP940 to unicode
 * unsigned char *str_o   => convert string (euc-kr, cp949, unicode)
 * int type               => 0: convert euc-kr,cp949 -> unicode
 *                         1: convert unicode -> ecu-kr,cp949
 *                         2: convert cp949 -> euc-kr (same as krNcrEncode(str_o,1))
 * int subtype          => if value of type set 1, 1 is euc-kr and 0 is cp949
 * unsigned char *start => front string of hex value of unicode (ex U+AC60; => U+)
 * unsigned char *end   => after string of hex value of unicode (ex U+AC60; => ; )
 */
unsigned char *uniConv (unsigned char *str_o, int type, int subtype, unsigned char *start, unsigned char *end)
{
	unsigned long i;
	unsigned int ncr;
	size_t len;
	unsigned char rc[256], *strs;
	unsigned char *ret = NULL;

	int regno,hexv,firsti,secondi;
	long slen = strlen(start);
	long elen = strlen(end);
	regex_t preg;
	unsigned char regex[12] = "[0-9a-f]{4}";
	unsigned char chkReg[5], conv[5], first[3], second[3];

	if ( str_o == NULL ) { return NULL; }
	else { len = strlen(str_o); }

	if (slen > 10 || elen > 10)
	{
		php_error(E_ERROR,"Can't use string over 10 charactors <br />\n" \
				          "on unicode start string or end string");
	}

	if (type == 1)
   	{
		regno = regcomp(&preg, regex, REG_EXTENDED|REG_ICASE);

		if (regno != 0)
	   	{
			php_error(E_WARNING, "Problem in Unicode start charactors or end charactocs");
			return str_o;
		}
	}

	for (i=0; i<len; i++)
   	{
		switch(type)
	   	{
			/* convert to euc-kr/cp949 from unicode */
			/* unicode is constructed start charactors and hex code and end charactors */
			case 1:
				/* make regex check value */
				memset(chkReg, str_o[i+slen], 1);
				memset(chkReg+1, str_o[i+slen+1], 1);
				memset(chkReg+2, str_o[i+slen+2], 1);
				memset(chkReg+3, str_o[i+slen+3], 1);
				memset(chkReg+4, '\0', 1);

				if(!strncmp(&str_o[i], start, slen) && regexec(&preg,chkReg, 0, NULL, 0) == 0 &&
				   !strncmp(&str_o[i+slen+4], end, elen))
			   	{
					hexv = hex2dec(chkReg, 0);
					sprintf(conv, "%x", getUniIDX(hexv));

					/* make first byte */
					memset (first, conv[0], 1);
					memset (first + 1, conv[1], 1);
					memset (first + 2, '\0', 1);
					firsti = hex2dec(first, 1);

					/* make second byte */
					memset (second, conv[2], 1);
					memset (second + 1, conv[3], 1);
					memset (second + 2, '\0', 1);
					secondi = hex2dec(second, 1);

					/* make complete 2byte charactor */
					memset (rc, firsti, 1);
					memset (rc + 1, secondi, 1);
					memset (rc + 2, '\0', 1);

					/* convert ncr code with outsize of EUC-KR range */
					if (subtype == 1)
				   	{
						if((rc[0] >= 0x81 && rc[0] <= 0xa0 && rc[1] >= 0x41 && rc[1] <=0xfe) ||
						   (rc[0] >= 0xa1 && rc[0] <= 0xc6 && rc[1] >= 0x41 && rc[1] <=0xa0))
					   	{
							ncr = getNcrIDX(rc[0], rc[1]);
							sprintf(rc, "&#%d;\0", uni_cp949_ncr_table[ncr]);
						}
					}

					i = i + 3 + slen+elen;
				}
			   	else
			   	{
					memset (rc, str_o[i], 1);
					memset (rc + 1, '\0', 1);
				}

				break;
			/* convert to unicode from euc-kr/cp949 */
			default:
				/* if 2byte charactor */
				if (str_o[i] & 0x80)
			   	{
					ncr = getNcrIDX(str_o[i], str_o[i+1]);
					sprintf(rc,"%s%X%s\0", start, uni_cp949_ncr_table[ncr], end);
					i++;
				}
			   	else
				{
					memset (rc, str_o[i], 1);
					memset (rc + 1, '\0', 1);
			   	}
		}

		if (strlen(rc) != 0)
	   	{
			unsigned int rclen = strlen(rc);
			if(ret != NULL)
		   	{
				unsigned int retlen = strlen(ret);
				ret = erealloc(ret,(sizeof(char) * (retlen + rclen + 1)));
				memmove(ret + retlen, rc, rclen);
				memset(ret + retlen + rclen, '\0', 1);
			}
		   	else
		   	{
				ret = erealloc(NULL,sizeof(char) * (rclen + 1));
				memmove(ret, rc, rclen);
				memset(ret + rclen, '\0', 1);
			}
		}
	}

	strs = (unsigned char *) estrndup(ret, strlen(ret));
	if (type == 1) { regfree(&preg); }
	efree(ret);
	return strs;
}
/* }}} */

/* {{{ unsigned char *convUTF8(unsigned char *str_o, int type)
 * convert utf8 string */
unsigned char *convUTF8(unsigned char *str_o, int type)
{
	unsigned long i = 0, start = 0;
	size_t len;
	int ncr;
	unsigned char rc[8], *strs, *ret = NULL;

	if ( str_o == NULL ) { return NULL; }
	else { len = strlen(str_o); }

	switch(type)
   	{
		/* utf8 -> euc-kr */
		case 1:
			ret = uniConv(convUTF8(str_o, 4), 1, 1, "U+", ";");
			break;

		/* utf8 -> cp949 */
		case 2:
			ret = uniConv(convUTF8(str_o, 4), 1, 0, "U+", ";");
			break;

		/* utf8 -> ncr */
		case 3:
			ret = krNcrEncode(convUTF8(str_o, 1), 0);
			break;

		/* utf8 -> unicode */
		case 4:
			/* if exists utf8 init charactor */
			if (str_o[0] == 0xef && str_o[1] == 0xbb && str_o[2] == 0xbf )
		   	{
				if ( len == 3 ) { return NULL; }
				else { start = 3; }
			}
			
			for( i=start ; i<len ; i++ )
		   	{
				if ( str_o[i] & 0x80 )
			   	{
					unsigned int unifirst, unisecond, unithird, uniforth;

					/*
					 * binary of utf8 is 3 byte that constructed
					 * xxxxxxxx(str_o[i]) xxxxxxxx(str_o[i+1]) xxxxxxxx(str_o[i+2]).
					 * unifirst is xxxx[xxxx] of str_o[i].
					 * unisecond is xx[xxxx]xx str_o[i+1].
					 * unithird is xxxxxx[xx] str_o[i] and xx[xx]xxxx of str_o[i+1].
					 * uniforth is xxxx[xxxx] str_o[i+1].
					 */
					unifirst  = 0x0F & str_o[i];
					unisecond = ((str_o[i+1] >> 2) & 0x0F);
					unithird  = ((str_o[i+1] & 0x03) << 2) + ((str_o[i+2] >> 4) & 0x03);
					uniforth  = str_o[i+2] & 0x0F;
					sprintf(rc, "U+%x%x%x%x;", unifirst, unisecond, unithird, uniforth);

					i += 2;
				}
			   	else
			   	{
					memset(rc, str_o[i], 1);
					memset(rc + 1 , '\0', 1);
			   	}

				if (strlen(rc) != 0)
			   	{
					unsigned int rclen = strlen(rc);
					if (ret != NULL)
				   	{
						unsigned int retlen = strlen(ret);
						ret = (unsigned char *) erealloc(ret, sizeof(char) * (retlen + rclen + 1));
						memmove (ret + retlen, rc, rclen);
						memset (ret + retlen + rclen, '\0', 1);
					}
				   	else
				   	{
					   	ret = (unsigned char *) estrdup(rc);
				   	}
				}
			}
			break;

		/* 2 byte charactor -> utf8 */
		default:
			for(i=0 ; i<len ; i++)
		   	{
				if ( str_o[i] & 0x80 )
			   	{
					unsigned int firstbyte, secondbyte, thirdbyte;
					ncr = getNcrIDX(str_o[i], str_o[i+1]);
					ncr = uni_cp949_ncr_table[ncr];

					/* utf8 1th byte => 1000 + [xxxx] xxxx xxxx xxxx */
					firstbyte = (ncr >> 12) + 0xE0;
					/* utf8 2th byte => 10 + xxxx [xxxx xx]xx xxxx */
					secondbyte = (0x8000 + ((ncr << 2) & 0x3F00)) >> 8;
					/* utf8 3th byte => 10 + xxxx xxxx xx[xx xxxx] */
					thirdbyte = (ncr & 0x003F) | 0x80;

					memset(rc, firstbyte, 1);
					memset(rc + 1, secondbyte, 1);
					memset(rc + 2, thirdbyte, 1);
					memset(rc + 3, '\0', 1);
					i++;
				}
			   	else
				{
					memset(rc, str_o[i], 1);
					memset(rc + 1 , '\0', 1);
				}

				if (strlen(rc) != 0)
			   	{
					unsigned int rclen = strlen(rc);
					if (ret != NULL)
				   	{
						unsigned int retlen = strlen(ret);
						ret = erealloc(ret,(sizeof(char) * (retlen + rclen + 1)));
						memmove (ret + retlen, rc, rclen);
						memset (ret + retlen + rclen, '\0', 1);
					}
				   	else
				   	{
						ret = erealloc(NULL,sizeof(char) * (rclen + 1));
						memmove (ret, rc, rclen);
						memset (ret + rclen, '\0', 1);
					}
				}
			}
	}

	if ( ret == NULL ) { return NULL; }
	strs = (unsigned char *) estrdup(ret);

	if (type != 1 && type != 2 && type != 3) { efree(ret); }
	return strs;
}
/* }}} */

/* {{{ unsigned int getNcrIDX (unsigned char str1, unsigned char str2) */
unsigned int getNcrIDX (unsigned char str1, unsigned char str2)
{
	unsigned int idx, ch;

	ch = str1;
	idx = (unsigned int) (ch << 8) | (unsigned int) str2;
	idx -= 33089;

	return idx;
}
/* }}} */

/* {{{ unsigned int getNcrArrayNo (unsigned char str1, unsigned char str2) */
unsigned int getNcrArrayNo (unsigned int key)
{
	unsigned int i = 0, array_no;
	for(i=0; i<31934; i++)
	{
		if ( uni_cp949_ncr_table[i] == key )
		{
			array_no = i;
			break;
		}
	}

	return array_no + 33089;
}
/* }}} */

/* {{{ unsigned int getUniIDX (unsigned int key) */
unsigned int getUniIDX (unsigned int key)
{
	int *ptr, *chk, result;

	ptr = (int *)bsearch(&key,cp949_2byte_ncr_table, 17048, sizeof(cp949_2byte_ncr_table[0]), comp);
	chk = cp949_2byte_ncr_table;

	if (ptr != NULL)
   	{
		result = ptr - chk;
		return cp949_2byte_uni_ncr_table[result];
	}
   	else { return 0; }
}
/* }}} */

/* {{{ unsigned int hex2dec (unsigned char *str_o,unsigned int type) */
unsigned int hex2dec (unsigned char *str_o, unsigned int type) {
	int i,buf[4],len = strlen(str_o);

	for(i=0;i<len;i++)
   	{
		/* range of alphabat a -> f */
		if((str_o[i] >= 0x61 && str_o[i] <= 0x66) || (str_o[i] >= 0x41 && str_o[i] <= 0x46))
	   	{
			switch(str_o[i])
		   	{
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
		}
		else
		{
			switch(str_o[i])
		   	{
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

	switch(type)
   	{
		case 1 :
			return ((buf[0] * 16) + buf[1]);
			break;
		default :
			return ((buf[0] * 16 * 16 * 16) + (buf[1] * 16 * 16) + (buf[2] * 16) + buf[3]);
			
	}
}
/* }}} */

/* {{{ int comp(const void *s1, const void *s2) */
int comp(const void *s1, const void *s2)
{
	return (*(int *)s1 - *(int *)s2);
}
/* }}} */

/* new utf8 */
/*****************************************************************************************/
/* Code A -> Code B  Start  */
/*****************************************************************************************/
// UTF-8 -> locale code OR locale code -> UTF-8

int XUCodeConv(char* dest, int max, int codeTo, const char* text, int length, int codeFrom)
{
	XUChar* buf;
	int len1;
	int len2;

	if( XUINITTABLE_CHECK == 0 )
	{
		XUInitTable();
		XUINITTABLE_CHECK = 1;
	}

	if(length < 0) length = strlen(text);
	buf = (XUChar*)malloc(sizeof(XUChar) * length);
	len1 = XUEncode(buf, length, text, length, codeFrom);	 // other -> unicode
	len2 = XUDecode(dest, max, buf, len1, codeTo);			// unicode -> other

	free(buf);
	return len2;
}

static void XUInitTable()
{
	int i, j;
	XUChar c;
	XUChar c1;
	XUChar c2; 

	for(i = 0; i < 1024; i++) 
	{
	   table_rev_latin2 [i] = 0;
	   table_rev_koi8r_1[i] = 0;
	   table_rev_koi8r_2[i] = 0;
	} 

	for(i = 0; i < 128; i++) 
	{
	   c = table_iso8859[1].data[i];
	   if(c) table_rev_latin2[c] = i + 0x0080;
	} 
	
	for(i = 0; i < 128; i++) 
	{
		c = table_koi8r[i];
		if(0x0080 <= c && c < 0x0480) 
			table_rev_koi8r_1[c - 0x0080] = i + 0x0080;
		else if(0x2200 <= c && c < 0x2600) 
			table_rev_koi8r_2[c - 0x2200] = i + 0x0080;
	} 

	for(i = 0; i < 65536; i++) table_rev_iso8859[i] = 0;
	for(i = 0; i < 65536; i++) table_rev_jis0208[i] = 0;
	for(i = 0; i < 65536; i++) table_rev_ksc5601[i] = 0;
	for(i = 0; i < 65536; i++) table_rev_gb2312 [i] = 0;
	for(i = 0; i < 65536; i++) table_rev_big5   [i] = 0; 

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
		if(c) table_rev_jis0208[c] = (c1 << 8) + c2;
		c2++;
		if(c2 == 0x7f) 
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
	   if(c) table_rev_gb2312[c] = (c1 << 8) + c2;
	   c2++;
	   if(c2 == 0x7f) 
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
	   if(c) table_rev_big5[c] = (c1 << 8) + c2;
	   c2++;
	   if(c2 == 0x7f) 
		   c2 = 0xa1;
	   else if(c2 == 0xff) 
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
	   if(c) table_rev_ksc5601[c] = (c1 << 8) + c2;
	   c2++;
	   if(c2 == 0x5b) 
		  c2 = 0x61;
	   else if(c2 == 0x7b) 
		  c2 = 0x81;
	   else if(c2 == 0xff) 
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
	   if(c) table_rev_ksc5601[c] = (c1 << 8) + c2;
	   c2++;
	   if(c2 == 0xff) 
	   {
		   c2 = 0xa1;
		   c1++;
	   }
	}
}


/*
 *   Other -> Unicode(UCS-2)
 */

int XUEncode(XUChar* dest, int max, const char* text, int length, int code)
{
	int ret = 0;
	int chlen; 
	
	if(length == 0) 
	{
		if(max > 0) *dest = 0;
		return 0;
	}

	for(;;) 
	{
		if(length < 0 && !*text) break;
		if(ret < max) 
		{
		   *dest = XUCharEncode(text, length, code);
		   dest++;
		}
		ret++;
		chlen = XUCharLen(text, length, code);
		text += chlen;
		if(length > 0) 
		{
			length -= chlen;
			if(length < 1) break;
		}
	} 

	if(ret < max) *dest = 0;
		   return ret;
}

/**
 *   Other -> Unicode
 *   */

XUChar XUCharEncode(const char* text, int max, int code)
{
	XUChar ret = 0;
	XUChar c1;
	XUChar c2;
	XUChar num; 

	if(max == 0 || !*text) return 0; 

	if(code == XU_CONV_LOCALE) code = xu_locale_encoding;

	if(code <= XU_CONV_NONE) return '?';
		   if(code == XU_CONV_UTF8) return XUutf8CharEncode(text, max); 

	c1 = (XUChar)(unsigned char)*text;
		   if(c1 < 0x80) return c1;

	if(code <= XU_CONV_ISO8859(15)) 
	{
		ret = table_iso8859[code - XU_CONV_ISO8859(1)].data[c1 - 0x80];
		if(ret) return ret;
		return '?';
	} 
	else if(code == XU_CONV_KOI8R) 
	{
		ret = table_koi8r[c1 - 0x80];
		if(ret) return ret;
		return '?';
	}

	if(code == XU_CONV_SJIS) 
	{
		if(0xa1 <= c1 && c1 <= 0xdf) return c1 + 0xfec0;
	}
	if(max == 1) return '?';
	text++; 

	c2 = (XUChar)(unsigned char)*text;
	if(!c2) return '?';
	if(code == XU_CONV_EUCJP) 
	{
		if(c1 == 0x8e) return c2 + 0xfec0;
		if(c1 < 0xa1 || c1 > 0xf4) return '?';
		if(c2 < 0xa1 || c2 > 0xfe) return '?';
		ret = table_jis0208[(c1 - 0xa1) * 94 + (c2 - 0xa1)];
	} 
	else if(code == XU_CONV_SJIS) 
	{
		if(c1 == 0x80 || c1 == 0xa0 || c1 > 0xfc) return '?';
		if(c2  < 0x40 || c2 == 0x7f || c2 > 0xfc) return '?';
		if(c1 >= 0xe0) c1 -= 0x40;
		if(c2 > 0x7e) c2--;
		num = (c1 - 0x81) * 188 + (c2 - 0x40);
		if(num < 7896) ret = table_jis0208[num];
	} 
	else if(code == XU_CONV_EUCKR) 
	{
		if(0x81 <= c1 && c1 <= 0xc8) 
		{
			if(c2 < 0x41 || c2 > 0xfe) return '?';
			if(0x5a < c2 && c2 < 0x61) return '?';
			if(0x7a < c2 && c2 < 0x81) return '?';
			if(c2 > 0x7a) c2 -= 6;
			if(c2 > 0x5a) c2 -= 6;
			ret = table_ksc5601[(c1 - 0x81) * 178 + (c2 - 0x41)];
		} 
		else if(0xca <= c1 && c1 <= 0xfd) 
		{
			if(c2 < 0xa1 || c2 > 0xfe) return '?';
			ret = table_ksc5601_hanja[(c1 - 0xca) * 94 + (c2 - 0xa1)];
		}
	} 
	else if(code == XU_CONV_EUCCN) 
	{
		if(c1 < 0xa1 || c1 > 0xf7) return '?';
		if(c2 < 0xa1 || c2 > 0xfe) return '?';
		ret = table_gb2312[(c1 - 0xa1) * 94 + (c2 - 0xa1)];
	} 
	else if(code == XU_CONV_BIG5) 
	{
		if(c1 < 0xa1 || c1 > 0xf9) return '?';
		if(c2 < 0x40 || c2 > 0xfe) return '?';
		if(0x7e < c2 && c2 < 0xa1) return '?';
		if(c2 > 0x7e) c2 -= 0x22;
		ret = table_big5[(c1 - 0xa1) * 157 + (c2 - 0x40)];
	}
	if(ret) return ret; 

	return '?';
}

int XUCharLen(const char* text, int max, int code)
{
	XUChar c1;
	XUChar c2; 

	if(max == 0 || !*text) return 1; 

	if(code == XU_CONV_LOCALE) code = xu_locale_encoding;
	if(code == XU_CONV_UTF8  ) return XUutf8CharLen(text, max);
	if(code <= XU_CONV_KOI8R ) return 1; 

	c1 = (XUChar)(unsigned char)*text;
	if(code == XU_CONV_SJIS) 
	{
		if(c1 < 0x80 || (0xa1 <= c1 && c1 <= 0xdf)) return 1;
	}
	if(max == 1) return 1;
	text++; 

	c2 = (XUChar)(unsigned char)*text;
	if(!c2) return 1;
	if(code == XU_CONV_EUCJP) 
	{
		if(c1 == 0x8e) return 2;
		if(c1 < 0xa1 || c1 > 0xf4) return 1;
		if(c2 < 0xa1 || c2 > 0xfe) return 1;
		return 2;
	} 
	else if(code == XU_CONV_SJIS) 
	{
		if(c1 == 0x80 || c1 == 0xa0 || c1 > 0xfc) return 1;
		if(c2  < 0x40 || c2 == 0x7f || c2 > 0xfc) return 1;
		return 2;
	} 
	else if(code == XU_CONV_EUCKR) 
	{
		if(0x81 <= c1 && c1 <= 0xc8) 
		{
			if(c2 < 0x41 || c2 > 0xfe) return 1;
			if(0x5a < c2 && c2 < 0x61) return 1;
			if(0x7a < c2 && c2 < 0x81) return 1;
			return 2;
		} 
		else if(0xca <= c1 && c1 <= 0xfd) 
		{
			if(c2 < 0xa1 || c2 > 0xfe) return 1;
			return 2;
		}
	} 
	else if(code == XU_CONV_EUCCN) 
	{
		if(c1 < 0xa1 || c1 > 0xf7) return 1;
		if(c2 < 0xa1 || c2 > 0xfe) return 1;
		return 2;
	} 
	else if(code == XU_CONV_BIG5) 
	{
		if(c1 < 0xa1 || c1 > 0xf9) return 1;
		if(c2 < 0x40 || c2 > 0xfe) return 1;
		if(0x7e < c2 && c2 < 0xa1) return 1;
		return 2;
	} 

	return 1;
}


/**
 *   Unicode(UCS-2) -> Other
 *   */

int XUDecode(char* dest, int max, const XUChar* text, int length, int code)
{
	int ret = 0;
	int chlen; 

	if(length == 0) 
	{
		if(max > 0) *dest = 0;
		return 0;
	}

	for(;;) 
	{
		if(length < 0 && !*text) break;
		chlen = XUCharDecode(dest, max, *text, code);
		ret += chlen;
		if(max > 0) 
		{
			dest += chlen;
			max -= chlen;
		}
		text++;
		if(length > 0) 
		{
			length--;
			if(length < 1) break;
		}
	} 

	if(max > 0) *dest = 0;
	return ret;
}

/**
 *   Unicode -> Other
 *   */

int XUCharDecode(char* dest, int max, XUChar ch, int code)
{
	XUChar ret = 0;
	XUChar r1;
	XUChar r2;
	XUChar i;
	XUChar* table; 

	if(code == XU_CONV_LOCALE) 
	{
		code = xu_locale_encoding;
	} 
	else if(code == XU_CONV_NONE) 
	{
		if(max >= 1) dest[0] = '?';
		return 1;
	} 
	else if(code == XU_CONV_UTF8) 
	{
		return XUutf8CharDecode(dest, max, ch);
	}	 
	if(ch < 0x0080) 
	{
		if(max >= 1) dest[0] = (char)ch;
		return 1;
	}
	if(code <= XU_CONV_ISO8859(15)) 
	{
		if(ch < 0x00a1) 
		{
			ret = ch;
		} 
		else 
		{
			table = table_iso8859[code - XU_CONV_ISO8859(1)].data;
			for(i = 0x21; i < 0x80; i++) 
			{
				if(ch == table[i]) 
				{
					ret = i + 0x80;
					break;
				}
			}
		}
		if(!ret) ret = '?';
		if(max >= 1) dest[0] = (char)ret;
		return 1;
	} 
	else if(code == XU_CONV_KOI8R) 
	{
		if(0x0080 <= ch && ch < 0x0480) 
		{
			 ret = table_rev_koi8r_1[ch - 0x0080];
		} 
		else if(0x2200 <= ch && ch < 0x2600) 
		{
			 ret = table_rev_koi8r_2[ch - 0x2200];
		}
		if(!ret) ret = '?';
		if(max >= 1) dest[0] = (char)ret;
		return 1;
	} 

	if(code == XU_CONV_EUCJP) 
	{
		if(0xff61 <= ch && ch <= 0xff9f) 
		{
			ret = (ch - 0xfec0) + 0x8e00;
		} 
		else 
		{
			ret = table_rev_jis0208[ch];
			if(ret) ret += 0x8080;
		}
	} 
	else if(code == XU_CONV_SJIS) 
	{
		if(0xff61 <= ch && ch <= 0xff9f) 
		{
			if(max >= 1) dest[0] = (char)(ch - 0xfec0);
			return 1;
		} 
		else 
		{
			 ret = table_rev_jis0208[ch];
			 r1 = (ret >> 8  ) - 0x21;
			 r2 = (ret & 0xff) - 0x21;
			 if(r1 & 1) r2 += 94;
			 r1 = (r1 >> 1) + 0x81;
			 if(r1 > 0x9f) r1 += 0x40;
			 r2 += 0x40;
			 if(r2 > 0x7e) r2++;
			 ret = (r1 << 8) + r2;
		 }
	} 
	else if(code == XU_CONV_EUCKR) 
	{
		 ret = table_rev_ksc5601[ch];
	} 
	else if(code == XU_CONV_EUCCN) 
	{
		 ret = table_rev_gb2312[ch];
		 if(ret) ret += 0x8080;
	} 
	else if(code == XU_CONV_BIG5) 
	{
		 ret = table_rev_big5[ch];
	}
	if(!ret) 
	{
		 if(max >= 1) dest[0] = '?';
		 return 1;
	} 
	
	if(max >= 2) 
	{
		dest[0] = (char)(ret >> 8);
		dest[1] = (char)(ret & 0xff);
	}
	return 2;
}


/*****************************************************************************************/
/* Code A -> Code B  End  */
/*****************************************************************************************/

/*****************************************************************************************/
/* UTF8 -> Unicode  Start  */
/*****************************************************************************************/

// utf-8 -> ucs
int XUutf8Encode(XUChar* dest, int max, const char* text, int length)
{
	int ret = 0;
	int chlen; 

	if(length == 0) 
	{
		if(max > 0) *dest = 0;
		return 0;
	} 

	for(;;) 
	{
		if(length < 0 && !*text) break;
		if(ret < max) 
		{
			 *dest = XUutf8CharEncode(text, length);
			  dest++;
		}
		ret++;
		chlen = XUutf8CharLen(text, length);
		text += chlen;
		if(length > 0) 
		{
			length -= chlen;
			if(length < 1) break;
		}
	} 

	if(ret < max) *dest = 0;
	return ret;
}

int XUutf8CharLen(const char* text, int max)
{
	XUChar c1;
	XUChar c2;
	XUChar ch;

	if(max == 0 || !*text) return 1;

	c1 = (XUChar)(unsigned char)*text;
	if(max == 1 || c1 < 0xc0 || c1 > 0xfd) return 1;
	text++; 

	c2 = (XUChar)(unsigned char)*text;
	if((c2 & 0xc0) != 0x80) return 1;
	c2 = c2 & 0x3f;
	if((c1 & 0xe0) == 0xc0) 
	{
		if(c1 < 0xc2) return 1;
		return 2;
	}
	if(max == 2) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if((ch & 0xc0) != 0x80) return 1;
	if((c1 & 0xf0) == 0xe0) 
	{
		 if(c2 < 0x20 && c1 < 0xe1) return 1;
		 return 3;
	}
	if(max == 3) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if((ch & 0xc0) != 0x80) return 1;
	if((c1 & 0xf8) == 0xf0) 
	{
		 if(c2 < 0x10 && c1 < 0xf1) return 1;
		 return 4;
	}
	if(max == 4) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if((ch & 0xc0) != 0x80) return 1;
	if((c1 & 0xfc) == 0xf8) 
	{
		if(c2 < 0x08 && c1 < 0xf9) return 1;
		return 5;
	}
	if(max == 5) return 1;
	text++; 

	ch = (XUChar)(unsigned char)*text;
	if((ch & 0xc0) != 0x80) return 1;
	if(c2 < 0x04 && c1 < 0xfd) return 1;
	return 6;
}

XUChar XUutf8CharEncode(const char* text, int max)
{
	XUChar c[6];

	if(max == 0 || !*text) return 0;

	c[0] = (XUChar)(unsigned char)*text;
	if(max == 1 || c[0] < 0xc0 || c[0] > 0xfd) return c[0];
	text++; 

	c[1] = (XUChar)(unsigned char)*text;
	if((c[1] & 0xc0) != 0x80) return c[0];
	c[1] = c[1] & 0x3f;
	if((c[0] & 0xe0) == 0xc0) 
	{
		if(c[0] < 0xc2) return c[0];
		return ((c[0] & 0x1f) << 6) + c[1];
	}
	if(max == 2) return c[0];
	text++; 

	c[2] = (XUChar)(unsigned char)*text;
	if((c[2] & 0xc0) != 0x80) return c[0];
	c[2] = c[2] & 0x3f;
	if((c[0] & 0xf0) == 0xe0) 
	{
		if(c[1] < 0x20 && c[0] < 0xe1) return c[0];
		return ((c[0] & 0x0f) << 12) + (c[1] << 6) + c[2];
	}
	if(max == 3) return c[0];
	text++; 

	c[3] = (XUChar)(unsigned char)*text;
	if((c[3] & 0xc0) != 0x80) return c[0];
	c[3] = c[3] & 0x3f;
	if((c[0] & 0xf8) == 0xf0) 
	{
		if(c[1] < 0x10 && c[0] < 0xf1) return c[0];
		return ((c[1] & 0x0f) << 12) + (c[2] << 6) + c[3];
	}
	if(max == 4) return c[0];
	text++; 

	c[4] = (XUChar)(unsigned char)*text;
	if((c[4] & 0xc0) != 0x80) return c[0];
	c[4] = c[4] & 0x3f;
	if((c[0] & 0xfc) == 0xf8) 
	{
		if(c[1] < 0x08 && c[0] < 0xf9) return c[0];
		return ((c[2] & 0x0f) << 12) + (c[3] << 6) + c[4];
	}
	if(max == 5) return c[0];
	text++; 

	c[5] = (XUChar)(unsigned char)*text;
	if((c[5] & 0xc0) != 0x80) return c[0];
	c[5] = c[5] & 0x3f;
	if(c[1] < 0x04 && c[0] < 0xfd) return c[0];
	return ((c[3] & 0x0f) << 12) + (c[4] << 6) + c[5];
}

/*****************************************************************************************/
/* UTF8 -> Unicode  End  */
/*****************************************************************************************/


/*****************************************************************************************/
/* Unicode -> UTF8 Start   */
/*****************************************************************************************/

// Unicode -> UTF8
int XUutf8Decode(char* dest, int max, const XUChar* text, int length)
{
	int ret = 0;
	int chlen; 

	if(length == 0) 
	{
	   if(max > 0) *dest = 0;
	   return 0;
	} 

	for(;;) 
	{
	   if(length < 0 && !*text) break;
	   chlen = XUutf8CharDecode(dest, max, *text);
	   ret += chlen;
	   if(max > 0) 
	   {
		   dest += chlen;
		   max -= chlen;
	   }
	   text++;
	   if(length > 0) 
	   {
		   length--;
		   if(length < 1) break;
	   }
	} 

	if(max > 0) *dest = 0;
	return ret;
}

int XUutf8CharDecode(char* dest, int max, XUChar ch)
{
	if(ch < 0x0080) 
	{
	   if(max >= 1) dest[0] = (char)ch;
	   if(max >= 2) dest[1] = '\0';
	   return 1;
	} 

	if(ch < 0x0800) 
	{
	   if(max >= 1) dest[0] = (char)(0xc0 + ((ch >> 6) & 0x001f));
	   if(max >= 2) dest[1] = (char)(0x80 + (ch & 0x003f));
	   if(max >= 3) dest[2] = '\0';
	   return 2;
	} 

	if(max >= 1) dest[0] = (char)(0xe0 + ((ch >> 12) & 0x000f));
	if(max >= 2) dest[1] = (char)(0x80 + ((ch >>  6) & 0x003f));
	if(max >= 3) dest[2] = (char)(0x80 + (ch & 0x003f));
	if(max >= 4) dest[3] = '\0';
	return 3;
}

/* 로컬 캐릭터를 UTF로 변경을 해서 값을 리턴한다. */
PHP_FUNCTION(getutf8)
{
	zval **srcstr, **characterset;
	char *utf8;
	int  utf8length,srclenth;
	int  charactersetcode=0;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &srcstr, &characterset) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(srcstr);
	convert_to_string_ex(characterset);
	if( !Z_STRLEN_PP(srcstr) || !Z_STRLEN_PP(characterset) ) 
	{
		RETURN_EMPTY_STRING();
	}

	srclenth = Z_STRLEN_PP(srcstr);
	utf8 = (char *)emalloc(srclenth*6);

	if(!strcasecmp(Z_STRVAL_PP(characterset), "EUC-KR")) charactersetcode = XU_CONV_EUCKR;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "KOR"))	charactersetcode = XU_CONV_EUCKR;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "ENG"))	charactersetcode = XU_CONV_EUCKR;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "CHI"))	charactersetcode = XU_CONV_BIG5;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "JPN"))	charactersetcode = XU_CONV_SJIS;

	XUCodeConv(utf8, srclenth*6, XU_CONV_UTF8, Z_STRVAL_PP(srcstr), srclenth, charactersetcode);
	utf8length = strlen(utf8);
	RETURN_STRINGL(utf8, utf8length, 0);
}

/*  UTF8캐릭터를 로컬케릭터셋으로 변경하는 함수 */
PHP_FUNCTION(getcharacterset)
{
	zval **srcstr, **characterset;
	char *newstr;
	int  newstrlength,srclenth;
	int  charactersetcode=0;

	if(ZEND_NUM_ARGS() != 2 || zend_get_parameters_ex(2, &srcstr, &characterset) == FAILURE) 
	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(srcstr);
	convert_to_string_ex(characterset);
	if( !Z_STRLEN_PP(srcstr) || !Z_STRLEN_PP(characterset) ) 
	{
		RETURN_EMPTY_STRING();
	}

	srclenth = Z_STRLEN_PP(srcstr);
	newstr = (char *)emalloc(srclenth*6);

	if(!strcasecmp(Z_STRVAL_PP(characterset), "EUC-KR")) charactersetcode = XU_CONV_EUCKR;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "KOR"))	charactersetcode = XU_CONV_EUCKR;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "ENG"))	charactersetcode = XU_CONV_EUCKR;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "CHI"))	charactersetcode = XU_CONV_BIG5;
	if(!strcasecmp(Z_STRVAL_PP(characterset), "JPN"))	charactersetcode = XU_CONV_SJIS;

	XUCodeConv(newstr, srclenth*6, charactersetcode, Z_STRVAL_PP(srcstr), srclenth, XU_CONV_UTF8);
	newstrlength = strlen(newstr);
	RETURN_STRINGL(newstr, newstrlength, 0);
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
