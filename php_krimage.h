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
$Id: php_krimage.h,v 1.3 2002-06-03 17:35:50 oops Exp $
*/
#ifndef PHP_KRIMAGE_H
#define PHP_KRIMAGE_H

#if HAVE_LIBGD
PHP_FUNCTION(imgresize_lib);

#define	GIFNEWTYPE	1
#define	PNGNEWTYPE	2
#define	JPGNEWTYPE	3

#endif /* HAVE_LIBGD */

#endif /* PHP_KRFILE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
