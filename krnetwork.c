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
			unsigned	nscount :16;	/* number of authority entries */
			unsigned	arcount :16;	/* number of resource entries */
		} HEADER;
		#define T_MX		15
		#define C_IN		1
	#endif
	#if HAVE_RESOLV_H
		#include <resolv.h>
	#endif
#endif

#include "php_kr.h"
#include "krregex.h"
#include "php_krfile.h"
#include "php_krparse.h"
#define HAVE_KR_GETHOSTBYADDR
#include "php_krnetwork.h"

#define PROXYSIZE 9

struct sockaddr_in sinfo;
struct hostent *hostinfo;
struct stat filestat;

/* {{{ proto string get_hostname_lib (int reverse [, string addr ])
 *    Return hostname or ip address
 *
 *    sapi_getenv return value needs efree
 */
PHP_FUNCTION(get_hostname_lib)
{
	zend_string  * addr = NULL;
	unsigned int   i;
	const char     delimiters[] = ", ";
	char         * token = NULL;
	char         * ret = NULL;
	char         * host = NULL, * check = NULL;
	char         * proxytype[PROXYSIZE] = {
	                "HTTP_CLIENT_IP","HTTP_X_FORWARDED_FOR","HTTP_X_COMING_FROM",
                  	"HTTP_X_FORWARDED","HTTP_FORWARDED_FOR","HTTP_FORWARDED",
                  	"HTTP_COMING_FROM","HTTP_PROXY","HTTP_SP_HOST"
	             };

	zend_bool      reverse;

	if ( kr_parameters ("b|S", &reverse, &addr) == FAILURE )
		return;

	if ( ZEND_NUM_ARGS() == 1 ) {
		char * tmphost;
		for ( i = 0; i < PROXYSIZE; i++ ) {
			tmphost = sapi_getenv (proxytype[i], STRLEN (proxytype[i]));

			if ( strcasecmp (tmphost, "(null)") )
				break;
		}

		if ( ! strcasecmp (tmphost, "(null)") ) {
			host = sapi_getenv ("REMOTE_ADDR", 11);
			if ( ! host )
				host = estrdup (getenv ("REMOTE_ADDR"));
			if ( ! host )
				host = estrdup (get_serverenv ("REMOTE_ADDR"));
		} else {
			if ( strchr (tmphost, ',') ) {
				token = strtok (tmphost, delimiters);
				if ( ! strcasecmp ("unknown", token) ) {
					token = strtok (NULL, delimiters);
					host = (token != NULL) ?
						estrdup (token) :
						sapi_getenv ("REMOTE_ADDR", 11);
				} else {
					host = estrdup (token);
					if ( ! host )
						host = sapi_getenv ("REMOTE_ADDR", 11);
				}
			} else {
				host = estrdup (tmphost);
				if ( ! host )
					host = sapi_getenv ("REMOTE_ADDR", 11);
			}
			if ( ! host )
				host = estrdup (getenv ("REMOTE_ADDR"));
			if ( ! host )
				host = estrdup (get_serverenv ("REMOTE_ADDR"));
		}

		kr_safe_efree (tmphost);
	}
	else
	{
		if ( ! addr || ! ZSTR_LEN (addr) ) {
			php_error (E_WARNING,"address is null value");
			RETURN_FALSE;
		}

		host = estrdup (ZSTR_VAL (addr));
	}

	check = reverse ? kr_gethostbyaddr (host) : "";

	if ( STRLEN (check) > 0 )
		ret = estrdup (check);
	else
		ret = estrdup (host);

	kr_safe_efree(host);

	RETVAL_STRING(ret);
	kr_safe_efree (ret);
}
/* }}} */

/* {{{ proto string readfile_lib (string path [, bool use_include_path])
 */
PHP_FUNCTION(readfile_lib)
{
	zend_string * filepath;
	char        * string;
	char          buf[8192] = { 0, };
	int           use_include_path=0;
	size_t        buflen =0, len = 0;
	php_stream  * stream;

	if ( kr_parameters ("S|b", &filepath, &use_include_path) == FAILURE )
		return;

	if ( ! ZSTR_LEN (filepath) ) {
		php_error (E_WARNING, "path is invalid or missing");
		RETURN_EMPTY_STRING ();
	}

	stream = php_stream_open_wrapper (
		ZSTR_VAL (filepath),
		"rb",
		(use_include_path ? USE_PATH : 0) | REPORT_ERRORS,
		NULL
	);

	if ( stream ) {
		string = emalloc (sizeof (char) * 8192);
		memset (buf, '\0', sizeof(buf));

		while ( (buflen = php_stream_read (stream, buf, sizeof (buf))) > 0 ) {
			if (len > 0)
				string = erealloc (string, sizeof (char) * (8192 + len));

			memmove (string + len, buf, buflen);
			memset (buf, '\0', sizeof (buf));
			len += buflen;
		}
		//php_stream_free (stream, PHP_STREAM_FREE_CLOSE | PHP_STREAM_FREE_RSRC_DTOR);
		php_stream_close (stream);

		RETVAL_STRINGL (string, len);
		kr_safe_efree (string);
	} else {
		php_stream_close (stream);
		RETURN_EMPTY_STRING ();
	}
}
/* }}} */

/* {{{ proto array sockmail_lib(string mail [, string from [, string to [, string helohost [, int debug ] ] ] ])
 *    Return a file or a URL */
PHP_FUNCTION(sockmail_lib)
{
	zend_string * mailbody = NULL,
	            * from     = NULL,
	            * to       = NULL,
	            * helo     = NULL;
	zend_bool     debug = 0;
	char        * hhost = NULL;
	const char    delimiters[] = ",";
	char        * faddr, * taddr, * mailaddr;
	char        * btoken;

	char        * src[4] = { "/[^<]*</", "/>.*/", "/[\\s]/", "/^.*$/" };
	char        * des[4] = { "", "", "", "<\\0>" };
	char        * t_addr = NULL;

	if ( kr_parameters ("SS|SSb", &mailbody, &from, &to, &helo, &debug) == FAILURE )
		return;

#if PHP_VERSION_ID < 70300
	if ( array_init (return_value) == FAILURE ) {
		php_error (E_WARNING, "Failed init array");
		RETURN_FALSE;
	}
#else
	array_init (return_value);
#endif

	if ( ZSTR_LEN (mailbody) < 1 ) {
		php_error (E_WARNING, "missing mail body");
		RETURN_FALSE;
	}

	hhost = (helo && ZSTR_LEN (helo)) ? ZSTR_VAL (helo) : "";

	/* mail context */
	if ( ZSTR_LEN (from) < 1 ) {
		char * f_src[4] = { "/\r*\n/i", "/.*From:([^!]+)!!ENTER!!.*/i", "/.*<([^>]+)>/i", "/^.*$/" };
		char * f_des[4] = { "!!ENTER!!", "\\1", "\\1", "<\\0>" };
		faddr = kr_regex_replace_arr (f_src, f_des, ZSTR_VAL (mailbody), (sizeof (f_src) / sizeof (f_src[0])));
	} else {
		char * f_src[2] = { "/.*<([^>]+)>/i", "/^.*$/" };
		char * f_des[2] = { "\\1", "<\\0>" };
		faddr = kr_regex_replace_arr (f_src, f_des, ZSTR_VAL (from), (sizeof (f_src) / sizeof (f_src[0])));
	}

	if ( ! to && ZSTR_LEN (to) < 1) {
		char * t_src[2] = { "/\r*\n/i", "/.*To:([^!]+)!!ENTER!!.*/i" };
		char * t_des[2] = { "!!ENTER!!", "\\1" };
		taddr = kr_regex_replace_arr (t_src, t_des, ZSTR_VAL (mailbody), (sizeof (t_src) / sizeof (t_src[0])));
	} else {
		taddr = estrdup (ZSTR_VAL (to));
	}

	if ( (mailaddr = strtok_r (taddr, delimiters, &btoken)) != NULL ) {
		do {
			char * err_host;
			int    hostlen = 0;

			t_addr = kr_regex_replace_arr(src, des, mailaddr, (sizeof (src) / sizeof (src[0])));
			hostlen = STRLEN (t_addr);
			err_host = emalloc (sizeof (char *) * hostlen + 1);
			memset (err_host, 0, hostlen + 1);
			strncpy (err_host, t_addr + 1, STRLEN (t_addr) - 2);

			if ( sock_sendmail (faddr, t_addr, ZSTR_VAL (mailbody), hhost, debug) == 1)
				add_next_index_string (return_value, err_host);

			kr_safe_efree (t_addr);
			kr_safe_efree (err_host);
		} while ( (mailaddr = strtok_r (NULL, delimiters, &btoken)) != NULL );
	}
	kr_safe_efree (taddr);
	kr_safe_efree (faddr);
}
/* }}} */

/* {{{ static char *kr_gethostbyaddr(char *ip) */
static char * kr_gethostbyaddr (char * ip)
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
	if (inet_pton(AF_INET6, ip, &addr6))
		hp = gethostbyaddr((char *) &addr6, sizeof(addr6), AF_INET6);
	else if (inet_pton(AF_INET, ip, &addr))
		hp = gethostbyaddr((char *) &addr, sizeof(addr), AF_INET);
	else
		return ip;
#else
	addr.s_addr = inet_addr (ip);

	if ( addr.s_addr == -1 ) 
		return ip;

	hp = gethostbyaddr ((char *) &addr, sizeof (addr), AF_INET);
#endif

	if ( ! hp )
		return ip;

	return hp->h_name;
}
/* }}} */

/* {{{ char * get_mx_record (char * str) */
char * get_mx_record (char * str)
{
	u_char    answer[8192] = { 0, }, * cp, * end;
	u_short   type, weight, tmpweight;
	char    * mxrecord = NULL;
	char    * host, * tmphost, tmpmx[256] = { 0, };
	int       i, qdc, count;
	HEADER  * hp;

	weight = 0;

	if ( (tmphost = strrchr (str, '@')) != NULL )
		host = kr_regex_replace ("/[^<]*<|>.*/", "", tmphost + 1);
	else
		host = kr_regex_replace ("/[^<]*<|>.*/", "", str);

	/* if don't exist mx record */
	if ( (i = res_search (host, C_IN, T_MX, answer, sizeof (answer))) < 0 ) {
		mxrecord = estrdup (host);
		kr_safe_efree (host);
		return mxrecord;
	}

	if ( i > sizeof (answer) )
		i = sizeof (answer);

	hp = (HEADER *) &answer;
	cp = (u_char *) &answer + 12;
	end = (u_char *) &answer + i;

	for ( qdc = ntohs ((unsigned short) hp->qdcount); qdc--; cp += i + 4 ) {
		if ( (i = dn_skipname (cp, end)) < 0 ) {
			mxrecord = estrdup (host);
			kr_safe_efree (host);
			return mxrecord;
		}
	}

	/* see also dnsmxrr() function */
	count = ntohs ((unsigned short) hp->ancount);
	while ( --count >= 0 && cp < end ) {
		if ( (i = dn_skipname (cp, end)) < 0 ) {
			mxrecord = estrdup (host);
			kr_safe_efree (host);
			return mxrecord;
		}
		cp += i;
		GETSHORT (type, cp);
		cp += 6;
		GETSHORT (i, cp);
		if ( type != T_MX ) {
			cp += i;
			continue;
		}
		GETSHORT (tmpweight, cp);
		if ( (i = dn_expand (answer, end, cp, tmpmx, sizeof (tmpmx)-1)) < 0 ) {
			mxrecord = estrdup (host);
			kr_safe_efree (host);
			return mxrecord;
		}
		cp += i;

		if ( mxrecord == NULL ) {
			mxrecord = estrdup (tmpmx);
			weight = tmpweight;
		} else {
			if ( weight > tmpweight ) {
				kr_safe_efree (mxrecord);
				weight = tmpweight;
				mxrecord = estrdup (tmpmx);
			}
		}
	}

	if ( mxrecord == NULL || STRLEN (mxrecord) < 1 ) {
		kr_safe_efree (mxrecord);
		mxrecord = estrdup (host);
	}

	kr_safe_efree (host);
	return mxrecord;
}
/* }}} */

/* {{{ int socksend (int sock, int deb, char * var, char * target) */
int socksend (int sock, int deb, char * var, char * target)
{
	char * cmd, msg[1024];
	int    rlen = 0, failed = 1, add = 0, tmplen;

	tmplen = STRLEN (var);

	if ( ! strcasecmp(target, "body") ) add = 6;
	else if ( ! strcasecmp(target, "mail")) add = 14;
	else if ( ! strcasecmp(target, "rcpt")) add = 12;
	else add = 3;

	{
		char * tmpcmd;
		tmpcmd = emalloc (sizeof (char) * (tmplen + add + 1));

		if ( ! strcasecmp (target, "mail") ) sprintf (tmpcmd, "MAIL From: %s\r\n", var);
		else if ( ! strcasecmp (target, "rcpt") ) sprintf (tmpcmd, "RCPT To: %s\r\n", var);
		else if ( ! strcasecmp (target, "body") ) sprintf (tmpcmd, "%s\r\n.\r\n", var);
		else sprintf (tmpcmd, "%s\r\n", var);

		tmpcmd[tmplen + add] = '\0';
		cmd = estrdup (tmpcmd);
		kr_safe_efree (tmpcmd);
	}

	/* print debug information */
	debug_msg (cmd, deb, 0);

	send (sock, cmd, STRLEN (cmd), 0);
	rlen = recv (sock, msg, 1024, 0);
	if ( ! strncmp (msg, "220", 3) || ! strncmp (msg, "221", 3)
		 || ! strncmp (msg, "250", 3) || ! strncmp (msg, "251", 3)
		 || ! strncmp (msg, "354", 3) )
		failed = 0;

	msg[rlen] = '\0';
	debug_msg (msg, deb, 0);
	if ( deb != 0 && ! strcasecmp (target, "quit") )
		php_printf("\r\n");
	kr_safe_efree (cmd);

	return failed;
}
/* }}} */

/* {{{ void debug_msg (char *msg, int info, int bar) */
void debug_msg (char * msg, int info, int bar)
{
	if ( info != 0 ) {
		php_printf ("DEBUG: %s", msg);
		if ( bar != 0 )
			php_printf ("----------------------------------------------------------------\n");
	}
}
/* }}} */

/* {{{ int sock_sendmail (char * fromaddr, char * toaddr, char * text, int debug) */
int sock_sendmail (char * fromaddr, char * toaddr, char * text, char * host, int debug)
{
	int    len, sock, failcode;
	char * addr;
	char   helocmd[1024] = { 0, };

	if ( STRLEN (host) < 1 )
		strcpy (helocmd, "HELO localhost");
	else
		sprintf (helocmd, "HELO %s", host);

	/* get mx record from 'to address' */
	addr = get_mx_record (toaddr);

	if ( debug == 1 ) {
		php_printf ("DEBUG: To addr   : %s\n", toaddr);
		php_printf ("DEBUG: MX record : %s\n", addr);
	}

	if ( !(hostinfo = gethostbyname (addr)) )
	{
		if ( debug == 1 ) {
			php_error (E_WARNING, "host name \"%s\" not found\n", addr);
		}
		kr_safe_efree (addr);
		return 1;
	}

	/* specify connect server information */
	sinfo.sin_family = AF_INET;
	sinfo.sin_port = ntohs (25);
	sinfo.sin_addr = *(struct in_addr *) *hostinfo->h_addr_list;
	len = sizeof (sinfo);

	/* create socket */
	if ( (sock = socket (AF_INET, SOCK_STREAM, 0)) == -1 ) {
		if ( debug == 1 )
			php_error (E_WARNING, "Failed to create socket\n");

		kr_safe_efree (addr);
		return 1;
	}

	/* connect to server in 25 port */
	if ( connect (sock, (struct sockaddr *) &sinfo, len) == -1 ) {
		if ( sock )
			close (sock);

		if ( debug == 1 )
			php_error (E_WARNING, "Failed connect %s\n", addr);

		kr_safe_efree (addr);
		return 1;
	} else {
		int recvlen = 0;
		char recvmsg[1024];
		char * str_t = NULL;

		recvlen = recv (sock, recvmsg, 1024, 0);
		recvmsg[recvlen] = '\0';
		if (debug == 1) {
			str_t = strtrim (recvmsg);
			php_printf ("\r\nConnect %s Start\r\n", addr);
			php_printf ("----------------------------------------------------------------\r\n\r\n");
			php_printf ("DEBUG: %s\r\n", str_t);
			kr_safe_efree (str_t);
		}
	}

	kr_safe_efree (addr);

	failcode = socksend (sock, debug, helocmd, "helo");
	if ( failcode == 1 ) {
		if ( sock )
			close(sock);
		return 1;
	}

	failcode = socksend (sock, debug, fromaddr, "mail");
	if ( failcode == 1 ) {
		if ( sock )
			close(sock);
		return 1;
	}

	failcode = socksend (sock, debug, toaddr, "rcpt");
	if ( failcode == 1 ) {
		if ( sock )
			close(sock);
		return 1;
	}

	failcode = socksend (sock, debug, "data", "data");
	if ( failcode == 1 ) {
		if ( sock )
			close(sock);
		return 1;
	}

	failcode = socksend (sock, debug, text, "body");
	if ( failcode == 1 ) {
		if ( sock )
			close(sock);
		return 1;
	}

	failcode = socksend (sock, debug, "quit", "quit");
	if ( failcode == 1 ) {
		if ( sock )
			close(sock);
		return 1;
	}

	if ( sock )
		close(sock);
	return 0;
}
/* }}} */

/* {{{ char *sockhttp (char *addr, size_t *retSize, int record, char *recfile)
 * addr : url path of read file
 * record : whether write of don't write read file with randsom name
 * recfile : if record is 1, write recfile name with read file
 */
char * sockhttp (char * addr, size_t * retSize, int record, char * recfile)
{
	FILE  * fp;
	char    tmpfilename[512] = { 0, },
		    cmd[1024] = { 0, },
			rc[4096] = { 0, };
	int     sock,
			len = 0,
			freechk = 0;
	char  * nullstr = "",
		  * string,
		  * tmpstr = NULL;
	size_t  tmplen = 0;

	// parse file path with url, uri
	// char *uri;
	char * chk, * url, * urlpoint;
	chk = estrdup (addr + 7);
	fp = NULL;

	urlpoint = strchr (chk, '/');

	if ( urlpoint != NULL )
		url = estrndup (chk, urlpoint - chk);
	else
		url = estrdup (chk);

	kr_safe_efree (chk);

	// check existed url
	if ( ! (hostinfo = gethostbyname (url)) )
	{
		php_error (E_WARNING, "host name \"%s\" not found\n", url);
		kr_safe_efree (url);
		return NULL;
	}

	kr_safe_efree (url);

	// specify connect server information
	sinfo.sin_family = AF_INET;
	sinfo.sin_port = ntohs (80);
	sinfo.sin_addr = *(struct in_addr *) * hostinfo->h_addr_list;
	len = sizeof (sinfo);

	sprintf (cmd, "GET %s\r\n", addr);

	// create socket
	if ( (sock = socket (AF_INET, SOCK_STREAM, 0)) == -1 ) {
		php_error (E_WARNING, "Failed to create socket\n");
		return NULL;
	}

	// connect to server in 80 port
	if ( connect (sock, (struct sockaddr *) &sinfo, len) == -1 ) {
		php_error (E_WARNING, "Failed connect %s\n", addr);
		return NULL;
	}

	send (sock, cmd, STRLEN (cmd), 0);

	if ( record == 1 ) {
		if ( STRLEN (recfile) > 0 ) {
			memmove (tmpfilename, recfile, STRLEN (recfile));
			tmpfilename[STRLEN (recfile)] = '\0';
		} else {
			time_t now = time (0);
			size_t tmpflen = 0;

			// get random temp file name
			srand (now);
#ifdef PHP_WIN32
			sprintf (tmpfilename, "c:\\tmpResize-%d", rand ());
#else
			sprintf (
				tmpfilename, "%s/tmpResize-%d",
				PG(upload_tmp_dir) ? PG(upload_tmp_dir) : "/tmp", rand()
			);
#endif
			tmpflen = STRLEN (tmpfilename);
			tmpfilename[tmpflen] = '\0';
		}

		if ( (fp = fopen(tmpfilename, "wb")) == NULL )
			php_error (E_ERROR, "Can't create temp file %s of remote file", tmpfilename);
	}

	// get document
	while ( (len = recv (sock, rc, 4096, 0)) > 0 ) {
		if ( record == 1 )
			fwrite (rc, 1, len, fp);
		else {
			if ( tmplen == 0 ) {
				freechk = 1;
				tmpstr = emalloc (sizeof (char) * (len + 1));
				memmove (tmpstr, rc, len);
				tmpstr[len] = '\0';
			} else {
				tmpstr = erealloc (tmpstr , sizeof (char) * (tmplen + len + 1));
				memmove (tmpstr + tmplen, rc, len);
				tmpstr[tmplen + len] = '\0';
			}
			tmplen += len;
			memset (rc, 0, 4096);
		}
	}
	close (sock);

	if ( record == 1 ) {
		fclose (fp);
		return nullstr;
	} else {
		// if empty document, return NULL
		if ( tmplen == 0 )
			return NULL;

		// return string length with pointer
		*retSize = tmplen;

		string = estrndup (tmpstr, tmplen);
		if (freechk == 1)
			kr_safe_efree (tmpstr);

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
