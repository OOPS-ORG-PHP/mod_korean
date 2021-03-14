/*
  +----------------------------------------------------------------------+
  | Copyright (c) 2021 The PHP Group                                     |
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
#ifndef PHP_KRIMAGE_H
#define PHP_KRIMAGE_H

#if HAVE_KRLIBGD
PHP_FUNCTION(imgresize_lib);

#define HAVE_GD_GIF_CREATE 1
#define HAVE_GD_GIF_READ 1
#define HAVE_GD_GIF_CTX 1

#define	GIFNEWTYPE	1
#define	PNGNEWTYPE	2
#define	JPGNEWTYPE	3

#endif /* HAVE_KRLIBGD */

int checkReg (char * str, char * regex_o);
char * sockhttp (char * addr, size_t * retSize, int record, char * recfile);

#endif /* PHP_KRFILE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
