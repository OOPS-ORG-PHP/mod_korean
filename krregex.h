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
#ifndef KRREGEX_H
#define KRREGEX_H

#ifndef PHP_PCRE_H
#define PHP_PCRE_H

#include <regex.h>

#if HAVE_PCRE || HAVE_BUNDLED_PCRE

#if PHP_VERSION_ID < 70300
	#if HAVE_BUNDLED_PCRE
		#ifdef COMPILE_DL_KOREAN
			#include "pcre/pcrelib/pcre.h"
		#else
			#include "pcrelib/pcre.h"
		#endif
	#else
		#include "pcre.h"
	#endif
#else
	#if HAVE_BUNDLED_PCRE
		#ifdef COMPILE_DL_KOREAN
			#include "pcre/pcre2lib/pcre2.h"
		#else
			#include "pcre2lib/pcre2.h"
		#endif
	#else
		#include "pcre2.h"
	#endif
#endif

#if HAVE_LOCALE_H
#include <locale.h>
#endif

#if PHP_VERSION_ID < 70300
typedef struct {
	pcre *re;
	pcre_extra *extra;
	int preg_options;
	int capture_count;
	int name_count;
#if HAVE_SETLOCALE
	unsigned const char *tables;
#endif
	int compile_options;
	int refcount;
} pcre_cache_entry;
#else
struct _pcre_cache_entry {
	pcre2_code *re;
	uint32_t preg_options;
	uint32_t capture_count;
	uint32_t name_count;
	uint32_t compile_options;
	uint32_t extra_compile_options;
	uint32_t refcount;
};
typedef struct _pcre_cache_entry pcre_cache_entry;
#endif

#if PHP_VERSION_ID < 70300

#if PHP_VERSION_ID < 70200
PHPAPI zend_string *php_pcre_replace(zend_string *regex, zend_string *subject_str, char *subject, int subject_len, zval *replace_val, int is_callable_replace, int limit, int *replace_count);
#else
PHPAPI zend_string *php_pcre_replace(zend_string *regex, zend_string *subject_str, char *subject, int subject_len, zend_string *replace_str, int limit, int *replace_count);
#endif
PHPAPI void  php_pcre_match_impl(  pcre_cache_entry *pce, char *subject, int subject_len, zval *return_value,
		zval *subpats, int global, int use_flags, zend_long flags, zend_long start_offset);

#else

PHPAPI zend_string *php_pcre_replace(zend_string *regex, zend_string *subject_str, char *subject, size_t subject_len, zend_string *replace_str, size_t limit, size_t *replace_count);
#if PHP_VERSION_ID < 70400
PHPAPI void  php_pcre_match_impl(  pcre_cache_entry *pce, char *subject, size_t subject_len, zval *return_value,
		zval *subpats, int global, int use_flags, zend_long flags, zend_off_t start_offset);
#else
PHPAPI void  php_pcre_match_impl(pcre_cache_entry *pce, zend_string *subject_str, zval *return_value,
		zval *subpats, int global, int use_flags, zend_long flags, zend_off_t start_offset);
#endif

#endif

extern zend_module_entry pcre_module_entry;
#define pcre_module_ptr &pcre_module_entry

PHPAPI pcre_cache_entry* pcre_get_compiled_regex_cache(zend_string *regex);

ZEND_BEGIN_MODULE_GLOBALS(pcre)
	HashTable pcre_cache;
	zend_long backtrack_limit;
	zend_long recursion_limit;
#ifdef HAVE_PCRE_JIT_SUPPORT
	zend_bool jit;
#endif
	int  error_code;
ZEND_END_MODULE_GLOBALS(pcre)

#endif /* HAVE_PCRE || HAVE_BUNDLED_PCRE */

#define phpext_pcre_ptr pcre_module_ptr

#endif /* PHP_PCRE_H */

/*
 * Defines for mod_korean
 */

char *kr_regex_replace(char *regex_o, char *replace_o, char *str_o);
char *kr_regex_replace_arr(char *regex[], char *replace[], char *str, int regex_no);
int checkReg(char *str, char *regex_o);
int pcre_match (char *regex, char *str);

#endif /* KRREGEX_H */
