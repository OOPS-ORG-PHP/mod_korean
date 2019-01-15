/*
  +----------------------------------------------------------------------+
  | PHP Version 7                                                        |
  +----------------------------------------------------------------------+
  | Copyright (c) 1997-2019 The PHP Group                                |
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
 
  gd 1.2 is copyright 1994, 1995, Quest Protein Database Center,
  Cold Spring Harbor Labs.

  Note that there is no code from the gd package in this file
*/

#ifdef HAVE_CONFIG_H
	#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <time.h>
#include <math.h>
#if HAVE_FCNTL_H
	#include <fcntl.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"

#if HAVE_SYS_WAIT_H
	#include <sys/wait.h>
#endif

#if HAVE_UNISTD_H
	#include <unistd.h>
#endif

#ifdef PHP_WIN32
	#include <io.h>
	#include <fcntl.h>
#endif

#if HAVE_KRLIBGD

#include "php_krparse.h"
#include "php_krimage.h"
#ifdef HAVE_GD_BUNDLED
#include "libgd/gd.h"
#else
#include <gd.h>
#endif
#include "php_kr.h"

/* file type markers */
#ifdef PHP_WIN32
const char php_sig_gif_kr[3] = {'G', 'I', 'F'};
const char php_sig_jpg_kr[3] = {(char) 0xff, (char) 0xd8, (char) 0xff};
const char php_sig_png_kr[8] = {(char) 0x89, (char) 0x50, (char) 0x4e, (char) 0x47,
	    (char) 0x0d, (char) 0x0a, (char) 0x1a, (char) 0x0a};
#else
PHPAPI const char php_sig_gif_kr[3] = {'G', 'I', 'F'};
PHPAPI const char php_sig_jpg_kr[3] = {(char) 0xff, (char) 0xd8, (char) 0xff};
PHPAPI const char php_sig_png_kr[8] = {(char) 0x89, (char) 0x50, (char) 0x4e, (char) 0x47,
	    (char) 0x0d, (char) 0x0a, (char) 0x1a, (char) 0x0a};
#endif

/* {{{ proto int imgresize(string filepath [, string new_type [, int width [, int height [, string newpath ] ] ] ])
 *  *  print move action to url */
PHP_FUNCTION(imgresize_lib)
{
	zend_string * Zpath   = NULL,
	            * Ztype   = NULL,
	            * Znewp   = NULL;
	int    width  = 50,
	       height = 50;

	gdImagePtr im = NULL,
	           nim = NULL;

	FILE * fp = NULL,
	     * tmp = NULL;
	int    new_type = 0,
	       old_width = 0,
	       old_height = 0,
	       issock = 0,
	       itype = 0,
	       newpath_len = 0;
	char   filetype[8] = { 0, },
	       tmpfilename[64] = { 0, };
	char   imgfile[1024] = { 0, },
	       newpath[1024] = { 0, };

	if ( kr_parameters ("S|SllS", &Zpath, &Ztype, &width, &height, &Znewp) == FAILURE )
		return;

	if ( ZSTR_LEN (Zpath) == 0 )
		RETURN_FALSE;

	if ( ! Ztype || ! ZSTR_LEN (Ztype) ) {
#ifdef HAVE_GD_JPG
		new_type = JPGNEWTYPE;
#elif HAVE_GD_GIF_CREATE
		new_type = GIFNEWTYPE;
#else
		new_type = PNGNEWTYPE;
#endif
	} else {
		if ( ! strcasecmp (ZSTR_VAL(Ztype), "gif") )
			new_type = GIFNEWTYPE;
		else if ( ! strcasecmp (ZSTR_VAL(Ztype), "png") )
			new_type = PNGNEWTYPE;
		else
			new_type = JPGNEWTYPE;
	}

	if ( ! width )
		width = 50;

	if ( ! height )
		height = 50;

	if ( Znewp && ZSTR_LEN (Znewp) > 0 ) {
		if ( VCWD_REALPATH (ZSTR_VAL (Znewp), newpath) == NULL )
			strcpy (newpath, ZSTR_VAL (Znewp));
		newpath_len = strlen (newpath);

		PHP_KR_CHECK_OPEN_BASEDIR (newpath);
	} else
		newpath_len = 0;

	/* if image is url */
	if ( checkReg (ZSTR_VAL(Zpath), "^[hH][tT][tT][pP]://") )
		issock = 1;

	if (issock == 1) {
		time_t   now = time (0);
		size_t   len = 0,
			   * retSize,
			     retSize_t = 0;

		retSize = &retSize_t;

		/* get random temp file name */
		srand (now);
#ifdef PHP_WIN32
		sprintf (tmpfilename, "c:\\tmpResize-%d", rand());
#else
		sprintf (tmpfilename, "%s/tmpResize-%d",
							 PG(upload_tmp_dir) ? PG(upload_tmp_dir) : "tmp", rand ());
#endif
		len = strlen (tmpfilename);
		tmpfilename[len] = '\0';

		sockhttp (ZSTR_VAL (Zpath), retSize, 1, tmpfilename);
		memset (imgfile, '\0', sizeof (imgfile));
		memcpy (imgfile, tmpfilename, strlen (tmpfilename));
	} else
		if ( VCWD_REALPATH (ZSTR_VAL (Zpath), imgfile) == NULL )
			strcpy (imgfile, ZSTR_VAL (Zpath));

	/* get origianl image type */
	PHP_KR_CHECK_OPEN_BASEDIR (imgfile);

	if ( (fp = fopen (imgfile, "rb")) == NULL ) {
		php_error (E_ERROR, "Can't open %s in read mode", ZSTR_VAL (Zpath));
		RETURN_FALSE;
	}

	if ( fread (filetype, sizeof (char), 3, fp) <= 0 ) {
		php_error (E_WARNING, "getimagesize: Read error!");
		RETURN_FALSE;
	}

    if ( ! memcmp (filetype, php_sig_gif_kr, 3) )
		itype = 1;
   	else if ( ! memcmp (filetype, php_sig_jpg_kr, 3) )
		itype = 2;
   	else if ( ! memcmp (filetype, php_sig_png_kr, 3) ) {
		fread (filetype + 3, sizeof (char), 5, fp);

		if ( ! memcmp (filetype, php_sig_png_kr, 8) )
			itype = 3;
		else 
			php_error(E_WARNING, "PNG file corrupted by ASCII conversion");
	} else
		php_error (E_ERROR, "Enable original file is type of GIF,JPG,PNG");

	/* move point to start in stream */
	fseek (fp, 0, SEEK_SET);

	switch (itype)
   	{
		case 1: // if gif
#if HAVE_GD_GIF_READ
			im = (gdImagePtr) gdImageCreateFromGif (fp);
#else
			fclose (fp);
			php_error (E_ERROR, "NO support GIF format in gd library");
#endif
			break;
		case 2: /* if jpeg */
#ifdef HAVE_GD_JPG
			im = gdImageCreateFromJpeg (fp);
#else /* HAVE_GD_JPG */
			fclose (fp);
			php_error (E_ERROR, "No JPEG support in this PHP build");
#endif /* HAVE_GD_JPG */
			break;
		case 3: /* if png */
#ifdef HAVE_GD_PNG
			im = gdImageCreateFromPng (fp);
#else /* HAVE_GD_PNG */
			fclose (fp);
			php_error (E_ERROR, "No PNG support in this PHP build");
#endif
			break;
	}

	fflush (fp);
	fclose (fp);

	/* get image size */
	old_width = gdImageSX (im);
	old_height = gdImageSY (im);

	if ( width < 1 )
		width = old_width * height / old_height;
	else if ( height < 1 )
		height = old_height * width / old_width;

	/* create new image */
#ifdef HAVE_GD2
		nim = (gdImagePtr) gdImageCreateTrueColor (width, height);
#else
		nim = (gdImagePtr) gdImageCreate (width, height);
#endif

	/* copy original point to new point to resize */
#ifdef HAVE_GD2
		gdImageCopyResampled (nim, im , 0, 0, 0, 0, width, height, old_width, old_height);
#else
		gdImageCopyResized (nim, im , 0, 0, 0, 0, width, height, old_width, old_height);
#endif

	if ( newpath_len > 0 )
		tmp = VCWD_FOPEN (newpath, "wb");
   	else
		tmp = tmpfile();

	if ( tmp == NULL ) {
		php_error (
			E_WARNING, "%s: unable to open temporary file",
			get_active_function_name (TSRMLS_C)
		);
		RETURN_FALSE;
	}

	gdImageDestroy (im);

	switch (new_type) {
		case JPGNEWTYPE :
#ifdef HAVE_GD_JPG
			gdImageJpeg (nim, tmp, 80);
#else /* HAVE_GD_JPG */
			php_error (E_ERROR, "No JPEG support in this PHP build");
#endif /* HAVE_GD_JPG */
			break;
		case PNGNEWTYPE :
#ifdef HAVE_GD_PNG
			gdImagePng (nim, tmp);
#else /* HAVE_GD_PNG */
			php_error (E_ERROR, "No PNG support in this PHP build");
#endif
			break;
		case GIFNEWTYPE :
#ifdef HAVE_GD_GIF_CREATE
			gdImageGif (nim, tmp);
#else // HAVE_GD_GIF_CREATE
			php_error (E_ERROR, "NO support GIF format in gd library");
#endif // HAVE_GD_GIF_CREATE
			break;
		default :
			//php_error (E_ERROR, "Supported new image only gif, png, jpg format");
			php_error (E_ERROR, "Supported new image only png, jpg format");
	}

	if (newpath_len == 0) {
		struct stat sbuf;
		int   fd, b,
			  len = 0;
		char  buf[4096] = { 0, },
			  sizeHeader[30] = { 0, };

		/* get file size */
		fseek (tmp, 0, SEEK_SET);
		fd = fileno (tmp);
		fstat (fd, &sbuf);
		len = sbuf.st_size;

		sprintf (sizeHeader, "Content-Length: %d", len);
		len = strlen (sizeHeader);
		sizeHeader[len] = '\0';

		/* print image header */
		switch (new_type) {
			case PNGNEWTYPE :
				sapi_add_header_ex ("Content-type: image/png", 23, 1, 1 TSRMLS_CC);
				sapi_add_header_ex ("Content-Disposition: inline; filename=resize_img.png", 52, 1, 1 TSRMLS_CC);
				break;
			case GIFNEWTYPE :
				sapi_add_header_ex ("Content-type: image/gif", 23, 1, 1 TSRMLS_CC);
				sapi_add_header_ex ("Content-Disposition: inline; filename=resize_img.gif", 52, 1, 1 TSRMLS_CC);
				break;
			default :
				sapi_add_header_ex ("Content-type: image/jpeg", 24, 1, 1 TSRMLS_CC);
				sapi_add_header_ex ("Content-Disposition: inline; filename=resize_img.jpg", 52, 1, 1 TSRMLS_CC);
		}
		sapi_add_header_ex (sizeHeader, strlen (sizeHeader), 1, 1 TSRMLS_CC);

#if APACHE && defined(CHARSET_EBCDIC)
		/* XXX this is unlikely to work any more thies@thieso.net */
		/* This is a binary file already: avoid EBCDIC->ASCII conversion */
		ap_bsetflag (php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
	
		while ( (b = fread (buf, 1, sizeof (buf), tmp)) > 0 )
			php_write (buf, b TSRMLS_CC);

		/* the temporary file is automatically deleted */
	} else
		fflush(tmp);

	fclose (tmp);
	gdImageDestroy (nim);

	if ( issock == 1 )
		unlink(tmpfilename);

	RETURN_TRUE;
}
/* }}} */

#endif  /* HAVE_KRLIBGD */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
