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

$Id: php_krcharset.h,v 1.2 2002-11-27 10:46:39 oops Exp $
*/

#ifndef PHP_KRCHARSET_H
#define PHP_KRCHARSET_H

PHP_FUNCTION(ncrencode_lib);
PHP_FUNCTION(ncrdecode_lib);
PHP_FUNCTION(uniencode_lib);
PHP_FUNCTION(unidecode_lib);
PHP_FUNCTION(utf8encode_lib);
PHP_FUNCTION(utf8decode_lib);
PHP_FUNCTION(getutf8); 
PHP_FUNCTION(getcharacterset);
PHP_FUNCTION(getunicode);

unsigned char *krNcrEncode (unsigned char *str_o, int type);
unsigned char *krNcrDecode (unsigned char *str_o);
unsigned char *uniConv (unsigned char *str_o, int type, int subtype, unsigned char *start, unsigned char *end);
unsigned int getNcrIDX (unsigned char str1, unsigned char str2);
unsigned int getNcrArrayNo (unsigned int key);
unsigned int getUniIDX (unsigned int key);
unsigned int hex2dec (unsigned char *str_o, unsigned int type);
int comp (const void *s1, const void *s2);

#define LKMS_VECTOR_SIZE	16
#define MEMORY_SHORTAGE		-1000
#define ILLEGAL_ARGUMENT	-1001
#define LKMS_TOOLONG		1024
#define LKMS_KANJI			0
#define LKMS_ALNUM			1
#define LKMS_ALPHA			2
#define LKMS_DIGIT			3
#define LKMS_KIGOU			4
#define LKMS_SPACE			5
#define LKMS_NONE			6
#define XU_CONV_LOCALE		-1
#define XU_CONV_NONE		0
#define XU_CONV_ISO8859(n)	(n) /* 1-15 */
#define XU_CONV_UTF8		16
#define XU_CONV_KOI8R		17
#define XU_CONV_EUCJP		20
#define XU_CONV_SJIS		21
#define XU_CONV_CP949		30
#define XU_CONV_EUCCN		40
#define XU_CONV_BIG5		50

#define MAX_VALUE_OF_2_BYTE_UINT	((1 << (8*2)) - 1)
#define UTIL_MALLOC(x)				malloc(x)
#define UTIL_REALLOC(x,y)			realloc(x,y)
#define UTIL_CALLOC(x,y)			calloc(x,y)
#define UTIL_FREE(x)				free(x)
#define ZENKAKU_PUNCTUATIONS_SIZE	(sizeof(zenkakuPunctuations)-1)
#define HANKAKU_LENGTH				1
#define ZENKAKU_LENGTH				2

/* 구조체선언 */
typedef unsigned short XUChar;

/* UTF8변화 */
static void XUInitTable();
extern int XUCodeConv(char* dest, int max, int codeTo, const char* text, int length, int codeFrom);
extern XUChar XUCharEncode(const char* text, int max, int code);
extern int XUCharDecode(char* dest, int max, XUChar ch, int code);
extern int XUCharLen(const char* text, int max, int code);
extern XUChar XUutf8CharEncode(const char* text, int max);
extern int XUutf8CharDecode(char* dest, int max, XUChar ch);
extern int XUutf8CharLen(const char* text, int max);
extern int XUutf8CharRLen(const char* text, int max);
extern int XUStrLen(const XUChar* text);
extern int XUEncode(XUChar* dest, int max, const char* text, int length, int code);
extern int XUDecode(char* dest, int max, const XUChar* text, int length, int code);
extern int XULen(const char* text, int max, int code);
extern int XUutf8Encode(XUChar* dest, int max, const char* text, int length);
extern int XUutf8Decode(char* dest, int max, const XUChar* text, int length);
extern int XUutf8Len(const char* text, int max);

#endif /* PHP_KRCHARSET_H */
