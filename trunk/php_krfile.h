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
$Id: php_krfile.h,v 1.11 2003-05-13 19:14:41 oops Exp $
*/
#ifndef PHP_KRFILE_H
#define PHP_KRFILE_H

PHP_FUNCTION(human_fsize_lib);
PHP_FUNCTION(getfiletype_lib);
PHP_FUNCTION(pcregrep_lib);
PHP_FUNCTION(getfile_lib);
PHP_FUNCTION(putfile_lib);
PHP_FUNCTION(filelist_lib);

void writefile(unsigned char *filename, unsigned char *str_o, unsigned int mode_o);
unsigned char *readfile(unsigned char *filename);
unsigned char *human_file_size (double size_o, int sub_o);
unsigned int check_filedev (unsigned char *path_f, unsigned char *filename);
unsigned char *includePath (unsigned char *filepath);

#define FILEBUFS 4096
#define MAXPATHLENGTH 1024
#define RETURN_FILE_TYPE 1
#define RETURN_DIR_TYPE 2
#define RETURN_LINK_TYPE 3

#ifdef PHP_WIN32
	#define __S_ISTYPE(mode, mask) (((mode) & S_IFMT) == (mask))
	#define S_ISDIR(mode) __S_ISTYPE((mode), S_IFDIR)
	#define S_ISREG(mode) __S_ISTYPE((mode), S_IFREG)
	#define S_ISLNK(mode) __S_ISTYPE((mode), S_IFLNK)
#endif

#endif /* PHP_KRFILE_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
