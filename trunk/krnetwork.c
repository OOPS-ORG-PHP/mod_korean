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

  $Id: krnetwork.c,v 1.4 2002-07-02 18:19:44 oops Exp $
*/

/*
 * PHP4 Korean String modue "korean" - only korean
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>

#include "php.h"
#include "php_ini.h"
#include "ext/standard/info.h"
#include "SAPI.h"
#include "fopen_wrappers.h"
#include "ext/standard/dns.c"

#include "php_krnetwork.h"

#define PROXYSIZE 7

/* {{{ proto string get_hostname_lib (int reverse [, string addr ])
 *    Return hostname or ip address */
PHP_FUNCTION(get_hostname_lib)
{
	pval **reverse, **addr;
	unsigned int i;
	char *tmphost = NULL, *host, *check, *ret;
	char *proxytype[PROXYSIZE]  = { "HTTP_VIA", "HTTP_X_COMING_FROM", "HTTP_X_FORWARDED_FOR",
									"HTTP_X_FORWARDED","HTTP_COMING_FROM","HTTP_FORWARDED_FOR",
									"HTTP_FORWARDED" };

	switch(ZEND_NUM_ARGS()) {
		case 1:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &reverse) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if(zend_get_parameters_ex(ZEND_NUM_ARGS(), &reverse, &addr) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_string_ex(addr);
			break;
		default:
			WRONG_PARAM_COUNT;
	}

	convert_to_long_ex(reverse);

	if ( ZEND_NUM_ARGS() == 1 ) {
		for ( i = 0; i < PROXYSIZE; i++ ) {
#ifdef PHP_WIN32
			if (getenv(proxytype[i]) != NULL) {
				tmphost = (char *) emalloc(strlen(getenv(proxytype[i])) + 1);
				tmphost = getenv(proxytype[i]);
#else
			if (sapi_module.getenv(proxytype[i], strlen(proxytype[i]) TSRMLS_CC) != NULL) {
				tmphost = (char *) emalloc(strlen(sapi_module.getenv(proxytype[i], strlen(proxytype[i]) TSRMLS_CC)) + 1);
				tmphost = sapi_module.getenv(proxytype[i], strlen(proxytype[i]) TSRMLS_CC);
#endif
				break;
			}
		}

		if ( tmphost == NULL ) {
#ifdef PHP_WIN32
			host = getenv("REMOTE_ADDR");
#else
			host = sapi_module.getenv("REMOTE_ADDR", 11 TSRMLS_CC);
#endif
		} else {
			host = tmphost;
			efree(tmphost);
		}
	} else {
		if ( Z_STRLEN_PP(addr) != 0 ) {
			host = Z_STRVAL_PP(addr);
		} else {
			php_error(E_WARNING,"address is null value");
			RETURN_FALSE;
		}
	}

	check = Z_LVAL_PP(reverse) ? php_gethostbyaddr(host) : NULL ;
	ret = check ? check : host ;

	RETURN_STRING(ret,1);
}
/* }}} */

/* {{{ proto string readfile_lib(string filename)
 *    Return a file or a URL */
PHP_FUNCTION(readfile_lib)
{
	zval **arg1, **arg2;
	FILE *fp;
	int use_include_path=0;
	int issock=0, socketd=0;
	int rsrc_id, len, i=0;

	char buf[8192], *tmpstr = NULL, *ret;

	/* check args */
	switch (ZEND_NUM_ARGS()) {
		case 1:
			if (zend_get_parameters_ex(1, &arg1) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			break;
		case 2:
			if (zend_get_parameters_ex(2, &arg1, &arg2) == FAILURE) {
				WRONG_PARAM_COUNT;
			}
			convert_to_long_ex(arg2);
			use_include_path = Z_LVAL_PP(arg2);
		default:
			WRONG_PARAM_COUNT;
	}
	convert_to_string_ex(arg1);

	/*
	 * We need a better way of returning error messages from
	 * php_fopen_wrapper().
	 */
	fp = php_fopen_wrapper(Z_STRVAL_PP(arg1), "rb", use_include_path | ENFORCE_SAFE_MODE, &issock, &socketd, NULL TSRMLS_CC);
	if (!fp && !socketd) {
		if (issock != BAD_URL) {
			char *tmp = estrndup(Z_STRVAL_PP(arg1), Z_STRLEN_PP(arg1));
			php_strip_url_passwd(tmp);
			php_error(E_WARNING, "readfile_lib(\"%s\") - %s", tmp, strerror(errno));
			efree(tmp);
		}
		RETURN_FALSE;
	}

	if (issock) {
		int *sock = emalloc(sizeof(int));
		*sock = socketd;
		rsrc_id = ZEND_REGISTER_RESOURCE(NULL, sock, php_file_le_socket());
	} else {
		rsrc_id = ZEND_REGISTER_RESOURCE(NULL, fp, php_file_le_fopen());
	}

	while (1) {
		if ((len = FP_FREAD(buf, 8190, socketd, fp, issock)) < 1) { break; }
		buf[len] = '\0';

		if ( tmpstr == NULL ) {
			tmpstr = emalloc(sizeof(char) * len + 1);
			strcpy(tmpstr,buf);
		} else {
			tmpstr = erealloc(tmpstr,sizeof(char) * (strlen(tmpstr) + len + 1));
			strcat(tmpstr,buf);
		}
	}

	zend_list_delete(rsrc_id);

	if (tmpstr != NULL) {
		ret = estrndup(tmpstr,strlen(tmpstr));
		efree(tmpstr);
		RETURN_STRING(ret,1);
	}
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

