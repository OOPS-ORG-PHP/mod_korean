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
$Id: php_krerror.h,v 1.1.1.1 2002-05-14 09:50:50 oops Exp $ 
*/

#ifndef PHP_KRERROR_H
#define PHP_KRERROR_H

PHP_FUNCTION(perror_lib);
PHP_FUNCTION(pnotice_lib);
unsigned char *print_error (unsigned char *str_o, unsigned int java_o, unsigned char *move_o, unsigned int sec_o);

#endif /* PHP_KRERROR_H */