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

  $Id: krparse.c,v 1.41 2002-09-18 10:47:19 oops Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_krparse.h"
#include "php_krcheck.h"
#include "krregex.h"
#include "SAPI.h"

#include "cp949_table.h"
#include "unicode_cp949_ncr_table.h"

#include <math.h>

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

/* {{{ proto string autolink_lib(string str)
 *   parse url in string */
PHP_FUNCTION(autolink_lib)
{
	pval **arg1;
	unsigned char *ret;

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
	ret = estrdup(autoLink(Z_STRVAL_PP(arg1)));

	if (ret)
   	{
		RETURN_STRING(ret, 1);
	}
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string substr_lib(string str, int start [, int length])
 *    Returns part of a multibyte string */
PHP_FUNCTION(substr_lib)
{
	zval **str, **from, **len, **utf8;
	unsigned char *cstr, *tmpstr, *string;
	int l, f, lenth, utf = 0;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 4 || zend_get_parameters_ex(argc, &str, &from, &len, &utf8) == FAILURE)
   	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_long_ex(from);

	if (argc == 4)
   	{
		convert_to_long_ex(utf8);
		utf = Z_LVAL_PP(utf8);
		tmpstr = convUTF8 (Z_STRVAL_PP(str), 2);
	}
   	else
	{
		cstr = krNcrDecode(Z_STRVAL_PP(str));
		tmpstr = cstr;
	}

	if (argc > 2)
   	{
		convert_to_long_ex(len);
		l = Z_LVAL_PP(len);
	}
   	else { l = strlen(tmpstr); }

	f = Z_LVAL_PP(from);
	lenth = strlen(tmpstr);

	/* if "from" position is negative, count start position from the end
	 * of the string */
	if (f < 0)
   	{
		f = lenth + f;
		if (f < 0) { f = 0; }
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string */
	if (l < 0)
   	{
		l = (lenth - f) + l;
		if (l < 0) { l = 0; }
	}

	if (f >= lenth) { RETURN_FALSE; }
	if((f + l) > lenth) { l = lenth - f; }

	/* check multibyte whether start return charactor */
	if(multibyte_check(tmpstr, f))
   	{
		f++;
		l--;
	} 

	/* check multibyte whether last return charactor */
	if(multibyte_check(tmpstr, f + l)) { l++; }

	tmpstr[f+l] = '\0';
	string = &tmpstr[f];
	if (utf == 1)
   	{
		RETURN_STRING(convUTF8(string, 0), 1);
	}
   	else
   	{
		RETURN_STRING(krNcrEncode(string, 1), 1);
	}
}
/* }}} */

/* {{{ proto array agentinfo_lib(void)
 *    Returns info of browser */
PHP_FUNCTION(agentinfo_lib)
{
	unsigned char *agent_o, *agent_v, *buf;

	agent_o = get_useragent();
	if ( !agent_o ) { RETURN_FALSE; }

	if (array_init(return_value) == FAILURE)
   	{
		php_error(E_WARNING, "Failed init array");
		RETURN_FALSE;
	}

	/* if Explorer */
	if (strstr(agent_o, "MSIE"))
   	{
		add_assoc_string(return_value, "br", "MSIE", 1);
		add_assoc_string(return_value, "co", "msie", 1);

		/* get user os */
		if (strstr(agent_o,"NT"))
	   	{
			add_assoc_string(return_value, "os", "NT", 1);
		}
	   	else if
		   	(strstr(agent_o,"Win")) {
			add_assoc_string(return_value, "os", "WIN", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = estrdup((unsigned char *) kr_regex_replace("/Mo.+MSIE ([^;]+);.+/i", "\\1", agent_o));
		agent_v = estrdup((unsigned char *) kr_regex_replace("[a-z]", "", buf));
		add_assoc_string(return_value, "vr", agent_v, 1);
	}
	
	/* if Opera */
	else if (strstr(agent_o, "Opera"))
   	{
		add_assoc_string(return_value, "br", "OPERA", 1);
		add_assoc_string(return_value, "co", "msie", 1);

		/* get user os */
		if (strstr(agent_o, "Linux"))
	   	{
			add_assoc_string(return_value, "os", "LINUX", 1);
		}
	   	else if (strstr(agent_o, "2000") || strstr(agent_o, "XP"))
	   	{
			add_assoc_string(return_value, "os", "NT", 1);
		}
	   	else if (strstr(agent_o, "Win"))
	   	{
			add_assoc_string(return_value, "os", "WIN", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = estrdup((unsigned char *) kr_regex_replace("/Opera\\/([0-9.]+).*/i","\\1", agent_o));
		add_assoc_string(return_value, "vr", buf, 1);

		/* get language */
		if (strstr(agent_o, "[ko]"))
	   	{
			add_assoc_string(return_value, "ln", "ko", 1);
		}
	   	else if (strstr(agent_o, "[en]"))
	   	{
			add_assoc_string(return_value, "ln", "en", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "ln", "other", 1);
		}
	}

	/* if Mozilla */
	else if ((strstr(agent_o,"Gecko") || strstr(agent_o,"Galeon")) && !strstr(agent_o,"Netscape"))
   	{
		add_assoc_string(return_value, "br", "MOZL", 1);
		add_assoc_string(return_value, "co", "mozilla", 1);

		/* get user os */
		if (strstr(agent_o, "NT"))
	   	{
			add_assoc_string(return_value, "os", "NT", 1);
		}
	   	else if (strstr(agent_o, "Win"))
	   	{
			add_assoc_string(return_value, "os", "WIN", 1);
		}
	   	else if (strstr(agent_o, "Linux"))
	   	{
			add_assoc_string(return_value, "os", "LINUX", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = estrdup((unsigned char *) kr_regex_replace("/Mozi[^(]+\\([^;]+;[^;]+;[^;]+;[^;]+;([^)]+)\\).*/i","\\1", agent_o));
		agent_v = estrdup((unsigned char *) kr_regex_replace("/rv:| /i", "", buf));
		add_assoc_string(return_value, "vr", agent_v, 1);

		/* get language */
		if (strstr(agent_o, "en-US"))
	   	{
			add_assoc_string(return_value, "ln", "en", 1);
		}
	   	else if (strstr(agent_o, "ko-KR"))
	   	{
			add_assoc_string(return_value, "ln", "ko", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "ln", "other", 1);
		}
	}

	/* if Konqueror */
	else if (strstr(agent_o,"Konqueror"))
   	{
		add_assoc_string(return_value, "br", "KONQ", 1);
		add_assoc_string(return_value, "co", "mozilla", 1);

		/* get user os */
		if (strstr(agent_o, "Linux"))
	   	{
			add_assoc_string(return_value, "os", "LINUX", 1);
		}
	   	else if (strstr(agent_o, "FreeBSD"))
	   	{
			add_assoc_string(return_value, "os", "FreeBSD", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = estrdup((unsigned char *) kr_regex_replace("/.*Konqueror\\/([0-9.]+).*/i","\\1", agent_o));
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if Lynx */
	else if (strstr(agent_o, "Lynx"))
   	{
		add_assoc_string(return_value, "br", "LYNX", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get version */
		buf = estrdup((unsigned char *) kr_regex_replace("/Lynx\\/([^ ]+).*/i","\\1", agent_o));
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if w3M */
	else if (strstr(agent_o, "w3m"))
   	{
		add_assoc_string(return_value, "br", "W3M", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get version */
		buf = estrdup((unsigned char *) kr_regex_replace("/w3m\\/([0-9.]+).*/i","\\1", agent_o));
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if LINKS */
	else if (strstr(agent_o, "Links"))
   	{
		add_assoc_string(return_value, "br", "LINKS", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get user os */
		if (strstr(agent_o, "Linux"))
	   	{
			add_assoc_string(return_value, "os", "LINUX", 1);
		}
	   	else if (strstr(agent_o, "FreeBSD"))
	   	{
			add_assoc_string(return_value, "os", "FreeBSD", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = estrdup((unsigned char *) kr_regex_replace("/Links \\(([^;]+);.*/i","\\1", agent_o));
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if Netscape */
	else if (strstr(agent_o, "Mozilla"))
   	{
		add_assoc_string(return_value, "br", "NS", 1);
		add_assoc_string(return_value, "co", "mozilla", 1);

		/* get user os */
		if (strstr(agent_o, "NT"))
	   	{
			add_assoc_string(return_value, "os", "NT", 1);
		}
	   	else if (strstr(agent_o, "Win"))
	   	{
			add_assoc_string(return_value, "os", "WIN", 1);
		}
	   	else if (strstr(agent_o, "Linux"))
	   	{
			add_assoc_string(return_value, "os", "LINUX", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get language */
		if (strstr(agent_o, "[ko]"))
	   	{
			add_assoc_string(return_value, "ln", "ko", 1);
		}
	   	else if (strstr(agent_o, "[ko]"))
	   	{
			add_assoc_string(return_value, "ln", "en", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "ln", "other", 1);
		}

		/* get version */
		if (strstr(agent_o, "Gecko"))
	   	{
			add_assoc_string(return_value, "vr", "6", 1);
		}
	   	else
	   	{
			add_assoc_string(return_value, "vr", "4", 1);
		}
	}

	/* other browser */
	else
   	{
		add_assoc_string(return_value, "br", "OTHER", 1);
		add_assoc_string(return_value, "co", "OTHER", 1);
	}
}
/* }}} */

/* {{{ proto string postposition_lib(string str, string postposition, int utf)
 *   make a decision about kreaon postposition */
PHP_FUNCTION(postposition_lib)
{
	pval **string, **posts, **utf;
	unsigned char *str, *post, *josa, *chkjosa[2];
	unsigned char *chkstr;
	int position, chkutf;

	switch(ZEND_NUM_ARGS())
   	{
		case 2:
			if(zend_get_parameters_ex(2, &string, &posts) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(3, &string, &posts, &utf) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(utf);
			chkutf = Z_LVAL_PP(utf);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(string);
	convert_to_string_ex(posts);

	if (chkutf == 1)
	{
		str = convUTF8( Z_STRVAL_PP(string), 2);
		josa = convUTF8( Z_STRVAL_PP(posts), 2);
	}
	else
	{
		str = Z_STRVAL_PP(string);
		josa = Z_STRVAL_PP(posts);
	}

	/* check korean postposition */
	if ( strlen(josa) < 1 )
	{
		php_error(E_ERROR, "Don't exists postposition\n", josa);
	}	
	else if ( (josa[0] == 0xc0 && josa[1] == 0xcc) || (josa[0] == 0xb0 && josa[1] == 0xa1) )
	{
		chkjosa[0] = "이";
		chkjosa[1] = "가";
	}
	else if ( (josa[0] == 0xc0 && josa[1] == 0xba) || (josa[0] == 0xb4 && josa[1] == 0xc2) )
	{
		chkjosa[0] = "은";
		chkjosa[1] = "는";
	}
	else if ( (josa[0] == 0xc0 && josa[1] == 0xbb) || (josa[0] == 0xb8 && josa[1] == 0xa6) )
	{
		chkjosa[0] = "을";
		chkjosa[1] = "를";
	}
	else if ( (josa[0] == 0xb0 && josa[1] == 0xfa) || (josa[0] == 0xbf && josa[1] == 0xcd) )
	{
		chkjosa[0] = "과";
		chkjosa[1] = "와";
	}
	else if ( (josa[0] == 0xbe && josa[1] == 0xc6) || (josa[0] == 0xbe && josa[1] == 0xdf) )
	{
		chkjosa[0] = "아";
		chkjosa[1] = "야";
	}
	else
	{
		php_error(E_ERROR, "%s is not korean postposition\n", josa);
	}

	if (strlen(str) > 1)
	{
	    chkstr = &str[strlen(str) - 2];
	}
	else if (strlen(str) > 0)
	{
	    chkstr = estrdup(str);
	}
	else
	{
		php_error(E_ERROR, "String is too short\n");
	}

	position = (int) get_postposition(chkstr);
	post = ( position == 1 ) ? estrdup(chkjosa[1]) : estrdup(chkjosa[0]);

	if (chkutf == 1) { RETURN_STRING( convUTF8(post, 0), 1); }
	else { RETURN_STRING(post, 1); }
}
/* }}} */

/* {{{ unsigned char *autoLink (unsigned char *str_o) */
unsigned char *autoLink (unsigned char *str_o)
{
	#define ARRAY_NO 19
	unsigned int agent_o;
	unsigned char tmp[512];
	unsigned char file_s[] = "(\\.(gz|tgz|tar|gzip|zip|rar|mpeg|mpg|exe|rpm|dep|rm|ram|asf|ace|viv|avi|mid|gif|jpg|png|bmp|eps|mov)\") TARGET=\"_blank\"";
	unsigned char http[] = "(http|https|ftp|telnet|news|mms):\\/\\/(([[:alnum:]\xA1-\xFE:_\\-]+\\.[[:alnum:]\xA1-\xFE,:;&#=_~%\\[\\]?\\/.,+\\-]+)([.]*[\\/a-z0-9\\[\\]]|=[\xA1-\xFE]+))";
	unsigned char mail[] = "([[:alnum:]\xA1-\xFE_.-]+)@([[:alnum:]\xA1-\xFE_-]+\\.[[:alnum:]\xA1-\xFE._-]*[a-z]{2,3}(\\?[[:alnum:]\xA1-\xFE=&\\?]+)*)";
	unsigned char *src[ARRAY_NO], *tar[ARRAY_NO];
	unsigned char *buf, *ptr;

	ptr = get_useragent();
	if ( ptr ) { agent_o = strstr(ptr, "MSIE") ? 1 : 0; }
	else { agent_o = 0; }

	/* &lt; 로 시작해서 3줄뒤에 &gt; 가 나올 경우와
	 * IMG tag 와 A tag 의 경우 링크가 여러줄에 걸쳐 이루어져 있을 경우
	 * 이를 한줄로 합침 (합치면서 부가 옵션들은 모두 삭제함) */
	src[0] = "/<([^<>\n]*)\n([^<>\n]\\+)\n([^<>\n]*)>/i";
	tar[0] = "<\\1\\2\\3>";
	src[1] = "/<([^<>\n]*)\n([^\n<>]*)>/i";
	tar[1] = "<\\1\\2>";

	sprintf(tmp, "/<(A|IMG)[^>]*(HREF|SRC)[^=]*=[ '\"\n]*(%s|mailto:%s)[^>]*>/i", http,mail);
	tmp[strlen(tmp)] = '\0';
	src[2] = estrdup(tmp);
	tar[2] = "<\\1 \\2=\"\\3\">";

	/* email 형식이나 URL 에 포함될 경우 URL 보호를 위해 @ 을 치환 */
	src[3] = "/(http|https|ftp|telnet|news|mms):\\/\\/([^ \n@]+)@/i";
	tar[3] = "\\1://\\2_HTTPAT_\\3";

	/* 특수 문자를 치환 및 html사용시 link 보호 */
	src[4] = "/&(quot|gt|lt)/i";
	tar[4] = "!\\1";

	sprintf(tmp, "/<a([^>]*)href=[\"' ]*(%s)[\"']*[^>]*>/i", http);
	tmp[strlen(tmp)] = '\0';
	src[5] = estrdup(tmp);
	tar[5] = "<A\\1HREF=\"\\3_orig://\\4\" TARGET=\"_blank\">";

	sprintf(tmp, "/href=[\"' ]*mailto:(%s)[\"']*>/i", mail);
	tmp[strlen(tmp)] = '\0';
	src[6] = estrdup(tmp);
	tar[6] = "HREF=\"mailto:\\2#-#\\3\">";

	sprintf(tmp, "/<([^>]*)(background|codebase|src)[ \n]*=[\n\"' ]*(%s)[\"']*/i", http);
	tmp[strlen(tmp)] = '\0';
	src[7] = estrdup(tmp);
	tar[7] = "<\\1\\2=\"\\4_orig://\\5\"";

	/* 링크가 안된 url및 email address 자동링크 */
	sprintf(tmp, "/((SRC|HREF|BASE|GROUND)[ ]*=[ ]*|[^=]|^)(%s)/i", http);
	tmp[strlen(tmp)] = '\0';
	src[8] = estrdup(tmp);
	tar[8] = "\\1<A HREF=\"\\3\" TARGET=\"_blank\">\\3</a>";

	sprintf(tmp, "/(%s)/i", mail);
	tmp[strlen(tmp)] = '\0';
	src[9] = estrdup(tmp);
	tar[9] = "<A HREF=\"mailto:\\1\">\\1</a>";
	src[10] = "/<A HREF=[^>]+>(<A HREF=[^>]+>)/i";
	tar[10] = "\\1";
	src[11] = "/<\\/A><\\/A>/i";
	tar[11] = "</A>";

	/* 보호를 위해 치환한 것들을 복구 */
	src[12] = "/!(quot|gt|lt)/i";
	tar[12] = "&\\1";
	src[13] = "/(http|https|ftp|telnet|news|mms)_orig/i";
	tar[13] = "\\1";
	src[14] = "'#-#'";
	tar[14] = "@";

	sprintf(tmp, "/%s/i", file_s);
	tmp[strlen(tmp)] = '\0';
	src[15] = estrdup(tmp);
	tar[15] = "\\1";

	/* email 주소를 변형한 뒤 URL 속의 @ 을 복구 */
	src[16] = "/_HTTPAT_/";
	tar[16] = "@";

	/* 이미지에 보더값 0 을 삽입 */
	src[17] = "/<(IMG SRC=\"[^\"]+\")>/i";
	tar[17] = "<\\1 BORDER=0>";

	/* IE 가 아닌 경우 embed tag 를 삭제함 */
	if(agent_o != 1)
   	{
		src[18] = "/<(embed[^>]*)>/i";
		tar[18] = "&lt;\\1&gt;";
	}
   	else
   	{
		src[18] = "/oops_php_lib_no_action/i";
		tar[18] = "";
	}
	
	buf = estrdup((unsigned char *) kr_regex_replace_arr (src, tar, str_o, ARRAY_NO));

	return buf;
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

/* {{{ unsigned char *get_useragent(void) */
unsigned char *get_useragent()
{
	unsigned char *ptr;
	TSRMLS_FETCH();
	
    ptr = estrdup(sapi_getenv("HTTP_USER_AGENT", 15 TSRMLS_CC));
	if ( !ptr ) { ptr = getenv("HTTP_USER_AGENT"); }
	if ( !ptr ) { ptr = get_serverenv("HTTP_USER_AGENT"); }
	if (!ptr) { ptr = ""; }

	return ptr;
}
/* }}} */

/* {{{ unsigned char *get_serverenv(unsigned char *para) */
unsigned char *get_serverenv(unsigned char *para)
{
	zval **data, **tmp, tmps;
	char *string_key;
	ulong num_key;
	unsigned char *parameters = NULL;
	TSRMLS_FETCH();

	zend_hash_find(&EG(symbol_table), "_SERVER", 8, (void **) &data);
	zend_hash_internal_pointer_reset(Z_ARRVAL_PP(data));
	while (zend_hash_get_current_data(Z_ARRVAL_PP(data), (void **) &tmp) == SUCCESS)
	{
		if (zend_hash_get_current_key(Z_ARRVAL_PP(data), &string_key, &num_key, 0) == HASH_KEY_IS_STRING)
		{
			if ( !strcasecmp (string_key, para) ) {
				tmps = **tmp;
				zval_copy_ctor(&tmps);
				convert_to_string(&tmps);
				parameters = Z_STRVAL(tmps);
				zval_dtor(&tmps);
				break;
			}
		}
		zend_hash_move_forward(Z_ARRVAL_PP(data));
	}

	if (!parameters) { parameters = ""; }
	return parameters;
}
/* }}} */

/* {{{ unsigned char *get_postposition (unsigned char *str) */
int get_postposition (unsigned char *str)
{
	unsigned char first, second;

	first = tolower(str[0]);
	if (strlen(str) > 1) { second = tolower(str[1]); }
	else { second = tolower(str[0]); }

	/* number area */
	if ( second > 47 && second < 58 )
	{
		if (first == 50 || first == 52 || first == 53 || first == 57) { return 0; }
		else { return 1; }
	}
	/* only 1 charactor */
	else if ( first == second )
	{
		if ( first == 114 || (first > 108 && first < 111) ) { return 0; }
		else { return 1; }
	}
	/* if 한a */
	else if ( first != second && (first > 127) && (second < 123 && second > 96) )
	{
		if ( second == 114 || (second > 108 && second < 111) ) { return 0; }
		else { return 1; }
	}
	/* if 한 */
	else if (first & 0x80)
	{
		unsigned int ncr;

		ncr = getNcrIDX(str[0], str[1]);
		if ( ((uni_cp949_ncr_table[ncr] - 16) % 28 ) == 0 ) { return 1; }
		else { return 0; }
	}
	/* if aa */
	else
	{
		/* last charactor is a or e or i or o or u or w or y  */
		if (second == 0x61 || second == 0x65 || second == 0x69 || second == 0x6f ||
			second == 0x75 || second == 0x77 || second == 0x79)
		{
			return 1;
		}
		/* last charactor is ed or er or or */
		else if ( (first == 0x65 && first == 0x72) || (first == 0x6f && first == 0x72) ||
				  (first == 0x65 && first == 0x64) )
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}
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
