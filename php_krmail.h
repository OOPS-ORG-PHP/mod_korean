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

#ifndef PHP_KRMAIL_H
#define PHP_KRMAIL_H

#define FILEBUFS 4096

PHP_FUNCTION(mailsource_lib);

char  * generate_mail (
	char * o_ln, char * o_form, char * o_to, char * o_title, char * o_text,
	char * o_ptext, char * o_attach
);
char * generate_from (char * email, char * set);
char * generate_to (char * toaddr, char * set);
char * generate_title (char * title, char * set);
char * generate_header (char * from, char * to, char * subject, char * boundary, char * is_attach);
char * generate_body (char * bset, char * bboundary, char * btext, char * bptext);
char * generate_attach (char * path, char * bound);
char * make_boundary ();
char * generate_mail_id (char * id);
char * generate_date ();
char * html_to_plain (char * text);
char * body_encode (const char * str, int chklen);
char * generate_mime (char * filename);

char * kr_regex_replace(char * regex_o, char * replace_o, char * str_o);
char * kr_regex_replace_arr(char * regex[], char * replace[], char * str, int regex_no);
PHPAPI extern zend_string * php_base64_encode (const UChar *, size_t);

#endif  /* end PHP_KRMAIL_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
