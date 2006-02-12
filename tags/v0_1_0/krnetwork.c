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

  $Id: krnetwork.c,v 1.46 2006-02-12 05:46:09 oops Exp $
*/

/*
 * PHP4 Korean String modue "korean" - only korean
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <time.h>

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"

#if HAVE_SYS_SOCKET_H
	#include <sys/socket.h>
#endif
#ifdef PHP_WIN32
	#include <winsock.h>
	#if HAVE_LIBBIND
		#ifndef WINNT
			#define WINNT 1
		#endif
		/* located in www.php.net/extra/bindlib.zip */
		#include "arpa/inet.h"
		#include "netdb.h"
		#include "arpa/nameser.h"
		#include "resolv.h"
	#endif
#else
	#include <netinet/in.h>
	#if HAVE_ARPA_INET_H
		#include <arpa/inet.h>
	#endif
	#include <netdb.h>
	#if HAVE_ARPA_NAMESER_H
		#include <arpa/nameser.h>
	#else
		typedef struct {
			unsigned	id :16;	/* query identification number */
			#if BYTE_ORDER == BIG_ENDIAN
					/* fields in third byte */
			unsigned	qr: 1;		/* response flag */
			unsigned	opcode: 4;	/* purpose of message */
			unsigned	aa: 1;		/* authoritive answer */
			unsigned	tc: 1;		/* truncated message */
			unsigned	rd: 1;		/* recursion desired */
					/* fields in fourth byte */
			unsigned	ra: 1;		/* recursion available */
			unsigned	unused :1;	/* unused bits (MBZ as of 4.9.3a3) */
			unsigned	ad: 1;		/* authentic data from named */
			unsigned	cd: 1;		/* checking disabled by resolver */
			unsigned	rcode :4;	/* response code */
			#endif
			#if BYTE_ORDER == LITTLE_ENDIAN || BYTE_ORDER == PDP_ENDIAN
					/* fields in third byte */
			unsigned	rd :1;		/* recursion desired */
			unsigned	tc :1;		/* truncated message */
			unsigned	aa :1;		/* authoritive answer */
			unsigned	opcode :4;	/* purpose of message */
			unsigned	qr :1;		/* response flag */
					/* fields in fourth byte */
			unsigned	rcode :4;	/* response code */
			unsigned	cd: 1;		/* checking disabled by resolver */
			unsigned	ad: 1;		/* authentic data from named */
			unsigned	unused :1;	/* unused bits (MBZ as of 4.9.3a3) */
			unsigned	ra :1;		/* recursion available */
			#endif
					/* remaining bytes */
			unsigned	qdcount :16;	/* number of question entries */
			unsigned	ancount :16;	/* number of answer entries */
			unsigned        nscount :16;	/* number of authority entries */
			unsigned        arcount :16;	/* number of resource entries */
		} HEADER;
		#define T_MX		15
		#define C_IN		1
	#endif
	#if HAVE_RESOLV_H
		#include <resolv.h>
	#endif
#endif

#include "krregex.h"
#include "php_krfile.h"
#include "php_krparse.h"
#include "php_krnetwork.h"

#define PROXYSIZE 9

struct sockaddr_in sinfo;
struct hostent *hostinfo;
struct stat filestat;

/* {{{ proto string get_hostname_lib (int reverse [, string addr ])
 *    Return hostname or ip address */
PHP_FUNCTION(get_hostname_lib)
{
	pval **reverse, **addr;
	unsigned int i;
	const char delimiters[] = ", ";
	unsigned char *token;
	static char ret[1024];
	char tmphost[1024], *host, *check;
	char *proxytype[PROXYSIZE]  = { "HTTP_CLIENT_IP","HTTP_X_FORWARDED_FOR","HTTP_X_COMING_FROM",
									"HTTP_X_FORWARDED","HTTP_FORWARDED_FOR","HTTP_FORWARDED",
									"HTTP_COMING_FROM","HTTP_PROXY","HTTP_SP_HOST" };
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
			memset (tmphost, '\0', 1024);
			sprintf (tmphost, "%s", sapi_getenv(proxytype[i], strlen(proxytype[i]) TSRMLS_CC));

			if ( strcasecmp(tmphost, "(null)") ) { break; }
		}

		if ( !strcasecmp(tmphost, "(null)") )
	   	{
			host = sapi_getenv("REMOTE_ADDR", 11 TSRMLS_CC);
			if ( !host ) { host = getenv("REMOTE_ADDR"); }
			if ( !host ) { host = (unsigned char *) get_serverenv("REMOTE_ADDR"); }
		}
	   	else
		{
			if (strchr(tmphost, ','))
			{
				token = strtok(tmphost, delimiters);
				if ( !strcasecmp("unknown", token) )
				{
					token = strtok(NULL, delimiters);
					host = (token != NULL) ? estrdup(token) : estrdup(sapi_getenv("REMOTE_ADDR", 11 TSRMLS_CC));
				}
				else
				{
					host = estrdup(token);
					if ( !host ) { host = sapi_getenv("REMOTE_ADDR", 11 TSRMLS_CC); }
				}
			}
			else
			{
		   		host = estrdup(tmphost);
				if ( !host ) { host = estrdup(sapi_getenv("REMOTE_ADDR", 11 TSRMLS_CC)); }
			}
			if ( !host ) { host = estrdup(getenv("REMOTE_ADDR")); }
			if ( !host ) { host = estrdup((unsigned char *) get_serverenv("REMOTE_ADDR")); }
		}
	}
	else
   	{
		if ( Z_STRLEN_PP(addr) > 0 ) { host = estrdup(Z_STRVAL_PP(addr)); }
	   	else
	   	{
			php_error(E_WARNING,"address is null value");
			RETURN_FALSE;
		}
	}

	check = Z_LVAL_PP(reverse) ? kr_gethostbyaddr(host) : "";

	memset (ret, '\0', sizeof(ret));
	if ( strlen(check) > 0 ) { memmove (ret, check, strlen(check)); }
	else { memmove (ret, host, strlen(host)); }
	safe_efree(host);

	RETURN_STRING(ret, 1);
}
/* }}} */

/* {{{ proto string readfile_lib(string filename)
 *    Return a file or a URL */
/* old function
PHP_FUNCTION(readfile_lib)
{
	zval **arg1, **arg2;
	static unsigned char *filepath, *filename, *string, getfilename[1024];
	int use_include_path=0, issock=0;
	size_t *retSize, retSize_t = 0;

	retSize = &retSize_t;

	// check args
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
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	filepath = (unsigned char *) strtrim(Z_STRVAL_PP(arg1));

	if (checkReg(filepath, "^[hH][tT][tT][pP]://")) { issock = 1; }

	if ( issock == 1 )
	{
		string = (unsigned char *) sockhttp (filepath, retSize, 0, "");
		if (string != NULL)
		{
			RETVAL_STRINGL(string, *retSize, 1);
			safe_efree (filepath);
			safe_efree (string);
		}
	}
	else
	{
		if ( use_include_path != 0 && filepath[0] != '/' )
		{
			filename = (unsigned char *) includePath(filepath);
		}
		else { filename = filepath; }

		VCWD_REALPATH(filename, getfilename);

		// get file info
		if( stat (getfilename, &filestat) == 0 )
		{
			string = (unsigned char *) readfile(getfilename);
			RETVAL_STRINGL(string, filestat.st_size, 1);
			safe_efree (filepath);
			safe_efree (string);
		}
		else
		{
			php_error(E_WARNING, "File/URL %s is not found \n", filename);
			safe_efree (filepath);
			RETURN_EMPTY_STRING();
		}

	}
}
*/

PHP_FUNCTION(readfile_lib)
{
	zval **arg1, **arg2;
	static unsigned char *filepath, *string;
	unsigned char buf[8192];
	int use_include_path=0;
	size_t buflen =0, len = 0;
	php_stream *stream;

	// check args
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
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);
	filepath = (unsigned char *) strtrim(Z_STRVAL_PP(arg1));

	stream = php_stream_open_wrapper (filepath, "rb",
		   	 (use_include_path ? USE_PATH : 0) | ENFORCE_SAFE_MODE | REPORT_ERRORS, NULL);

	if (stream) {
		string = emalloc (sizeof (char) * 8192);
		memset (buf, '\0', sizeof(buf));

		while ( (buflen = php_stream_read (stream, buf, sizeof(buf))) > 0 ) {
			if (len > 0)
				string = erealloc(string, sizeof(char) * (8192 + len));

			memmove (string + len, buf, buflen);
			memset (buf, '\0', sizeof(buf));
			len += buflen;
		}
		php_stream_close(stream);

		RETVAL_STRINGL(string, len, 1);
		safe_efree(filepath);
		safe_efree(string);
	} else {
		safe_efree(filepath);
		RETURN_EMPTY_STRING();
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
	char *btoken;
	int debug = 0, error_no = 0;

	unsigned char *src[4] = { "/[^<]*</", "/>.*/", "/[\\s]/", "/^.*$/" };
	unsigned char *des[4] = { "", "", "", "<\\0>" };
	unsigned char *t_addr;

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

	if (strlen(tmpfrom) < 1) {
		unsigned char *f_src[4] = { "/\r*\n/i", "/.*From:([^!]+)!!ENTER!!.*/i", "/.*<([^>]+)>/i", "/^.*$/" };
		unsigned char *f_des[4] = { "!!ENTER!!", "\\1", "\\1", "<\\0>" };
		faddr = (unsigned char *) kr_regex_replace_arr(f_src, f_des, text, (sizeof (f_src) / sizeof (f_src[0])));
	} else {
		unsigned char *f_src[2] = { "/.*<([^>]+)>/i", "/^.*$/" };
		unsigned char *f_des[2] = { "\\1", "<\\0>" };
		faddr = (unsigned char *) kr_regex_replace_arr(f_src, f_des, tmpfrom, (sizeof (f_src) / sizeof (f_src[0])));
	}

	if (strlen(tmpto) < 1) {
		unsigned char *t_src[2] = { "/\r*\n/i", "/.*To:([^!]+)!!ENTER!!.*/i" };
		unsigned char *t_des[2] = { "!!ENTER!!", "\\1" };
		taddr = (unsigned char *) kr_regex_replace_arr(t_src, t_des, text, (sizeof (t_src) / sizeof (t_src[0])));
	} else {
		taddr = tmpto;
	}

	if ( (mailaddr = strtok_r (taddr, delimiters, &btoken)) != NULL ) {
		do {
			error_no++;
			t_addr = (unsigned char *) kr_regex_replace_arr(src, des, mailaddr, (sizeof (src) / sizeof (src[0])));

			if (sock_sendmail(faddr, t_addr, text, debug) == 1) {
				RETURN_LONG(error_no);
			}
			t_addr = NULL;
		} while ( (mailaddr = strtok_r (NULL, delimiters, &btoken)) != NULL );
	}
	error_no = 0;

	RETURN_LONG(error_no);
}
/* }}} */

/* {{{ static char *kr_gethostbyaddr(char *ip) */
static char *kr_gethostbyaddr(char *ip)
{
#if HAVE_IPV6 && !defined(__MacOSX__)
	/* MacOSX at this time has support for IPv6, but not inet_pton()
	 * so disabling IPv6 until further notice.  MacOSX 10.1.2 (kalowsky) */
	 struct in6_addr addr6;
#endif
	struct in_addr addr;
	struct hostent *hp;

#if HAVE_IPV6 && !defined(__MacOSX__)
	/* MacOSX at this time has support for IPv6, but not inet_pton()
	 * so disabling IPv6 until further notice.  MacOSX 10.1.2 (kalowsky) */
	if (inet_pton(AF_INET6, ip, &addr6)) {
		hp = gethostbyaddr((char *) &addr6, sizeof(addr6), AF_INET6);
	} else if (inet_pton(AF_INET, ip, &addr)) {
		hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
	} else {
		return NULL;
	}
#else
	addr.s_addr = inet_addr(ip);

	if (addr.s_addr == -1) {
		return NULL;
	}

	hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
#endif

	if (!hp) {
		return ip;
	}

	return hp->h_name;
}
/* }}} */

/* {{{ unsigned char *get_mx_record(unsigned char *str) */
unsigned char *get_mx_record(unsigned char *str)
{
	u_char answer[8192], *cp, *end;
	u_short type, weight, tmpweight;
	static char mxrecord[256];
	unsigned char *host, *tmphost, tmpmx[256];
	unsigned int i, qdc, count, tmpmxlen = 0;
	HEADER *hp;

	weight = 0;
	memset (tmpmx, '\0', sizeof(tmpmx));
	memset (mxrecord, '\0', sizeof(mxrecord));

	if ( (tmphost = strrchr(str, '@')) != NULL )
	{
		host = (unsigned char *) kr_regex_replace ("/[^<]*<|>.*/", "", tmphost + 1);
	}
	else
	{
		host = (unsigned char *) kr_regex_replace ("/[^<]*<|>.*/", "", str);
	}

	/* if don't exist mx record */
	if ( (i = res_search(host, C_IN, T_MX, answer, sizeof(answer))) < 0 ) {
		strcpy (mxrecord, host);
		free (host);
		return mxrecord;
	}

	if ( i > sizeof(answer) ) { i = sizeof(answer); }

	hp = (HEADER *) &answer;
	cp = (u_char *) &answer + 12;
	end = (u_char *) &answer + i;

	for ( qdc = ntohs((unsigned short) hp->qdcount); qdc--; cp += i + 4)
	{
		if ( (i = dn_skipname(cp, end)) < 0 )
		{
			strcpy (mxrecord, host);
			free (host);
			return mxrecord;
		}
	}

	/* see also dnsmxrr() function */
	count = ntohs((unsigned short) hp->ancount);
	while ( --count >= 0 && cp < end )
	{
		if ( (i = dn_skipname(cp, end)) < 0 )
		{
			strcpy (mxrecord, host);
			free (host);
			return mxrecord;
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
			strcpy (mxrecord, host);
			free (host);
			return mxrecord;
		}
		cp += i;
		tmpmxlen = strlen(tmpmx);

		if ( strlen(mxrecord) < 1 )
		{
			sprintf(mxrecord, "%s", tmpmx);
			mxrecord[tmpmxlen] = '\0';
			weight = tmpweight;
		}
		else
		{
			if ( weight > tmpweight )
			{
				weight = tmpweight;
				sprintf(mxrecord, "%s", tmpmx);
				mxrecord[tmpmxlen] = '\0';
			}
		}
	}

	if ( strlen(mxrecord) < 1 ) {
		strcpy (mxrecord, host);
		free (host);
		return mxrecord;
	} else { return mxrecord; }
}
/* }}} */

/* {{{ int socksend (int sock, int deb, unsigned char *var, unsigned char *target) */
int socksend (int sock, int deb, unsigned char *var, unsigned char *target)
{
	unsigned char *cmd, msg[1024];
	int rlen = 0, failed = 1, bar = 0, add = 0, tmplen;

	tmplen = strlen (var);

	if ( !strcasecmp(target, "body") ) { add = 6; }
	else if ( !strcasecmp(target, "mail")) { add = 14; }
	else if ( !strcasecmp(target, "rcpt")) { add = 12; }
	else { add = 3; }

	{
		unsigned char *tmpcmd;
		tmpcmd = emalloc( sizeof(char) * (tmplen + add + 1) );

		if ( !strcasecmp (target, "mail") ) { sprintf(tmpcmd, "MAIL From: %s\r\n", var); }
		else if ( !strcasecmp (target, "rcpt") ) { sprintf(tmpcmd, "RCPT To: %s\r\n", var); }
		else if ( !strcasecmp (target, "body") ) { sprintf(tmpcmd, "%s\r\n.\r\n", var); }
		else { sprintf(tmpcmd, "%s\r\n", var); }

		tmpcmd[tmplen + add] = '\0';
		cmd = (unsigned char *) estrdup(tmpcmd);
		safe_efree(tmpcmd);
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
	safe_efree(cmd);

	return failed;
}
/* }}} */

/* {{{ void debug_msg (unsigned char *msg, int info, int bar) */
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
/* }}} */

/* {{{ int sock_sendmail (unsigned char *fromaddr, unsigned char *toaddr, unsigned char *text, int debug) */
int sock_sendmail (unsigned char *fromaddr, unsigned char *toaddr, unsigned char *text, int debug)
{
	int len, sock, failcode;
	unsigned char *addr;

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
		unsigned char * str_t = NULL;

		recvlen = recv (sock, recvmsg, 1024, 0);
		recvmsg[recvlen] = '\0';
		if (debug == 1)
		{
			str_t = (unsigned char *) strtrim (recvmsg);
			php_printf("\r\nConnect %s Start\r\n", addr);
			php_printf("----------------------------------------------------------------\r\n\r\n");
			php_printf("DEBUG: %s\r\n", str_t);
			safe_efree (str_t);
		}
	}

	failcode = socksend (sock, debug, "HELO localhost", "helo");
    if ( failcode == 1 ) {
	   	close(sock);
	   	return 1;
   	}

	failcode = socksend (sock, debug, fromaddr, "mail");
	if ( failcode == 1 ) {
	   	close(sock);
	   	return 1;
   	}

	failcode = socksend (sock, debug, toaddr, "rcpt");
	if ( failcode == 1 ) {
	   	close(sock);
	   	return 1;
   	}

	failcode = socksend (sock, debug, "data", "data");
	if ( failcode == 1 ) {
	   	close(sock);
	   	return 1;
   	}

	failcode = socksend (sock, debug, text, "body");
	if ( failcode == 1 ) {
	   	close(sock);
	   	return 1;
   	}

	failcode = socksend (sock, debug, "quit", "quit");
	if ( failcode == 1 ) {
	   	close(sock);
	   	return 1;
   	}

	close(sock);
	return 0;
}
/* }}} */

/* {{{ unsigned char *sockhttp (unsigned char *addr, size_t *retSize, int record, unsigned char *recfile)
 * addr : url path of read file
 * record : whether write of don't write read file with randsom name
 * recfile : if record is 1, write recfile name with read file
 */
unsigned char *sockhttp (unsigned char *addr, size_t *retSize, int record, unsigned char *recfile)
{
	FILE *fp;
	unsigned char tmpfilename[512];
	int sock, len = 0, freechk = 0;
	unsigned char cmd[1024], rc[4096], *tmpstr = NULL;
	static unsigned char *nullstr = "", *string;
	size_t tmplen = 0;

	// parse file path with url, uri
	//unsigned char *uri;
	unsigned char *chk, *url, *urlpoint;
	chk = (unsigned char *) estrdup(addr + 7);
	fp = NULL;

	urlpoint = strchr(chk, '/');

	if (urlpoint != NULL)
	{
		url = (unsigned char *) estrndup(chk, urlpoint - chk);
	}
	else
	{
		url = (unsigned char *) estrdup(chk);
	}

	safe_efree(chk);

	// check existed url
	if ( !(hostinfo = gethostbyname(url)) )
	{
		php_error(E_WARNING, "host name \"%s\" not found\n", url);
		safe_efree(url);
		return NULL;
	}

	safe_efree(url);

	// specify connect server information
	sinfo.sin_family = AF_INET;
	sinfo.sin_port = ntohs(80);
	sinfo.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
	len = sizeof(sinfo);

	sprintf(cmd, "GET %s\r\n", addr);

	// create socket
	if ( (sock = socket(AF_INET, SOCK_STREAM, 0)) == -1 )
	{
		php_error(E_WARNING, "Failed to create socket\n");
		return NULL;
	}

	// connect to server in 80 port
	if ( connect (sock, (struct sockaddr *) &sinfo, len) == -1 )
	{
		php_error(E_WARNING, "Failed connect %s\n", addr);
		return NULL;
	}

	send(sock, cmd, strlen(cmd), 0);

	if(record == 1)
	{
		if ( strlen(recfile) > 0 )
		{
			memmove(tmpfilename, recfile, strlen(recfile));
			tmpfilename[strlen(recfile)] = '\0';
		}
		else
		{
			time_t now = time(0);
			size_t tmpflen = 0;

			// get random temp file name
			srand(now);
#ifdef PHP_WIN32
			sprintf(tmpfilename, "c:\\tmpResize-%d", rand());
#else
			sprintf(tmpfilename, "%s/tmpResize-%d",
								 PG(upload_tmp_dir) ? PG(upload_tmp_dir) : "/tmp", rand());
#endif
			tmpflen = strlen(tmpfilename);
			tmpfilename[tmpflen] = '\0';
		}

		if((fp = fopen(tmpfilename, "wb")) == NULL)
		{
			php_error(E_ERROR, "Can't create temp file %s of remote file", tmpfilename);
		}
	}

	// get document
	while( (len = recv(sock, rc, 4096, 0)) > 0 )
	{
		if(record == 1)
		{
			fwrite (rc, 1, len, fp);
		}
		else
		{
			if (tmplen == 0)
			{
				freechk = 1;
				tmpstr = emalloc(sizeof(char) * len + 1);
				memmove(tmpstr, rc, len);
				tmpstr[len] = '\0';
			}
			else
			{
				tmpstr = erealloc(tmpstr , sizeof(char) * (tmplen + len + 1));
				memmove(tmpstr + tmplen, rc, len);
				tmpstr[tmplen + len] = '\0';
			}
			tmplen += len;
			memset(rc, '\0', 4096);
		}
	}
	close(sock);

	if (record == 1)
	{
		fclose(fp);
		return nullstr;
	}
	else
	{
		// if empty document, return NULL
		if ( tmplen == 0 ) { return NULL; }

		// return string length with pointer
		*retSize = tmplen;

		string = (unsigned char *) estrndup(tmpstr, tmplen);
		if (freechk == 1) { safe_efree(tmpstr); }

		return string;
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
