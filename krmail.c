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
#include "php_kr.h"
#include "php_krmail.h"
#include "php_krcheck.h"
#include "php_krparse.h"

#ifdef PHP_WIN32
	#include "ext/standard/php_string.h"
#endif

struct tm *loctime;

/* {{{ proto int mailsource_lib(char * ln, char *ctype, char *from
 *								char * to, Char *title, char *text
 *								char * ptext, char *attach)
 * make mail source */
PHP_FUNCTION(mailsource_lib)
{
	char        * c_ln, * c_from, * c_to, * c_title;
	char        * c_text, * c_ptext, * c_attach;
	char          attachfile[1024] = { 0, };
	char        * ret;

	zend_string * lang   = NULL,
	            * from   = NULL,
	            * to     = NULL,
	            * title  = NULL,
	            * text   = NULL,
	            * ptext  = NULL,
	            * attach = NULL;

	c_attach = NULL;
	c_ptext  = NULL;

	if (
		kr_parameters (
			"SSSSS|SS",
			&lang, &from, &to, &title,
			&text, &ptext, &attach
		) == FAILURE
	) {
		return;
	}

	if ( ZSTR_LEN(lang) == 0 )
		c_ln = estrdup ("utf-8");
	else
		c_ln = strtrim (ZSTR_VAL (lang));

	c_from  = strtrim (ZSTR_VAL (from));
	c_to    = strtrim (ZSTR_VAL (to));
	c_title = strtrim (ZSTR_VAL (title));
	c_text  = strtrim (ZSTR_VAL (text));

	if ( ptext && ZSTR_LEN (ptext) )
		c_ptext = strtrim (ZSTR_VAL (ptext));

	if ( attach && ZSTR_LEN (attach) )
		c_attach = strtrim (ZSTR_VAL (attach));

	memset(attachfile, '\0', sizeof(attachfile));
	if (c_attach != NULL) {
		if ( VCWD_REALPATH((char *) c_attach, (char *) attachfile) == NULL )
			strcpy ((char *) attachfile, (char *) c_attach);
		PHP_KR_CHECK_OPEN_BASEDIR (attachfile);
	}

	if ( (ret = (char *) generate_mail(c_ln, c_from, c_to, c_title, c_text, c_ptext, attachfile)) == NULL )
		RETURN_EMPTY_STRING();

	RETVAL_STRING(ret);

	kr_safe_efree (c_ptext);
	kr_safe_efree (c_attach);
	kr_safe_efree (c_ln);
	kr_safe_efree (c_from);
	kr_safe_efree (c_to);
	kr_safe_efree (c_title);
	kr_safe_efree (c_text);
	kr_safe_efree (ret);
}
/* }}} */

char * generate_mail (char * o_ln, char * o_from, char * o_to,
					  char * o_title, char * o_text, char * o_ptext,
					  char * o_attach) // {{{
{
	char       * return_header = NULL,
	           * return_body = NULL,
	           * return_attach = NULL,
	           * return_mail = NULL;
	char       * boundary = NULL,
	           * charset = NULL,
	           * attbound = NULL;
	char       * from, * to, * title;
	unsigned int i = 0;
	size_t       return_mail_len = 0;

	// make charset
	for ( i=0; i<STRLEN (o_ln); i++ )
		o_ln[i] = tolower(o_ln[i]);

	if ( ! strcmp (o_ln, "ko" ) )
		charset = "EUC-KR";
	else
		charset = o_ln;

	// make from
	if ( (from = generate_from (o_from, charset)) == NULL )
		return NULL;

	// make to
	if ( (to = generate_to (o_to, charset)) == NULL )
		return NULL;

	// make title
	if ( (title = generate_title (o_title, charset)) == NULL )
		return NULL;

	// make boundary
	boundary = make_boundary ();

	if ( STRLEN (o_attach) > 0 ) {
		// attach boundary
		attbound = make_boundary ();

		return_attach = generate_attach (o_attach, attbound);
		return_header = generate_header (from, to, title, attbound, o_attach);
	} else
		return_header = generate_header (from, to, title, boundary, o_attach);

	if ( (return_body = generate_body (charset, boundary, o_text, o_ptext)) == NULL ) {
		kr_safe_efree (boundary);
		kr_safe_efree (attbound);
		return NULL;
	}

	if ( STRLEN (o_attach) > 0 && return_attach > 0 ) {
		int athead_len = strlen (boundary) + strlen (attbound) + 128;
		char * tmp_attach_header;

		return_mail_len = strlen (return_header) + strlen (return_body) + strlen (return_attach) +
							strlen (attbound) + athead_len + 128;

		tmp_attach_header = emalloc (sizeof (char) * (athead_len));
		return_mail = emalloc (sizeof (char) * (return_mail_len));

		sprintf (
			tmp_attach_header,
			"\r\n--%s\r\n" \
			"Content-Type: multipart/alternative;\r\n" \
			"              boundary=\"%s\"\r\n\r\n",
			attbound, boundary
		);

		sprintf (
			return_mail,
			"%s\r\nThis is a multi-part message in MIME format.\r\n" \
			"%s%s\r\n%s\r\n--%s--\r\n",
		   	return_header, tmp_attach_header, return_body, return_attach, attbound
		);

		kr_safe_efree (return_attach);
		kr_safe_efree (tmp_attach_header);
	} else {
		return_mail_len = STRLEN (return_header) + STRLEN (return_body) + 128;
		return_mail = emalloc(sizeof (char) * (return_mail_len + 1));

		sprintf((char *) return_mail,
			"%s\r\nThis is a multi-part message in MIME format." \
			"\r\n%s\r\n",
			return_header, return_body
		);
	}

	kr_safe_efree (boundary);
	kr_safe_efree (attbound);

	kr_safe_efree (to);
	kr_safe_efree (from);
	kr_safe_efree (title);
	kr_safe_efree (return_body);
	kr_safe_efree (return_header);

	return return_mail;
} // }}}

char * generate_attach (char * path, char * bound) // {{{
{
	struct stat filebuf;
	FILE * fp;
	size_t fsize, filelen = 0, sumlen = 0, fencodelen = 0;
	char * fencode;
	char * mimetype, * tmpname, * filename,
		 * contents, * base64text, getattach[FILEBUFS] = { 0, };

	if ( (fp = fopen(path, "rb")) == NULL ) {
		php_error(E_WARNING, "Can't open attach file '%s' in read mode", path);
		return "";
	}

	// get file info
	stat (path, &filebuf);
	// original file size
	fsize = filebuf.st_size;

	// get file name from path
	if ( (tmpname = strrchr (path, '/')) != NULL )
	   	filename = estrdup (&path[tmpname - path + 1]);
	else if ( (tmpname = strrchr (path, '\\')) != NULL )
	   	filename = estrdup (&path[tmpname - path + 1]);
	else
	   	filename = estrdup (path);

	// get mime type
	mimetype = generate_mime ((char *) filename);

	contents = emalloc (sizeof (char) * (fsize + 32));
	memset (contents, '\n', sizeof (char) * (fsize + 32));

	while ( (filelen = fread (getattach, sizeof (char), FILEBUFS, fp)) > 0 ) {
		memmove (contents + sumlen, getattach, filelen);	
		sumlen += filelen;
	}
	fclose (fp);
	//contents[sumlen-1] = '\0';
	
	base64text = body_encode (contents, sumlen);

	fencodelen = STRLEN (bound) + STRLEN (mimetype) + (STRLEN (filename) * 2) + STRLEN (base64text) + 256;
	fencode = emalloc(sizeof (char) * (fencodelen));

	sprintf (fencode, "--%s\r\nContent-Type: %s; name=\"%s\"\r\nContent-Transfer-Encoding: " \
			          "base64\r\nContent-Disposition: inline; filename=\"%s\"\r\n\r\n%s\r\n",
			bound, mimetype, filename, filename, base64text);

	kr_safe_efree (base64text);
	kr_safe_efree (filename);
	kr_safe_efree (contents);

	return fencode;
} // }}}

char * generate_body (char * bset, char * bboundary, char * btext, char * bptext) // {{{
{
	char * rbody = NULL;
	char * plain = NULL,
	     * base64html = NULL,
	     * base64plain = NULL,
	     * buf = NULL; 
	int    plainlen = 0, htmllen = 0, nobptext = 0;

	if ( STRLEN (btext) > 0 ) {
		if ( bptext == NULL )
			nobptext = 1;
		else if
			( STRLEN (bptext) < 1 ) nobptext = 1;

		if ( nobptext ) {
			buf = html_to_plain (btext);
			plain = strtrim (buf);
			kr_safe_efree (buf);
		} else
			plain = strtrim (bptext);

		base64plain = body_encode (plain, -1);
		base64html  = body_encode (btext, -1);

		plainlen = STRLEN (base64plain);
		htmllen  = STRLEN (base64html);

		{
			int    tmp_body_len = plainlen + htmllen + (STRLEN (bset) * 2) + (STRLEN (bboundary) * 3) +182;
			char * tmp_body;
			tmp_body = emalloc (sizeof (char) * (tmp_body_len + 1));

			sprintf (
				tmp_body,
				"\r\n--%s\r\nContent-Type: text/plain; charset=%s\r\n" \
				"Content-Transfer-Encoding: base64\r\n\r\n%s\r\n\r\n--%s\r\n" \
				"Content-Type: text/html; charset=%s\r\nContent-Transfer-Encoding: " \
				"base64\r\n\r\n%s\r\n\r\n--%s--\r\n",
				bboundary, bset, base64plain, bboundary, bset, base64html, bboundary
			);

			rbody = estrdup (tmp_body);
			kr_safe_efree (tmp_body);
		}

		kr_safe_efree (plain);
		kr_safe_efree (base64plain);
		kr_safe_efree (base64html);
	}
	else
	{
		php_error (E_WARNING, "Empty mail body context");
		return NULL;
	}

	return rbody;
} // }}}

char * generate_header (char *from, char *to, char *subject,
						 char *boundary, char *is_attach) // {{{
{
	char * mailid, * datehead, * mimetype;
	int    buflen;
	char * buf;

	mimetype = (strlen (is_attach) > 0) ? "mixed" : "alternative";

	 /* make mail id */
	buf = kr_regex_replace ((char *) "/[^<]*<([^>]+)>.*/i", "\\1", from);
	mailid = generate_mail_id ((char *) buf);
	kr_safe_efree (buf);
	datehead = generate_date ();

	buflen = STRLEN (mailid) + STRLEN (from) + STRLEN (datehead) +
			STRLEN (to) + STRLEN (subject) + STRLEN (boundary) +
			STRLEN (mimetype) + 256;
	buf = emalloc ( sizeof (char) * buflen );

	sprintf (
		buf,
		"Message-ID: <%s>\r\nFrom: %s\r\nMIME-Version: 1.0\r\nDate: %s\r\n" \
		"To: %s\r\nSubject: %s\r\nContent-Type: multipart/%s;\r\n              " \
		"boundary=\"%s\"\r\n\r\n",
		mailid, from, datehead, to, subject, mimetype, boundary
	);

	kr_safe_efree (datehead);
	kr_safe_efree (mailid);

	return buf;
} // }}}

char * generate_from (char * email, char * set) // {{{
{
	char * rfrom = NULL;
	char * name = NULL, * mail = NULL, * name_t = NULL;
	int   namelen = 0, maillen = 0, setlen = STRLEN (set);

	name_t = NULL;

	if ( strlen (email) < 1 ) {
		php_error (E_WARNING, "Empty FROM address.");
		return NULL;
	}

	// get email address on NAME <email@address> form
	if ( strchr (email, '<' ) != NULL ) {
		char * buf = kr_regex_replace ("/[^<]*<([^>]+)>.*/i", "\\1", email);
		mail = strtrim (buf);
		kr_safe_efree (buf);
	} else
		mail = strtrim (email);

	maillen = strlen (mail);

	// get name on NAME <email@address> form
	if ( strchr (email, '<') != NULL ) {
		char * buf = kr_regex_replace ("/([^<]*)<[^>]+>.*/i", "\\1", email);
		name_t = strtrim (buf);
		name = name_t;
		kr_safe_efree (buf);
	} else
		name = "";
	
	if ( checkAddr (mail, 0) != 1 ) {
		php_error (E_WARNING, "%s is invalid email address form.", mail);
		return NULL;
	}
	
	if ( strlen (name) < 1 ) {
		rfrom = emalloc (sizeof (char) * ( maillen + 3));
		sprintf (rfrom, "<%s>", mail);
	} else {
		zend_string * cname = NULL;
		int from_len;

		cname = php_base64_encode ((unsigned char *) name, strlen (name));
		namelen = ZSTR_LEN (cname);

		from_len = setlen + maillen + namelen + 11;

		rfrom = emalloc (sizeof (char) * (from_len + 1));
		sprintf (rfrom, "=?%s?B?%s?= <%s>", set, ZSTR_VAL (cname), mail);
		zend_string_release (cname);
	}

	kr_safe_efree (mail);
	kr_safe_efree (name_t);

	return rfrom;
} // }}}

char * generate_to (char * toaddr, char * set) // {{{
{
	char  * to = NULL;
	const char delimiters[] = ",";
	char  * token, * btoken;
	char  * t_mail, * _t_name, * t_name;
	int maillen = 0, namelen = 0, setlen = STRLEN (set);

	if ( STRLEN (toaddr) < 1 ) {
		php_error(E_WARNING, "Empty TO address.");
		return NULL;
	}

	_t_name = NULL;

	token = strtok_r (toaddr, delimiters, &btoken);
	if ( token != NULL ) {
		// get email address on NAME <email@address> form
		if ( strchr (token, '<') != NULL ) {
			char * buf;

			buf = kr_regex_replace ("/[^<]*<([^>]+)>.*/i", "\\1", token);
			t_mail = strtrim (buf);
			kr_safe_efree (buf);

			buf = kr_regex_replace ("/([^<]*)<[^>]+>.*/i", "\\1", token);
			_t_name = strtrim (buf);
			kr_safe_efree (buf);

			t_name = _t_name;
			maillen = strlen (t_mail);
			namelen = strlen (t_name);
		} else {
			t_mail = strtrim (token);
			t_name = "";
		}

		// whether vaild or invalid email form
		if ( checkAddr (t_mail, 0) ) {
			char * t_to;
			int    to_lenth;

			t_to = NULL;

			if ( namelen < 1 ) {
				t_to = strtrim (token);
				to_lenth = strlen (t_to);
				to = emalloc (sizeof (char) * (to_lenth + 3));
				sprintf (to, "<%s>", t_to);
			} else {
				zend_string * cname = NULL;

				to_lenth = setlen + maillen + namelen + 32;
				to = emalloc (sizeof(char) * (to_lenth + 1));
				cname = php_base64_encode ((unsigned char *) t_name, namelen);
				sprintf (to, "=?%s?B?%s?= <%s>", set, ZSTR_VAL (cname), t_mail);
				zend_string_release (cname);
			}
			kr_safe_efree (t_to);
		}
		kr_safe_efree (t_mail);
		kr_safe_efree (_t_name);

		while ( (token = strtok_r (NULL, delimiters, &btoken)) != NULL ) {
			char * s_mail, * s_name, * _s_name;
			int    snlen = 0;

			_s_name = NULL;

			// get email address on NAME <email@address> form
			if ( strchr (token, '<') != NULL ) {
				char * buf;

				buf = kr_regex_replace ("/[^<]*<([^>]+)>.*/i", "\\1", token);
				s_mail = strtrim (buf);
				kr_safe_efree (buf);

				buf = kr_regex_replace ("/([^<]*)<[^>]+>.*/i", "\\1", token);
				_s_name = strtrim (buf);
				kr_safe_efree (buf);

				s_name = _s_name;
				snlen = strlen (s_name);
			} else {
				s_mail = strtrim (token);
				s_name = "";
			}

			// whether vaild or invalid email form
			if ( checkAddr (s_mail, 0) ) {
				char s_to[1024];

				memset (s_to, 0, sizeof (s_to));

				if ( snlen < 1 ) {
					memset (s_to, '\0', sizeof (s_to));
					sprintf (s_to, "<%s>", s_mail);
				} else {
					zend_string * sub_cname = NULL;

					sub_cname = php_base64_encode ((unsigned char *) s_name, snlen);
					sprintf (s_to, "=?%s?B?%s?= <%s>", set, ZSTR_VAL (sub_cname), s_mail);
					zend_string_release (sub_cname);
				}

				if ( to == NULL )
					to = estrdup ((char *) s_to);
				else {
					int add_to_len;
					char add_to[1024] = { 0, };

					sprintf ((char *) add_to, ", %s", s_to);
					add_to_len = strlen (add_to);
					to = erealloc (to, sizeof (char) * (strlen (to) + add_to_len + 1));
					strcat (to, add_to);
				}
			}

			kr_safe_efree (s_mail);
			if ( _s_name != NULL )
				kr_safe_efree (_s_name);
		}
	}

	if ( to == NULL ) {
		php_error (E_WARNING, "No valid TO address.");
		return NULL;
	}

	return to;
} // }}}

char * generate_title (char * title, char * set) // {{{
{
	int           len = 0, set_len = STRLEN (set);
	char        * subject = NULL;
	zend_string * base64 = NULL;

	if ( strlen (title) < 1 ) {
		php_error (E_WARNING, "Empty mail subject.");
		return NULL;
	}

	base64 = php_base64_encode ((unsigned char *) title, STRLEN (title));
	len = ZSTR_LEN (base64);

	{
		int subject_lenth = len + set_len + 8;
		subject = emalloc (sizeof (char) * (subject_lenth + 1));
		sprintf ( subject, "=?%s?B?%s?=", set, ZSTR_VAL (base64));
	}

	zend_string_release (base64);

	return subject;
} // }}}

char * generate_date () { // {{{
	time_t  now = time (NULL);
	char    buf[64] = { 0, },
		  * rdate = NULL;

	setlocale (LC_TIME, "C");
	loctime = localtime (&now);
	strftime (buf, 64, "%a, %d %b %Y %H:%M:%S %Z", loctime);

#ifdef PHP_WIN32
	rdate = (char *) kr_regex_replace ("/대한민국 표준시/", "+09:00", buf);
#else
	rdate = estrdup (buf);
#endif

	return rdate;
} // }}}

char * generate_mail_id (char * id) // {{{
{
	time_t now = time (0);
	char mailid[60] = { 0, };
	char idtime[15], *mark;
	char * ret;

	/* convert localtime */
	loctime = localtime (&now);
	strftime (idtime, 15, "%Y%m%d%H%M%S", loctime);

	/* mail id check */
	if ( STRLEN (id) == 0 )
		id = "OOPS_PHP_LIB";
	else {
		mark = strchr ( id, '@' );
		if ( mark != NULL )
			id[mark - id] = '\0';
	}

	/* get random number */
	srand (now);
	sprintf (mailid, "%s%d@%s", idtime, rand (), id);
	ret = estrdup (mailid);

	return ret;
} // }}}

char * make_boundary () // {{{
{
	int sec, usec, len;
	char bound[40] = { 0, };
	char bid[14];
	char first[9], second[9];
#if defined(__CYGWIN__)
    struct timespec tv;
#else
    struct timeval tv;
#endif
	char * ret;

	/* make uniq id */
	gettimeofday ((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
#if defined(__CYGWIN__)
	usec = (int) (tv.tv_nsec % 1000000);
#else
	usec = (int) (tv.tv_usec % 1000000);
#endif
	sprintf (bid, "%05x%07x", usec,sec);

	/* get lenth of uniq id */
	len = STRLEN (bid);

	sprintf (first,"%c%c%c%c%c%c%c%c",
			toupper(bid[1]),toupper(bid[2]),toupper(bid[3]),toupper(bid[4]),
			toupper(bid[5]),toupper(bid[6]),toupper(bid[7]),toupper(bid[8]));
	sprintf (second,"%c%c%c%c%c%c%c%c",
			toupper(bid[len-1]),toupper(bid[len-2]),toupper(bid[len-3]),
			toupper(bid[len-4]),toupper(bid[len-5]),toupper(bid[len-6]),
			toupper(bid[len-7]),toupper(bid[len-8]));

	sprintf (bound,"--=_NextPart_000_0%c%c%c_%s.%s",
			toupper(bid[3]), toupper(bid[1]), toupper(bid[0]), first, second);

	ret = estrdup (bound);
	return ret;
} // }}}

char * html_to_plain (char * source) // {{{
{
	char * rptext;
	char * src[3] = { ":^.*<BODY[^>]*>:si", ":<\\/BODY>.*$:si", ":</?[a-z][^>]*>:si" };
	char * des[3] = { " ", " ", " " };
			    
	rptext = kr_regex_replace_arr (src, des, source, 3);
				    
	return rptext;
} // }}}

char * body_encode (const char * str, int chklen) // {{{
{
	zend_string * Zenbase = NULL;
	char        * rencode = NULL,
	            * enbase = NULL;
	int           len = 0,
	              devide = 0,
	              nlen=0,
	              olen=0,
	              breakpoint =0,
	              tlen = 0;

	if ( chklen < 0 )
		chklen = strlen (str);

	Zenbase = php_base64_encode ((unsigned char *) str, (size_t) chklen);
	enbase = ZSTR_VAL (Zenbase);
	len = ZSTR_LEN (Zenbase);
	//zend_string_release (Zenbase);

	devide = (int) (len / 60);

	if ( len < 61 ) {
		zend_string_release (Zenbase);
		return enbase;
	}

	rencode = emalloc (sizeof (char) * (len + 16 + devide * 2));
	memset (rencode, '\0', len + 16 + devide * 2);

	while (1) {
		if ( strlen (enbase + olen) < 60 ) {
			tlen = strlen (enbase + olen);
			breakpoint = 1;
		} else
			tlen = 60;

		memmove (rencode + nlen, enbase + olen, tlen);
		nlen += tlen;
		olen += 60;
		memmove (rencode + nlen, "\r\n", 2);
		nlen += 2;

		if ( breakpoint == 1 )
			break;
	}

	zend_string_release (Zenbase);
	return rencode;
} // }}}

char *generate_mime (char *filename) // {{{
{
	char * tmpext, *ext;
	if ( (tmpext = strrchr (filename, '.')) == NULL )
		ext = "";
	ext = &filename[tmpext - filename + 1];

	if ( ! strcmp("ez", ext) ) return "application/andrew-inset";
	else if ( ! strcmp ("hqx", ext)) return "application/mac-binhex40";
	else if ( ! strcmp ("cpt", ext)) return "application/mac-compactpro";
	else if ( ! strcmp ("doc", ext)) return "application/msword";
	else if ( ! strcmp ("oda", ext)) return "application/oda";
	else if ( ! strcmp ("pdf", ext)) return "application/pdf";
	else if ( ! strcmp ("rtf", ext)) return "application/rtf";
	else if ( ! strcmp ("mif", ext)) return "application/vnd.mif";
	else if ( ! strcmp ("ppt", ext)) return "application/vnd.ms-powerpoint";
	else if ( ! strcmp ("slc", ext)) return "application/vnd.wap.slc";
	else if ( ! strcmp ("sic", ext)) return "application/vnd.wap.sic";
	else if ( ! strcmp ("wmlc", ext)) return "application/vnd.wap.wmlc";
	else if ( ! strcmp ("wmlsc", ext)) return "application/vnd.wap.wmlscriptc";
	else if ( ! strcmp ("bcpio", ext)) return "application/x-bcpio";
	else if ( ! strcmp ("bz2", ext)) return "application/x-bzip2";
	else if ( ! strcmp ("vcd", ext)) return "application/x-cdlink";
	else if ( ! strcmp ("pgn", ext)) return "application/x-chess-pgn";
	else if ( ! strcmp ("cpio", ext)) return "application/x-cpio";
	else if ( ! strcmp ("csh", ext)) return "application/x-csh";
	else if ( ! strcmp ("dvi", ext)) return "application/x-dvi";
	else if ( ! strcmp ("spl", ext)) return "application/x-futuresplash";
	else if ( ! strcmp ("gtar", ext)) return "application/x-gtar";
	else if ( ! strcmp ("hdf", ext)) return "application/x-hdf";
	else if ( ! strcmp ("js", ext)) return "application/x-javascript";
	else if ( ! strcmp ("ksp", ext)) return "application/x-kspread";
	else if ( ! strcmp ("kpr", ext) || ! strcmp ("kpt", ext)) return "application/x-kpresenter";
	else if ( ! strcmp ("chrt", ext)) return "application/x-kchart";
	else if ( ! strcmp ("kil", ext)) return "application/x-killustrator";
	else if ( ! strcmp ("skp", ext) || ! strcmp ("skd", ext) || ! strcmp ("skt", ext) ||
			 ! strcmp ("skm", ext)) return "application/x-koan";
	else if ( ! strcmp ("latex", ext)) return "application/x-latex";
	else if ( ! strcmp ("nc", ext) || ! strcmp ("cdf", ext)) return "application/x-netcdf";
	else if ( ! strcmp ("rpm", ext)) return "application/x-rpm";
	else if ( ! strcmp ("sh", ext)) return "application/x-sh";
	else if ( ! strcmp ("shar", ext)) return "application/x-shar";
	else if ( ! strcmp ("swf", ext)) return "application/x-shockwave-flash";
	else if ( ! strcmp ("sit", ext)) return "application/x-stuffit";
	else if ( ! strcmp ("sv4cpio", ext)) return "application/x-sv4cpio";
	else if ( ! strcmp ("sv4crc", ext)) return "application/x-sv4crc";
	else if ( ! strcmp ("tar", ext)) return "application/x-tar";
	else if ( ! strcmp ("tcl", ext)) return "application/x-tcl";
	else if ( ! strcmp ("tex", ext)) return "application/x-tex";
	else if ( ! strcmp ("texinfo", ext) || ! strcmp ("texi", ext)) return "application/x-texinfo";
	else if ( ! strcmp ("t", ext) || ! strcmp ("tr", ext) ||
			 ! strcmp ("roff", ext)) return "application/x-troff";
	else if ( ! strcmp("man", ext)) return "application/x-troff-man";
	else if ( ! strcmp("me", ext)) return "application/x-troff-me";
	else if ( ! strcmp("ms", ext)) return "application/x-troff-ms";
	else if ( ! strcmp("ustar", ext)) return "application/x-ustar";
	else if ( ! strcmp("src", ext)) return "application/x-wais-source";
	else if ( ! strcmp("zip", ext)) return "application/zip";
	else if ( ! strcmp("gif", ext)) return "image/gif";
	else if ( ! strcmp("ief", ext)) return "image/ief";
	else if ( ! strcmp("wbmp", ext)) return "image/vnd.wap.wbmp";
	else if ( ! strcmp("ras", ext)) return "image/x-cmu-raster";
	else if ( ! strcmp("pnm", ext)) return "image/x-portable-anymap";
	else if ( ! strcmp("pbm", ext)) return "image/x-portable-bitmap";
	else if ( ! strcmp("pgm", ext)) return "image/x-portable-graymap";
	else if ( ! strcmp("ppm", ext)) return "image/x-portable-pixmap";
	else if ( ! strcmp("rgb", ext)) return "image/x-rgb";
	else if ( ! strcmp("xbm", ext)) return "image/x-xbitmap";
	else if ( ! strcmp("xpm", ext)) return "image/x-xpixmap";
	else if ( ! strcmp("xwd", ext)) return "image/x-xwindowdump";
	else if ( ! strcmp("css", ext)) return "text/css";
	else if ( ! strcmp("rtx", ext)) return "text/richtext";
	else if ( ! strcmp("rtf", ext)) return "text/rtf";
	else if ( ! strcmp("tsv", ext)) return "text/tab-separated-values";
	else if ( ! strcmp("sl", ext)) return "text/vnd.wap.sl";
	else if ( ! strcmp("si", ext)) return "text/vnd.wap.si";
	else if ( ! strcmp("wml", ext)) return "text/vnd.wap.wml";
	else if ( ! strcmp("wmls", ext)) return "text/vnd.wap.wmlscript";
	else if ( ! strcmp("etx", ext)) return "text/x-setext";
	else if ( ! strcmp("xml", ext)) return "text/xml";
	else if ( ! strcmp("avi", ext)) return "video/x-msvideo";
	else if ( ! strcmp("movie", ext)) return "video/x-sgi-movie";
	else if ( ! strcmp("wma", ext)) return "audio/x-ms-wma";
	else if ( ! strcmp("wax", ext)) return "audio/x-ms-wax";
	else if ( ! strcmp("wmv", ext)) return "video/x-ms-wmv";
	else if ( ! strcmp("wvx", ext)) return "video/x-ms-wvx";
	else if ( ! strcmp("wm", ext)) return "video/x-ms-wm";
	else if ( ! strcmp("wmx", ext)) return "video/x-ms-wmx";
	else if ( ! strcmp("wmz", ext)) return "application/x-ms-wmz";
	else if ( ! strcmp("wmd", ext)) return "application/x-ms-wmd";
	else if ( ! strcmp("ice", ext)) return "x-conference/x-cooltalk";
	else if ( ! strcmp("ra", ext)) return "audio/x-realaudio";
	else if ( ! strcmp("wav", ext)) return "audio/x-wav";
	else if ( ! strcmp("png", ext)) return "image/png";
	else if ( ! strcmp("asf", ext) || ! strcmp ("asx", ext)) return "video/x-ms-asf";
	else if ( ! strcmp("html", ext) || ! strcmp ("htm", ext)) return "text/html";
	else if ( ! strcmp("smi", ext) || ! strcmp ("smil", ext)) return "application/smil";
	else if ( ! strcmp("gz", ext) || ! strcmp ("tgz", ext)) return "application/x-gzip";
	else if ( ! strcmp("kwd", ext) || ! strcmp ("kwt", ext)) return "application/x-kword";
	else if ( ! strcmp("kpr", ext) || ! strcmp ("kpt", ext)) return "application/x-kpresenter";
	else if ( ! strcmp("au", ext) || ! strcmp ("snd", ext)) return "audio/basic";
	else if ( ! strcmp("ram", ext) || ! strcmp ("rm", ext)) return "audio/x-pn-realaudio";
	else if ( ! strcmp("pdb", ext) || ! strcmp ("xyz", ext)) return "chemical/x-pdb";
	else if ( ! strcmp("tiff", ext) || ! strcmp ("tif", ext)) return "image/tiff";
	else if ( ! strcmp("igs", ext) || ! strcmp ("iges", ext)) return "model/iges";
	else if ( ! strcmp("wrl", ext) || ! strcmp ("vrml", ext)) return "model/vrml";
	else if ( ! strcmp("asc", ext) || ! strcmp ("txt", ext) || ! strcmp ("php", ext)) return "text/plain";
	else if ( ! strcmp("sgml", ext) || ! strcmp ("sgm", ext)) return "text/sgml";
	else if ( ! strcmp("qt", ext) || ! strcmp ("mov", ext)) return "video/quicktime";
	else if ( ! strcmp("ai", ext) || ! strcmp ("eps", ext) || ! strcmp ("ps", ext)) return "application/postscript";
	else if ( ! strcmp("dcr", ext) || ! strcmp ("dir", ext) || ! strcmp ("dxr", ext)) return "application/x-director";
	else if ( ! strcmp("mid", ext) || ! strcmp ("midi", ext) || ! strcmp ("kar", ext)) return "audio/midi";
	else if ( ! strcmp("mpga", ext) || ! strcmp ("mp2", ext) || ! strcmp ("mp3", ext)) return "audio/mpeg";
	else if ( ! strcmp("aif", ext) || ! strcmp ("aiff", ext) || ! strcmp ("aifc", ext)) return "audio/x-aiff";
	else if ( ! strcmp("jpeg", ext) || ! strcmp ("jpg", ext) || ! strcmp ("jpe", ext)) return "image/jpeg";
	else if ( ! strcmp("msh", ext) || ! strcmp ("mesh", ext) || ! strcmp ("silo", ext)) return "model/mesh";
	else if ( ! strcmp("mpeg", ext) || ! strcmp ("mpg", ext) || ! strcmp ("mpe", ext)) return "video/mpeg";
	else return "application/octet-stream";
	/*
	else if (!strcmp("bin", ext) || !strcmp("dms", ext) || !strcmp("lha", ext) ||
			 !strcmp("lzh", ext) || !strcmp("exe", ext) || !strcmp("class", ext))
		return "application/octet-stream";
	else return "file/unknown";
	*/
} // }}}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
