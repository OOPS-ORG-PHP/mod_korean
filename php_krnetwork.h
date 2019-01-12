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

#ifndef PHP_KRNETWORK_H
#define PHP_KRNETWORK_H

PHP_FUNCTION(get_hostname_lib);
PHP_FUNCTION(readfile_lib);
PHP_FUNCTION(sockmail_lib);

static char *kr_gethostbyaddr(char *ip);
int socksend (int sock, int deb, unsigned char *var, unsigned char *target);
unsigned char *get_mx_record (unsigned char *str);
void debug_msg (unsigned char *msg, int info, int bar);
int sock_sendmail (unsigned char *fromaddr, unsigned char *toaddr, unsigned char *text, unsigned char *host, int debug);
unsigned char *sockhttp (unsigned char *addr, size_t *retSize, int record, unsigned char *recfile);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
