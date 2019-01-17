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

char * generate_mail (char *o_ln, char *o_form, char *o_to,
			       char *o_title, char *o_text, char *o_ptext,
			       char *o_attach);
char * generate_from (char *email, char *set);
char * generate_to (char *toaddr, char *set);
char * generate_title (char *title, char *set);
char * generate_header (char *from, char *to, char *subject, char *boundary, char *is_attach);
char * generate_body (char *bset, char *bboundary, char *btext, char *bptext);
char * generate_attach (char *path, char *bound);
char * make_boundary ();
char * generate_mail_id (char * id);
char * generate_date ();
char * html_to_plain (char * text);
char * body_encode (const char *str, int chklen);
char *generate_mime (char *filename);

char *kr_regex_replace(char *regex_o, char *replace_o, char *str_o);
char *kr_regex_replace_arr(char *regex[], char *replace[], char *str, int regex_no);
PHPAPI extern UChar *php_base64_encode(const UChar *, int, int *);

#endif  /* end PHP_KRMAIL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
