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
  
  $Id: krmail.c,v 1.25 2004-09-14 08:11:53 oops Exp $
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <locale.h>

#ifdef PHP_WIN32
	#include "win32/time.h"
#else
	#include <sys/time.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_API.h"
#include "php_krmail.h"
#include "php_krcheck.h"
#include "php_krparse.h"

#ifdef PHP_WIN32
	#include "krregex.h"
	#include "ext/standard/base64.h"
	#include "ext/standard/php_string.h"
#endif

struct tm *loctime;

/* {{{ proto int mailsource_lib(unsigned char *ln, unsigned char *ctype, unsigned char *from
 *								unsigned char *to, unsigned char *title, unsigned char *text
 *								unsigned char *ptext, unsigned char *attach)
 * make mail source */
PHP_FUNCTION(mailsource_lib)
{
	pval **ln, **from, **to, **title, **text, **ptext, **attach;
	unsigned char *c_ln, *c_from, *c_to, *c_title;
	unsigned char *c_text, *c_ptext, *c_attach, *ret;
	unsigned char attachfile[1024];

	c_attach = NULL;
	c_ptext = NULL;

	switch( ZEND_NUM_ARGS() )
	{
		case 5:
			if( zend_get_parameters_ex(5, &ln, &from, &to, &title, &text) == FAILURE )
			{
				WRONG_PARAM_COUNT;
			}
			break;
		case 6:
			if( zend_get_parameters_ex(6, &ln, &from, &to, &title, &text, &ptext) == FAILURE )
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(ptext);
			c_ptext = (unsigned char *) strtrim ( Z_STRVAL_PP(ptext) );
			break; 
		case 7:
			if( zend_get_parameters_ex(7, &ln, &from, &to, &title, &text, &ptext, &attach) == FAILURE )
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(ptext);
			c_ptext = (unsigned char *) strtrim ( Z_STRVAL_PP(ptext) );
			convert_to_string_ex(attach);
			c_attach = (unsigned char *) strtrim ( Z_STRVAL_PP(attach) );
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(ln);
	convert_to_string_ex(from);
	convert_to_string_ex(to);
	convert_to_string_ex(title);
	convert_to_string_ex(text);

	c_ln = (unsigned char *) strtrim( Z_STRVAL_PP(ln) );
	c_from = (unsigned char *) strtrim( Z_STRVAL_PP(from) );
	c_to = (unsigned char *) strtrim( Z_STRVAL_PP(to) );
	c_title = (unsigned char *) strtrim( Z_STRVAL_PP(title) );
	c_text = (unsigned char *) strtrim( Z_STRVAL_PP(text) );

	memset(attachfile, '\0', sizeof(attachfile));
	if (c_attach != NULL) { VCWD_REALPATH(c_attach, attachfile); }

	ret = generate_mail(c_ln, c_from, c_to, c_title, c_text, c_ptext, attachfile);
	RETVAL_STRING(ret,1);

	efree (c_ptext);
	efree (c_attach);
	efree (c_ln);
	efree (c_from);
	efree (c_to);
	efree (c_title);
	efree (c_text);
	efree (ret);
}
/* }}} */

unsigned char * generate_mail (unsigned char *o_ln, unsigned char *o_from, unsigned char *o_to,
							   unsigned char *o_title, unsigned char *o_text, unsigned char *o_ptext,
							   unsigned char *o_attach)
{
	static unsigned char *return_header, *return_body, *return_attach, *return_mail;
	static char boundary[64], *charset, attbound[64];
	unsigned char *from, *to, *title;
	unsigned int i = 0;
	size_t return_mail_len = 0;

	// make charset
	for( i=0; i < strlen(o_ln); i++ )
   	{
		o_ln[i] = tolower(o_ln[i]);
	}
	charset = !strcmp(o_ln,"ko") ? "EUC-KR" : "US-ASCII";

	// make from
	from = generate_from(o_from, charset);

	// make to
	to = generate_to(o_to, charset);

	// make title
	title = generate_title(o_title, charset);

	// make boundary
	strcpy(boundary, make_boundary());

	if (strlen(o_attach) > 0)
	{
		// attach boundary
		strcpy (attbound, make_boundary());

		return_attach = generate_attach (o_attach, attbound);
		return_header = generate_header (from, to, title, attbound, o_attach);
	}
	else
	{
		return_header = generate_header (from, to, title, boundary, o_attach);
	}
	return_body = generate_body (charset, boundary, o_text, o_ptext);

	if (strlen(o_attach) > 0)
	{
		unsigned int athead_len = strlen(boundary) + strlen(attbound) + 128;
		unsigned char *tmp_attach_header;

		return_mail_len = strlen(return_header) + strlen(return_body) + strlen(return_attach) +
							strlen(attbound) + athead_len + 128;

		tmp_attach_header = emalloc( sizeof(char) * (athead_len) );
		return_mail = emalloc( sizeof(char) * (return_mail_len) );

		sprintf(tmp_attach_header, "\r\n--%s\r\nContent-Type: multipart/alternative;\r\n" \
								   "              boundary=\"%s\"\r\n\r\n", attbound, boundary);

		sprintf(return_mail, "%s\r\nThis is a multi-part message in MIME format.\r\n" \
				                 "%s%s\r\n%s\r\n--%s--\r\n",
			   	return_header, tmp_attach_header, return_body, return_attach, attbound);

		efree(return_attach);
		efree(tmp_attach_header);
	}
	else
	{
		return_mail_len = strlen(return_header) + strlen(return_body) + 128;
		return_mail = emalloc( sizeof(char) * (return_mail_len + 1) );

		sprintf(return_mail, "%s\r\nThis is a multi-part message in MIME format." \
								 "\r\n%s\r\n", return_header, return_body);
	}

	efree (to);
	efree (from);
	efree (title);
	efree (return_body);
	efree (return_header);

	return return_mail;
}

unsigned char * generate_attach (unsigned char *path, unsigned char *bound)
{
	struct stat filebuf;
	FILE *fp;
	size_t fsize, filelen = 0, sumlen = 0, fencodelen = 0;
	static unsigned char *fencode;
	unsigned char *mimetype, *tmpname, *filename, *contents, *base64text, getattach[FILEBUFS];

	if((fp = fopen(path, "rb")) == NULL)
	{
		php_error(E_ERROR, "Can't open attach file '%s' in read mode", path);
	}

	// get file info
	stat (path, &filebuf);
	// original file size
	fsize = filebuf.st_size;

	// get file name from path
	if ( (tmpname = strrchr(path, '/')) != NULL )
   	{
	   	filename = estrdup(&path[tmpname - path + 1]);
	}
	else if ( (tmpname = strrchr(path, '\\')) != NULL )
   	{
	   	filename = estrdup(&path[tmpname - path + 1]);
	}
	else
   	{
	   	filename = estrdup(path);
   	}

	// get mime type
	mimetype = generate_mime(filename);

	contents = emalloc(sizeof(char) * (fsize + 32));
	memset(contents, '\n', sizeof(contents));

	while ( (filelen = fread(getattach, sizeof(char), FILEBUFS, fp)) > 0 )
	{
		memmove (contents + sumlen, getattach, filelen);	
		sumlen += filelen;
	}
	fclose(fp);
	//contents[sumlen-1] = '\0';
	
	base64text = body_encode(contents, sumlen);

	fencodelen = strlen(bound) + strlen(mimetype) + (strlen(filename) * 2) + strlen(base64text) + 256;
	fencode = emalloc( sizeof(char) * (fencodelen) );

	sprintf(fencode, "--%s\r\nContent-Type: %s; name=\"%s\"\r\nContent-Transfer-Encoding: " \
			          "base64\r\nContent-Disposition: inline; filename=\"%s\"\r\n\r\n%s\r\n\0",
			bound, mimetype, filename, filename, base64text);

	efree(base64text);
	efree(filename);
	efree(contents);

	return fencode;
}

unsigned char * generate_body (unsigned char *bset, unsigned char *bboundary, unsigned char *btext,
							   unsigned char *bptext)
{
	static unsigned char *rbody;
	unsigned char *plain, *base64html, *base64plain, *plain_t;
	unsigned int plainlen = 0, htmllen = 0, nobptext = 0;

	if ( strlen(btext) > 0 )
	{
		if ( bptext == NULL ) nobptext = 1;
		else if ( strlen (bptext) < 1 ) nobptext = 1;
		plain = nobptext ? strtrim (html_to_plain (btext)) : strtrim (bptext);

		base64plain = body_encode(plain, -1);
		base64html  = body_encode(btext, -1);

		plainlen = strlen(base64plain);
		htmllen  = strlen(base64html);

		{
			unsigned int tmp_body_len = plainlen + htmllen + (strlen(bset) * 2) + (strlen(bboundary) * 3) +182;
			unsigned char *tmp_body;
			tmp_body = emalloc( sizeof(char) * (tmp_body_len + 1) );

			sprintf(tmp_body, "\r\n--%s\r\nContent-Type: text/plain; charset=%s\r\n" \
							  "Content-Transfer-Encoding: base64\r\n\r\n%s\r\n\r\n--%s\r\n" \
							  "Content-Type: text/html; charset=%s\r\nContent-Transfer-Encoding: " \
							  "base64\r\n\r\n%s\r\n\r\n--%s--\r\n",
					bboundary, bset, base64plain, bboundary, bset, base64html, bboundary);

			rbody = estrdup(tmp_body);
			efree(tmp_body);
		}

		efree (plain);
		efree (base64plain);
		efree (base64html);
	}
	else
	{
		php_error(E_ERROR, "Don't exist mail body context");
	}

	return rbody;
}

unsigned char * generate_header (unsigned char *from, unsigned char *to, unsigned char *subject,
								 char *boundary, unsigned char *is_attach)
{
	char *mailid, *datehead, *mimetype;
	static unsigned char *rheader;

	if (strlen(is_attach) > 0) { mimetype = "mixed"; }
	else { mimetype = "alternative"; }

	 /* make mail id */
	mailid = generate_mail_id( (char *) kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1", from) );
	datehead = generate_date();

	{
		unsigned int buflen = strlen(mailid) + strlen(from) + strlen(datehead) + strlen(to) + strlen(subject) + strlen(boundary) + strlen(mimetype) + 256;
		unsigned char *buf;
		buf = emalloc( sizeof(char) * (buflen + 1) );

		sprintf(buf, "Message-ID: <%s>\r\nFrom: %s\r\nMIME-Version: 1.0\r\nDate: %s\r\n" \
					 "To: %s\r\nSubject: %s\r\nContent-Type: multipart/%s;\r\n              " \
					 "boundary=\"%s\"\r\n\r\n",
				mailid, from, datehead, to, subject, mimetype, boundary);
		rheader = (char *) estrdup(buf);
		efree(buf);
	}

	return rheader;
}

unsigned char * generate_from (unsigned char *email, char *set)
{
	static unsigned char *rfrom;
	unsigned char *name, *cname, *mail, *name_t;
	unsigned int namelen = 0, maillen = 0, setlen = strlen(set);

	name_t = NULL;

	if ( strlen(email) < 1 ) {
		php_error(E_ERROR, "Don't exist FROM address.");
	}

	// get email address on NAME <email@address> form
	mail = (unsigned char *) strtrim((unsigned char *) kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1", email));
	maillen = strlen(mail);

	// get name on NAME <email@address> form
	if ( strchr(email,'<') != NULL ) {
		name_t = (unsigned char *) strtrim((unsigned char *) kr_regex_replace("/([^<]*)<[^>]+>.*/i","\\1", email));
		name = name_t;
	} else { name = ""; }
	
	if ( checkAddr (mail,0) != 1 ) {
		php_error(E_ERROR, "%s is invalid email address form.", mail);
	}
	
	if ( strlen(name) < 1 ) {
		rfrom = estrndup(mail, maillen);
	} else {
		cname = (unsigned char *) php_base64_encode(name, strlen(name), &namelen);
		{
			int from_lenth = setlen + maillen + namelen + 11;
			unsigned char *tmp_from;
			tmp_from = emalloc( sizeof(char) * (from_lenth + 1) );
			sprintf(tmp_from, "=?%s?B?%s?= <%s>", set, cname, mail);
			rfrom = estrndup( tmp_from, strlen(tmp_from) );
			efree(tmp_from);
		}
	}

	efree (mail);
	efree (name_t);

	return rfrom;
}

unsigned char * generate_to (unsigned char *toaddr, char *set)
{
	static unsigned char *to = NULL;
	unsigned char delimiters[] = ",";
	char *token, *btoken;
	unsigned char *t_mail, *t_name, *cname, *_t_name;
	int maillen = 0, namelen = 0, setlen = strlen(set);

	if ( strlen(toaddr) < 1 ) {
		php_error(E_ERROR, "Don't exist TO address.");
	}

	_t_name = NULL;

	token = strtok_r (toaddr, delimiters, &btoken);
	if ( token != NULL ) {
		// get email address on NAME <email@address> form
		if ( strchr(token,'<') != NULL ) {
			t_mail = strtrim( (unsigned char *) kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1",token) );
			_t_name = strtrim( (unsigned char *) kr_regex_replace("/([^<]*)<[^>]+>.*/i","\\1",token) );
			t_name = _t_name;
			maillen = strlen(t_mail);
			namelen = strlen(t_name);
		} else {
			t_mail = (unsigned char *) strtrim(token);
			t_name = "";
		}

		// whether vaild or invalid email form
		if ( checkAddr(t_mail, 0) ) {
			if ( namelen < 1 ) {
				to = estrdup((unsigned char *) strtrim(token));
			} else {
				int to_lenth = setlen + maillen + namelen + 32;
				unsigned char *t_to;
				t_to = emalloc( sizeof(char) * (to_lenth + 1) );
				cname = estrdup( (unsigned char *) php_base64_encode(t_name, namelen, &namelen) );
				sprintf(t_to, "=?%s?B?%s?= <%s>", set, cname, t_mail);
				to = estrdup(t_to);
				efree(t_to);
			}

		}
		efree (t_mail);
		efree (_t_name);

		while ( (token = strtok_r (NULL, delimiters, &btoken)) != NULL ) {
			{
				unsigned char *s_name, *s_mail, *sub_cname, *_s_name;
				unsigned int snlen = 0, smlen = 0;

				_s_name = NULL;

				// get email address on NAME <email@address> form
				if ( strchr(token,'<') != NULL ) {
					s_mail = strtrim( (unsigned char *) kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1", token) );
					_s_name = strtrim( (unsigned char *) kr_regex_replace("/([^<]*)<[^>]+>.*/i","\\1", token) );
					s_name = _s_name;
					smlen = strlen(s_mail);
					snlen = strlen(s_name);
				} else {
					s_mail = strtrim( (unsigned char *) token);
					s_name = "";
				}

				// whether vaild or invalid email form
				if ( checkAddr(s_mail, 0) ) {
					unsigned char s_to[200];
					if ( snlen < 1 ) {
						memset(s_to, '\0', 20);
						memmove(s_to, s_mail, strlen(s_mail));
					} else {
						sub_cname = (unsigned char *) php_base64_encode(s_name, snlen, &namelen);
						sprintf(s_to, "=?%s?B?%s?= <%s>", set, sub_cname, s_mail);
					}

					if ( to == NULL ) {
						to = estrdup(s_to);
					} else {
						unsigned int add_to_len = strlen(s_to) + 3;
						unsigned char *add_to;
						add_to = emalloc( sizeof(char) * (add_to_len + 1) );
						sprintf(add_to, ", %s", s_to);
						to = (unsigned char *) erealloc(to, strlen(to) + add_to_len);
						strcat(to, add_to);
						efree(add_to);
					}
				}

				efree (s_mail);
				if ( _s_name != NULL )
					efree (_s_name);
			}
		}
	}

	if ( to == NULL ) {
		php_error(E_ERROR, "Don't exist valid TO address.");
	}

	return to;
}

unsigned char * generate_title (unsigned char *title, unsigned char *set)
{
	unsigned int len = 0, set_len = strlen(set);
	static unsigned char *subject;
	unsigned char *base64;

	if ( strlen(title) < 1 )
	{
		php_error(E_ERROR, "Don't exists mail subject.");
	}

	base64 = (unsigned char *) php_base64_encode(title, strlen(title), &len);

	{
		unsigned int subject_lenth = len + set_len + 8;
		subject = emalloc( sizeof(char) * (subject_lenth + 1) );
		sprintf(subject, "=?%s?B?%s?=", set, base64);
	}

	return subject;
}

char * generate_date () {
	time_t now = time(NULL);
	char buf[64], *rdate;
	int len = 0;
	static char retbuf[128];

	memset (buf, 0, sizeof (buf));
	memset (retbuf, 0, sizeof (retbuf));

	setlocale (LC_TIME, "C");
	loctime = localtime(&now);
	strftime(buf, 64, "%a, %d %b %Y %H:%M:%S %Z", loctime);

#ifdef PHP_WIN32
	rdate = (char *) kr_regex_replace("/대한민국 표준시/","+09:00",buf);
#else
	rdate = estrdup(buf);
#endif

	len = strlen (rdate);

	if ( len < 128 ) strcpy (retbuf, rdate);
	else strncpy (retbuf, rdate, 127);

	free (rdate);

	return retbuf;
}

char * generate_mail_id (char *id)
{
	time_t now = time(0);
	static char mailid[60];
	char idtime[15], *mark;

	/* convert localtime */
	loctime = localtime(&now);
	strftime (idtime, 15, "%Y%m%d%H%M%S", loctime);

	/* mail id check */
	if ( strlen(id) == 0 ) { id = "OOPS_PHP_LIB"; }
	else
	{
		mark = strchr( id, '@' );
		if ( mark != NULL ) { id[mark - id] = '\0'; }
	}

	/* get random number */
	srand(now);
	sprintf(mailid, "%s%d@%s", idtime, rand(), id);

	return mailid;
}

char * make_boundary ()
{
	int sec, usec, len;
	static char bound[40];
	char bid[14];
	char first[9], second[9];
#if defined(__CYGWIN__)
    struct timespec tv;
#else
    struct timeval tv;
#endif

	/* make uniq id */
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
#if defined(__CYGWIN__)
	usec = (int) (tv.tv_nsec % 1000000);
#else
	usec = (int) (tv.tv_usec % 1000000);
#endif
	sprintf(bid,"%05x%07x",usec,sec);

	/* get lenth of uniq id */
	len = strlen(bid);

	sprintf(first,"%c%c%c%c%c%c%c%c",
			toupper(bid[1]),toupper(bid[2]),toupper(bid[3]),toupper(bid[4]),
			toupper(bid[5]),toupper(bid[6]),toupper(bid[7]),toupper(bid[8]));
	sprintf(second,"%c%c%c%c%c%c%c%c",
			toupper(bid[len-1]),toupper(bid[len-2]),toupper(bid[len-3]),
			toupper(bid[len-4]),toupper(bid[len-5]),toupper(bid[len-6]),
			toupper(bid[len-7]),toupper(bid[len-8]));

	sprintf(bound,"--=_NextPart_000_0%c%c%c_%s.%s",toupper(bid[3]), toupper(bid[1]), toupper(bid[0]), first, second);

	return bound;
}

unsigned char * html_to_plain (unsigned char * source)
{
	static unsigned char *strip, *rptext;
	unsigned char *src[3] = { ":^.*<BODY[^>]*>:si", ":<\\/BODY>.*$:si", ":</?[a-z][^>]*>:si" };
	unsigned char *des[3] = { "", "", "" };
			    
	rptext = (unsigned char *) kr_regex_replace_arr (src, des, source, 3);
				    
	return rptext;
}

unsigned char * body_encode (const unsigned char *str, int chklen)
{
	static unsigned char *rencode, *enbase;
	int len = 0, devide = 0, nlen=0, olen=0, breakpoint =0, tlen = 0;

	if ( chklen < 0 ) { chklen = strlen(str); }

	enbase = (unsigned char *) php_base64_encode(str, chklen, &len);
	devide = (int) len / 60;

	if ( len < 61 ) { return enbase; }
	rencode = emalloc(sizeof(char) * (len + 16 + devide * 2));
	memset(rencode, '\0', len + 16 + devide * 2);

	while(1)
	{
		if (strlen(enbase + olen) < 60)
		{
			tlen = strlen(enbase + olen);
			breakpoint = 1;
		}
		else { tlen = 60; }

		memmove(rencode + nlen, enbase + olen, tlen);
		nlen += 60;
		olen += 60;
		memmove(rencode + nlen, "\r\n", 2);
		nlen += 2;

		if (breakpoint == 1) { break; }
	}

	return rencode;
}

unsigned char *generate_mime (unsigned char *filename)
{
	static char *mime;
	unsigned char *tmpext, *ext;
	if ( (tmpext = strrchr(filename, '.')) == NULL ) { ext = ""; }
	ext = estrdup(&filename[tmpext - filename + 1]);

	if (!strcmp("ez", ext)) { mime = "application/andrew-inset"; }
	else if (!strcmp("hqx", ext)) { mime = "application/mac-binhex40"; }
	else if (!strcmp("cpt", ext)) { mime = "application/mac-compactpro"; }
	else if (!strcmp("doc", ext)) { mime = "application/msword"; }
	else if (!strcmp("oda", ext)) { mime = "application/oda"; }
	else if (!strcmp("pdf", ext)) { mime = "application/pdf"; }
	else if (!strcmp("rtf", ext)) { mime = "application/rtf"; }
	else if (!strcmp("mif", ext)) { mime = "application/vnd.mif"; }
	else if (!strcmp("ppt", ext)) { mime = "application/vnd.ms-powerpoint"; }
	else if (!strcmp("slc", ext)) { mime = "application/vnd.wap.slc"; }
	else if (!strcmp("sic", ext)) { mime = "application/vnd.wap.sic"; }
	else if (!strcmp("wmlc", ext)) { mime = "application/vnd.wap.wmlc"; }
	else if (!strcmp("wmlsc", ext)) { mime = "application/vnd.wap.wmlscriptc"; }
	else if (!strcmp("bcpio", ext)) { mime = "application/x-bcpio"; }
	else if (!strcmp("bz2", ext)) { mime = "application/x-bzip2"; }
	else if (!strcmp("vcd", ext)) { mime = "application/x-cdlink"; }
	else if (!strcmp("pgn", ext)) { mime = "application/x-chess-pgn"; }
	else if (!strcmp("cpio", ext)) { mime = "application/x-cpio"; }
	else if (!strcmp("csh", ext)) { mime = "application/x-csh"; }
	else if (!strcmp("dvi", ext)) { mime = "application/x-dvi"; }
	else if (!strcmp("spl", ext)) { mime = "application/x-futuresplash"; }
	else if (!strcmp("gtar", ext)) { mime = "application/x-gtar"; }
	else if (!strcmp("hdf", ext)) { mime = "application/x-hdf"; }
	else if (!strcmp("js", ext)) { mime = "application/x-javascript"; }
	else if (!strcmp("ksp", ext)) { mime = "application/x-kspread"; }
	else if (!strcmp("kpr", ext) || !strcmp("kpt", ext)) { mime = "application/x-kpresenter"; }
	else if (!strcmp("chrt", ext)) { mime = "application/x-kchart"; }
	else if (!strcmp("kil", ext)) { mime = "application/x-killustrator"; }
	else if (!strcmp("skp", ext) || !strcmp("skd", ext) || !strcmp("skt", ext) ||
			 !strcmp("skm", ext)) { mime = "application/x-koan"; }
	else if (!strcmp("latex", ext)) { mime = "application/x-latex"; }
	else if (!strcmp("nc", ext) || !strcmp("cdf", ext)) { mime = "application/x-netcdf"; }
	else if (!strcmp("rpm", ext)) { mime = "application/x-rpm"; }
	else if (!strcmp("sh", ext)) { mime = "application/x-sh"; }
	else if (!strcmp("shar", ext)) { mime = "application/x-shar"; }
	else if (!strcmp("swf", ext)) { mime = "application/x-shockwave-flash"; }
	else if (!strcmp("sit", ext)) { mime = "application/x-stuffit"; }
	else if (!strcmp("sv4cpio", ext)) { mime = "application/x-sv4cpio"; }
	else if (!strcmp("sv4crc", ext)) { mime = "application/x-sv4crc"; }
	else if (!strcmp("tar", ext)) { mime = "application/x-tar"; }
	else if (!strcmp("tcl", ext)) { mime = "application/x-tcl"; }
	else if (!strcmp("tex", ext)) { mime = "application/x-tex"; }
	else if (!strcmp("texinfo", ext) || !strcmp("texi", ext)) { mime = "application/x-texinfo"; }
	else if (!strcmp("t", ext) || !strcmp("tr", ext) ||
			 !strcmp("roff", ext)) { mime = "application/x-troff"; }
	else if (!strcmp("man", ext)) { mime = "application/x-troff-man"; }
	else if (!strcmp("me", ext)) { mime = "application/x-troff-me"; }
	else if (!strcmp("ms", ext)) { mime = "application/x-troff-ms"; }
	else if (!strcmp("ustar", ext)) { mime = "application/x-ustar"; }
	else if (!strcmp("src", ext)) { mime = "application/x-wais-source"; }
	else if (!strcmp("zip", ext)) { mime = "application/zip"; }
	else if (!strcmp("gif", ext)) { mime = "image/gif"; }
	else if (!strcmp("ief", ext)) { mime = "image/ief"; }
	else if (!strcmp("wbmp", ext)) { mime = "image/vnd.wap.wbmp"; }
	else if (!strcmp("ras", ext)) { mime = "image/x-cmu-raster"; }
	else if (!strcmp("pnm", ext)) { mime = "image/x-portable-anymap"; }
	else if (!strcmp("pbm", ext)) { mime = "image/x-portable-bitmap"; }
	else if (!strcmp("pgm", ext)) { mime = "image/x-portable-graymap"; }
	else if (!strcmp("ppm", ext)) { mime = "image/x-portable-pixmap"; }
	else if (!strcmp("rgb", ext)) { mime = "image/x-rgb"; }
	else if (!strcmp("xbm", ext)) { mime = "image/x-xbitmap"; }
	else if (!strcmp("xpm", ext)) { mime = "image/x-xpixmap"; }
	else if (!strcmp("xwd", ext)) { mime = "image/x-xwindowdump"; }
	else if (!strcmp("css", ext)) { mime = "text/css"; }
	else if (!strcmp("rtx", ext)) { mime = "text/richtext"; }
	else if (!strcmp("rtf", ext)) { mime = "text/rtf"; }
	else if (!strcmp("tsv", ext)) { mime = "text/tab-separated-values"; }
	else if (!strcmp("sl", ext)) { mime = "text/vnd.wap.sl"; }
	else if (!strcmp("si", ext)) { mime = "text/vnd.wap.si"; }
	else if (!strcmp("wml", ext)) { mime = "text/vnd.wap.wml"; }
	else if (!strcmp("wmls", ext)) { mime = "text/vnd.wap.wmlscript"; }
	else if (!strcmp("etx", ext)) { mime = "text/x-setext"; }
	else if (!strcmp("xml", ext)) { mime = "text/xml"; }
	else if (!strcmp("avi", ext)) { mime = "video/x-msvideo"; }
	else if (!strcmp("movie", ext)) { mime = "video/x-sgi-movie"; }
	else if (!strcmp("wma", ext)) { mime = "audio/x-ms-wma"; }
	else if (!strcmp("wax", ext)) { mime = "audio/x-ms-wax"; }
	else if (!strcmp("wmv", ext)) { mime = "video/x-ms-wmv"; }
	else if (!strcmp("wvx", ext)) { mime = "video/x-ms-wvx"; }
	else if (!strcmp("wm", ext)) { mime = "video/x-ms-wm"; }
	else if (!strcmp("wmx", ext)) { mime = "video/x-ms-wmx"; }
	else if (!strcmp("wmz", ext)) { mime = "application/x-ms-wmz"; }
	else if (!strcmp("wmd", ext)) { mime = "application/x-ms-wmd"; }
	else if (!strcmp("ice", ext)) { mime = "x-conference/x-cooltalk"; }
	else if (!strcmp("ra", ext)) { mime = "audio/x-realaudio"; }
	else if (!strcmp("wav", ext)) { mime = "audio/x-wav"; }
	else if (!strcmp("png", ext)) { mime = "image/png"; }
	else if (!strcmp("asf", ext) || !strcmp("asx", ext)) { mime = "video/x-ms-asf"; }
	else if (!strcmp("html", ext) || !strcmp("htm", ext)) { mime = "text/html"; }
	else if (!strcmp("smi", ext) || !strcmp("smil", ext)) { mime = "application/smil"; }
	else if (!strcmp("gz", ext) || !strcmp("tgz", ext)) { mime = "application/x-gzip"; }
	else if (!strcmp("kwd", ext) || !strcmp("kwt", ext)) { mime = "application/x-kword"; }
	else if (!strcmp("kpr", ext) || !strcmp("kpt", ext)) { mime = "application/x-kpresenter"; }
	else if (!strcmp("au", ext) || !strcmp("snd", ext)) { mime = "audio/basic"; }
	else if (!strcmp("ram", ext) || !strcmp("rm", ext)) { mime = "audio/x-pn-realaudio"; }
	else if (!strcmp("pdb", ext) || !strcmp("xyz", ext)) { mime = "chemical/x-pdb"; }
	else if (!strcmp("tiff", ext) || !strcmp("tif", ext)) { mime = "image/tiff"; }
	else if (!strcmp("igs", ext) || !strcmp("iges", ext)) { mime = "model/iges"; }
	else if (!strcmp("wrl", ext) || !strcmp("vrml", ext)) { mime = "model/vrml"; }
	else if (!strcmp("asc", ext) || !strcmp("txt", ext) || !strcmp("php", ext)) { mime = "text/plain"; }
	else if (!strcmp("sgml", ext) || !strcmp("sgm", ext)) { mime = "text/sgml"; }
	else if (!strcmp("qt", ext) || !strcmp("mov", ext)) { mime = "video/quicktime"; }
	else if (!strcmp("ai", ext) || !strcmp("eps", ext) || !strcmp("ps", ext)) { mime = "application/postscript"; }
	else if (!strcmp("dcr", ext) || !strcmp("dir", ext) || !strcmp("dxr", ext)) { mime = "application/x-director"; }
	else if (!strcmp("mid", ext) || !strcmp("midi", ext) || !strcmp("kar", ext)) { mime = "audio/midi"; }
	else if (!strcmp("mpga", ext) || !strcmp("mp2", ext) || !strcmp("mp3", ext)) { mime = "audio/mpeg"; }
	else if (!strcmp("aif", ext) || !strcmp("aiff", ext) || !strcmp("aifc", ext)) { mime = "audio/x-aiff"; }
	else if (!strcmp("jpeg", ext) || !strcmp("jpg", ext) || !strcmp("jpe", ext)) { mime = "image/jpeg"; }
	else if (!strcmp("msh", ext) || !strcmp("mesh", ext) || !strcmp("silo", ext)) { mime = "model/mesh"; }
	else if (!strcmp("mpeg", ext) || !strcmp("mpg", ext) || !strcmp("mpe", ext)) { mime = "video/mpeg"; }
	else { mime = "application/octet-stream"; }
	/*
	else if (!strcmp("bin", ext) || !strcmp("dms", ext) || !strcmp("lha", ext) ||
			 !strcmp("lzh", ext) || !strcmp("exe", ext) || !strcmp("class", ext))
	{ mime = "application/octet-stream"; }
	else { mime = "file/unknown"; }
	*/

	return mime;
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
