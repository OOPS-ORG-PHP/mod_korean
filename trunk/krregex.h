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
$Id: krregex.h,v 1.9 2004-09-14 06:52:22 oops Exp $
*/
#ifndef KRREGEX_H
#define KRREGEX_H

/*************************************************
*       Perl-Compatible Regular Expressions      *
*************************************************/

/* Copyright (c) 1997-2000 University of Cambridge */

#ifndef _PCRE_H
#define _PCRE_H

/* The file pcre.h is build by "configure". Do not edit it; instead
make changes to pcre.in. */

#include "php_compat.h"
#define PCRE_MAJOR 4
#define PCRE_MINOR 3
#define PCRE_DATE  21-May-2003

/* Win32 uses DLL by default */

#ifdef _WIN32
#  ifdef PCRE_DEFINITION
#    ifdef DLL_EXPORT
#      define PCRE_DATA_SCOPE __declspec(dllexport)
#    endif
#  else
#    ifndef PCRE_STATIC
#      define PCRE_DATA_SCOPE __declspec(dllimport)
#    endif
#  endif
#endif
#ifndef PCRE_DATA_SCOPE
#  define PCRE_DATA_SCOPE     extern
#endif

/* Have to include stdlib.h in order to ensure that size_t is defined;
it is needed here for malloc. */

#include <stdlib.h>

/* Allow for C++ users */

#ifdef __cplusplus
extern "C" {
#endif

/* Options */

#define PCRE_CASELESS           0x0001
#define PCRE_MULTILINE          0x0002
#define PCRE_DOTALL             0x0004
#define PCRE_EXTENDED           0x0008
#define PCRE_ANCHORED           0x0010
#define PCRE_DOLLAR_ENDONLY     0x0020
#define PCRE_EXTRA              0x0040
#define PCRE_NOTBOL             0x0080
#define PCRE_NOTEOL             0x0100
#define PCRE_UNGREEDY           0x0200
#define PCRE_NOTEMPTY           0x0400
#define PCRE_UTF8               0x0800
#define PCRE_NO_AUTO_CAPTURE    0x1000

/* Exec-time and get/set-time error codes */

#define PCRE_ERROR_NOMATCH        (-1)
#define PCRE_ERROR_NULL           (-2)
#define PCRE_ERROR_BADOPTION      (-3)
#define PCRE_ERROR_BADMAGIC       (-4)
#define PCRE_ERROR_UNKNOWN_NODE   (-5)
#define PCRE_ERROR_NOMEMORY       (-6)
#define PCRE_ERROR_NOSUBSTRING    (-7)
#define PCRE_ERROR_MATCHLIMIT     (-8)
#define PCRE_ERROR_CALLOUT        (-9)  /* Never used by PCRE itself */

/* Request types for pcre_fullinfo() */

#define PCRE_INFO_OPTIONS            0
#define PCRE_INFO_SIZE               1
#define PCRE_INFO_CAPTURECOUNT       2
#define PCRE_INFO_BACKREFMAX         3
#define PCRE_INFO_FIRSTBYTE          4
#define PCRE_INFO_FIRSTCHAR          4  /* For backwards compatibility */
#define PCRE_INFO_FIRSTTABLE         5
#define PCRE_INFO_LASTLITERAL        6
#define PCRE_INFO_NAMEENTRYSIZE      7
#define PCRE_INFO_NAMECOUNT          8
#define PCRE_INFO_NAMETABLE          9
#define PCRE_INFO_STUDYSIZE         10

/* Request types for pcre_config() */

#define PCRE_CONFIG_UTF8                    0
#define PCRE_CONFIG_NEWLINE                 1
#define PCRE_CONFIG_LINK_SIZE               2
#define PCRE_CONFIG_POSIX_MALLOC_THRESHOLD  3
#define PCRE_CONFIG_MATCH_LIMIT             4

/* Bit flags for the pcre_extra structure */

#define PCRE_EXTRA_STUDY_DATA          0x0001
#define PCRE_EXTRA_MATCH_LIMIT         0x0002
#define PCRE_EXTRA_CALLOUT_DATA        0x0004

/* Types */

struct real_pcre;                 /* declaration; the definition is private  */
typedef struct real_pcre pcre;

/* The structure for passing additional data to pcre_exec(). This is defined in
 * such as way as to be extensible. */

typedef struct pcre_extra {
  unsigned long int flags;        /* Bits for which fields are set */
  void *study_data;               /* Opaque data from pcre_study() */
  unsigned long int match_limit;  /* Maximum number of calls to match() */
  void *callout_data;             /* Data passed back in callouts */
} pcre_extra;

/* The structure for passing out data via the pcre_callout_function. We use a
 * structure so that new fields can be added on the end in future versions,
 * without changing the API of the function, thereby allowing old clients to work
 * without modification. */

typedef struct pcre_callout_block {
  int          version;           /* Identifies version of block */
  /* ------------------------ Version 0 ------------------------------- */
  int          callout_number;    /* Number compiled into pattern */
  int         *offset_vector;     /* The offset vector */
  const char  *subject;           /* The subject being matched */
  int          subject_length;    /* The length of the subject */
  int          start_match;       /* Offset to start of this match attempt */
  int          current_position;  /* Where we currently are */
  int          capture_top;       /* Max current capture */
  int          capture_last;      /* Most recently closed capture */
  void        *callout_data;      /* Data passed in with the call */
  /* ------------------------------------------------------------------ */
} pcre_callout_block;

/* Indirection for store get and free functions. These can be set to
 * alternative malloc/free functions if required. There is also an optional
 * callout function that is triggered by the (?) regex item. Some magic is
 * required for Win32 DLL; it is null on other OS. For Virtual Pascal, these
 * have to be different again. */

/* Store get and free functions. These can be set to alternative malloc/free
functions if required. Some magic is required for Win32 DLL; it is null on
other OS. */

#ifndef VPCOMPAT
PCRE_DATA_SCOPE void *(*pcre_malloc)(size_t);
PCRE_DATA_SCOPE void  (*pcre_free)(void *);
PCRE_DATA_SCOPE int   (*pcre_callout)(pcre_callout_block *);
#else   /* VPCOMPAT */
extern void *pcre_malloc(size_t);
extern void  pcre_free(void *);
extern int   pcre_callout(pcre_callout_block *);
#endif  /* VPCOMPAT */

/* Functions */

extern pcre *pcre_compile(const char *, int, const char **,
		int *, const unsigned char *);
extern int  pcre_config(int, void *);
extern int  pcre_copy_named_substring(const pcre *, const char *,
		int *, int, const char *, char *, int);
extern int  pcre_copy_substring(const char *, int *, int, int, char *, int);
extern int  pcre_exec(const pcre *, const pcre_extra *,
		const char *, int, int, int, int *, int);
extern void pcre_free_substring(const char *);
extern void pcre_free_substring_list(const char **);
extern int  pcre_fullinfo(const pcre *, const pcre_extra *, int, void *);
extern int  pcre_get_named_substring(const pcre *, const char *,
		int *, int,  const char *, const char **);
extern int  pcre_get_stringnumber(const pcre *, const char *);
extern int  pcre_get_substring(const char *, int *, int, int,
		const char **);
extern int  pcre_get_substring_list(const char *, int *, int,
		const char ***);
extern int  pcre_info(const pcre *, int *, int *);
extern const unsigned char *pcre_maketables(void);
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
PHPAPI pcre* pcre_get_compiled_regex(char *regex, pcre_extra **extra, int *options);

extern zend_module_entry pcre_module_entry;
#define pcre_module_ptr &pcre_module_entry

typedef struct {
	pcre *re;
	pcre_extra *extra;
	int preg_options;
#if HAVE_SETLOCALE
	char *locale;
	unsigned const char *tables;
#endif
} pcre_cache_entry;

ZEND_BEGIN_MODULE_GLOBALS(pcre)
	HashTable pcre_cache;
ZEND_END_MODULE_GLOBALS(pcre)

#ifdef ZTS
# define PCRE_G(v) TSRMG(pcre_globals_id, zend_pcre_globals *, v)
#else
# define PCRE_G(v)      (pcre_globals.v)
#endif

#define phpext_pcre_ptr pcre_module_ptr

#endif /* PHP_PCRE_H */

unsigned char *kr_regex_replace(unsigned char *regex_o, unsigned char *replace_o, unsigned char *str_o);
unsigned char *kr_regex_replace_arr(unsigned char *regex[], unsigned char *replace[], unsigned char *str, unsigned int regex_no);
unsigned int checkReg(unsigned char *str, unsigned char *regex_o);
int pcre_match (unsigned char *regex, unsigned char *str);

#endif /* KRREGEX_H */
