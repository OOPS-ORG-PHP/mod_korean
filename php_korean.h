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
  | Author:                                                              |
  +----------------------------------------------------------------------+

  $Id: php_korean.h,v 1.42 2003-01-07 11:50:19 oops Exp $
*/

#ifndef PHP_KOREAN_H
#define PHP_KOREAN_H

extern zend_module_entry korean_module_entry;
#define phpext_korean_ptr &korean_module_entry

#ifdef PHP_WIN32
#define PHP_KOREAN_API __declspec(dllexport)
#else
#define PHP_KOREAN_API
#endif

#ifdef ZTS
#include "TSRM.h"
#endif

PHP_MINIT_FUNCTION(korean);
PHP_MSHUTDOWN_FUNCTION(korean);
PHP_RINIT_FUNCTION(korean);
PHP_RSHUTDOWN_FUNCTION(korean);
PHP_MINFO_FUNCTION(korean);

PHP_FUNCTION(buildno_lib);
PHP_FUNCTION(version_lib);
PHP_FUNCTION(movepage_lib);
PHP_FUNCTION(get_microtime_lib);

/* 
  	Declare any global variables you may need between the BEGIN
	and END macros here:     

ZEND_BEGIN_MODULE_GLOBALS(korean)
	int   global_value;
	char *global_string;
ZEND_END_MODULE_GLOBALS(korean)
*/

/* In every utility function you add that needs to use variables 
   in php_korean_globals, call TSRM_FETCH(); after declaring other 
   variables used by that function, or better yet, pass in TSRMG_CC
   after the last function argument and declare your utility function
   with TSRMG_DC after the last declared argument.  Always refer to
   the globals in your function as KOREAN_G(variable).  You are 
   encouraged to rename these macros something shorter, see
   examples in any other php module directory.
*/

#ifdef ZTS
#define KOREAN_G(v) TSRMG(korean_globals_id, zend_korean_globals *, v)
#else
#define KOREAN_G(v) (korean_globals.v)
#endif

#define BUILDNO "200301072048"
#define BUILDVER "0.0.5devel"

#endif	/* PHP_KOREAN_H */

/*
 * Local variables:
 * tab-width: 4
 * c-basic-offset: 4
 * End:
 * vim600: noet sw=4 ts=4 fdm=marker
 * vim<600: noet sw=4 ts=4
 */
