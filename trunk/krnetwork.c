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

  $Id: krnetwork.c,v 1.11 2002-08-09 11:41:08 oops Exp $
*/

/*
 * PHP4 Korean String modue "korean" - only korean
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "SAPI.h"
#include "fopen_wrappers.h"
#include "ext/standard/dns.c"

#include "php_krnetwork.h"

#define PROXYSIZE 7

/* {{{ proto string get_hostname_lib (int reverse [, string addr ])
 *    Return hostname or ip address */
PHP_FUNCTION(get_hostname_lib)
{
	pval **reverse, **addr;
	unsigned int i;
	char *tmphost = NULL, *host, *check, *ret;
	char *proxytype[PROXYSIZE]  = { "HTTP_VIA", "HTTP_X_COMING_FROM", "HTTP_X_FORWARDED_FOR",
									"HTTP_X_FORWARDED", "HTTP_COMING_FROM", "HTTP_FORWARDED_FOR",
									"HTTP_FORWARDED" };

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &reverse) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &reverse, &addr) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(addr);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(reverse);

	if ( ZEND_NUM_ARGS() == 1 )
   	{
		for ( i = 0; i < PROXYSIZE; i++ )
	   	{
#ifdef PHP_WIN32
			if (getenv(proxytype[i]) != NULL)
		   	{
				tmphost = (char *) emalloc(strlen(getenv(proxytype[i])) + 1);
				tmphost = getenv(proxytype[i]);
#else
			if (sapi_module.getenv(proxytype[i], strlen(proxytype[i]) TSRMLS_CC) != NULL)
		   	{
				tmphost = (char *) emalloc(strlen(sapi_module.getenv(proxytype[i], strlen(proxytype[i]) TSRMLS_CC)) + 1);
				tmphost = sapi_module.getenv(proxytype[i], strlen(proxytype[i]) TSRMLS_CC);
#endif
				break;
			}
		}

		if ( tmphost == NULL )
	   	{
#ifdef PHP_WIN32
			host = getenv("REMOTE_ADDR");
#else
			host = sapi_module.getenv("REMOTE_ADDR", 11 TSRMLS_CC);
#endif
		}
	   	else
	   	{
			host = tmphost;
			efree(tmphost);
		}
	}
	else
   	{
		if ( Z_STRLEN_PP(addr) != 0 ) { host = Z_STRVAL_PP(addr); }
	   	else
	   	{
			php_error(E_WARNING,"address is null value");
			RETURN_FALSE;
		}
	}

	check = Z_LVAL_PP(reverse) ? php_gethostbyaddr(host) : NULL ;
	ret = check ? check : host ;

	RETURN_STRING(ret, 1);
}
/* }}} */

/* {{{ proto string readfile_lib(string filename)
 *    Return a file or a URL */
PHP_FUNCTION(readfile_lib)
{
	zval **arg1, **arg2;
	FILE *fp;
	int use_include_path=0;
	int issock=0, socketd=0;
	int rsrc_id, len, i=0;

	char buf[8192], *tmpstr = NULL, *ret;

	/* check args */
	switch (ZEND_NUM_ARGS())
   	{
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(arg2);
			use_include_path = Z_LVAL_PP(arg2);
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	/*
	 * We need a better way of returning error messages from
	 * php_fopen_wrapper().
	 */
	fp = php_fopen_wrapper(Z_STRVAL_PP(arg1), "rb", use_include_path | ENFORCE_SAFE_MODE, &issock, &socketd, NULL TSRMLS_CC);
	if (!fp && !socketd)
   	{
		if (issock != BAD_URL)
	   	{
			char *tmp = estrndup(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1));
			php_strip_url_passwd(tmp);
			php_error(E_WARNING, "readfile_lib(\"%s\") - %s", tmp, strerror(errno));
			efree(tmp);
		}
		RETURN_FALSE;
	}

	if (issock)
   	{
		int *sock = emalloc(sizeof(int));
		*sock = socketd;
		rsrc_id = ZEND_REGISTER_RESOURCE(NULL, sock, php_file_le_socket());
	}
   	else
   	{
		rsrc_id = ZEND_REGISTER_RESOURCE(NULL, fp, php_file_le_fopen());
	}

	while (1)
   	{
		if ((len = FP_FREAD(buf, 8190, socketd, fp, issock)) < 1) { break; }
		buf[len] = '\0';

		if ( tmpstr == NULL )
	   	{
			tmpstr = emalloc(sizeof(char) * len + 1);
			strcpy(tmpstr, buf);
		}
	   	else
	   	{
			tmpstr = erealloc(tmpstr, sizeof(char) * (strlen(tmpstr) + len + 1));
			strcat(tmpstr, buf);
		}
	}

	zend_list_delete(rsrc_id);

	if (tmpstr != NULL)
   	{
		ret = estrndup(tmpstr, strlen(tmpstr));
		efree(tmpstr);
		RETURN_STRING(ret, 1);
	}
}
/* }}} */

/* {{{ proto int sockmail_lib(string mail [, string from [, string to [, int debug ] ] ])
 *    Return a file or a URL */
PHP_FUNCTION(sockmail_lib)
{
	zval **mail, **from, **to, **debugs;
	unsigned char delimiters[] = ",";
	unsigned char *text, *faddr, *taddr, *tmpfrom, *tmpto, *mailaddr;
	int sock, debug = 0, len = 0, failcode = 0, error_no = 0;

	struct sockaddr_in sinfo;
	struct hostent *hostinfo;

	/* {{{ check args */
	switch (ZEND_NUM_ARGS())
   	{
		case 1:
			if (zend_get_parameters_ex(ZEND_NUM_ARGS(), &mail) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			tmpfrom = "";
			tmpto = "";
			break;
		case 2:
			if (zend_get_parameters_ex(ZEND_NUM_ARGS(), &mail, &from) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}

			/* get from address */
			convert_to_string_ex(from);
			tmpfrom = Z_STRVAL_PP(from);
			/* get to address */
			tmpto = "";
			break;
		case 3:
			if (zend_get_parameters_ex(ZEND_NUM_ARGS(), &mail, &from, &to) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			/* get from address */
			convert_to_string_ex(from);
			tmpfrom = Z_STRVAL_PP(from);
			/* get to address */
			convert_to_string_ex(to);
			tmpto = Z_STRVAL_PP(to);
			break;
		case 4:
			if (zend_get_parameters_ex(ZEND_NUM_ARGS(), &mail, &from, &to, &debugs) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			/* get from address */
			convert_to_string_ex(from);
			tmpfrom = Z_STRVAL_PP(from);
			/* get to address */
			convert_to_string_ex(to);
			tmpto = Z_STRVAL_PP(to);
			/* get bebug information */
			convert_to_long_ex(debugs);
			debug = Z_LVAL_PP(debugs);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	/* }}} */

	/* mail context */
	convert_to_string_ex(mail);
	text = Z_STRVAL_PP(mail);

	if (strlen(tmpfrom) < 1)
	{
		unsigned char *src[3] = { "/\r*\n/i", "/.*From:([^!]+)!!ENTER!!.*/i", "/.*<([^>]+)>/i" };
		unsigned char *des[3] = { "!!ENTER!!", "\\1", "\\1" };

		faddr = (unsigned char *) kr_regex_replace_arr(src, des, text, (sizeof (src) / sizeof (src[0])));
	}
   	else
	{
		unsigned char *src = "/.*<([^>]+)>/i";
		unsigned char *des = "\\1";
		faddr = (unsigned char *) kr_regex_replace(src, des, tmpfrom);
	}

	if (strlen(tmpto) < 1)
	{
		unsigned char *src[2] = { "/\r*\n/i", "/.*To:([^!]+)!!ENTER!!.*/i" };
		unsigned char *des[2] = { "!!ENTER!!", "\\1" };
		taddr = (unsigned char *) kr_regex_replace_arr(src, des, text, (sizeof (src) / sizeof (src[0])));
	}
	else
	{
		taddr = estrdup(tmpto);
	}

	if ( (mailaddr = strtok(taddr, delimiters)) != NULL ) {
		do {
			unsigned char *src[3] = { "/[^<]*</i", "/>.*/i", "[\\s]" };
			unsigned char *des[3] = { "", "", "" };
			unsigned char *mailserver;

			error_no++;
			mailserver = (unsigned char *) kr_regex_replace_arr(src, des, mailaddr, (sizeof (src) / sizeof (src[0])));
			if (sock_sendmail(faddr, mailserver, text, debug) == 1) { RETURN_LONG(error_no); }
		} while ( (mailaddr = strtok(NULL, delimiters)) != NULL );
	}
	error_no = 0;

	RETURN_LONG(error_no);
}
/* }}} */

unsigned char *get_mx_record(unsigned char *str)
{
	u_char answer[8192], *cp, *end;
	u_short type, weight, tmpweight;
	unsigned char *tmphost, *host, tmpmx[256], *mxrecord = "", *retaddr;
	unsigned int i, qdc, count;
	HEADER *hp;


	if ( (tmphost = strrchr(str, '@')) != NULL )
	{
		host = estrdup(&str[tmphost - str + 1]);
	}
	else
	{
		host = estrdup(str);
	}

	/* if don't exist mx record */
	if ( (i = res_search(host, C_IN, T_MX, answer, sizeof(answer))) < 0 ) { return host; }

	if ( i > sizeof(answer) ) { i = sizeof(answer); }

	hp = (HEADER *) &answer;
	cp = (u_char *) &answer + 12;
	end = (u_char *) &answer + i;

	for ( qdc = ntohs((unsigned short) hp->qdcount); qdc--; cp += i + 4)
	{
		if ( (i = dn_skipname(cp, end)) < 0 )
		{
			return host;
		}
	}

	/* see also dnsmxrr() function */
	count = ntohs((unsigned short) hp->ancount);
	while ( --count >= 0 && cp < end )
	{
		if ( (i = dn_skipname(cp, end)) < 0 )
		{
			return host;
			break;
		}
		cp += i;
		GETSHORT(type, cp);
		cp += 6;
		GETSHORT(i, cp);
		if ( type != T_MX )
		{
			cp += i;
			continue;
		}
		GETSHORT(tmpweight, cp);
		if ( (i = dn_expand(answer, end, cp, tmpmx, sizeof(tmpmx)-1)) < 0 ) {
			return host;
			break;
		}
		cp += i;

		if ( strlen(mxrecord) < 1 )
		{
			mxrecord = estrdup(tmpmx);
			weight = tmpweight;
		}
		else
		{
			if ( weight > tmpweight )
			{
				weight = tmpweight;
				mxrecord = realloc(mxrecord, sizeof(char) * (strlen(tmpmx) + 1));
				strcpy(mxrecord, tmpmx);
			}
		}
	}

	if ( strlen(mxrecord) < 1 ) { return host; }
	else
	{
		retaddr = estrdup(mxrecord);
		efree(mxrecord);
		return retaddr;
	}
}

int socksend (int sock, int deb, unsigned char *var, unsigned char *target)
{
	unsigned char *cmd, msg[1024];
	int rlen = 0, failed = 1, bar = 0, add = 0, tmplen = strlen(var);

	if ( !strcasecmp(target, "body") ) { add = 6; }
	else if ( !strcasecmp(target, "mail")) { add = 14; }
	else if ( !strcasecmp(target, "rcpt")) { add = 12; }
	else { add = 3; }

	{
		unsigned char tmpcmd[tmplen + add];

		if ( !strcasecmp (target, "mail") ) { sprintf(tmpcmd, "MAIL From: %s\r\n", var); }
		else if ( !strcasecmp (target, "rcpt") ) { sprintf(tmpcmd, "RCPT To: %s\r\n", var); }
		else if ( !strcasecmp (target, "body") ) { sprintf(tmpcmd, "%s\r\n.\r\n", var); }
		else { sprintf(tmpcmd, "%s\r\n", var); }

		tmpcmd[tmplen + add] = '\0';
		cmd = (unsigned char *) estrdup(tmpcmd);
	}

	/* print debug information */
	debug_msg(cmd, deb, 0);

	send (sock, cmd, strlen(cmd), 0);
	rlen = recv (sock, msg, 1024, 0);
	if ( !strncmp(msg, "220", 3) || !strncmp(msg, "221", 3) || !strncmp(msg, "250", 3)
		 || !strncmp(msg, "251", 3) || !strncmp(msg, "354", 3) )
	{
		failed = 0;
	}

	msg[rlen] = '\0';
	debug_msg (msg, deb, 0);
	if ( deb != 0 && !strcasecmp (target, "quit") ) { php_printf("\r\n"); }

	return failed;
}

void debug_msg (unsigned char *msg, int info, int bar)
{
	if ( info != 0 )
	{
		php_printf("DEBUG: %s", msg);
		if ( bar != 0 )
		{
			php_printf("----------------------------------------------------------------\n");
		}
	}
}


int sock_sendmail (unsigned char *fromaddr, unsigned char *toaddr, unsigned char *text, int debug)
{
	int len, sock, failcode;
	unsigned char *addr;
	struct sockaddr_in sinfo;
	struct hostent *hostinfo;

	/* get mx record from 'to address' */
	addr = get_mx_record(toaddr);

	if ( !(hostinfo = gethostbyname(addr)) )
	{
		if ( debug == 1 )
		{
			php_error(E_WARNING, "host name \"%s\" not found\n", addr);
		}
		return 1;
	}

	/* specify connect server information */
	sinfo.sin_family = AF_INET;
	sinfo.sin_port = ntohs(25);
	sinfo.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
	len = sizeof(sinfo);

	/* create socket */
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		if ( debug == 1 )
		{
			php_error(E_WARNING, "Failed to create socket\n");
		}
		return 1;
	}

	/* connect to server in 25 port */
	if ( connect (sock, (struct sockaddr *) &sinfo, len) == -1 )
	{
		if ( debug == 1 )
		{
			close (sock);
			php_error(E_WARNING, "Failed connect %s\n", addr);
		}
		return 1;
	}
	else
	{
		unsigned int recvlen = 0;
		unsigned char recvmsg[1024];

		recvlen = recv (sock, recvmsg, 1024, 0);
		recvmsg[recvlen] = '\0';
		if (debug == 1)
		{
			php_printf("\r\nConnect %s Start\r\n", addr);
			php_printf("----------------------------------------------------------------\r\n\r\n");
			php_printf("DEBUG: %s\r\n", strtrim(recvmsg), recvlen);
		}
	}

	failcode = socksend (sock, debug, "HELO localhost", "helo");
    if ( failcode == 1 )
   	{
	   	close(sock);
	   	return 1;
   	}
	failcode = socksend (sock, debug, fromaddr, "mail");
	if ( failcode == 1 )
   	{
	   	close(sock);
	   	return 1;
   	}
	failcode = socksend (sock, debug, toaddr, "rcpt");
	if ( failcode == 1 )
   	{
	   	close(sock);
	   	return 1;
   	}
	failcode = socksend (sock, debug, "data", "data");
	if ( failcode == 1 )
   	{
	   	close(sock);
	   	return 1;
   	}
	failcode = socksend (sock, debug, text, "body");
	if ( failcode == 1 )
   	{
	   	close(sock);
	   	return 1;
   	}
	failcode = socksend (sock, debug, "quit", "quit");
	if ( failcode == 1 )
   	{
	   	close(sock);
	   	return 1;
   	}

	close(sock);
	return 0;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
