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
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id: php_krnetwork.h,v 1.6 2002-08-23 01:36:22 oops Exp $
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
int sock_sendmail (unsigned char *fromaddr, unsigned char *toaddr, unsigned char *text, int debug);
unsigned char *sockhttp (unsigned char *addr, int record, unsigned char *recfile);

#endif
/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

