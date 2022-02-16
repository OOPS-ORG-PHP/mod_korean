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
#ifndef PHP_KRFILE_H
#define PHP_KRFILE_H

PHP_FUNCTION(human_fsize_lib);
PHP_FUNCTION(getfiletype_lib);
PHP_FUNCTION(pcregrep_lib);
PHP_FUNCTION(getfile_lib);
PHP_FUNCTION(putfile_lib);
PHP_FUNCTION(filelist_lib);

int writefile (char * filename, char * str_o, int mode_o);
char * readfile (char * filename);
char * human_file_size (double size_o, int sub_o, int unit, int cunit);
int check_filedev (char * path_f, char * filename);
char * includePath (char * filepath);
int numberOfchar (char * str, char chk);
int pcre_match (char * regex, char * str);

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
