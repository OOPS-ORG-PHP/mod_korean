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
 
  $Id: krimage.c,v 1.1 2002-06-03 13:36:53 oops Exp $ 

  gd 1.2 is copyright 1994, 1995, Quest Protein Database Center,
  Cold Spring Harbor Labs.

  Note that there is no code from the gd package in this file
*/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#if HAVE_FCNTL_H
# include <fcntl.h>
#endif

#include "php.h"
#include "php_ini.h"
#include "SAPI.h"
#include "fopen_wrappers.h"
#include "ext/standard/head.h"
#include "ext/standard/info.h"

#if HAVE_SYS_WAIT_H
# include <sys/wait.h>
#endif

#if HAVE_UNISTD_H
# include <unistd.h>
#endif

#ifdef PHP_WIN32
# include <io.h>
# include <fcntl.h>
#endif

#include "php_krimage.h"

#if HAVE_LIBGD

#include <gd.h>

/* {{{ proto int imgresize(string filepath [, string new_type [, int new_width [, int new_height [, string new_path ] ] ] ])
 *  *  print move action to url */
PHP_FUNCTION(imgresize_lib) {
	pval **opath, **ntype, **nwid, **nhei, **npath;
	gdImagePtr im, nim;
	FILE *fp, *tmp;
	int issock=0, socketd=0, rsrc_id, itype = 0;
	char filetype[8];

	unsigned char *original, *new_path;
	int new_type = 0, new_width = 0, new_height = 0, old_width = 0, old_height = 0;

	switch(ZEND_NUM_ARGS()) {
		case 5:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &opath, &ntype, &nwid, &nhei, &npath) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 4:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &opath, &ntype, &nwid, &nhei) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 3:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &opath, &ntype, &nwid) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &opath, &ntype) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 1:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &opath) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_string_ex(opath);
	original = Z_STRVAL_PP(opath);

	if (ZEND_NUM_ARGS() > 1) {
		convert_to_string_ex(ntype);

		if (!strcasecmp(Z_STRVAL_PP(ntype),"gif")) {
			new_type = GIFNEWTYPE;
		} else if (!strcasecmp(Z_STRVAL_PP(ntype),"png")) {
			new_type = PNGNEWTYPE;
		} else {
			new_type = JPGNEWTYPE;
		}
	}

	if (ZEND_NUM_ARGS() > 2) {
		convert_to_long_ex(nwid);
		new_width = Z_LVAL_PP(nwid);
	}

	if (ZEND_NUM_ARGS() > 3) {
		convert_to_long_ex(nhei);
		new_height = Z_LVAL_PP(nhei);
	}

	if (ZEND_NUM_ARGS() > 4) {
		convert_to_string_ex(npath);
		new_path = Z_STRVAL_PP(npath);
		if (strlen(new_path) < 1) { new_path = NULL; }
	} else {
		new_path = NULL;
	}

	/* get origianl image type */
#ifdef PHP_WIN32
	fp = VCWD_FOPEN(original, "rb");
#else
	fp = php_fopen_wrapper(original, "rb", IGNORE_PATH|IGNORE_URL_WIN, &issock, &socketd, NULL TSRMLS_CC);
#endif

	if (!fp && !socketd) {
		if (issock != BAD_URL) {
			php_strip_url_passwd(original);
			php_error(E_WARNING, "ImgResize_lib: Unable to open '%s' for reading.", original);
		}
		RETURN_FALSE;
	}

    if (issock) {
		int *sock=emalloc(sizeof(int));
		*sock = socketd;
	}

	if((FP_FREAD(filetype, 3, socketd, fp, issock)) <= 0) {
		php_error(E_WARNING, "getimagesize: Read error!");
		RETURN_FALSE;
	}

    if (!memcmp(filetype, php_sig_gif, 3)) {
		itype = 1;
	} else if (!memcmp(filetype, php_sig_jpg, 3)) {
		itype = 2;
	} else if (!memcmp(filetype, php_sig_png, 3)) {
		FP_FREAD(filetype+3, 5, socketd, fp, issock);
		if (!memcmp(filetype, php_sig_png, 8)) {
			itype = 3;
		} else php_error(E_WARNING, "PNG file corrupted by ASCII conversion");
	} else {
		php_error(E_ERROR, "Enable original file is type of GIF,JPG,PNG");
	}

	/* move point to start in stream */
	fseek(fp,0,SEEK_SET);

	switch (itype) {
		case 1: /* if gif */
#if HAVE_GD_GIF_READ
			im = gdImageCreateFromGif(fp);
#else
			fclose(fp);
			php_error(E_ERROR,"NO support GIF format in gd library");
#endif
			break;
		case 2: /* if jpeg */
#ifdef HAVE_GD_JPG
			im = gdImageCreateFromJpeg(fp);
#else /* HAVE_GD_JPG */
			fclose(fp);
			php_error(E_ERROR, "No JPEG support in this PHP build");
#endif /* HAVE_GD_JPG */
			break;
		case 3: /* if png */
#ifdef HAVE_GD_PNG
			im = gdImageCreateFromPng(fp);
#else /* HAVE_GD_PNG */
			fclose(fp);
			php_error(E_ERROR, "No PNG support in this PHP build");
#endif /* HAVE_GD_PNG */
			break;
	}

	fflush(fp);
	fclose(fp);

	/* get image size */
	old_width = gdImageSX(im);
	old_height = gdImageSY(im);

	/* default value of new_width and new_height is 50 */
	if ( new_width < 1 && new_height < 1 ) {
		new_width = new_height = 50;
	}

	if ( new_height < 1 ) {
		new_height = old_height * new_width / old_width;
	}

	/* create new image */
	nim = gdImageCreate(new_width, new_height);

	/* copy original point to new point to resize */
	gdImageCopyResized(nim, im , 0, 0, 0, 0, new_width, new_height, old_width, old_height);

	if (new_path != NULL) {
		tmp = VCWD_FOPEN(new_path, "wb");
	} else {
		tmp = tmpfile();
	}

	if (tmp == NULL) {
		php_error(E_WARNING, "%s: unable to open temporary file", get_active_function_name(TSRMLS_C));
		RETURN_FALSE;
	}

	if (new_type == JPGNEWTYPE) {
#ifdef HAVE_GD_JPG
		gdImageJpeg(nim,tmp,80);
#else /* HAVE_GD_JPG */
		gdImageDestroy(im);
		php_error(E_ERROR, "No JPEG support in this PHP build");
#endif /* HAVE_GD_JPG */
	} else if (new_type == PNGNEWTYPE) {
#ifdef HAVE_GD_PNG
		gdImagePng(nim,tmp);
#else /* HAVE_GD_PNG */
		gdImageDestroy(im);
		php_error(E_ERROR, "No PNG support in this PHP build");
#endif /* HAVE_GD_PNG */
	} else if (new_type == GIFNEWTYPE) {
#ifdef HAVE_GD_GIF_CREATE
		gdImageGif(nim,tmp);
#else /* HAVE_GD_GIF_CREATE */
		gdImageDestroy(im);
		php_error(E_ERROR,"NO support GIF format in gd library");
#endif /* HAVE_GD_GIF_CREATE */
	} else {
		gdImageDestroy(im);
		php_error(E_ERROR, "Supported new image only gif, png, jpg format");
	}

	gdImageDestroy(im);

	if (new_path == NULL) {
		int   b;
		char  buf[4096];

		fseek(tmp, 0, SEEK_SET);

#if APACHE && defined(CHARSET_EBCDIC)
		/* XXX this is unlikely to work any more thies@thieso.net */
		/* This is a binary file already: avoid EBCDIC->ASCII conversion */
		ap_bsetflag(php3_rqst->connection->client, B_EBCDIC2ASCII, 0);
#endif
	
		while ((b = fread(buf, 1, sizeof(buf), tmp)) > 0) {
			php_write(buf, b TSRMLS_CC);
		}
		/* the temporary file is automatically deleted */
	} else {
		fflush(tmp);
	}
	fclose(tmp);

	RETURN_TRUE;
}
/* }}} */

#endif  /* HAVE_LIBGD */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
