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
$Id: php_krfile.h,v 1.2 2002-08-22 09:27:12 oops Exp $
*/
#ifndef PHP_KRFILE_H
#define PHP_KRFILE_H

PHP_FUNCTION(human_fsize_lib);
PHP_FUNCTION(getfiletype_lib);
PHP_FUNCTION(getfile_lib);
PHP_FUNCTION(putfile_lib);
PHP_FUNCTION(filelist_lib);

void writefile(unsigned char *filename, unsigned char *str_o, unsigned int mode_o);
unsigned char *read_file(unsigned char *filename, unsigned int filesize);
unsigned char *human_file_size (double size_o, int sub_o);
unsigned int round_value (double size_o);
unsigned int check_filedev (unsigned char *path_f, unsigned char *filename);
unsigned char *includePath (unsigned char *filepath);

#define RETURN_FILE_TYPE 1
#define RETURN_DIR_TYPE 2
#define RETURN_LINK_TYPE 3

#endif /* PHP_KRFILE_H */
