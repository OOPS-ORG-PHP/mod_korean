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

  $Id: krparse.c,v 1.10 2002-07-24 10:10:24 oops Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_krparse.h"
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

	switch(argc) {
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(arg2);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (argc > 1 && Z_LVAL_PP(arg2) == 1) type = 1;
	else type = 0;

	convert_to_string_ex(arg1);

	if (strlen(Z_STRVAL_PP(arg1)) > 0) {
		string = krNcrConv(Z_STRVAL_PP(arg1),type);
		RETURN_STRING(string,1);
	} else RETURN_EMPTY_STRING();
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

	switch(argc) {
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg2);
			break;
		case 3:
			if(zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
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

	if (strlen(str) == 0) {
		php_error(E_ERROR, "Can't use null value of argument 1");
		RETURN_FALSE;
	}

	if (argc < 2) {
		start = "U+";
		end   = ";";
	} else if (argc == 2) {
		start = Z_STRVAL_PP(arg2);
		end   = ";";
	} else {
		start = Z_STRVAL_PP(arg2);
		end   = Z_STRVAL_PP(arg3);
	}

	string = uniConv(str,0,0,start,end);

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
	unsigned char *str, *from, *to, *start, *end, *string;

	argc = ZEND_NUM_ARGS();

	switch(argc) {
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(3, &arg1, &arg2, &arg3) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg3);
			break;
		case 4:
			if(zend_get_parameters_ex(4, &arg1, &arg2, &arg3, &arg4) == FAILURE) {
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

	if (strlen(str) == 0 || strlen(to) == 0) {
		php_error(E_ERROR, "Can't use null value of argument 1");
		RETURN_FALSE;
	}

	if (strcasecmp(to,"euc-kr") && strcasecmp(to,"cp949")) {
		php_error(E_ERROR, "Unknown encoding \"%s\"",to);
		RETURN_FALSE;
	}

	if (!strcasecmp(to,"euc-kr")) {
		type = 1;
		subtype = 1;
	} else if (!strcasecmp(to,"cp949")) {
		type = 1;
		subtype = 0;
	} else {
		type = 1;
		subtype = 0;
	}

	if (argc < 3) {
		start = "U+";
		end   = ";";
	} else if (argc == 3) {
		start = Z_STRVAL_PP(arg3);
		end   = ";";
	} else {
		start = Z_STRVAL_PP(arg3);
		end   = Z_STRVAL_PP(arg4);
	}
	string = uniConv(str,type,subtype,start,end);

	RETURN_STRING(string,1);
}

/* }}} */

/* {{{ proto string utf8encode_lib(string str)
   Return utf8 string from euc-kr or cp949 */
PHP_FUNCTION(utf8encode_lib) {
	pval **str;
	unsigned char *string;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &str) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(str);

			if(Z_STRLEN_PP(str) < 1) {
				php_error(E_ERROR, "Can't use null value of argument");
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	string = convUTF8 (Z_STRVAL_PP(str),0);

	RETURN_STRING(string,1);
}
/* }}} */

/* {{{ proto string utf8decode_lib(string str, string type)
   Return euc-kr or cp949 or unicode string from utf8 */
PHP_FUNCTION(utf8decode_lib) {
	pval **arg1, **arg2;
	int chk;
	unsigned char *str, *type, *string;

	switch(ZEND_NUM_ARGS()) {
		case 2:
			if(zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(arg1);
			convert_to_string_ex(arg2);
			str  = Z_STRVAL_PP(arg1);
			type = Z_STRVAL_PP(arg2);

			if (strlen(str) == 0 || strlen(type) == 0) {
				php_error(E_ERROR, "Can't use null value of argument 1 or 2");
				RETURN_FALSE;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if ( strcasecmp(type,"euc-kr") == 0 ) {
		chk = 1;
	} else if (strcasecmp(type,"cp949") == 0 ) {
		chk = 2;
	} else if (strcasecmp(type,"ncr") == 0 ) {
		chk = 3;
	} else if (strcasecmp(type,"unicode") == 0 ) {
		chk = 4;
	} else {
		php_error(E_ERROR, "Unsupported charactor set : %s", type);
		RETURN_FALSE;
	}

	string = convUTF8 (str,chk);

	RETURN_STRING(string,1);
}
/* }}} */

/* {{{ proto string autolink_lib(string str)
 *   parse url in string */
PHP_FUNCTION(autolink_lib)
{
	pval **arg1;
	unsigned int agent_v;
	unsigned char *ret;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(arg1);

	ret = autoLink(Z_STRVAL_PP(arg1));

	if (ret) { RETURN_STRING(ret,1); }
	RETURN_FALSE;
}
/* }}} */

/* {{{ proto string substr_lib(string str, int start [, int length])
 *    Returns part of a multibyte string */
PHP_FUNCTION(substr_lib)
{
	zval **str, **from, **len, **utf8;
	unsigned char *tmpstr, *string;
	int l, f, lenth, utf = 0;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 4 || zend_get_parameters_ex(argc, &str, &from, &len, &utf8) == FAILURE) {
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_long_ex(from);

	if (argc == 4) {
		convert_to_long_ex(utf8);
		utf = Z_LVAL_PP(utf8);
		tmpstr = convUTF8 (Z_STRVAL_PP(str), 2);
	} else {
		tmpstr = Z_STRVAL_PP(str);
	}

	if (argc > 2) {
		convert_to_long_ex(len);
		l = Z_LVAL_PP(len);
	} else {
		l = strlen(tmpstr);
	}

	f = Z_LVAL_PP(from);
	lenth = strlen(tmpstr);

	/* if "from" position is negative, count start position from the end
	 * of the string */
	if (f < 0) {
		f = lenth + f;
		if (f < 0) { f = 0; }
	}

	/* if "length" position is negative, set it to the length
	 * needed to stop that many chars from the end of the string */
	if (l < 0) {
		l = (lenth - f) + l;
		if (l < 0) { l = 0; }
	}

	if (f >= lenth) {
		RETURN_FALSE;
	}

	if((f + l) > lenth) {
		l = lenth - f;
	}

	/* check multibyte whether start return charactor */
	if(multibyte_check(tmpstr,f)) {
		f++;
		l--;
	} 

	/* check multibyte whether last return charactor */
	if(multibyte_check(tmpstr,f+l)) { l++; }


	if (utf == 1) {
		tmpstr[f+l] = '\0';
		string = &tmpstr[f];
		RETURN_STRING(convUTF8(string, 0), 1);
	} else {
		RETURN_STRINGL(Z_STRVAL_PP(str) + f, l, 1);
	}
}
/* }}} */

/* {{{ proto array agentinfo_lib(void)
 *    Returns info of browser */
PHP_FUNCTION(agentinfo_lib)
{
	unsigned char *agent_o, *agent_v, *buf;
#ifdef PHP_WIN32
	agent_o = getenv("HTTP_USER_AGENT");
#else
	agent_o = sapi_module.getenv("HTTP_USER_AGENT", 15 TSRMLS_CC);
#endif

	if (array_init(return_value) == FAILURE) {
		php_error(E_WARNING,"Failed init array");
		RETURN_FALSE;
	}

	/* if Explorer */
	if (strstr(agent_o,"MSIE")) {
		add_assoc_string(return_value, "br", "MSIE", 1);
		add_assoc_string(return_value, "co", "msie", 1);

		/* get user os */
		if (strstr(agent_o,"NT")) {
			add_assoc_string(return_value, "os", "NT", 1);
		} else if (strstr(agent_o,"Win")) {
			add_assoc_string(return_value, "os", "WIN", 1);
		} else {
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/Mo.+MSIE ([^;]+);.+/i","\\1",agent_o);
		agent_v = (unsigned char *) kr_regex_replace("[a-z]","",buf);
		add_assoc_string(return_value, "vr", agent_v, 1);
	}
	
	/* if Opera */
	else if (strstr(agent_o,"Opera")) {
		add_assoc_string(return_value, "br", "OPERA", 1);
		add_assoc_string(return_value, "co", "msie", 1);

		/* get user os */
		if (strstr(agent_o,"Linux")) {
			add_assoc_string(return_value, "os", "LINUX", 1);
		} else if (strstr(agent_o,"2000") || strstr(agent_o,"XP")) {
			add_assoc_string(return_value, "os", "NT", 1);
		} else if (strstr(agent_o,"Win")) {
			add_assoc_string(return_value, "os", "WIN", 1);
		} else {
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/Opera\\/([0-9.]+).*/i","\\1",agent_o);
		add_assoc_string(return_value, "vr", buf, 1);

		/* get language */
		if (strstr(agent_o, "[ko]")) {
			add_assoc_string(return_value, "ln", "ko", 1);
		} else if (strstr(agent_o, "[en]")) {
			add_assoc_string(return_value, "ln", "en", 1);
		} else {
			add_assoc_string(return_value, "ln", "other", 1);
		}
	}

	/* if Mozilla */
	else if ((strstr(agent_o,"Gecko") || strstr(agent_o,"Galeon")) && !strstr(agent_o,"Netscape")) {
		add_assoc_string(return_value, "br", "MOZL", 1);
		add_assoc_string(return_value, "co", "mozilla", 1);

		/* get user os */
		if (strstr(agent_o,"NT")) {
			add_assoc_string(return_value, "os", "NT", 1);
		} else if (strstr(agent_o,"Win")) {
			add_assoc_string(return_value, "os", "WIN", 1);
		} else if (strstr(agent_o,"Linux")) {
			add_assoc_string(return_value, "os", "LINUX", 1);
		} else {
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/Mozi[^(]+\\([^;]+;[^;]+;[^;]+;[^;]+;([^)]+)\\).*/i","\\1",agent_o);
		agent_v = (unsigned char *) kr_regex_replace("/rv:| /i","",buf);
		add_assoc_string(return_value, "vr", agent_v, 1);

		/* get language */
		if (strstr(agent_o, "en-US")) {
			add_assoc_string(return_value, "ln", "en", 1);
		} else if (strstr(agent_o, "ko-KR")) {
			add_assoc_string(return_value, "ln", "ko", 1);
		} else {
			add_assoc_string(return_value, "ln", "other", 1);
		}
	}

	/* if Konqueror */
	else if (strstr(agent_o,"Konqueror")) {
		add_assoc_string(return_value, "br", "KONQ", 1);
		add_assoc_string(return_value, "co", "mozilla", 1);

		/* get user os */
		if (strstr(agent_o,"Linux")) {
			add_assoc_string(return_value, "os", "LINUX", 1);
		} else if (strstr(agent_o,"FreeBSD")) {
			add_assoc_string(return_value, "os", "FreeBSD", 1);
		} else {
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/.*Konqueror\\/([0-9.]+).*/i","\\1",agent_o);
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if Lynx */
	else if (strstr(agent_o,"Lynx")) {
		add_assoc_string(return_value, "br", "LYNX", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/Lynx\\/([^ ]+).*/i","\\1",agent_o);
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if w3M */
	else if (strstr(agent_o,"w3m")) {
		add_assoc_string(return_value, "br", "W3M", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/w3m\\/([0-9.]+).*/i","\\1",agent_o);
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if LINKS */
	else if (strstr(agent_o,"Links")) {
		add_assoc_string(return_value, "br", "LINKS", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get user os */
		if (strstr(agent_o,"Linux")) {
			add_assoc_string(return_value, "os", "LINUX", 1);
		} else if (strstr(agent_o,"FreeBSD")) {
			add_assoc_string(return_value, "os", "FreeBSD", 1);
		} else {
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/Links \\(([^;]+);.*/i","\\1",agent_o);
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if Netscape */
	else if (strstr(agent_o,"Mozilla")) {
		add_assoc_string(return_value, "br", "NS", 1);
		add_assoc_string(return_value, "co", "mozilla", 1);

		/* get user os */
		if (strstr(agent_o,"NT")) {
			add_assoc_string(return_value, "os", "NT", 1);
		} else if (strstr(agent_o,"Win")) {
			add_assoc_string(return_value, "os", "WIN", 1);
		} else if (strstr(agent_o,"Linux")) {
			add_assoc_string(return_value, "os", "LINUX", 1);
		} else {
			add_assoc_string(return_value, "os", "OTHER", 1);
		}

		if (strstr(agent_o,"[ko]")) {
			add_assoc_string(return_value, "ln", "ko", 1);
		} else if (strstr(agent_o,"[ko]")) {
			add_assoc_string(return_value, "ln", "en", 1);
		} else {
			add_assoc_string(return_value, "ln", "other", 1);
		}

		/* get version */
		if (strstr(agent_o,"Gecko")) {
			add_assoc_string(return_value, "vr", "6", 1);
		} else {
			add_assoc_string(return_value, "vr", "4", 1);
		}
	}

	/* other browser */
	else {
		add_assoc_string(return_value, "br", "OTHER", 1);
		add_assoc_string(return_value, "co", "OTHER", 1);
	}
}
/* }}} */

/* {{{ unsigned char *autoLink (unsigned char *str_o) */
unsigned char *autoLink (unsigned char *str_o)
{
	unsigned int array_no = 19, agent_o;
	unsigned char *tmp;
	unsigned char file_s[] = "(\\.(gz|tgz|tar|gzip|zip|rar|mpeg|mpg|exe|rpm|dep|rm|ram|asf|ace|viv|avi|mid|gif|jpg|png|bmp|eps|mov)\") TARGET=\"_blank\"";
	unsigned char http[] = "(http|https|ftp|telnet|news|mms):\\/\\/(([[:alnum:]\xA1-\xFE:_\\-]+\\.[[:alnum:]\xA1-\xFE,:;&#=_~%\\[\\]?\\/.,+\\-]+)([.]*[\\/a-z0-9\\[\\]]|=[\xA1-\xFE]+))";
	unsigned char mail[] = "([[:alnum:]\xA1-\xFE_.-]+)@([[:alnum:]\xA1-\xFE_-]+\\.[[:alnum:]\xA1-\xFE._-]*[a-z]{2,3}(\\?[[:alnum:]\xA1-\xFE=&\\?]+)*)";
	unsigned char *src[array_no], *tar[array_no];
	unsigned char *buf;

#ifdef PHP_WIN32
	agent_o = strstr(getenv("HTTP_USER_AGENT"),"MSIE") ? 1 : 0; 
#else
	agent_o = strstr(sapi_module.getenv("HTTP_USER_AGENT", 15 TSRMLS_CC),"MSIE") ? 1 : 0; 
#endif

	/* &lt; 로 시작해서 3줄뒤에 &gt; 가 나올 경우와
	 * IMG tag 와 A tag 의 경우 링크가 여러줄에 걸쳐 이루어져 있을 경우
	 * 이를 한줄로 합침 (합치면서 부가 옵션들은 모두 삭제함) */
	src[0] = "/<([^<>\n]*)\n([^<>\n]\\+)\n([^<>\n]*)>/i";
	tar[0] = "<\\1\\2\\3>";
	src[1] = "/<([^<>\n]*)\n([^\n<>]*)>/i";
	tar[1] = "<\\1\\2>";

	tmp = emalloc(350);
	sprintf(tmp,"/<(A|IMG)[^>]*(HREF|SRC)[^=]*=[ '\"\n]*(%s|mailto:%s)[^>]*>/i",http,mail);
	src[2] = tmp;
	tar[2] = "<\\1 \\2=\"\\3\">";

	/* email 형식이나 URL 에 포함될 경우 URL 보호를 위해 @ 을 치환 */
	src[3] = "/(http|https|ftp|telnet|news|mms):\\/\\/([^ \n@]+)@/i";
	tar[3] = "\\1://\\2_HTTPAT_\\3";

	/* 특수 문자를 치환 및 html사용시 link 보호 */
	src[4] = "/&(quot|gt|lt)/i";
	tar[4] = "!\\1";

	tmp = emalloc(350);
	sprintf(tmp,"/<a([^>]*)href=[\"' ]*(%s)[\"']*[^>]*>/i",http);
	src[5] = tmp;
	tar[5] = "<A\\1HREF=\"\\3_orig://\\4\" TARGET=\"_blank\">";

	tmp = emalloc(350);
	sprintf(tmp,"/href=[\"' ]*mailto:(%s)[\"']*>/i",mail);
	src[6] = tmp;
	tar[6] = "HREF=\"mailto:\\2#-#\\3\">";

	tmp = emalloc(350);
	sprintf(tmp,"/<([^>]*)(background|codebase|src)[ \n]*=[\n\"' ]*(%s)[\"']*/i",http);
	src[7] = tmp;
	tar[7] = "<\\1\\2=\"\\4_orig://\\5\"";

	/* 링크가 안된 url및 email address 자동링크 */
	tmp = emalloc(350);
	sprintf(tmp,"/((SRC|HREF|BASE|GROUND)[ ]*=[ ]*|[^=]|^)(%s)/i",http);
	src[8] = tmp;
	tar[8] = "\\1<A HREF=\"\\3\" TARGET=\"_blank\">\\3</a>";

	tmp = emalloc(350);
	sprintf(tmp,"/(%s)/i",mail);
	src[9] = tmp;
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

	tmp = emalloc(350);
	sprintf(tmp,"/%s/i",file_s);
	src[15] = tmp;
	tar[15] = "\\1";

	/* email 주소를 변형한 뒤 URL 속의 @ 을 복구 */
	src[16] = "/_HTTPAT_/";
	tar[16] = "@";

	/* 이미지에 보더값 0 을 삽입 */
	src[17] = "/<(IMG SRC=\"[^\"]+\")>/i";
	tar[17] = "<\\1 BORDER=0>";

	/* IE 가 아닌 경우 embed tag 를 삭제함 */
	if(agent_o != 1) {
		src[18] = "/<(embed[^>]*)>/i";
		tar[18] = "&lt;\\1&gt;";
	} else {
		src[18] = "/oops_php_lib_no_action/i";
		tar[18] = "";
	}
	
	buf = (unsigned char *) kr_regex_replace_arr (src,tar,str_o,array_no);

	efree(tmp);
	return buf;
}
/* }}} */

/* {{{ unsigned char *krNcrConv (unsigned char *str_o, int type)
 * convert euc-kr to ncr code, or convert outside EUC-KR range to ncr code
 * unsigned chart *str_o => EUC-KR/CP949 string
 * int type              => convert whole string(0) or outside EUC-KR range(1)
 */
unsigned char *krNcrConv (unsigned char *str_o, int type)
{
	unsigned long i;
	unsigned int ncr;
	size_t len = strlen(str_o);
	unsigned char *rc, *strs;
	unsigned char *ret = NULL;

	rc = emalloc(sizeof(char) * 8 + 1);
	for(i=0;i<len;i++) {
		/* if 2byte charactor */
		if (str_o[i] & 0x80) {
			switch(type) {
				case 1:
					if((str_o[i] >= 0x81 && str_o[i] <= 0xa0 && str_o[i+1] >= 0x41 && str_o[i+1] <=0xfe) ||
					   (str_o[i] >= 0xa1 && str_o[i] <= 0xc6 && str_o[i+1] >= 0x41 && str_o[i+1] <=0xa0)) {
						ncr = getNcrIDX(str_o[i],str_o[i+1]);
						sprintf(rc,"&#%d;",uni_cp949_ncr_table[ncr]);
						i++;
					} else sprintf(rc,"%c",str_o[i]);

					break;
				default:
					ncr = getNcrIDX(str_o[i],str_o[i+1]);
					sprintf(rc,"&#%d;",uni_cp949_ncr_table[ncr]);
					i++;
			}
		}
		/* 1 byte charactor */
		else { sprintf(rc,"%c",str_o[i]); }

		if (strlen(rc) != 0) {
			if (ret != NULL) {
				ret = erealloc(ret,sizeof(char) * (strlen(ret) + strlen(rc) + 1));
				strcat(ret,rc);
			} else {
				ret = erealloc(NULL,sizeof(char) * (strlen(rc) + 1));
				strcpy(ret,rc);
			}
		}
	}

	strs = (unsigned char *) estrndup(ret,strlen(ret));
	efree(rc);
	efree(ret);
	return strs;
}
/* }}} */

/* {{{ unsigned char *uniConv (unsigned char *str_o, int type, int subtype, unsigned char *start, unsigned char *end)
 * Convert EUC-KR/CP940 to unicode
 * unsigned char *str_o   => convert string (euc-kr, cp949, unicode)
 * int type               => 0: convert euc-kr,cp949 -> unicode
 *                         1: convert unicode -> ecu-kr,cp949
 *                         2: convert cp949 -> euc-kr (same as krNcrConv(str_o,1))
 * int subtype          => if value of type set 1, 1 is euc-kr and 0 is cp949
 * unsigned char *start => front string of hex value of unicode (ex U+AC60; => U+)
 * unsigned char *end   => after string of hex value of unicode (ex U+AC60; => ; )
 */
unsigned char *uniConv (unsigned char *str_o, int type, int subtype, unsigned char *start, unsigned char *end)
{
	unsigned long i;
	unsigned int ncr;
	size_t len = strlen(str_o);
	unsigned char *rc, *strs;
	unsigned char *ret = NULL;

	int regno,hexv,firsti,secondi,rc_len = 7;
	long slen = strlen(start);
	long elen = strlen(end);
	regex_t preg;
	unsigned char regex[12] = "[0-9a-f]{4}";
	unsigned char chkReg[5], conv[5],first[3],second[3];

	if (type == 1) {
		regno = regcomp(&preg,regex,REG_EXTENDED|REG_ICASE);

		if (regno != 0) {
			php_error(E_WARNING,"Problem in Unicode start charactors or end charactocs");
			return str_o;
		}

		if (subtype == 1) rc_len = 8;
		else rc_len = slen + elen + 4;
	}

	for (i=0; i<len; i++) {
		rc = emalloc(sizeof(char) * (rc_len + 1));
		switch(type) {
			/* convert to euc-kr/cp949 from unicode */
			case 1:
				/* unicode is constructed start charactors and hex code and end charactors */
				sprintf(chkReg,"%c%c%c%c",str_o[i+slen],str_o[i+slen+1],str_o[i+slen+2],str_o[i+slen+3]);
				if(!strncmp(&str_o[i],start,slen) && regexec(&preg,chkReg,0,NULL,0) == 0 &&
				   !strncmp(&str_o[i+slen+4],end,elen)) {

					hexv = hex2dec(chkReg,0);
					sprintf(conv,"%x",getUniIDX(hexv));
					sprintf(first,"%c%c",conv[0],conv[1]);
					firsti = hex2dec(first,1);
					sprintf(second,"%c%c",conv[2],conv[3]);
					secondi = hex2dec(second,1);

					sprintf(rc,"%c%c",firsti,secondi);

					/* convert ncr code with outsize of EUC-KR range */
					if (subtype == 1) {
						if((rc[0] >= 0x81 && rc[0] <= 0xa0 && rc[1] >= 0x41 && rc[1] <=0xfe) ||
						   (rc[0] >= 0xa1 && rc[0] <= 0xc6 && rc[1] >= 0x41 && rc[1] <=0xa0)) {
							ncr = getNcrIDX(rc[0],rc[1]);
							sprintf(rc,"&#%d;",uni_cp949_ncr_table[ncr]);
						}
					}

					i = i+3+slen+elen;
				} else {
					sprintf(rc,"%c",str_o[i]);
				}

				break;
			/* convert to unicode from euc-kr/cp949 */
			default:
				/* if 2byte charactor */
				if (str_o[i] & 0x80) {
					ncr = getNcrIDX(str_o[i],str_o[i+1]);
					sprintf(rc,"%s%X%s",start,uni_cp949_ncr_table[ncr],end);
					i++;
				} else {
					sprintf(rc,"%c",str_o[i]);
				}
		}

		if (strlen(rc) != 0) {
			if(ret != NULL) {
				ret = erealloc(ret,(sizeof(char) * (strlen(ret) + strlen(rc) + 1)));
				strcat(ret,rc);
			} else {
				ret = erealloc(NULL,sizeof(char) * (strlen(rc) + 1));
				strcpy(ret,rc);
			}
		}
	}

	strs = estrndup(ret,strlen(ret));
	if (type == 1) { regfree(&preg); }
	efree(rc);
	efree(ret);
	return strs;
}
/* }}} */

/* {{{ unsigned char *convUTF8(unsigned char *str_o, int type)
 * convert utf8 string */
unsigned char *convUTF8(unsigned char *str_o, int type)
{
	unsigned long i;
	size_t len = strlen(str_o);
	int ncr;
	unsigned char var[5], *byte[4], *bin[3], *rc, *strs, *ret = NULL;
	unsigned char utfonebyte[9], utftwobyte[9], utfthreebyte[9];

	switch(type) {
		/* utf8 -> euc-kr */
		case 1:
			rc = convUTF8(str_o,4);
			ret = uniConv(rc,1,1,"U+",";");
			break;

		/* utf8 -> cp949 */
		case 2:
			rc = convUTF8(str_o,4);
			ret = uniConv(rc,1,0,"U+",";");
			break;

		/* utf8 -> ncr */
		case 3:
			rc = convUTF8(str_o,1);
			ret = krNcrConv(rc,0);
			break;

		/* utf8 -> unicode */
		case 4:
			rc = emalloc(8);
			for( i=0 ; i<len ; i++ ) {
				if ( str_o[i] & 0x80 ) {
					/* 2byte 의 utf8 문자를 각 byte 별의 2진수로 변환 */
					sprintf(var,"%x",str_o[i]);
					sprintf(utfonebyte,"%s%s",hex2bin(var[0]),hex2bin(var[1]));
					sprintf(var,"%x",str_o[i+1]);
					sprintf(utftwobyte,"%s%s",hex2bin(var[0]),hex2bin(var[1]));
					sprintf(var,"%x",str_o[i+2]);
					sprintf(utfthreebyte,"%s%s",hex2bin(var[0]),hex2bin(var[1]));

					sprintf(var,"%c%c%c%c",utfonebyte[4],utfonebyte[5],utfonebyte[6],utfonebyte[7]);
					byte[0] = bin2hex(var);
					sprintf(var,"%c%c%c%c",utftwobyte[2],utftwobyte[3],utftwobyte[4],utftwobyte[5]);
					byte[1] = bin2hex(var);
					sprintf(var,"%c%c%c%c",utftwobyte[6],utftwobyte[7],utfthreebyte[2],utfthreebyte[3]);
					byte[2] = bin2hex(var);
					sprintf(var,"%c%c%c%c",utfthreebyte[4],utfthreebyte[5],utfthreebyte[6],utfthreebyte[7]);
					byte[3] = bin2hex(var);

					sprintf(rc,"U+%s%s%s%s;",byte[0],byte[1],byte[2],byte[3]);
					i += 2;
				} else {
					sprintf(rc,"%c",str_o[i]);
				}

				if (strlen(rc) != 0) {
					if (ret != NULL) {
						ret = (unsigned char *) erealloc(ret, strlen(ret) + strlen(rc) + 1);
						strcat(ret,rc);
					} else {
						ret = estrdup(rc);
					}
				}
			}
			break;

		/* 2 byte charactor -> utf8 */
		default:
			rc = emalloc(4);
			for(i=0 ; i<len ; i++) {
				if ( str_o[i] & 0x80 ) {
					ncr = getNcrIDX(str_o[i],str_o[i+1]);
					ncr = uni_cp949_ncr_table[ncr];
					sprintf(var,"%x",ncr);

					byte[0] = hex2bin(var[0]);
					byte[1] = hex2bin(var[1]);
					byte[2] = hex2bin(var[2]);
					byte[3] = hex2bin(var[3]);

					sprintf(utfonebyte,"1110%s",byte[0]);
					sprintf(utftwobyte,"10%s%c%c",byte[1],byte[2][0],byte[2][1]);
					sprintf(utfthreebyte,"10%c%c%s",byte[2][2],byte[2][3],byte[3]);

					sprintf(rc,"%c%c%c",bin2dec(utfonebyte),bin2dec(utftwobyte),bin2dec(utfthreebyte));
					i++;
				} else {
					sprintf(rc,"%c",str_o[i]);
				}

				if (strlen(rc) != 0) {
					if (ret != NULL) {
						ret = erealloc(ret,(sizeof(char) * (strlen(ret) + strlen(rc) + 1)));
						strcat(ret,rc);
					} else {
						ret = erealloc(NULL,sizeof(char) * (strlen(rc) + 1));
						strcpy(ret,rc);
					}
				}
			}
	}

	strs = estrdup(ret);

	if (type != 1 && type != 2 && type != 3) {
		efree(rc);
		efree(ret);
	}
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

/* {{{ unsigned int getUniIDX (unsigned int key) */
unsigned int getUniIDX (unsigned int key) {
	int *ptr, no, *chk, result;

	ptr = (int *)bsearch(&key,cp949_2byte_ncr_table,18068,sizeof(cp949_2byte_ncr_table[0]),comp);
	chk = cp949_2byte_ncr_table;

	if (ptr != NULL) {
		result = ptr - chk;
		return cp949_2byte_uni_ncr_table[result];
	} else
		return 0;
}
/* }}} */

/* {{{ unsigned int hex2dec (unsigned char *str_o,unsigned int type) */
unsigned int hex2dec (unsigned char *str_o,unsigned int type) {
	int i,buf[4],len = strlen(str_o);

	for(i=0;i<len;i++) {
		/* range of alphabat a -> f */
		if((str_o[i] >= 0x61 && str_o[i] <= 0x66) || (str_o[i] >= 0x41 && str_o[i] <= 0x46)) {
			switch(str_o[i]) {
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
			switch(str_o[i]) {
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

	switch(type) {
		case 1 :
			return ((buf[0] * 16) + buf[1]);
			break;
		default :
			return ((buf[0] * 16 * 16 * 16) + (buf[1] * 16 * 16) + (buf[2] * 16) + buf[3]);
			
	}
}
/* }}} */

/* {{{ unsigned char *hex2bin(unsigned char str_o) */
unsigned char *hex2bin(unsigned char str_o)
{
	unsigned char *buf;

	if((str_o >= 0x61 && str_o <= 0x7a) || (str_o >= 0x41 && str_o <= 0x5a)) {
		switch(str_o) {
			case 'a' : buf = "1010"; break;
			case 'b' : buf = "1011"; break;
			case 'c' : buf = "1100"; break;
			case 'd' : buf = "1101"; break;
			case 'e' : buf = "1110"; break;
			case 'f' : buf = "1111"; break;
			case 'A' : buf = "1010"; break;
			case 'B' : buf = "1011"; break;
			case 'C' : buf = "1100"; break;
			case 'D' : buf = "1101"; break;
			case 'E' : buf = "1110"; break;
			case 'F' : buf = "1111"; break;
			}
	} else {
		switch(str_o) {
			case '0' : buf = "0000"; break;
			case '1' : buf = "0001"; break;
			case '2' : buf = "0010"; break;
			case '3' : buf = "0011"; break;
			case '4' : buf = "0100"; break;
			case '5' : buf = "0101"; break;
			case '6' : buf = "0110"; break;
			case '7' : buf = "0111"; break;
			case '8' : buf = "1000"; break;
			case '9' : buf = "1001"; break;
		}
	}
	return buf;
}
/* }}} */

/* {{{ unsigned int bin2dec(unsigned char *str_o) */
unsigned int bin2dec(unsigned char *str_o)
{
	int i, ret = 0;
	unsigned char var[2];

	for(i=0 ; i<8 ; i++) {
		sprintf(var,"%c",str_o[i]);
		ret += atoi(var) << (7 - i);
	}

	return ret;
}
/* }}} */

/* {{{ unsigned char *bin2hex(unsigned char *str_o) */
unsigned char *bin2hex(unsigned char *str_o)
{
	unsigned int i, buf = 0;
	unsigned char *ret, *strs, var[2];

	for (i=0 ; i<4 ; i++) {
		sprintf(var,"%c",str_o[i]);
		buf += atoi(var) << (3 - i);
	}
	
	ret = emalloc(2);
	sprintf(ret,"%X",buf);
	strs = estrndup(ret,strlen(ret));
	free(ret);
	return strs;
}
/* }}} */

/* {{{ int comp(const void *s1, const void *s2) */
int comp(const void *s1, const void *s2)
{
	return (*(int *)s1 - *(int *)s2);
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
