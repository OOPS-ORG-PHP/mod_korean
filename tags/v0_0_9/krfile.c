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
 
  $Id: krfile.c,v 1.37 2005-04-20 16:59:24 oops Exp $ 
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>

#ifdef PHP_WIN32
	#include "win32/readdir.h"
#else
	#include <dirent.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "zend_API.h"
#include "php_krfile.h"
#include "php_krparse.h"
#include "php_krmath.h"

struct stat filestat;

/* {{{ proto int human_fsize_lib(int filesize, int pt, int unit, int cunit)
 * filesize => init value (byte or bit)
 * pt       => whether print origianl value (ex: 7.5 MB (7,500,000 bytes))
 * unit     => bit or byte (0: byte, 1: bit)
 * cunit    => byte or bit unit
 * print move action to url */
PHP_FUNCTION(human_fsize_lib)
{
	pval **fsize, **pt, **units, **cunits;
	unsigned int sub = 0, unit = 0, cunit = 1024;
	unsigned char *ret;

	switch(ZEND_NUM_ARGS())
	{
		case 1:
			if(zend_get_parameters_ex(1, &fsize) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &fsize, &pt) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(pt);
			sub = Z_LVAL_PP(pt);
			break;
		case 3:
			if(zend_get_parameters_ex(3, &fsize, &pt, &units) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(pt);
			sub = Z_LVAL_PP(pt);
			convert_to_long_ex(units);
			unit = Z_LVAL_PP(units);
			break;
		case 4:
			if(zend_get_parameters_ex(4, &fsize, &pt, &units, &cunits) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(pt);
			sub = Z_LVAL_PP(pt);
			convert_to_long_ex(units);
			unit = Z_LVAL_PP(units);
			convert_to_long_ex(cunits);
			cunit = Z_LVAL_PP(cunits);
			break;
			break;
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_double_ex(fsize);

	ret = human_file_size(Z_DVAL_PP(fsize), sub, unit, cunit);
	RETURN_STRING(ret, 1);
}
/* }}} */

/* {{{ proto array filelist_lib(string path, [ string mode, [ string regex ] ])
 *  * return file list in path */
PHP_FUNCTION(filelist_lib)
{
	pval **path, **mode, **regex;
	DIR *dp;
	unsigned char *mode_s, *regex_s, dirpath[MAXPATHLENGTH];
	regex_t preg;

	struct dirent *d;

	switch(ZEND_NUM_ARGS())
   	{
		case 1:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &path) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			mode_s = "";
			regex_s = "";
			break;
		case 2:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &path, &mode) == FAILURE)
		   	{
				 WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(path);
			mode_s = Z_STRVAL_PP(mode);
			regex_s = "";
			break;
		case 3:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &path, &mode, &regex) == FAILURE)
		   	{
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

	if ( array_init(return_value) == FAILURE)
   	{
		RETURN_FALSE;
	}

	VCWD_REALPATH(Z_STRVAL_PP(path), dirpath);

	if ( (dp = opendir(dirpath)) == NULL )
   	{
		php_error(E_ERROR, "Can't open %s directory in read mode", Z_STRVAL_PP(path));
	}

	if (strlen(regex_s))
   	{
		if (regcomp(&preg,regex_s, REG_EXTENDED) != 0)
	   	{
			php_error(E_WARNING, "Problem REGEX compile in PHP");
			RETURN_FALSE;
		}
	}

	while ( (d = readdir(dp)) )
	{
		if ( d->d_ino != 0)
		{
			if ( !strcmp(d->d_name, ".") || !strcmp( d->d_name, "..")) { continue; }

			if (!strcmp(mode_s,"f"))
			{
				if (check_filedev(dirpath, d->d_name) != RETURN_FILE_TYPE) { continue; }
			}
			else if (!strcmp(mode_s, "d"))
			{
				if (check_filedev(dirpath, d->d_name) != RETURN_DIR_TYPE) { continue; }
			}
			else if (!strcmp(mode_s, "l"))
		   	{
				if (check_filedev(dirpath, d->d_name) != RETURN_LINK_TYPE) { continue; }
			}
		   	else if (!strcmp(mode_s, "fd"))
		   	{
				if (check_filedev(dirpath, d->d_name) != RETURN_FILE_TYPE &&
					check_filedev(dirpath, d->d_name) != RETURN_DIR_TYPE) { continue; }
			}
		   	else if (!strcmp(mode_s, "fl"))
		   	{
				if (check_filedev(dirpath, d->d_name) != RETURN_FILE_TYPE &&
					check_filedev(dirpath, d->d_name) != RETURN_LINK_TYPE) { continue; }
			}
		   	else if (!strcmp(mode_s, "dl"))
		   	{
				if (check_filedev(dirpath, d->d_name) != RETURN_LINK_TYPE &&
					check_filedev(dirpath, d->d_name) != RETURN_DIR_TYPE) { continue; }
			} 

			if ( strlen(regex_s) && regexec(&preg,d->d_name, 0, NULL, 0) != 0) { continue; }

			add_next_index_string(return_value,d->d_name, 1);
		}
	}

	if (strlen(regex_s)) { regfree(&preg); }
	closedir(dp);
}
/* }}} */

/* {{{ proto void putfile_lib(string filename, stirng str, [ int mode ])
 *  * write file */
PHP_FUNCTION(putfile_lib)
{
	pval **filename, **str, **mode;
	unsigned int write = 0;
	unsigned char filepath[MAXPATHLENGTH];

	switch(ZEND_NUM_ARGS())
	{
		case 2:
			if(zend_get_parameters_ex(2, &filename, &str) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(3, &filename, &str, &mode) == FAILURE)
		   	{
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

	if (Z_STRLEN_PP(filename) == 0)
   	{
		RETURN_FALSE;
	}

	VCWD_REALPATH(Z_STRVAL_PP(filename), filepath);

	RETURN_LONG(writefile(filepath, Z_STRVAL_PP(str), write));
}
/* }}} */

/* {{{ proto string getfile_lib(string filename, [ int readsize ])
 * return file context */
PHP_FUNCTION(getfile_lib)
{
	pval **filename, **getsize;
	unsigned char *str, getfilename[MAXPATHLENGTH];
	size_t size = 0, orgsize = 0, chksize = 0;
	struct stat buf;

	switch(ZEND_NUM_ARGS())
	{
		case 1:
			if(zend_get_parameters_ex(1, &filename) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(2, &filename, &getsize) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(getsize);
			size = Z_LVAL_PP(getsize);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(filename);
	VCWD_REALPATH(Z_STRVAL_PP(filename), getfilename);

	// get file info
	stat (getfilename, &buf);
	// original file size
	orgsize = buf.st_size;

	if ( size > orgsize ) { chksize = orgsize; }
	else { chksize = ( size == 0 ) ? orgsize : size; }

	if ( Z_STRLEN_PP(filename) == 0 || chksize < 0 )
   	{
		RETURN_FALSE;
	}

	str = readfile(getfilename);

	RETVAL_STRINGL(str, chksize, 1);
	safe_efree (str);
}
/* }}} */

/* {{{ proto string getfiletype_lib(string filename)
 *  * return file extensions */
PHP_FUNCTION(getfiletype_lib)
{
	pval **filename;
	unsigned char *files, *files_o;

	switch(ZEND_NUM_ARGS())
	{
		case 1:
			if(zend_get_parameters_ex(1, &filename) == FAILURE)
		   	{
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(filename);
			files_o = Z_STRVAL_PP(filename);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	if (strlen(files_o) == 0)
   	{
		RETURN_FALSE;
	}

	files = strrchr(files_o, '.');

	if (files == NULL)
   	{
		RETURN_FALSE;
	} else {
		RETURN_STRING(&files_o[files-files_o+1], 1);
	}
}
/* }}} */

/* {{{ proto string pcregrep_lib(string text) */
PHP_FUNCTION(pcregrep_lib)
{
	pval **getregex, **gettext, **getopt;
	char *regex, *text, *bufstr, buf[4096];
	char *str;
	int opt = 0, retval = 0, len = 0, buflen = 0, newline;
	const char delimiters[] = "\n";
	char **sep, **sep_t;

	switch(ZEND_NUM_ARGS())
	{
		case 2:
			if(zend_get_parameters_ex(2, &getregex, &gettext) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(3, &getregex, &gettext, &getopt) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(getopt);
			opt = Z_LVAL_PP(getopt);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(getregex);
	regex = Z_STRVAL_PP(getregex);
	convert_to_string_ex(gettext);
	text = Z_STRVAL_PP(gettext);

	if (strlen(text) < 1) {
		RETURN_EMPTY_STRING();
	}

	newline = numberOfchar (text, '\n');
	sep = emalloc ( sizeof (char *) * (newline + 3) );
	sep_t = sep;
	str = emalloc ( sizeof (char) );
	bufstr = estrdup (text);

	while ( (*sep = strsep (&bufstr, delimiters)) != NULL ) {
		if ( **sep != 0 ) {
			memset (buf, 0, 4096);
			memmove (buf, *sep, strlen(*sep));
			buflen = strlen (buf);

			retval = pcre_match (regex, buf);
			if (retval < 0) {
				safe_efree (str);
				RETURN_FALSE;
			}

			// print matched
			if ( opt)
				retval = ! retval ? 1 : 0;

			if (retval) {
				str = erealloc ( str, sizeof (char) * (len + buflen + 3) );
				memcpy ( str + len, buf, buflen);
				len += buflen;
				memset ( str + len, '\n', 1);
				len++;
				memset ( str + len, 0, 1);
			}

			sep++;
		}
	}

	safe_efree (sep_t);

	if (len < 1) {
		safe_efree (str);
		RETURN_EMPTY_STRING();
	}

	RETVAL_STRINGL(str, len - 1, 1);
	safe_efree (str);
}
/* }}} */

/* {{{ void writefile(unsigned char *filename, unsigned char *str_o, unsigned int mode_o) */
int writefile(unsigned char *filename, unsigned char *str_o, unsigned int mode_o)
{
	struct stat s;

	FILE *fp;
	unsigned char *act, *string;
	int ret;

	if ( mode_o == 1)
   	{
		ret = stat (filename, &s);

		if (ret < 0)
	   	{
			act = "wb";
			string = estrdup(str_o);
		}
	   	else
	   	{
			act = "ab";
			string = (char *) emalloc(strlen(str_o) + 32);
			sprintf(string, "\n%s", str_o);
		}
	}
   	else
   	{
		act = "wb";
		string = estrdup(str_o);
	}

	if ( (fp = fopen(filename, act)) == NULL )
   	{
		php_error(E_WARNING, "Can't open %s in write mode", filename);
		safe_efree (string);
		return -1;
	}

	if (fwrite(string, sizeof(char), strlen(string), fp) != strlen(string))
   	{
		fclose(fp);
		php_error(E_WARNING, "Error writing to file %s", filename);
		safe_efree (string);
		return -1;
	}

	safe_efree(string);

	fclose(fp);

	return 0;
}
/* }}} */

/* {{{ unsigned char *readfile(unsigned char *filename) */
unsigned char *readfile(unsigned char *filename)
{
	struct stat filebuf;

	FILE *fp;
	size_t filesize = 0, len = 0, strlength = 0;
	static unsigned char *text, tmptext[FILEBUFS];

	/* get file info */
	stat (filename, &filebuf);
	/* original file size */
	filesize = filebuf.st_size;

	if ((fp = fopen(filename, "rb")) == NULL)
   	{
		 php_error(E_ERROR, "Can't open %s in read mode", filename);
		 return NULL;
	}

	text = emalloc(sizeof(char) * (filesize + 32));
	memset (tmptext, '\0', sizeof(tmptext));

	while ( (len = fread(tmptext, sizeof(char), FILEBUFS, fp)) > 0 )
	{
		tmptext[len] = '\0';
		memmove (text + strlength, tmptext, len);
		strlength += len;
	}
	if (strlength < filesize) { filesize = strlength; }
	text[filesize] = '\0';

	fclose(fp);

	return text;
}
/* }}} */

/* {{{ unsigned char *human_file_size (double size_o, int sub_o, int unit, double cunit) */
unsigned char *human_file_size (double size_o, int sub_o, int unit, double cunit)
{
	float res;
	static unsigned char ret[32];
	unsigned char sunit[6], ssunit, danwe[3];
	unsigned char *BYTES_C = (char *) kr_math_number_format(size_o, 0, '.', ',');

	memset (sunit, '\0', sizeof(sunit));
	memset (danwe, '\0', sizeof(danwe));

	if (unit != 1) {
		strcpy (sunit, "Bytes");
		ssunit = 'B';
	} else {
		strcpy (sunit, "Bits");
		ssunit = 'b';
	}

	if(size_o < cunit)
	{
		sprintf(ret, "%s %s", BYTES_C, sunit);
	}
   	else
   	{
		if (size_o < ( cunit * cunit ))
	   	{
			res = (float) size_o / cunit;
			memset (danwe, 'K', 1);
		}
	   	else if (size_o < ( cunit * cunit * cunit ) )
	   	{
			res = (float) size_o / (cunit * cunit);
			memset (danwe, 'M', 1);
		}
	   	else if (size_o < ( cunit * cunit * cunit * cunit ))
	   	{
			res = (float) size_o / ( cunit * cunit * cunit );
			memset (danwe, 'G', 1);
		}
		else
		{
			res = (float) size_o / ( cunit * cunit * cunit * cunit );
			memset (danwe, 'T', 1);
		}
		memset (danwe + 1, ssunit, 1);

		if(sub_o)
	   	{
			sprintf(ret, "%.2f %s (%s %s)", res, danwe, BYTES_C, sunit);
		}
	   	else
	   	{
			sprintf(ret, "%.2f %s", res, danwe);
		}
	}

	safe_efree(BYTES_C);

	return ret;
}
/* }}} */

/* {{{ unsigned int check_filedev (unsigned char *path_f, unsigned char *filename) */
unsigned int check_filedev (unsigned char *path_f, unsigned char *filename)
{
	struct stat s;
	unsigned char *fullpath;
	int ret;

	fullpath = emalloc(sizeof(char) * (strlen(path_f) + strlen(filename) + 2));
	sprintf(fullpath, "%s/%s", path_f, filename);

	ret = lstat(fullpath, &s);
	safe_efree(fullpath);

	if ( S_ISDIR(s.st_mode) ) { return RETURN_DIR_TYPE; }
	else if ( S_ISREG(s.st_mode) ) { return RETURN_FILE_TYPE; }
	else if ( S_ISLNK(s.st_mode) ) { return RETURN_LINK_TYPE; }
	else { return 0; }
}
/* }}} */

/* {{{ unsigned char *includePath (unsigned char *filepath) */
unsigned char *includePath (unsigned char *filepath)
{
	const char delimiters[] = ":";
	static unsigned char filename[1024];
	unsigned char *token, chkfile[512];
	unsigned char *includetmp, *includepath;
	int exists = 1;
	//static void ***tsrm_ls;

	includetmp = PG(include_path);
	includepath = (includetmp == NULL) ? "" : estrdup(includetmp);

	if ( strchr(includepath, ':') != NULL)
	{
		token = strtok(includepath, delimiters);
		while (token != NULL)
		{
			if(strcmp(token, ".") && strcmp(token, "./"))
			{
				sprintf(chkfile, "%s/%s", token, filepath);

				if((exists = stat (chkfile, &filestat)) == 0)
				{
					memmove (filename, chkfile, strlen(chkfile));
					break;
				}
			}
			token = strtok (NULL, delimiters);
		}
	}
	else
	{
		unsigned char tmpfilename[512];

		if(strlen(includepath) > 0)
		{
			sprintf(tmpfilename, "%s/%s", includepath, filepath);
		}
		else
		{
			sprintf(tmpfilename, "%s", filepath);
		}

		memmove (filename, tmpfilename, strlen(tmpfilename));
	}

	if (strlen(filename) == 0 || filename == NULL)
	{
		memmove (filename, filepath, strlen(filepath));
	}

	if ( strlen(includepath) > 0 ) { safe_efree(includepath); }

	return filename;
}
/* }}} */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
