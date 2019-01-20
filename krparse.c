/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2019 The PHP Group                                |
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
#include "php_krparse.h"
#include "php_krcheck.h"
#include "php_krcharset.h"
#include "krregex.h"
#include "SAPI.h"
#include "php_kr.h"
#include "charset/ksc5601.h"

#include <math.h>

/* {{{ proto string autolink_lib(string str)
 *   parse url in string */
PHP_FUNCTION(autolink_lib)
{
	zend_string * input = NULL;
	char        * result = NULL;

	if ( kr_parameters ("S", &input) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	result = autoLink (ZSTR_VAL (input));

	RETVAL_STRING (result);
	kr_safe_efree (result);
}
/* }}} */

/* {{{ proto string substr_lib(string str, int start [, int length [, int utf8] ])
 *    Returns part of a multibyte string */
PHP_FUNCTION(substr_lib)
{
	zend_string * input = NULL;
	zend_long     l = 0, f = 0;
	char        * tmpstr, * retstr = NULL;
	char        * dechar;
	int           lenth, utf = 0;
	//int           argc = ZEND_NUM_ARGS();

	char        * str = NULL;
	int           slen;

	if ( kr_parameters ("Sl|lb", &input, &f, &l, &utf) == FAILURE )
		return;

	if ( ZSTR_LEN (input) == 0 )
		RETURN_EMPTY_STRING ();

	str  = ZSTR_VAL (input);
	slen = ZSTR_LEN (input);

	if ( utf == 0 && ! is_utf8 (str) )
		utf = 1;

	tmpstr = emalloc (sizeof(char) * (slen * 6));

	if ( utf )
		XUCodeConv (tmpstr, slen * 6, XU_CONV_CP949, str, slen, XU_CONV_UTF8 );
	else {
		dechar = krNcrDecode (str);
		strcpy (tmpstr, dechar);
		kr_safe_efree (dechar);
	}

	l = l ? l : strlen (tmpstr);
	lenth = strlen (tmpstr);

	// if "from" position is negative, count start position from the end
	// of the string
	if ( f < 0 ) {
		f += lenth;
		if ( f < 0 )
			f = 0;
	}

	// if "length" position is negative, set it to the length
	// needed to stop that many chars from the end of the string
	if (l < 0) {
		l = (lenth - f) + l;
		if ( l < 0 )
			l = 0;
	}

	if ( f >= lenth )
		RETURN_FALSE;

	if ( (f + l) > lenth )
		l = lenth - f;

	// check multibyte whether start return charactor
	if ( multibyte_check (tmpstr, f) ) {
		f++;
		l--;
	} 

	// check multibyte whether last return charactor
	if ( multibyte_check (tmpstr, f + l) )
		l++;

	tmpstr[f+l] = 0;

	if ( utf ) {
		retstr = emalloc (sizeof (char) * strlen (tmpstr + f) * 6);
		XUCodeConv (retstr, strlen (tmpstr + f) * 6, XU_CONV_UTF8, tmpstr + f, strlen (tmpstr + f), XU_CONV_CP949);
		RETVAL_STRINGL(retstr, strlen (retstr));
	}
	else
	{
		retstr = krNcrEncode (tmpstr + f, 1);
		RETVAL_STRINGL(retstr, strlen (retstr));
	}
	kr_safe_efree (retstr);
	kr_safe_efree (tmpstr);
}
/* }}} */

/* {{{ proto array agentinfo_lib(void)
 *    Returns info of browser */
PHP_FUNCTION(agentinfo_lib)
{
	char * agent_o, * buf;

	agent_o = get_useragent ();
	if ( ! agent_o ) 
		RETURN_FALSE;

#if PHP_VERSION_ID < 70300
	if ( array_init (return_value) == FAILURE ) {
		php_error (E_WARNING, "Failed init array");
		RETURN_FALSE;
	}
#else
	array_init (return_value);
#endif

	/* if Explorer */
	if ( strstr (agent_o, "MSIE") ) {
		add_assoc_string (return_value, "br", "MSIE");
		add_assoc_string (return_value, "co", "msie");

		/* get user os */
		if ( strstr (agent_o, "NT") )
			add_assoc_string (return_value, "os", "NT");
		else if ( strstr (agent_o, "Win") )
			add_assoc_string (return_value, "os", "WIN");
		else
			add_assoc_string (return_value, "os", "OTHER");

		/* get version */
		buf = (char *) kr_regex_replace ("/Mo.+MSIE ([^;]+);.+/i", "\\1", agent_o);
		{
			char * tbuf = (char *) kr_regex_replace ("/[a-z]/", "", buf);
			add_assoc_string (return_value, "vr", tbuf);
			kr_safe_efree (tbuf);
		}
		kr_safe_efree (buf);
	}

	/* if Edge */
	else if ( strstr (agent_o, "Edge/") ) {
		add_assoc_string (return_value, "br", "Edge");
		add_assoc_string (return_value, "co", "mozilla");
		add_assoc_string (return_value, "os", "NT");
		buf = (char *) kr_regex_replace ("/.* Edge\\/([0-9.]+).*/", "\\1", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);
	}

	/* if Chrome */
	else if ( strstr (agent_o, "Chrome") || strstr (agent_o, "CriOS") || strstr (agent_o, "CrMo") ) {
		add_assoc_string (return_value, "br", "Chrome");
		add_assoc_string (return_value, "co", "mozilla");

		if ( strstr (agent_o, "Linux") )
			add_assoc_string (return_value, "os", "LINUX");
		else if ( strstr (agent_o, "2000" ) || strstr (agent_o, "XP") )
			add_assoc_string (return_value, "os", "NT");
		else if ( strstr (agent_o, "Win") )
			add_assoc_string(return_value, "os", "WIN");
		else
			add_assoc_string(return_value, "os", "OTHER");
		buf = (char *) kr_regex_replace ("/.*(Chrome|CriOS|CrMo)\\/([0-9]+(\\.[0-9]+)?).*/", "\\2", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);
	}
	
	/* if Safari */
	else if ( strstr (agent_o, "Safari") || strstr (agent_o, "AppleWebKit") ) {
		add_assoc_string (return_value, "br", "Safari");
		add_assoc_string (return_value, "co", "mozilla");

		if ( strstr (agent_o, "Linux") )
			add_assoc_string (return_value, "os", "LINUX");
		else if ( strstr (agent_o, "2000" ) || strstr (agent_o, "XP") )
			add_assoc_string (return_value, "os", "NT");
		else if ( strstr (agent_o, "Win") )
			add_assoc_string(return_value, "os", "WIN");
		else
			add_assoc_string(return_value, "os", "OTHER");
		buf = (char *) kr_regex_replace ("/.*/Safari\\/([0-9]+(\\.[0-9]+)?).*/", "\\1", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);
	}
	
	/* if Opera */
	else if ( strstr (agent_o, "Opera") ) {
		add_assoc_string (return_value, "br", "OPERA");
		add_assoc_string (return_value, "co", "msie");

		/* get user os */
		if ( strstr (agent_o, "Linux") )
			add_assoc_string (return_value, "os", "LINUX");
		else if ( strstr (agent_o, "2000" ) || strstr (agent_o, "XP") )
			add_assoc_string (return_value, "os", "NT");
		else if ( strstr (agent_o, "Win") )
			add_assoc_string(return_value, "os", "WIN");
		else
			add_assoc_string(return_value, "os", "OTHER");

		/* get version */
		buf = (char *) kr_regex_replace ("/Opera\\/([0-9.]+).*/i","\\1", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);

		/* get language */
		if ( strstr (agent_o, "[ko]") )
			add_assoc_string (return_value, "ln", "ko");
		else if ( strstr (agent_o, "[en]") )
			add_assoc_string(return_value, "ln", "en");
		else
			add_assoc_string(return_value, "ln", "other");
	}

	/* if Mozilla */
	else if ((strstr (agent_o, "Gecko") || strstr (agent_o, "Galeon")) && ! strstr (agent_o, "Netscape") ) {
		add_assoc_string (return_value, "br", "MOZL");
		add_assoc_string (return_value, "co", "mozilla");

		/* get user os */
		if ( strstr (agent_o, "NT") )
			add_assoc_string (return_value, "os", "NT");
		else if ( strstr (agent_o, "Win") )
			add_assoc_string (return_value, "os", "WIN");
		else if ( strstr (agent_o, "Linux") )
			add_assoc_string (return_value, "os", "LINUX");
		else
			add_assoc_string (return_value, "os", "OTHER");

		/* get version */
		buf = (char *) kr_regex_replace("/Mozi[^(]+\\([^;]+;[^;]+;[^;]+;[^;]+;([^)]+)\\).*/i","\\1", agent_o);
		{
			char * tbuf = (char *) kr_regex_replace("/rv:| /i", "", buf);
			add_assoc_string (return_value, "vr", tbuf);
			kr_safe_efree (tbuf);
		}
		kr_safe_efree (buf);

		/* get language */
		if (strstr(agent_o, "en-US"))
		{
			add_assoc_string(return_value, "ln", "en");
		}
		else if (strstr(agent_o, "ko-KR"))
		{
			add_assoc_string(return_value, "ln", "ko");
		}
		else
		{
			add_assoc_string(return_value, "ln", "other");
		}
	}

	/* if Konqueror */
	else if ( strstr (agent_o, "Konqueror") ) {
		add_assoc_string (return_value, "br", "KONQ");
		add_assoc_string (return_value, "co", "mozilla");

		/* get user os */
		if ( strstr (agent_o, "Linux") )
			add_assoc_string (return_value, "os", "LINUX");
		else if ( strstr (agent_o, "FreeBSD") )
			add_assoc_string (return_value, "os", "FreeBSD");
		else
			add_assoc_string (return_value, "os", "OTHER");

		/* get version */
		buf = (char *) kr_regex_replace ("/.*Konqueror\\/([0-9.]+).*/i","\\1", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);
	}

	/* if Lynx */
	else if ( strstr (agent_o, "Lynx") ) {
		add_assoc_string (return_value, "br", "LYNX");
		add_assoc_string (return_value, "co", "TextBR");

		/* get version */
		buf = (char *) kr_regex_replace ("/Lynx\\/([^ ]+).*/i","\\1", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);
	}

	/* if w3M */
	else if ( strstr (agent_o, "w3m") ) {
		add_assoc_string (return_value, "br", "W3M");
		add_assoc_string (return_value, "co", "TextBR");

		/* get version */
		buf = (char *) kr_regex_replace ("/w3m\\/([0-9.]+).*/i","\\1", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);
	}

	/* if LINKS */
	else if ( strstr(agent_o, "Links") ) {
		add_assoc_string (return_value, "br", "LINKS");
		add_assoc_string (return_value, "co", "TextBR");

		/* get user os */
		if ( strstr (agent_o, "Linux") )
			add_assoc_string (return_value, "os", "LINUX");
		else if ( strstr (agent_o, "FreeBSD") )
			add_assoc_string (return_value, "os", "FreeBSD");
		else
			add_assoc_string (return_value, "os", "OTHER");

		/* get version */
		buf = (char *) kr_regex_replace ("/Links \\(([^;]+);.*/i","\\1", agent_o);
		add_assoc_string (return_value, "vr", buf);
		kr_safe_efree (buf);
	}

	/* if Netscape */
	else if ( strstr (agent_o, "Mozilla") ) {
		add_assoc_string (return_value, "br", "NS");
		add_assoc_string (return_value, "co", "mozilla");

		/* get user os */
		if ( strstr (agent_o, "NT") )
			add_assoc_string (return_value, "os", "NT");
		else if ( strstr (agent_o, "Win") )
			add_assoc_string (return_value, "os", "WIN");
		else if ( strstr (agent_o, "Linux") )
			add_assoc_string (return_value, "os", "LINUX");
		else
			add_assoc_string (return_value, "os", "OTHER");

		/* get language */
		if ( strstr (agent_o, "[ko]") )
			add_assoc_string (return_value, "ln", "ko");
		else if ( strstr (agent_o, "[en]") )
			add_assoc_string (return_value, "ln", "en");
		else
			add_assoc_string (return_value, "ln", "other");

		/* get version */
		if ( strstr (agent_o, "Gecko") )
			add_assoc_string (return_value, "vr", "6");
		else
			add_assoc_string (return_value, "vr", "4");
	}

	/* other browser */
	else {
		add_assoc_string (return_value, "br", "OTHER");
		add_assoc_string (return_value, "co", "OTHER");
	}
}
/* }}} */

/* {{{ proto string postposition_lib(string str, string postposition, int utf)
 *   make a decision about kreaon postposition */
PHP_FUNCTION(postposition_lib)
{
	char        * str, * chkjosa[2] = { 0, };
	char        * chkstr = NULL, utfpost[4] = { 0, }, post[3] = { 0, };
	UChar         josa[8] = { 0, };
	int           position;

	zend_string * string = NULL;
	zend_string * posts = NULL;
	int           utf = 0, slen = 0, plen = 0;

	if ( kr_parameters ("SS|b", &string, &posts, &utf) == FAILURE )
		return;

	slen = ZSTR_LEN (string);
	plen = ZSTR_LEN (posts);

	if ( slen == 0 || plen == 0 )
		RETURN_EMPTY_STRING ();

	if ( utf == 0 && ! is_utf8 (ZSTR_VAL (string)) )
		utf = 1;

	str = emalloc (sizeof (char) * (slen * 6));

	if ( utf ) {
		XUCodeConv (str, slen * 6, XU_CONV_CP949, ZSTR_VAL (string), slen, XU_CONV_UTF8);
		XUCodeConv ((char *) josa, plen * 6, XU_CONV_CP949, ZSTR_VAL (posts), plen, XU_CONV_UTF8);
	} else {
		memmove (str, ZSTR_VAL (string), slen);
		memmove (josa, ZSTR_VAL (posts), plen);
	}
	memset(str + slen, 0, 1);
	memset(josa + plen, 0, 1);

	/* check korean postposition */
	if ( STRLEN (josa) < 1 )
		php_error (E_ERROR, "Don't exists postposition\n");
	else if ( (josa[0] == 0xc0 && josa[1] == 0xcc) || (josa[0] == 0xb0 && josa[1] == 0xa1) ) {
		chkjosa[0] = "이";
		chkjosa[1] = "가";
	} else if ( (josa[0] == 0xc0 && josa[1] == 0xba) || (josa[0] == 0xb4 && josa[1] == 0xc2) ) {
		chkjosa[0] = "은";
		chkjosa[1] = "는";
	} else if ( (josa[0] == 0xc0 && josa[1] == 0xbb) || (josa[0] == 0xb8 && josa[1] == 0xa6) ) {
		chkjosa[0] = "을";
		chkjosa[1] = "를";
	} else if ( (josa[0] == 0xb0 && josa[1] == 0xfa) || (josa[0] == 0xbf && josa[1] == 0xcd) ) {
		chkjosa[0] = "과";
		chkjosa[1] = "와";
	} else if ( (josa[0] == 0xbe && josa[1] == 0xc6) || (josa[0] == 0xbe && josa[1] == 0xdf) ) {
		chkjosa[0] = "아";
		chkjosa[1] = "야";
	} else {
		if ( utf )
			XUCodeConv ((char *) josa, 6, XU_CONV_UTF8, (char *) josa, 2, XU_CONV_CP949);
		php_error (E_ERROR, "'%s' is not korean postposition.", josa);
	}

	if ( STRLEN (str) > 1) {
		int chkstrlen = STRLEN (str) - 2;
		chkstr = estrdup (str + chkstrlen);
	}
	else if ( STRLEN (str) > 0 )
		chkstr = estrdup (str);
	else
		php_error (E_ERROR, "String is too short.");

	position = get_postposition (chkstr);
	if ( position == 1 )
		memmove (utfpost, chkjosa[1], 3);
	else
		memmove (utfpost, chkjosa[0], 3);

	kr_safe_efree (chkstr);
	kr_safe_efree (str);

	if ( ! utf ) {
		XUCodeConv (post, 12, XU_CONV_CP949, utfpost, 2, XU_CONV_UTF8);
		RETURN_STRING (post);
	} else
		RETURN_STRING(utfpost);
}
/* }}} */

/* {{{ char * autoLink (char * str_o) */
char * autoLink (char * str_o)
{
	#define ARRAY_NO 19
	int agent_o;
	char tmp[1024];
	char file_s[] = "(\\.(gz|tgz|tar|gzip|zip|rar|mpeg|mpg|exe|rpm|dep|rm|ram|asf|ace|viv|avi|mid|gif|jpg|png|bmp|eps|mov)\") target=\"_blank\"";
	char http[] = "(http|https|ftp|telnet|news|mms):\\/\\/(([[:alnum:]\\xA1-\\xFE:_\\-]+\\.[[:alnum:]\\xA1-\\xFE,:;&#=_~%\\[\\]?\\/.,+\\-]+)([.]*[\\/a-z0-9\\[\\]]|=[\\xA1-\\xFE]+))";
	char mail[] = "([[:alnum:]\\xA1-\\xFE_.-]+)@([[:alnum:]\\xA1-\\xFE_-]+\\.[[:alnum:]\\xA1-\\xFE._-]*[a-z]{2,3}(\\?[[:alnum:]\\xA1-\\xFE=&\\?]+)*)";
	char u_http[] = "(http|https|ftp|telnet|news|mms):\\/\\/(([[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}:_\\-]+\\.[[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF},:;&#=_~%\\[\\]?\\/.,+\\-]+)([.]*[\\/a-z0-9\\[\\]]|=[\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}]+))";
	char u_mail[] = "([[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}_.-]+)@([[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}_-]+\\.[[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}._-]*[a-z]{2,3}(\\?[[:alnum:]\\x{1100}-\\x{11FF}\\x{3130}-\\x{318F}\\x{AC00}-\\x{D7AF}=&\\?]+)*)";
	char * src[ARRAY_NO], * tar[ARRAY_NO];
	char * buf, * ptr;
	int    utf8 = 0;

	ptr = get_useragent ();
	agent_o = ptr ?
		strstr(ptr, "MSIE") ? 1 : 0 : 0;

	if ( ! is_utf8 (str_o) )
		utf8 = 1;

	/* &lt; 로 시작해서 3줄뒤에 &gt; 가 나올 경우와
	 * IMG tag 와 A tag 의 경우 링크가 여러줄에 걸쳐 이루어져 있을 경우
	 * 이를 한줄로 합침 (합치면서 부가 옵션들은 모두 삭제함) */
	src[0] = "/<([^<>\\n]*)\\n([^<>\\n]\\+)\\n([^<>\\n]*)>/i";
	tar[0] = "<\\1\\2\\3>";
	src[1] = "/<([^<>\\n]*)\\n([^\\n<>]*)>/i";
	tar[1] = "<\\1\\2>";

	memset (tmp, 0, sizeof (tmp));
	if ( utf8 ) {
		sprintf (
			tmp,
			"/<(a|img)[^>=]*(href|src)[^=]*=[ '\"\\n]*(%s|mailto:%s)[^>]*>/iu",
			u_http, u_mail
		);
	} else {
		sprintf (
			tmp,
			"/<(a|img)[^>=]*(href|src)[^=]*=[ '\"\\n]*(%s|mailto:%s)[^>]*>/i",
			http, mail
		);
	}
	src[2] = estrdup (tmp);
	tar[2] = "<\\1 \\2=\"\\3\">";

	/* email 형식이나 URL 에 포함될 경우 URL 보호를 위해 @ 을 치환 */
	src[3] = "/(http|https|ftp|telnet|news|mms):\\/\\/([^ \\n@]+)@/i";
	tar[3] = "\\1://\\2_HTTPAT_\\3";

	/* 특수 문자를 치환 및 html사용시 link 보호 */
	src[4] = "/&(quot|gt|lt)/i";
	tar[4] = "!\\1";

	memset (tmp, 0, sizeof (tmp));
	if ( utf8 )
		sprintf (tmp, "/<a([^>]*)href=[\"' ]*(%s)[\"']*[^>]*>/iu", u_http);
	else
		sprintf (tmp, "/<a([^>]*)href=[\"' ]*(%s)[\"']*[^>]*>/i", http);
	src[5] = estrdup (tmp);
	tar[5] = "<a\\1href=\"\\3_orig://\\4\" target=\"_blank\">";

	memset (tmp, 0, sizeof (tmp));
	if ( utf8 )
		sprintf (tmp, "/href=[\"' ]*mailto:(%s)[\"']*>/iu", u_mail);
	else
		sprintf (tmp, "/href=[\"' ]*mailto:(%s)[\"']*>/i", mail);
	src[6] = estrdup (tmp);
	tar[6] = "href=\"mailto:\\2#-#\\3\">";

	memset (tmp, 0, sizeof (tmp));
	if ( utf8 )
		sprintf (tmp, "/<([^>]*)(background|codebase|src)[ \\n]*=[\\n\"' ]*(%s)[\"']*/iu", u_http);
	else
		sprintf (tmp, "/<([^>]*)(background|codebase|src)[ \\n]*=[\\n\"' ]*(%s)[\"']*/i", http);
	src[7] = estrdup (tmp);
	tar[7] = "<\\1\\2=\"\\4_orig://\\5\"";

	/* 링크가 안된 url및 email address 자동링크 */
	memset (tmp, 0, sizeof (tmp));
	if ( utf8 )
		sprintf (tmp, "/((src|href|base|ground)[ ]*=[ ]*|[^=]|^)(%s)/iu", u_http);
	else
		sprintf (tmp, "/((src|href|base|ground)[ ]*=[ ]*|[^=]|^)(%s)/i", http);
	src[8] = estrdup (tmp);
	tar[8] = "\\1<a href=\"\\3\" target=\"_blank\">\\3</a>";

	memset (tmp, 0, sizeof (tmp));
	if ( utf8 )
		sprintf (tmp, "/(%s)/iu", u_mail);
	else
		sprintf (tmp, "/(%s)/i", mail);
	src[9] = estrdup (tmp);
	tar[9] = "<a href=\"mailto:\\1\">\\1</a>";
	src[10] = "/(<a href=[^>]+>)<a href=[^>]+>/i";
	tar[10] = "\\1";
	src[11] = "/<\\/A><\\/A>/i";
	tar[11] = "</a>";

	/* 보호를 위해 치환한 것들을 복구 */
	src[12] = "/!(quot|gt|lt)/i";
	tar[12] = "&\\1";
	src[13] = "/(http|https|ftp|telnet|news|mms)_orig/i";
	tar[13] = "\\1";
	src[14] = "'#-#'";
	tar[14] = "@";

	memset (tmp, 0, sizeof (tmp));
	sprintf (tmp, "/%s/i", file_s);
	src[15] = estrdup (tmp);
	tar[15] = "\\1";

	/* email 주소를 변형한 뒤 URL 속의 @ 을 복구 */
	src[16] = "/_HTTPAT_/";
	tar[16] = "@";

	/* 이미지에 보더값 0 을 삽입 */
	src[17] = "/<(img src=\"[^\"]+\")>/i";
	tar[17] = "<\\1 border=0>";

	/* IE 가 아닌 경우 embed tag 를 삭제함 */
	if(agent_o != 1) {
		src[18] = "/<(embed[^>]*)>/i";
		tar[18] = "&lt;\\1&gt;";
	} else {
		src[18] = "/oops_php_lib_no_action/i";
		tar[18] = "";
	}
	
	buf = kr_regex_replace_arr (src, tar, str_o, ARRAY_NO);

	kr_safe_efree (src[2]);
	kr_safe_efree (src[5]);
	kr_safe_efree (src[6]);
	kr_safe_efree (src[7]);
	kr_safe_efree (src[8]);
	kr_safe_efree (src[9]);
	kr_safe_efree (src[15]);

	return buf;
}
/* }}} */

/* {{{ char * get_useragent (void) */
char * get_useragent ()
{
	char *ptr;
	TSRMLS_FETCH();
	
	ptr = sapi_getenv ("HTTP_USER_AGENT", 15 TSRMLS_CC);
	if ( ! ptr ) ptr = getenv ("HTTP_USER_AGENT");
	if ( ! ptr ) ptr = get_serverenv ("HTTP_USER_AGENT");
	if ( !ptr ) ptr = "";

	return ptr;
}
/* }}} */

/* {{{ char * get_serverenv (char *para) */
char * get_serverenv (char * para)
{
	zval *data, *tmp, tmps;
	zend_string *string_key;
	zend_ulong num_key;
	char *parameters = NULL;

	zend_string * keyname;

	keyname = zend_string_init ("_SERVER", 7, 0);

	if ( (data = zend_hash_find (&EG(symbol_table), keyname)) != NULL ) {
		ZEND_HASH_FOREACH_KEY_VAL(Z_ARRVAL_P(data), num_key, string_key, tmp) {
			if ( string_key ) {
				if ( strcasecmp (ZSTR_VAL (string_key), para) == 0 ) {
					ZVAL_COPY_VALUE (&tmps, tmp);
					if ( Z_TYPE(tmps) != IS_STRING ) {
						tmp = NULL;
						zval_copy_ctor (&tmps);
						convert_to_string (&tmps);
					}

					parameters = Z_STRVAL (tmps);
					if ( !tmp )
						zval_dtor (&tmps);
					break;
				}
			}
		} ZEND_HASH_FOREACH_END();
	}

	zend_string_release (keyname);

	if ( parameters == NULL )
		parameters = "";

	return parameters;
}
/* }}} */

/* {{{ int get_postposition (char *str)
 *
 * This range is EUC-KR!
 */
int get_postposition (char * ss)
{
	UChar * str;
	UChar   first, second;
	int     no;

	str = (UChar *) ss;

	first = tolower (str[0]);
	no = (STRLEN (str) > 1) ? 1 : 0;
	second = tolower (str[no]);

	/* if 한 */
	if ( first & 0x80 ) {
		unsigned int ncr;
		UChar c1, c2;
		c1 = (UChar) str[0];
		c2 = (UChar) str[1];

		if ( c2 > 0x7a ) c2 -= 6;
		if ( c2 > 0x5a ) c2 -= 6;
		ncr = (c1 - 0x81) * 178 + (c2 - 0x41);

		return ((table_ksc5601[ncr] - 16) % 28 == 0) ? 1 : 0;
	}
	/* number area */
	else if ( second > 47 && second < 58 ) {
		return (first == 50 || first == 52 || first == 53 || first == 57) ? 0 : 1;
	}
	/* only 1 charactor */
	else if ( first == second ) {
		return (first == 114 || (first > 108 && first < 111)) ? 0 : 1;
	}
	/* if 한a */
	else if ( first != second && first > 127 && (second < 123 && second > 96) )
	{
		return (second == 114 || (second > 108 && second < 111)) ? 0 : 1;
	}
	/* if aa */
	else
	{
		/* last charactor is a or e or i or o or u or w or y  */
		if ( second == 0x61 || second == 0x65 || second == 0x69 || second == 0x6f ||
				second == 0x75 || second == 0x77 || second == 0x79 ) {
			return 1;
		}
		/* last charactor is ed or er or or */
		else if ( (first == 0x65 && first == 0x72) || (first == 0x6f && first == 0x72) ||
				(first == 0x65 && first == 0x64) ) {
			return 1;
		} else
			return 0;
	}
	return 0;
}
/* }}} */

/* {{{ char * strtrim ( char *str ) */
char * strtrim (char * str) {
	int     len, i;
	char * str_r;

	len = strlen (str);
	str_r = emalloc (sizeof (char *) * (len + 1));
	memset (str_r, 0, sizeof (char *) * (len + 1));

	for ( i = 0; i < len; i++ ) {
		if ( ! isspace (str[i]) ) {
			strcpy ((char *) str_r, (char *) str + i);
			break;
		}
	}

	len = strlen (str_r);

	for ( i = len - 1; i>-1; i-- ) {
		if ( isspace (str_r[i]) ) {
			memset (str_r + i, 0, 1);
		} else {
			break;
		}
	}

	return str_r;
}
/* }}} */

/* {{{ void kr_safe_efree ( void * str ) */
void kr_safe_efree ( void * str ) {
	if ( str != NULL )
		efree (str);
	str = NULL;
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
