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
 
  $Id$ 
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
#include "php_kr.h"

#include "standard/file.h"

#include <regex.h>

struct stat filestat;

#if ! defined(E_DEPRECATED)
#define E_DEPRECATED E_WARNING
#endif

/* {{{ proto int human_fsize_lib(int filesize, int sub, int unit, int cunit)
 * filesize => init value (byte or bit)
 * sub      => whether print origianl value (ex: 7.5 MB (7,500,000 bytes))
 * unit     => bit or byte (0: byte, 1: bit)
 * cunit    => byte or bit unit (default: 1024)
 * print move action to url */
PHP_FUNCTION(human_fsize_lib)
{
	double   fsize = 0.0;
	int      sub   = 0;
	int      unit  = 0;
	int      cunit = 0;
	char   * ret = NULL;

	if ( kr_parameters("d|bbb", &fsize, &sub, &unit, &cunit) == FAILURE )
		return;

	cunit = cunit ? 1000 : 1024;

	ret = human_file_size (fsize, sub, unit, cunit);
	RETVAL_STRING (ret, 1);
	safe_efree (ret);
}
/* }}} */

/* {{{ proto array filelist_lib (string path, [ string mode, [ string regex ] ])
 * return file list in path */
PHP_FUNCTION(filelist_lib)
{
	struct dirent *d;

	char  * input = NULL,
		  * mode  = "",
		  * regex = "";
	int     inlen = 0,
		    mlen  = 0,
		    rlen  = 0;

	DIR   * dp;
	char    dirpath[MAXPATHLENGTH] = { 0, };
	regex_t preg;

	if ( kr_parameters ("s|ss", &input, &inlen, &mode, &mlen, &regex, &rlen) == FAILURE )
		return;

	if ( inlen == 0 )
		RETURN_FALSE;

	if ( array_init (return_value) == FAILURE )
		RETURN_FALSE;

	if ( VCWD_REALPATH (input, dirpath) == NULL )
		strcpy (dirpath, input);

	PHP_KR_CHECK_OPEN_BASEDIR (dirpath);
	if ( (dp = opendir (dirpath)) == NULL ) {
		php_error (E_ERROR, "Can't open %s directory in read mode", input);
		RETURN_FALSE;
	}

	if ( rlen ) {
		if ( regcomp (&preg, regex, REG_EXTENDED) != 0 ) {
			php_error (E_WARNING, "Problem REGEX compile in PHP");
			RETURN_FALSE;
		}
	}

	while ( (d = readdir (dp)) ) {
		if ( d->d_ino != 0) {
			if ( ! strcmp (d->d_name, ".") || ! strcmp (d->d_name, "..") )
				continue;

			if ( ! strcmp (mode, "f") ) {
				if ( check_filedev (dirpath, d->d_name) != RETURN_FILE_TYPE )
					continue;
			} else if (! strcmp (mode, "d") ) {
				if ( check_filedev(dirpath, d->d_name) != RETURN_DIR_TYPE )
					continue;
			} else if ( ! strcmp (mode, "l") ) {
				if ( check_filedev(dirpath, d->d_name) != RETURN_LINK_TYPE )
					continue;
			} else if ( ! strcmp (mode, "fd") ) {
				if ( check_filedev(dirpath, d->d_name) != RETURN_FILE_TYPE &&
					check_filedev(dirpath, d->d_name) != RETURN_DIR_TYPE )
					continue;
			} else if ( ! strcmp (mode, "fl") ) {
				if ( check_filedev(dirpath, d->d_name) != RETURN_FILE_TYPE &&
					check_filedev(dirpath, d->d_name) != RETURN_LINK_TYPE )
					continue;
			} else if ( ! strcmp (mode, "dl") ) {
				if ( check_filedev(dirpath, d->d_name) != RETURN_LINK_TYPE &&
					check_filedev(dirpath, d->d_name) != RETURN_DIR_TYPE )
					continue;
			}

			if ( rlen && regexec(&preg,d->d_name, 0, NULL, 0) != 0)
				continue;

			add_next_index_string (return_value, d->d_name, 1);
		}
	}

	if ( rlen )
		regfree (&preg);

	closedir(dp);
}
/* }}} */

/* {{{ proto void putfile_lib(string filename, stirng str, [ int mode ])
 * write file */
PHP_FUNCTION(putfile_lib)
{
	char   filepath[MAXPATHLENGTH] = { 0, };
	char * fname = NULL,
		 * input = "";
	int    flen  = 0,
		   inlen = 0,
		   mode  = 0;

	php_error (E_DEPRECATED, "Use file_put_contents function instead of putfile_lib");

	if ( kr_parameters ("ss|b", &fname, &flen, &input, &inlen, &mode) == FAILURE )
		return;

	if ( flen == 0 )
		RETURN_FALSE;

	if ( VCWD_REALPATH (fname, filepath) == NULL )
		strcpy (filepath, fname);

	PHP_KR_CHECK_OPEN_BASEDIR (filepath);
	RETURN_LONG(writefile(filepath, input, mode));
}
/* }}} */

/* {{{ proto string getfile_lib(string filename, [ int readsize ])
 * return file context */
PHP_FUNCTION(getfile_lib)
{
	char *str, getfilename[MAXPATHLENGTH] = { 0, };
	size_t orgsize = 0, chksize = 0;
	struct stat buf;

	char * input = NULL;
	size_t inlen = 0,
		   size  = 0;

	php_error (E_DEPRECATED, "Use file_get_contents function instead of getfile_lib");

	if ( kr_parameters ("s|l", &input, &inlen, &size) == FAILURE )
		return;

	if ( inlen == 0 )
		RETURN_FALSE;

	if ( VCWD_REALPATH (input, getfilename) == NULL )
		strcpy (getfilename, input);

	// get file info
	stat (getfilename, &buf);
	// original file size
	orgsize = buf.st_size;

	if ( size > orgsize )
		chksize = orgsize;
	else
		chksize = ! size ? orgsize : size;

	if ( chksize < 0 )
		RETURN_FALSE;

	PHP_KR_CHECK_OPEN_BASEDIR (getfilename);
	str = readfile (getfilename);

	RETVAL_STRINGL (str, chksize, 1);
	safe_efree (str);
}
/* }}} */

/* {{{ proto string getfiletype_lib(string filename)
 * return file extensions */
PHP_FUNCTION(getfiletype_lib)
{
	char * input = NULL,
		 * ext   = NULL;
	int    inlen = 0;

	if ( kr_parameters ("s", &input, &inlen) == FAILURE )
		return;

	if ( inlen == 0 )
		RETURN_FALSE;

	if ( (ext = strrchr (input, '.')) == NULL )
		RETURN_EMPTY_STRING ();

	RETURN_STRING (ext + 1, 1);
}
/* }}} */

/* {{{ proto string pcregrep_lib(string text) */
PHP_FUNCTION(pcregrep_lib)
{
	const char delimiters[] = "\n";

	char * regex = NULL,
		 * input = NULL;
	int    rlen  = 0,
		   inlen = 0,
		   opt   = 0;

	char * str   = NULL,
		 * bufstr = NULL,
		   buf[4096] = { 0, };

	char ** sep,
		 ** sep_t;

	int    retval = 0,
		   len = 0,
		   buflen = 0,
		   newline;

	if ( kr_parameters ("ss|b", &regex, &rlen, &input, &inlen, &opt) == FAILURE )
		return;

	if ( rlen == 0 || inlen == 0 )
		RETURN_EMPTY_STRING ();

	newline = numberOfchar (input, '\n');
	sep = emalloc ( sizeof (char *) * (newline + 3) );
	sep_t = sep;
	str = emalloc ( sizeof (char) );
	bufstr = estrdup (input);

	while ( (*sep = strsep (&bufstr, delimiters)) != NULL ) {
		if ( **sep != 0 ) {
			memset (buf, 0, 4096);
			memmove (buf, * sep, strlen(*sep));
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

/* {{{ PHPAPI void writefile (char * filename, char * str, int mode) */
int writefile (char * filename, char * str, int mode)
{
	struct stat s;

	FILE * fp;
	char * act,
	     * string;
	int    ret;

	if ( mode == 1 ) {
		ret = stat (filename, &s);
		act = (ret < 0) ? "wb" : "ab";
	} else
		act = "wb";

	string = estrdup (str);

	if ( (fp = fopen (filename, act)) == NULL ) {
		php_error (E_WARNING, "Can't open %s in write mode", filename);
		safe_efree (string);
		return -1;
	}

	if ( fwrite (string, sizeof (char), strlen (string), fp) != strlen (string) ) {
		fclose (fp);
		php_error (E_WARNING, "Error writing to file %s", filename);
		safe_efree (string);
		return -1;
	}

	safe_efree (string);

	fclose (fp);

	return 0;
}
/* }}} */

/* {{{ PHPAPI char * readfile (char * filename) */
char * readfile (char * filename)
{
	struct stat filebuf;

	FILE * fp;
	size_t filesize = 0,
	       len = 0,
	       strlength = 0;
	char * text = NULL,
	       tmptext[FILEBUFS];

	/* get file info */
	stat (filename, &filebuf);
	/* original file size */
	filesize = filebuf.st_size;

	if ( (fp = fopen (filename, "rb")) == NULL ) {
		 php_error (E_ERROR, "Can't open %s in read mode", filename);
		 return NULL;
	}

	text = emalloc (sizeof (char) * (filesize + 32));
	memset (tmptext, '\0', sizeof (tmptext));

	while ( (len = fread (tmptext, sizeof(char), FILEBUFS, fp)) > 0 )
	{
		tmptext[len] = '\0';
		memmove (text + strlength, tmptext, len);
		strlength += len;
	}
	if ( strlength < filesize )
		filesize = strlength;

	text[filesize] = '\0';

	fclose(fp);

	return text;
}
/* }}} */

/* {{{ char *human_file_size (double size_o, int sub_o, int unit, int cunit) */
char * human_file_size (double size_o, int sub_o, int unit, int cunit)
{
	float res;
	char buf[32] = { 0, },
		 sunit[6] = "Bytes",
		 ssunit = 'B',
		 danwe[3] = { 0, };
	char * BYTES_C = (char *) kr_math_number_format (size_o, 0, '.', ',');
	char * ret;

	if ( unit == 1 ) {
		strcpy (sunit, "Bits");
		ssunit = 'b';
	}

	if ( size_o < cunit ) {
		sprintf (buf, "%s %s", BYTES_C, sunit);
	} else {
		if ( size_o < (cunit * cunit) ) {
			res = (float) size_o / cunit;
			memset (danwe, 'K', 1);
		} else if ( size_o < (cunit * cunit * cunit) ) {
			res = (float) size_o / (cunit * cunit);
			memset (danwe, 'M', 1);
		} else if ( size_o < (cunit * cunit * cunit * cunit) ) {
			res = (float) size_o / (cunit * cunit * cunit);
			memset (danwe, 'G', 1);
		} else {
			res = (float) size_o / (cunit * cunit * cunit * cunit);
			memset (danwe, 'T', 1);
		}
		memset (danwe + 1, ssunit, 1);

		if(sub_o)
			sprintf(buf, "%.2f %s (%s %s)", res, danwe, BYTES_C, sunit);
	   	else
			sprintf(buf, "%.2f %s", res, danwe);
	}

	safe_efree(BYTES_C);
	ret = estrdup (buf);

	return ret;
}
/* }}} */

/* {{{ int check_filedev (char *path_f, char *filename) */
int check_filedev (char * path_f, char * filename)
{
	struct stat s;
	char * fullpath;
	int ret;

	fullpath = emalloc (sizeof (char) * (strlen (path_f) + strlen (filename) + 2));
	sprintf (fullpath, "%s/%s", path_f, filename);

	ret = lstat (fullpath, &s);
	safe_efree (fullpath);

	if ( S_ISDIR(s.st_mode) )
		return RETURN_DIR_TYPE;
	else if ( S_ISREG(s.st_mode) )
		return RETURN_FILE_TYPE;
	else if ( S_ISLNK(s.st_mode) )
		return RETURN_LINK_TYPE;

	return 0;
}
/* }}} */

/* {{{ char * includePath (char * filepath) */
char * includePath (char * filepath) {
	const char delimiters[] = ":";
	char * filename = NULL;
	char * token,
	       chkfile[512] = { 0, };
	char * includetmp,
	     * includepath;
	int    exists = 1;
	//static void ***tsrm_ls;

	includetmp = PG(include_path);
	includepath = (includetmp == NULL) ? "" : estrdup (includetmp);

	if ( strchr (includepath, ':') != NULL ) {
		token = strtok (includepath, delimiters);
		while (token != NULL) {
			if ( strcmp (token, ".") && strcmp (token, "./") ) {
				sprintf (chkfile, "%s/%s", token, filepath);

				if( (exists = stat (chkfile, &filestat)) == 0 ) {
					filename = emalloc (sizeof (char) * (strlen (chkfile) + 1));
					memmove (filename, chkfile, strlen (chkfile));
					break;
				}
			}
			token = strtok (NULL, delimiters);
		}
	} else {
		char tmpfilename[512] = { 0, };

		if ( strlen (includepath) > 0 )
			sprintf (tmpfilename, "%s/%s", includepath, filepath);
		else
			sprintf (tmpfilename, "%s", filepath);

		filename = emalloc (sizeof (char) * (strlen (tmpfilename) + 1));
		memmove (filename, tmpfilename, strlen (tmpfilename));
	}

	if ( strlen (filename) == 0 || filename == NULL ) {
		filename = erealloc (filename, sizeof (char) * (strlen (filepath) + 1));
		memmove (filename, filepath, strlen (filepath));
	}

	if ( strlen (includepath) > 0 )
		safe_efree (includepath);

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
