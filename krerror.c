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
#include "php_krerror.h"
#include "php_krparse.h"
#include "krregex.h"
#include "SAPI.h"

#include "php_kr.h"

/* {{{ proto void perror_lib(string str [, int use_java [, string move_page [, int move second ] ] ])
 *  *  print error message */
PHP_FUNCTION(perror_lib)
{
	zend_string * input = NULL;
	zend_string * move  = NULL;
	zend_bool     java  = 0;
	zend_long     sec   = 5;
	char        * istr;
	char        * mstr;
	char        * ret;

	if ( kr_parameters ("S|bSl", &input, &java, &move, &sec) == FAILURE )
		return;

	istr = ! ZSTR_LEN (input) ? "Problem in your request!" : ZSTR_VAL (input);
	mstr = ! move ? "1" : ZSTR_VAL (move);

	ret = print_error (istr, java, mstr, sec);
	php_printf ("%s\n", ret);
	kr_safe_efree (ret);
}
/* }}} */

/* {{{ proto void pnotice_lib(string str, int use_java)
 *  *  print notice */
PHP_FUNCTION(pnotice_lib)
{
	zend_string * input = NULL;
	int           java  = 0;
	char        * ret;

	if ( kr_parameters ("S|b", &input, &java) == FAILURE )
		return;

	if ( ZSTR_VAL (input) == 0 ) {
		php_error (E_ERROR, "Can't use empty value of 1st argument");
		RETURN_FALSE;
	}

	ret = print_error (ZSTR_VAL (input), java, "notice", 0);
	php_printf ("%s\n", ret);
	kr_safe_efree (ret);
}
/* }}} */

char * print_error (char * str_o, int java_o, char * move_o, int sec_o)
{
	int    textBR = 0;
	char * buf_str = NULL,
		 * buf_move = NULL;
	char * reg[2] = { "/\n/i", "/'|#/i" };
	char * rep[2] = { "\\n", "\\\\\\0" };

	char * buf = NULL,
		 * mv = NULL;
	char * result,
		 * agent_o = NULL;

	agent_o = sapi_getenv ("HTTP_USER_AGENT", 15 TSRMLS_CC);
	if ( agent_o == NULL )
		agent_o = estrdup (get_serverenv ("HTTP_USER_AGENT"));

	/* text browser check */
	if ( STRLEN (agent_o) > 0 )
		if ( strstr (agent_o, "Lynx") || strstr (agent_o, "Links") || strstr (agent_o, "w3m"))
			textBR = 1;

	if ( java_o == 0 || textBR == 1 || STRLEN (agent_o) == 0 ) {
		buf = emalloc (sizeof (char) * (STRLEN (str_o) + 2));
		sprintf (buf, "%s\n", str_o);
		if ( strcmp (move_o, "notice") && strcmp (move_o, "1")) {
			buf_move = kr_regex_replace ("/ /i", "%20", move_o);
			mv = emalloc (sizeof (char) * (STRLEN (buf_move) + 60));
			sprintf (mv, "<meta http-equiv=\"refresh\" content=\"%d; url=%s\">\n", sec_o, buf_move);
			mv[STRLEN (mv)] = '\0';
			efree (buf_move);

			result = emalloc (sizeof (char) * (STRLEN (buf) + STRLEN (mv) + 3));
			sprintf (result, "%s\n%s\n", buf, mv);
			kr_safe_efree (mv);
		} else {
			result = emalloc (sizeof (char) * (STRLEN (buf) + 2));
			sprintf (result, "%s\n", buf);
		}
	} else {
		buf_str = kr_regex_replace_arr ( reg,  rep, str_o, 2);
		
		buf = emalloc (sizeof (char) * (STRLEN (buf_str) + 60));
		if ( ! strcmp (move_o, "1") )
		{
			sprintf (buf, "<script type=\"javascript\">\n\talert('%s');\nhistory.back();\n</script>\n", buf_str);

			result = emalloc (sizeof (char) * (STRLEN (buf) + 2));
			sprintf (result, "%s\n", buf);
		}
	   	else
	   	{
			sprintf (buf, "<script type=\"javascript\">\n\talert('%s')\n</script>\n", buf_str);
			if ( strcmp (move_o, "notice") ) {
				buf_move = kr_regex_replace ("/ /i", "%20", move_o);
				mv = emalloc (sizeof (char) * (STRLEN (buf_move) + 50));
				sprintf (mv, "<meta http-equiv=\"refresh\" content=\"%d; url=%s\">\n", sec_o, buf_move);
				mv[STRLEN (mv)] = '\0';
				efree (buf_move);

				result = emalloc (sizeof (char) * (STRLEN (buf) + STRLEN (mv) + 3));
				sprintf (result,"%s\n%s\n", buf, mv);
				kr_safe_efree (mv);
			} else {
				result = emalloc (sizeof (char) * (STRLEN (buf) + 2));
				sprintf (result, "%s\n", buf);
			}
		}
		kr_safe_efree (buf_str);
	}

	kr_safe_efree (agent_o);
	kr_safe_efree (buf);

	return result;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

