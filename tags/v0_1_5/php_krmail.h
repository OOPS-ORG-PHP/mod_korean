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

UChar * generate_mail (UChar *o_ln, UChar *o_form, UChar *o_to,
			       UChar *o_title, UChar *o_text, UChar *o_ptext,
			       UChar *o_attach);
UChar * generate_from (UChar *email, char *set);
UChar * generate_to (UChar *toaddr, char *set);
UChar * generate_title (UChar *title, UChar *set);
UChar * generate_header (UChar *from, UChar *to, UChar *subject, char *boundary, UChar *is_attach);
UChar * generate_body (UChar *bset, UChar *bboundary, UChar *btext, UChar *bptext);
UChar * generate_attach (UChar *path, UChar *bound);
char * make_boundary ();
char * generate_mail_id (char * id);
char * generate_date ();
UChar * html_to_plain (UChar * text);
UChar * body_encode (const UChar *str, int chklen);
UChar *generate_mime (UChar *filename);

UChar *kr_regex_replace(UChar *regex_o, UChar *replace_o, UChar *str_o);
UChar *kr_regex_replace_arr(UChar *regex[], UChar *replace[], UChar *str, unsigned int regex_no);
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
