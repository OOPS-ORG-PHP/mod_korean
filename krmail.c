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
  
  $Id: krmail.c,v 1.1 2002-08-05 18:26:09 oops Exp $ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <time.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "zend_API.h"
#include "php_krmail.h"

struct tm *loctime;

/* {{{ proto int mailsource_lib(unsigned char *ln, unsigned char *ctype, unsigned char *from
 *								unsigned char *to, unsigned char *title, unsigned char *text
 *								unsigned char *ptext)
 * make mail source */
PHP_FUNCTION(mailsource_lib)
{
	pval **ln, **from, **to, **title, **text, **attach;
	unsigned char *c_ln, *c_from, *c_to, *c_title;
	unsigned char *c_text, *c_attach, *ret;

	switch( ZEND_NUM_ARGS() )
	{
		case 5:
			if( zend_get_parameters_ex(5, &ln, &from, &to, &title, &text) == FAILURE )
			{
				WRONG_PARAM_COUNT;
			}
			c_attach = NULL;
			break;
		case 6:
			if( zend_get_parameters_ex(6, &ln, &from, &to, &title, &text, &attach) == FAILURE )
		   	{
				WRONG_PARAM_COUNT;
			}
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

	ret = generate_mail(c_ln, c_from, c_to, c_title, c_text, c_attach);
	RETURN_STRING(ret,1);
}
/* }}} */

unsigned char * generate_mail (unsigned char *o_ln, unsigned char *o_from, unsigned char *o_to,
							   unsigned char *o_title, unsigned char *o_text, unsigned char *o_attach)
{
	unsigned char *return_header, header[4096];
	char *boundary, *charset;
	unsigned char *from, *to, *title, *text;
	unsigned int i = 0;

	/* make charset */
	for( i=0; i < strlen(o_ln); i++ )
   	{
		o_ln[i] = tolower(o_ln[i]);
	}
	charset = !strcmp(o_ln,"ko") ? "EUC-KR" : "iso-8859-1";

	/* make from */
	from = generate_from(o_from, charset);

	/* make to */
	to = generate_to(o_to, charset);

	/* make title */
	title = generate_title(o_title, charset);

	/* make boundary */
	boundary = make_boundary();

	return_header = generate_header (from, to, title, boundary);

	//sprintf(header, "%s\n%s\n%s\n%s\n%s\n", charset, boundary, from, to, title);
	//return_header = estrdup(header);
	return return_header;
}

unsigned char * generate_header (unsigned char *from, unsigned char *to, unsigned char *subject,
								 char *boundary)
{
	char *mailid, *datehead;
	unsigned char buf[4096], *ret;

	 /* make mail id */
	mailid = generate_mail_id( (char *) kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1", from) );
	datehead = generate_date();

	sprintf(buf, "Message-ID: <%s>\r\nFrom: %s\r\nMIME-Version: 1.0\r\nDate: %s\r\nTo: %s\r\nSubject: %s\r\nContent-Type: multipart/alternative;\r\n              boundary=\"--%s\"\r\n\r\n", mailid, from, datehead, to, subject, boundary);

	ret = (char *) estrdup(buf);
	return ret;
}

unsigned char * generate_from (unsigned char *email, char *set)
{
	unsigned char *ret, *name, *cname, *mail;
	unsigned int namelen = 0, maillen = 0, setlen = strlen(set);

	if ( strlen(email) < 1 )
	{
		php_error(E_ERROR, "Don't exist FROM address.");
	}

	/* get email address on NAME <email@address> form */
	mail = (unsigned char *) strtrim(kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1", email));
	maillen = strlen(mail);

	/* get name on NAME <email@address> form */
	if ( strchr(email,'<') != NULL )
   	{ name = (unsigned char *) strtrim(kr_regex_replace("/([^<]*)<[^>]+>.*/i","\\1", email)); }
	else
	{ name = ""; }
	
	if ( checkAddr (mail,0) != 1 )
	{ php_error(E_ERROR, "%s is invalid email address form.", mail); }
	
	if ( strlen(name) < 1 )
   	{ ret = estrndup(mail, maillen); }
	else
	{
		cname = (unsigned char *) php_base64_encode(name, strlen(name), &namelen);
		{
			unsigned char tmp_from[setlen + maillen + namelen + 11];
			sprintf(tmp_from, "=?%s?B?%s?= <%s>", set, cname, mail);
			ret = estrndup( tmp_from, strlen(tmp_from) );
		}
	}

	return ret;
}

unsigned char * generate_to (unsigned char *toaddr, char *set)
{
	unsigned char delimiters[] = ",";
	unsigned char *token, *t_mail, *t_name, *to = NULL, *cname, *ret;
	int free = 0, maillen = 0, namelen = 0, setlen = strlen(set);

	if ( strlen(toaddr) < 1 )
	{
		php_error(E_ERROR, "Don't exist TO address.");
	}

	token = strtok(toaddr, delimiters);
	if ( token != NULL )
	{

		/* get email address on NAME <email@address> form */
		if ( strchr(token,'<') != NULL )
		{
			t_mail = (unsigned char *) strtrim( kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1",token) );
			t_name = (unsigned char *) strtrim( kr_regex_replace("/([^<]*)<[^>]+>.*/i","\\1",token) );
			maillen = strlen(t_mail);
			namelen = strlen(t_name);
		}
		else
		{
			t_mail = estrdup( (unsigned char *) strtrim(token) );
			t_name = "";
		}

		/* whether vaild or invalid email form */
		if ( checkAddr(t_mail, 0) )
		{

			if ( namelen < 1 )
			{
				to = estrdup( (unsigned char *) strtrim(token) );
			}
			else
			{
				unsigned char t_to[setlen + maillen + namelen  + 11];
				cname = (unsigned char *) php_base64_encode(t_name, namelen, &namelen);
				sprintf(t_to, "=?%s?B?%s?= <%s>", set, cname, t_mail);
				to = estrdup(t_to);
			}
		}

		while ( (token = strtok(NULL, delimiters)) != NULL )
		{
			{
				unsigned char *s_name, *s_mail, *sub_cname;
				unsigned int snlen = 0, smlen = 0;

				/* get email address on NAME <email@address> form */
				if ( strchr(token,'<') != NULL )
				{
					s_mail = (unsigned char *) strtrim( kr_regex_replace("/[^<]*<([^>]+)>.*/i","\\1", token) );
					s_name = (unsigned char *) strtrim( kr_regex_replace("/([^<]*)<[^>]+>.*/i","\\1", token) );
					smlen = strlen(s_mail);
					snlen = strlen(s_name);
				}
				else
				{
					s_mail = estrdup( (unsigned char *) strtrim(token) );
					s_name = "";
				}

				/* whether vaild or invalid email form */
				if ( checkAddr(s_mail, 0) )
				{
					unsigned char s_to[200];
					if ( snlen < 1 )
					{
						sprintf(s_to, "%s", s_mail);
					}
					else
					{
						sub_cname = (unsigned char *) php_base64_encode(s_name, snlen, &namelen);
						sprintf(s_to, "=?%s?B?%s?= <%s>", set, sub_cname, s_mail);
					}

					if ( to == NULL )
					{
						to = (unsigned char *) estrdup(s_to);
					}
					else
					{
						unsigned int add_to_len = strlen(s_to) + 3;
						unsigned char add_to[add_to_len];
						free = 1;
						sprintf(add_to, ", %s", s_to);
						to = (unsigned char *) erealloc(to, strlen(to) + add_to_len);
						strcat(to, add_to);
					}
				}
			}
		}
	}

	if ( to != NULL )
	{
		ret = (unsigned char *) estrdup(to);
		if ( free == 1) { efree(to); }
	}
	else
	{
		php_error(E_ERROR, "Don't exist valid TO address.");
	}

	return ret;
}

unsigned char * generate_title (unsigned char *title, unsigned char *set)
{
	unsigned int len = 0, set_len = strlen(set);
	unsigned char *base64, *ret;

	if ( strlen(title) < 1 )
	{
		php_error(E_ERROR, "Don't exists mail subject.");
	}

	base64 = (unsigned char *) php_base64_encode(title, strlen(title), &len);

	{
		unsigned char subject[len + set_len + 8];
		sprintf(subject, "=?%s?B?%s?=", set, base64);

		ret = (unsigned char *) estrdup(subject);
	}

	return ret;
}

char * generate_date () {
	time_t now = time(NULL);
	char buf[40], *ret;

	loctime = localtime(&now);
	strftime(buf, 40, "%a, %d %b %Y %H:%M:%S %Z", loctime);

	ret = estrdup(buf);
	return ret;
}

char * generate_mail_id (char *id)
{
	char *mark, generate[60], *mailid;
	time_t now = time(0);
	unsigned char idtime[15];

	/* convert localtime */
	loctime = localtime(&now);
	strftime (idtime, 15, "%Y%m%d%H%M%S", loctime);

	/* mail id check */
	if ( strlen(id) == 0 ) { id = "OOPS_PHP_LIB"; }
	else
	{
		if ( (mark = strchr(id,'@')) != NULL ) { id[mark-id] = '\0'; }
	}

	/* get random number */
	srand(now);
	sprintf(generate, "%s%d@%s", idtime, rand(), id);
	mailid = (char *) strdup(generate);

	return mailid;
}

char * make_boundary ()
{
	int sec, usec, len, i;
	char bid[14], bound[40], *ret;
	char first[2], second[9], third[9];
    struct timeval tv;

	/* make uniq id */
	gettimeofday((struct timeval *) &tv, (struct timezone *) NULL);
	sec = (int) tv.tv_sec;
	usec = (int) (tv.tv_usec % 1000000);
	sprintf(bid,"%07x%05x",sec,usec);

	/* get lenth of uniq id */
	len = strlen(bid);

	sprintf(first,"%c",bid[0]);
	sprintf(second,"%c%c%c%c%c%c%c%c",
			toupper(bid[1]),toupper(bid[2]),toupper(bid[3]),toupper(bid[4]),
			toupper(bid[5]),toupper(bid[6]),toupper(bid[7]),toupper(bid[8]));
	sprintf(third,"%c%c%c%c%c%c%c%c",
			toupper(bid[len-1]),toupper(bid[len-2]),toupper(bid[len-3]),
			toupper(bid[len-4]),toupper(bid[len-5]),toupper(bid[len-6]),
			toupper(bid[len-7]),toupper(bid[len-8]));


	sprintf(bound,"--=_NextPart_000_000%s_%s.%s",first,second,third);
	ret = (char *) strdup(bound);

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
