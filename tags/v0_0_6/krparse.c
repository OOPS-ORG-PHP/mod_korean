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

  $Id: krparse.c,v 1.54 2003-07-16 10:58:59 oops Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "php_krparse.h"
#include "php_krcheck.h"
#include "php_krcharset.h"
#include "krregex.h"
#include "SAPI.h"
#include "charset/ksc5601.h"

#include <math.h>

/* {{{ proto string autolink_lib(string str)
 *   parse url in string */
PHP_FUNCTION(autolink_lib)
{
	pval **arg1;

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

	RETURN_STRING(autoLink(Z_STRVAL_PP(arg1)), 1);
}
/* }}} */

/* {{{ proto string substr_lib(string str, int start [, int length [, int utf8] ])
 *    Returns part of a multibyte string */
PHP_FUNCTION(substr_lib)
{
	zval **str, **from, **len, **utf8;
	static unsigned char *tmpstr, *retstr;
	unsigned char *dechar;
	int l, f, lenth, utf = 0;
	int argc = ZEND_NUM_ARGS();

	if (argc < 2 || argc > 4 || zend_get_parameters_ex(argc, &str, &from, &len, &utf8) == FAILURE)
   	{
		WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);
	convert_to_long_ex(from);

	tmpstr = (unsigned char *) emalloc (sizeof(char) * Z_STRLEN_PP(str) * 6);

	if (argc == 4)
   	{
		convert_to_long_ex(utf8);
		utf = Z_LVAL_PP(utf8);

		if ( utf == 1 )
		{
			XUCodeConv ( tmpstr, Z_STRLEN_PP(str) * 6, XU_CONV_CP949, Z_STRVAL_PP(str), Z_STRLEN_PP(str), XU_CONV_UTF8 );
		}
		else
		{
			dechar = krNcrDecode(Z_STRVAL_PP(str));
			strcpy (tmpstr, dechar);
			efree(dechar);
		}
	}
   	else
	{
		dechar = krNcrDecode(Z_STRVAL_PP(str));
		strcpy(tmpstr, dechar);
		efree(dechar);
	}

	if (argc > 2)
   	{
		convert_to_long_ex(len);
		l = Z_LVAL_PP(len);
	}
   	else { l = strlen(tmpstr); }

	f = Z_LVAL_PP(from);
	lenth = strlen(tmpstr);

	// if "from" position is negative, count start position from the end
	// of the string
	if (f < 0)
   	{
		f = lenth + f;
		if (f < 0) { f = 0; }
	}

	// if "length" position is negative, set it to the length
	// needed to stop that many chars from the end of the string
	if (l < 0)
   	{
		l = (lenth - f) + l;
		if (l < 0) { l = 0; }
	}

	if (f >= lenth) { RETURN_FALSE; }
	if((f + l) > lenth) { l = lenth - f; }

	// check multibyte whether start return charactor
	if(multibyte_check(tmpstr, f))
   	{
		f++;
		l--;
	} 

	// check multibyte whether last return charactor
	if(multibyte_check(tmpstr, f + l)) { l++; }

	tmpstr[f+l] = '\0';

	if (utf == 1)
   	{
		retstr = (unsigned char *) emalloc(sizeof(char) * strlen(tmpstr + f) * 6);
		XUCodeConv (retstr, strlen(tmpstr + f) * 6, XU_CONV_UTF8, tmpstr + f, strlen(tmpstr + f), XU_CONV_CP949);
		RETVAL_STRINGL(retstr, strlen(retstr), 1);
	}
   	else
   	{
		retstr = krNcrEncode(tmpstr + f, 1);
		RETVAL_STRINGL(retstr, strlen(retstr), 1);
	}
	efree (retstr);
	efree (tmpstr);
}
/* }}} */

/* {{{ proto array agentinfo_lib(void)
 *    Returns info of browser */
PHP_FUNCTION(agentinfo_lib)
{
	unsigned char *agent_o, *buf;

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
		buf = (unsigned char *) kr_regex_replace("/Mo.+MSIE ([^;]+);.+/i", "\\1", agent_o);
		add_assoc_string(return_value, "vr", (unsigned char *) kr_regex_replace("[a-z]", "", buf), 1);
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
		buf = (unsigned char *) kr_regex_replace("/Opera\\/([0-9.]+).*/i","\\1", agent_o);
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
		buf = (unsigned char *) kr_regex_replace("/Mozi[^(]+\\([^;]+;[^;]+;[^;]+;[^;]+;([^)]+)\\).*/i","\\1", agent_o);
		add_assoc_string(return_value, "vr", (unsigned char *) kr_regex_replace("/rv:| /i", "", buf), 1);

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
		buf = (unsigned char *) kr_regex_replace("/.*Konqueror\\/([0-9.]+).*/i","\\1", agent_o);
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if Lynx */
	else if (strstr(agent_o, "Lynx"))
   	{
		add_assoc_string(return_value, "br", "LYNX", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/Lynx\\/([^ ]+).*/i","\\1", agent_o);
		add_assoc_string(return_value, "vr", buf, 1);
	}

	/* if w3M */
	else if (strstr(agent_o, "w3m"))
   	{
		add_assoc_string(return_value, "br", "W3M", 1);
		add_assoc_string(return_value, "co", "TextBR", 1);

		/* get version */
		buf = (unsigned char *) kr_regex_replace("/w3m\\/([0-9.]+).*/i","\\1", agent_o);
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
		buf = (unsigned char *) kr_regex_replace("/Links \\(([^;]+);.*/i","\\1", agent_o);
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
	   	else if (strstr(agent_o, "[en]"))
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
	static unsigned char *str, josa[8], *chkjosa[2];
	static unsigned char *chkstr, utfpost[4], post[3];
	int slength = 0, plength = 0, position, chkutf = 0;

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

	slength = Z_STRLEN_PP(string);
	plength = Z_STRLEN_PP(posts);

	str = (unsigned char *) emalloc (sizeof(char) * slength * 6);

	if (chkutf == 1)
	{
		XUCodeConv (str, slength * 6, XU_CONV_CP949, Z_STRVAL_PP(string), slength, XU_CONV_UTF8);
		XUCodeConv (josa, plength * 6, XU_CONV_CP949, Z_STRVAL_PP(posts), plength, XU_CONV_UTF8);
	}
	else
	{
		memmove (str, Z_STRVAL_PP(string), slength);
		memmove (josa, Z_STRVAL_PP(posts), plength);
	}
	memset(str + slength, '\0', 1);
	memset(josa + plength, '\0', 1);

	/* check korean postposition */
	if ( strlen(josa) < 1 )
	{
		php_error(E_ERROR, "Don't exists postposition\n", josa);
	}	
	else if ( (josa[0] == 0xc0 && josa[1] == 0xcc) || (josa[0] == 0xb0 && josa[1] == 0xa1) )
	{
		chkjosa[0] = "��";
		chkjosa[1] = "��";
	}
	else if ( (josa[0] == 0xc0 && josa[1] == 0xba) || (josa[0] == 0xb4 && josa[1] == 0xc2) )
	{
		chkjosa[0] = "��";
		chkjosa[1] = "��";
	}
	else if ( (josa[0] == 0xc0 && josa[1] == 0xbb) || (josa[0] == 0xb8 && josa[1] == 0xa6) )
	{
		chkjosa[0] = "��";
		chkjosa[1] = "��";
	}
	else if ( (josa[0] == 0xb0 && josa[1] == 0xfa) || (josa[0] == 0xbf && josa[1] == 0xcd) )
	{
		chkjosa[0] = "��";
		chkjosa[1] = "��";
	}
	else if ( (josa[0] == 0xbe && josa[1] == 0xc6) || (josa[0] == 0xbe && josa[1] == 0xdf) )
	{
		chkjosa[0] = "��";
		chkjosa[1] = "��";
	}
	else
	{
		php_error(E_ERROR, "%s is not korean postposition\n", josa);
	}

	if (strlen(str) > 1)
	{
		int chkstrlen = strlen(str) - 2;
		chkstr = estrdup(str + chkstrlen);
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
	if ( position == 1 ) { memmove (post, chkjosa[1], 2); }
	else { memmove (post, chkjosa[0], 2); }

	efree(chkstr);
	efree(str);

	if (chkutf == 1)
	{
		XUCodeConv ( utfpost, 12, XU_CONV_UTF8, post, 2, XU_CONV_CP949 );
		RETURN_STRING(utfpost, 1);
	}
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
	static unsigned char *buf, *ptr;

	ptr = get_useragent();
	if ( ptr ) { agent_o = strstr(ptr, "MSIE") ? 1 : 0; }
	else { agent_o = 0; }

	/* &lt; �� �����ؼ� 3�ٵڿ� &gt; �� ���� ����
	 * IMG tag �� A tag �� ��� ��ũ�� �����ٿ� ���� �̷���� ���� ���
	 * �̸� ���ٷ� ��ħ (��ġ�鼭 �ΰ� �ɼǵ��� ��� ������) */
	src[0] = "/<([^<>\n]*)\n([^<>\n]\\+)\n([^<>\n]*)>/i";
	tar[0] = "<\\1\\2\\3>";
	src[1] = "/<([^<>\n]*)\n([^\n<>]*)>/i";
	tar[1] = "<\\1\\2>";

	sprintf(tmp, "/<(A|IMG)[^>=]*(HREF|SRC)[^=]*=[ '\"\n]*(%s|mailto:%s)[^>]*>/i", http,mail);
	tmp[strlen(tmp)] = '\0';
	src[2] = estrdup(tmp);
	tar[2] = "<\\1 \\2=\"\\3\">";

	/* email �����̳� URL �� ���Ե� ��� URL ��ȣ�� ���� @ �� ġȯ */
	src[3] = "/(http|https|ftp|telnet|news|mms):\\/\\/([^ \n@]+)@/i";
	tar[3] = "\\1://\\2_HTTPAT_\\3";

	/* Ư�� ���ڸ� ġȯ �� html���� link ��ȣ */
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

	/* ��ũ�� �ȵ� url�� email address �ڵ���ũ */
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

	/* ��ȣ�� ���� ġȯ�� �͵��� ���� */
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

	/* email �ּҸ� ������ �� URL ���� @ �� ���� */
	src[16] = "/_HTTPAT_/";
	tar[16] = "@";

	/* �̹����� ������ 0 �� ���� */
	src[17] = "/<(IMG SRC=\"[^\"]+\")>/i";
	tar[17] = "<\\1 BORDER=0>";

	/* IE �� �ƴ� ��� embed tag �� ������ */
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
	
	buf = (unsigned char *) kr_regex_replace_arr (src, tar, str_o, ARRAY_NO);

	efree (src[2]);
	efree (src[5]);
	efree (src[6]);
	efree (src[7]);
	efree (src[8]);
	efree (src[9]);
	efree (src[15]);

	return buf;
}
/* }}} */

/* {{{ unsigned char *get_useragent(void) */
unsigned char *get_useragent()
{
	static unsigned char *ptr;
	TSRMLS_FETCH();
	
    ptr = sapi_getenv("HTTP_USER_AGENT", 15 TSRMLS_CC);
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
	static unsigned char *parameters = NULL;
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

	/* if �� */
	if ( first & 0x80 )
	{
		unsigned int ncr;

		if(str[1] > 0x7a) str[1] -= 6;
		if(str[1] > 0x5a) str[1] -= 6;
		ncr = (str[0] - 0x81) * 178 + (str[0+1] - 0x41);

		if ( (table_ksc5601[ncr] - 16) % 28 == 0 ) { return 1; }
		else { return 0; }
	}
	/* number area */
	else if ( second > 47 && second < 58 )
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
	/* if ��a */
	else if ( first != second && first > 127 && (second < 123 && second > 96) )
	{
		if ( second == 114 || (second > 108 && second < 111) ) { return 0; }
		else { return 1; }
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
