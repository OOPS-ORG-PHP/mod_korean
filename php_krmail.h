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
$Id$
*/

#ifndef PHP_KRMAIL_H
#define PHP_KRMAIL_H

#define FILEBUFS 4096

PHP_FUNCTION(mailsource_lib);

unsigned char * generate_mail (unsigned char *o_ln, unsigned char *o_form, unsigned char *o_to,
			       unsigned char *o_title, unsigned char *o_text, unsigned char *o_ptext,
			       unsigned char *o_attach);
unsigned char * generate_from (unsigned char *email, char *set);
unsigned char * generate_to (unsigned char *toaddr, char *set);
unsigned char * generate_title (unsigned char *title, unsigned char *set);
unsigned char * generate_header (unsigned char *from, unsigned char *to, unsigned char *subject, char *boundary, unsigned char *is_attach);
unsigned char * generate_body (unsigned char *bset, unsigned char *bboundary, unsigned char *btext, unsigned char *bptext);
unsigned char * generate_attach (unsigned char *path, unsigned char *bound);
char * make_boundary ();
char * generate_mail_id (char * id);
char * generate_date ();
unsigned char * html_to_plain (unsigned char * text);
unsigned char * body_encode (const unsigned char *str, int chklen);
unsigned char *generate_mime (unsigned char *filename);

unsigned char *kr_regex_replace(unsigned char *regex_o, unsigned char *replace_o, unsigned char *str_o);
unsigned char *kr_regex_replace_arr(unsigned char *regex[], unsigned char *replace[], unsigned char *str, unsigned int regex_no);
PHPAPI extern unsigned char *php_base64_encode(const unsigned char *, int, int *);

#endif  /* end PHP_KRMAIL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
