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
 
  $Id: krfile.c,v 1.2 2002-06-01 22:34:33 oops Exp $ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <dirent.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "ext/standard/php_math.h"
#include "zend_API.h"
#include "php_krfile.h"

/* {{{ proto int human_fsize_lib(int filesize, int pt)
 *  *  print move action to url */
PHP_FUNCTION(human_fsize_lib) {
	pval **fsize, **pt;
	unsigned int sub = 0;
	unsigned char *ret;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &fsize) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &fsize, &pt) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(pt);
			sub = Z_LVAL_PP(pt);
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(fsize);

	ret = human_file_size(Z_DVAL_PP(fsize),sub);
	RETURN_STRING(ret,1);
}
/* }}} */

/* {{{ proto array filelist_lib(string path, [ string mode, [ string regex ] ])
 *  * return file list in path */
PHP_FUNCTION(filelist_lib) {
	pval **path, **mode, **regex;
	DIR *dp;
	unsigned char *mode_s, *regex_s;
	int chkReg = 0;
	regex_t preg;

	struct dirent *d;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &path) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			mode_s = "";
			regex_s = "";
			break;
		case 2:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &path, &mode) == FAILURE) {
				 WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(path);
			mode_s = Z_STRVAL_PP(mode);
			regex_s = "";
			break;
		case 3:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &path, &mode, &regex) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(path);
			convert_to_string_ex(regex);

			if (Z_STRLEN_PP(mode) == 0) { mode_s = ""; }
			else { mode_s = Z_STRVAL_PP(mode); }

			if (Z_STRLEN_PP(regex) > 0) { regex_s = Z_STRVAL_PP(regex); }
			else { regex_s = ""; }
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(path);
	if (Z_STRLEN_PP(path) == 0) { RETURN_FALSE; }

	if ( array_init(return_value) == FAILURE) {
		RETURN_FALSE;
	}

	if ( (dp = opendir(Z_STRVAL_PP(path))) == NULL ) {
		php_error(E_ERROR,"Can't open %s directory in read mode",Z_STRVAL_PP(path));
	}

	if (strlen(regex_s)) {
		if (regcomp(&preg,regex_s,REG_EXTENDED) != 0) {
			php_error(E_WARNING,"Problem REGEX compile in PHP");
			RETURN_FALSE;
		}
	}

	while ( d = readdir(dp) ) {
		if ( d->d_ino != 0) {
			if ( !strcmp(d->d_name,".") || !strcmp(d->d_name,"..")) { continue; }

			if (!strcmp(mode_s,"f")) {
				if (check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_FILE_TYPE) { continue; }
			} else if (!strcmp(mode_s,"d")) {
				if (check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_DIR_TYPE) { continue; }
			} else if (!strcmp(mode_s,"l")) {
				if (check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_LINK_TYPE) { continue; }
			} else if (!strcmp(mode_s,"fd")) {
				if (check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_FILE_TYPE &&
					check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_DIR_TYPE) { continue; }
			} else if (!strcmp(mode_s,"fl")) {
				if (check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_FILE_TYPE &&
					check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_LINK_TYPE) { continue; }
			} else if (!strcmp(mode_s,"dl")) {
				if (check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_LINK_TYPE &&
					check_filedev (Z_STRVAL_PP(path),d->d_name) != RETURN_DIR_TYPE) { continue; }
			} 

			if ( strlen(regex_s) && regexec(&preg,d->d_name,0,NULL,0) != 0) { continue; }

			add_next_index_string(return_value,d->d_name,1);
		}
	}

	if (strlen(regex_s)) { regfree(&preg); }
	closedir(dp);
}
/* }}} */

/* {{{ proto void put_file_lib(string filename, stirng str, [ int mode ])
 *  * write file */
PHP_FUNCTION(putfile_lib) {
	pval **filename, **str, **mode;
	unsigned int write = 0;

	switch(ZEND_NUM_ARGS()) {
		case 2:
			if(zend_get_parameters_ex(2, &filename, &str) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(3, &filename, &str, &mode) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(mode);
			if ( Z_LVAL_PP(mode) == 1 ) { write = 1; }
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(filename);
	convert_to_string_ex(str);

	if (Z_STRLEN_PP(filename) == 0) {
		RETURN_FALSE;
	}

	writefile(Z_STRVAL_PP(filename),Z_STRVAL_PP(str),write);
}
/* }}} */

/* {{{ proto string get_file_lib(string filename, [ int readsize ])
 *  * return file context */
PHP_FUNCTION(getfile_lib) {
	pval **filename, **fsize;
	FILE *fp;
	unsigned size = 0;
	unsigned char *str;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &filename) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &filename, &fsize) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(fsize);
			size = Z_LVAL_PP(fsize);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(filename);

	if (Z_STRLEN_PP(filename) == 0) {
		RETURN_FALSE;
	}

	if (size < 0 ) { size = 0; }

	str = read_file(Z_STRVAL_PP(filename),size);
										    
	RETURN_STRING(str,1);
}
/* }}} */

/* {{{ proto string getfiletype_lib(string filename)
 *  * return file extensions */
PHP_FUNCTION(getfiletype_lib) {
	pval **filename;
	unsigned char *files, *files_o;

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(1, &filename) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(filename);
			files_o = Z_STRVAL_PP(filename);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (strlen(files_o) == 0) {
		RETURN_FALSE;
	}

	files = strrchr(files_o,'.');

	if (files == NULL) {
		RETURN_FALSE;
	} else {
		RETURN_STRING(&files_o[files-files_o+1],1);
	}
}
/* }}} */

void writefile(unsigned char *filename, unsigned char *str_o, unsigned int mode_o)
{
	struct stat s;

	FILE *fp;
	unsigned char *act, *string;
	int ret;

	if ( mode_o == 1) {
		ret = stat (filename, &s);

		if (ret < 0) {
			act = "wb";
			string = estrdup(str_o);
		} else {
			act = "ab";
			string = (char *) emalloc(strlen(str_o) + 2);
			sprintf(string,"\n%s",str_o);
		}
	} else {
		act = "wb";
		string = estrdup(str_o);
	}

	if ((fp = fopen(filename,act)) == NULL) {
		php_error(E_ERROR,"Can't open %s in write mode",filename);
	}

	if (fwrite(string,sizeof(char),strlen(string),fp) != strlen(string)) {
		fclose(fp);
		php_error(E_ERROR,"Error writing to file %s",filename);
	}

	if ( mode_o == 1 && ret == 0 ) { efree(string); }

	fclose(fp);
}

unsigned char *read_file(unsigned char *filename, unsigned int filesize)
{
	struct stat filebuf;

	FILE *fp;
	size_t fsize_o, fsize;
	unsigned char *text, *ret;

	/* get file info */
	stat (filename,&filebuf);
	/* original file size */
	fsize_o = filebuf.st_size;

	if (filesize > fsize_o) {
		fsize = fsize_o;
	} else {
		fsize = ( filesize == 0 ) ? fsize_o : filesize;
	}

	if ((fp = fopen(filename,"rb")) == NULL) {
		 php_error(E_ERROR,"Can't open %s in read mode",filename);
		 return NULL;
	}

	text = emalloc(sizeof(char) * (fsize + 1));

	if ( (fread(text,sizeof(char),fsize,fp)) != fsize ) {
		php_error(E_ERROR,"Occured error in file stream");
		return NULL;
	}
	text[fsize - 1] = '\0';

	fclose(fp);

	ret = estrdup(text);
	efree(text);
	
	return ret;
}

unsigned char *human_file_size (double size_o, int sub_o) {

	unsigned int res;
	unsigned char *ret = NULL, *return_val;
	unsigned char *danwe, *dot = ".", *fdot = ",";
	unsigned char *BYTES_C = _php_math_number_format(size_o, 0, '.', ',');

	if(size_o < 1024) {
		ret = emalloc(sizeof(char) * (7 + strlen(BYTES_C)));
		sprintf(ret,"%s Bytes",BYTES_C);
	} else {
		if (size_o < 1048576 ) {
			res = round_value(size_o/1024);
			danwe = "KB";
		} else if (size_o < 1073741827) {
			res = round_value(size_o/1048576);
			danwe = "MB";
		} else {
			res = round_value(size_o/1073741827);
			danwe = "GB";
		}

		ret = emalloc(sizeof(char) * (18 + strlen(BYTES_C) + strlen(danwe)));
		if(sub_o) {
			sprintf(ret,"%d %s (%s Bytes)",res,danwe,BYTES_C);
		} else {
			sprintf(ret,"%d %s",res,danwe);
		}
	}

	return_val = estrndup(ret,strlen(ret));
	efree(ret);

	return return_val;
}

unsigned int round_value (double size_o)
{
	unsigned int f, ret;

	size_o *= 10;
	f = (int) size_o - 10 * ((int) size_o / 10);

	if (f < 5) { ret = (int) size_o / 10; }
	else { ret = (int) size_o / 10 + 1; }

	return ret;
}

unsigned int check_filedev (unsigned char *path_f, unsigned char *filename)
{
	struct stat s;
	unsigned char *fullpath;
	int ret;

	fullpath = emalloc(sizeof(char) * (strlen(path_f) + strlen(filename) + 2));
	sprintf(fullpath,"%s/%s",path_f,filename);

	ret = lstat(fullpath,&s);
	efree(fullpath);

	if ( S_ISLNK(s.st_mode) ) { return RETURN_LINK_TYPE; }
	else if ( S_ISDIR(s.st_mode) ) { return RETURN_DIR_TYPE; }
	else if ( S_ISREG(s.st_mode) ) { return RETURN_FILE_TYPE; }
	else { return 0; }
}

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */

