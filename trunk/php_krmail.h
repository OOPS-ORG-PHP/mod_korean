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
$Id: php_krmail.h,v 1.1 2002-08-05 18:26:09 oops Exp $
*/

#ifndef PHP_KRMAIL_H
#define PHP_KRMAIL_H

PHP_FUNCTION(mailsource_lib);

unsigned char * generate_mail (unsigned char *ln, unsigned char *form, unsigned char *to,
			       unsigned char *title, unsigned char *text, unsigned char *attach);
unsigned char * generate_from (unsigned char *email, char *set);
unsigned char * generate_to (unsigned char *toaddr, char *set);
unsigned char * generate_title (unsigned char *title, unsigned char *set);
unsigned char * generate_header (unsigned char *from, unsigned char *to, unsigned char *subject, char *boundary);
char * make_boundary ();
char * generate_mail_id (char * id);
char * generate_date ();

#endif  /* end PHP_KRMAIL_H */
