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

  $Id: krerror.c,v 1.5 2002-08-10 07:13:08 oops Exp $ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "php_krerror.h"
#include "SAPI.h"

/* {{{ proto void perror_lib(string str [, int use_java [, string move_page [, int move second ] ] ])
 *  *  print error message */
PHP_FUNCTION(perror_lib)
{
	pval **str, **java, **move, **seconds;
	unsigned int ujava = 0, sec = 5;
	unsigned char *err, *umove;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(1, &str) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			ujava = 0;
			umove = "1";
			break;
		case 2:
			if(zend_get_parameters_ex(2, &str, &java) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(java);
			ujava = Z_LVAL_PP(java);
			umove = "1";
			break;
		case 3:
			if(zend_get_parameters_ex(3, &str, &java, &move) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(java);
			convert_to_string_ex(move);

			ujava = Z_LVAL_PP(java);
			umove = Z_STRVAL_PP(move);

			if ( strlen(umove) == 0 ) umove = "1";
			break;
		case 4:
			if(zend_get_parameters_ex(4, &str, &java, &move, &seconds) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(java);
			convert_to_string_ex(move);
			convert_to_long_ex(seconds);

			ujava = Z_LVAL_PP(java);
			umove = Z_STRVAL_PP(move);
			sec   = Z_LVAL_PP(seconds);

			if ( strlen(umove) == 0 ) umove = "1";
			sec = !sec ? 5 : sec;
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if (Z_STRLEN_PP(str) == 0)
   	{
		err = "Problem in your request!";
	}
   	else
   	{
		err = Z_STRVAL_PP(str);
	}

	php_printf("%s\n", print_error(err, ujava, umove, sec));
	zend_bailout();
}
/* }}} */

/* {{{ proto void pnotice_lib(string str, int use_java)
 *  *  print notice */
PHP_FUNCTION(pnotice_lib)
{
	pval **str, **java;
	unsigned int ujava = 0;

	switch(ZEND_NUM_ARGS())
	{
		case 1:
			if(zend_get_parameters_ex(1, &str) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &str, &java) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(java);
			ujava = Z_LVAL_PP(java);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(str);

	if (Z_STRLEN_PP(str) == 0) {
		php_error (E_ERROR, "Can't use null value of argument 1\n");
		RETURN_FALSE;
	}

	php_printf("%s", print_error(Z_STRVAL_PP(str), ujava, "notice", 0));
}
/* }}} */

unsigned char *print_error (unsigned char *str_o, unsigned int java_o, unsigned char *move_o, unsigned int sec_o)
{
	unsigned int textBR = 0;
	unsigned char *buf_str, *buf_move;
	unsigned char *reg[2] = { "/\n/i", "/'|#/i" };
	unsigned char *rep[2] = { "\\n", "\\\\\\0" };

	unsigned char *buf = NULL, *mv = NULL;
	unsigned char *result, *ret, *agent_o;
	TSRMLS_FETCH();

	agent_o = sapi_getenv("HTTP_USER_AGENT", 15 TSRMLS_CC);
	if (agent_o == NULL) { agent_o = (unsigned char *) get_serverenv("HTTP_USER_AGENT"); }

	/* text browser check */
	if( strlen(agent_o) > 0) {
		if (strstr(agent_o, "Lynx") || strstr(agent_o, "Links") || strstr(agent_o, "w3m"))
		{
			textBR = 1;
		}
	}

	if ( java_o == 0 || textBR == 1 || strlen(agent_o) == 0 )
   	{
		buf = emalloc(sizeof(char) * (strlen(str_o) + 2));
		sprintf(buf, "%s\n", str_o);
		if (strcmp(move_o, "notice") && strcmp(move_o, "1"))
		{
			buf_move = (unsigned char *) kr_regex_replace ("/ /i", "%20", move_o);
			mv = emalloc(sizeof(char) * (strlen(buf_move) + 60));
			sprintf(mv, "<META http-equiv=\"refresh\" content=\"%d;URL=%s\">\n", sec_o,buf_move);
			mv[strlen(mv)] = '\0';

			result = emalloc(sizeof(char) * (strlen(buf) + strlen(mv) + 3));
			sprintf (result, "%s\n%s\n", buf, mv);
			efree(mv);
		}
	   	else
	   	{
			result = emalloc(sizeof(char) * (strlen(buf) + 2));
			sprintf (result, "%s\n", buf);
		}
	} else {
		buf_str = (unsigned char *) kr_regex_replace_arr (reg, rep, str_o, 2);
		
		buf = emalloc(sizeof(char) * (strlen(buf_str) + 60));
		if (!strcmp(move_o, "1"))
		{
			sprintf(buf, "<SCRIPT>\nalert('%s');\nhistory.back();\n</SCRIPT>\n", buf_str);

			result = emalloc(sizeof(char) * (strlen(buf) + 2));
			sprintf (result, "%s\n", buf);
		}
	   	else
	   	{
			sprintf(buf, "<SCRIPT>\nalert('%s')\n</SCRIPT>\n", buf_str);
			if ( strcmp(move_o,"notice")) {
				buf_move = (unsigned char *) kr_regex_replace ("/ /i","%20", move_o);
				mv = emalloc(sizeof(char) * (strlen(buf_move) + 50));
				sprintf(mv, "<META http-equiv=\"refresh\" content=\"0;URL=%s\">\n", buf_move);
				mv[strlen(mv)] = '\0';

				result = emalloc(sizeof(char) * (strlen(buf) + strlen(mv) + 3));
				sprintf (result,"%s\n%s\n", buf, mv);
				efree(mv);
			} else {
				result = emalloc(sizeof(char) * (strlen(buf) + 2));
				sprintf (result, "%s\n", buf);
			}
		}
	}

	ret = result;
	efree(buf);
	efree(result);

	return ret;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

