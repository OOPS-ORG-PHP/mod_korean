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
$Id: krregex.h,v 1.5 2003-03-01 11:17:59 oops Exp $
*/

/*************************************************
*       Perl-Compatible Regular Expressions      *
*************************************************/

/* Copyright (c) 1997-2000 University of Cambridge */

#ifndef _PCRE_H
#define _PCRE_H

/* The file pcre.h is build by "configure". Do not edit it; instead
make changes to pcre.in. */

#include "php_compat.h"
#define PCRE_MAJOR 3
#define PCRE_MINOR 4
#define PCRE_DATE  22-Aug-2000

/* Win32 uses DLL by default */

#ifdef _WIN32
# ifdef STATIC
#  define PCRE_DL_IMPORT
# else
#  define PCRE_DL_IMPORT __declspec(dllimport)
# endif
#else
# define PCRE_DL_IMPORT
#endif

/* Have to include stdlib.h in order to ensure that size_t is defined;
it is needed here for malloc. */

#include <stdlib.h>

/* Allow for C++ users */

#ifdef __cplusplus
extern "C" {
#endif

/* Options */

#define PCRE_CASELESS        0x0001
#define PCRE_MULTILINE       0x0002
#define PCRE_DOTALL          0x0004
#define PCRE_EXTENDED        0x0008
#define PCRE_ANCHORED        0x0010
#define PCRE_DOLLAR_ENDONLY  0x0020
#define PCRE_EXTRA           0x0040
#define PCRE_NOTBOL          0x0080
#define PCRE_NOTEOL          0x0100
#define PCRE_UNGREEDY        0x0200
#define PCRE_NOTEMPTY        0x0400
#define PCRE_UTF8            0x0800

/* Exec-time and get-time error codes */

#define PCRE_ERROR_NOMATCH        (-1)
#define PCRE_ERROR_NULL           (-2)
#define PCRE_ERROR_BADOPTION      (-3)
#define PCRE_ERROR_BADMAGIC       (-4)
#define PCRE_ERROR_UNKNOWN_NODE   (-5)
#define PCRE_ERROR_NOMEMORY       (-6)
#define PCRE_ERROR_NOSUBSTRING    (-7)

/* Request types for pcre_fullinfo() */

#define PCRE_INFO_OPTIONS         0
#define PCRE_INFO_SIZE            1
#define PCRE_INFO_CAPTURECOUNT    2
#define PCRE_INFO_BACKREFMAX      3
#define PCRE_INFO_FIRSTCHAR       4
#define PCRE_INFO_FIRSTTABLE      5
#define PCRE_INFO_LASTLITERAL     6

/* Types */

typedef void pcre;
typedef void pcre_extra;

/* Store get and free functions. These can be set to alternative malloc/free
functions if required. Some magic is required for Win32 DLL; it is null on
other OS. */

PCRE_DL_IMPORT extern void *(*pcre_malloc)(size_t);
PCRE_DL_IMPORT extern void  (*pcre_free)(void *);

#undef PCRE_DL_IMPORT

/* Functions */

extern pcre *pcre_compile(const char *, int, const char **, int *,
              const unsigned char *);
extern int  pcre_copy_substring(const char *, int *, int, int, char *, int);
extern int  pcre_exec(const pcre *, const pcre_extra *, const char *,
              int, int, int, int *, int);
extern void pcre_free_substring(const char *);
extern void pcre_free_substring_list(const char **);
extern int  pcre_get_substring(const char *, int *, int, int, const char **);
extern int  pcre_get_substring_list(const char *, int *, int, const char ***);
extern int  pcre_info(const pcre *, int *, int *);
extern int  pcre_fullinfo(const pcre *, const pcre_extra *, int, void *);
extern unsigned const char *pcre_maketables(void);
extern pcre_extra *pcre_study(const pcre *, int, const char **);
extern const char *pcre_version(void);

#ifdef __cplusplus
}  /* extern "C" */
#endif

#endif /* End of PCRE_H */

#ifndef PHP_PCRE_H
#define PHP_PCRE_H

#if HAVE_LOCALE_H
#include <locale.h>
#endif

PHPAPI char *php_pcre_replace(char *regex,   int regex_len, char *subject, int subject_len, zval *replace_val, int is_callable_replace, int *result_len, int limit TSRMLS_DC);
PHPAPI pcre *pcre_get_compiled_regex(char *regex, pcre_extra **extra, int *options);

typedef struct {
	pcre *re;
	pcre_extra *extra;
	int preg_options;
#if HAVE_SETLOCALE
	char *locale;
	unsigned const char *tables;
#endif
} pcre_cache_entry;

#endif /* PHP_PCRE_H */

unsigned char *kr_regex_replace(unsigned char *regex_o, unsigned char *replace_o, unsigned char *str_o);
unsigned char *kr_regex_replace_arr(unsigned char *regex[], unsigned char *replace[], unsigned char *str, unsigned int regex_no);
unsigned int checkReg(unsigned char *str, unsigned char *regex_o);
unsigned char * strtrim(unsigned char *str);
