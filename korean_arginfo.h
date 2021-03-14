/* This is a generated file, edit the .stub.php file instead.
 * Stub hash: f2895e6499eacc01eb5aec1f397d070e82ce463d */

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_buildno_lib, 0, 0, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_version_lib arginfo_buildno_lib

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_movepage_lib, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, timeout, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if PHP_VESRION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_microtime_lib, 0, 2, IS_DOUBLE, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_get_microtime_lib, 0, 2, MAY_BE_DOUBLE|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, old, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, new, IS_LONG, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ncrencode_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_ncrdecode_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 1)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_uniencode_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prefix, IS_STRING, 0, "\"\\\\u\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, postfix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_unidecode_lib, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO(0, charset, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, prefix, IS_STRING, 0, "\"\\\\u\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, postfix, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_utf8encode_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 1)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, charset, IS_STRING, 1, "\"CP949\"")
ZEND_END_ARG_INFO()

#define arginfo_utf8decode_lib arginfo_utf8encode_lib

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_check_uristr_lib, 0, 1, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_email_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, email, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_url_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, url, IS_STRING, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_hangul_lib, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

#define arginfo_check_htmltable_lib arginfo_check_uristr_lib

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_is_iis_lib, 0, 0, IS_LONG, 0)
ZEND_END_ARG_INFO()

#define arginfo_is_windows_lib arginfo_is_iis_lib

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_perror_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, java, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, move, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sec, IS_LONG, 0, "5")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pnotice_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, java, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_human_fsize_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, size, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, sub, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, unit, _IS_BOOL, 0, "false")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, cunit, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_filelist_lib, 0, 1, IS_ARRAY, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_filelist_lib, 0, 1, MAY_BE_ARRAY|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, regex, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_putfile_lib, 0, 2, IS_LONG, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_putfile_lib, 0, 2, MAY_BE_LONG|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, mode, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_getfile_lib, 0, 1, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getfile_lib, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, size, IS_LONG, 0, "0")
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_getfiletype_lib, 0, 1, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_getfiletype_lib, 0, 1, MAY_BE_STRING|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_pcregrep_lib, 0, 2, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_pcregrep_lib, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, regex, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, opt, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#if HAVE_KRLIBGD
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_imgresize_lib, 0, 1, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO(0, path, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, type, IS_STRING, 0, "\"jpg\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, width, IS_LONG, 0, "50")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, height, IS_LONG, 0, "50")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, newpath, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()
#endif

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_mailsource_lib, 0, 5, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, lang, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, to, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, subject, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, context, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, plain_context, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, attach, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_get_hostname_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, reverse, _IS_BOOL, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, ip, IS_STRING, 0, "\"\"")
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_readfile_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, file, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, use_include_path, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_sockmail_lib, 0, 5, IS_ARRAY, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_sockmail_lib, 0, 5, MAY_BE_ARRAY|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, body, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, from, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, to, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, helohost, IS_STRING, 0, "\"\"")
	ZEND_ARG_TYPE_INFO(0, debug, _IS_BOOL, 0)
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_autolink_lib, 0, 1, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_substr_lib, 0, 2, IS_STRING, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_substr_lib, 0, 2, MAY_BE_STRING|MAY_BE_FALSE)
#endif
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, start, IS_LONG, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, length, IS_LONG, 0, "0")
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, utf8, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()

#if PHP_VERSION_ID < 80000
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_agentinfo_lib, 0, 0, IS_ARRAY, 0)
#else
ZEND_BEGIN_ARG_WITH_RETURN_TYPE_MASK_EX(arginfo_agentinfo_lib, 0, 0, MAY_BE_ARRAY|MAY_BE_FALSE)
#endif
ZEND_END_ARG_INFO()

ZEND_BEGIN_ARG_WITH_RETURN_TYPE_INFO_EX(arginfo_postposition_lib, 0, 2, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, str, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO(0, postposition, IS_STRING, 0)
	ZEND_ARG_TYPE_INFO_WITH_DEFAULT_VALUE(0, utf8, _IS_BOOL, 0, "false")
ZEND_END_ARG_INFO()


/** declare each header file
ZEND_FUNCTION(buildno_lib);
ZEND_FUNCTION(version_lib);
ZEND_FUNCTION(movepage_lib);
ZEND_FUNCTION(get_microtime_lib);
ZEND_FUNCTION(ncrencode_lib);
ZEND_FUNCTION(ncrdecode_lib);
ZEND_FUNCTION(uniencode_lib);
ZEND_FUNCTION(unidecode_lib);
ZEND_FUNCTION(utf8encode_lib);
ZEND_FUNCTION(utf8decode_lib);
ZEND_FUNCTION(check_uristr_lib);
ZEND_FUNCTION(is_email_lib);
ZEND_FUNCTION(is_url_lib);
ZEND_FUNCTION(is_hangul_lib);
ZEND_FUNCTION(check_htmltable_lib);
ZEND_FUNCTION(is_iis_lib);
ZEND_FUNCTION(is_windows_lib);
ZEND_FUNCTION(perror_lib);
ZEND_FUNCTION(pnotice_lib);
ZEND_FUNCTION(human_fsize_lib);
ZEND_FUNCTION(filelist_lib);
ZEND_FUNCTION(putfile_lib);
ZEND_FUNCTION(getfile_lib);
ZEND_FUNCTION(getfiletype_lib);
ZEND_FUNCTION(pcregrep_lib);
#if HAVE_KRLIBGD
ZEND_FUNCTION(imgresize_lib);
#endif
ZEND_FUNCTION(mailsource_lib);
ZEND_FUNCTION(get_hostname_lib);
ZEND_FUNCTION(readfile_lib);
ZEND_FUNCTION(sockmail_lib);
ZEND_FUNCTION(autolink_lib);
ZEND_FUNCTION(substr_lib);
ZEND_FUNCTION(agentinfo_lib);
ZEND_FUNCTION(postposition_lib);
*/


static const zend_function_entry korean_functions[] = {
	ZEND_FE(buildno_lib, arginfo_buildno_lib)
	ZEND_FE(version_lib, arginfo_version_lib)
	ZEND_FE(movepage_lib, arginfo_movepage_lib)
	ZEND_FE(get_microtime_lib, arginfo_get_microtime_lib)
	ZEND_FE(ncrencode_lib, arginfo_ncrencode_lib)
	ZEND_FE(ncrdecode_lib, arginfo_ncrdecode_lib)
	ZEND_FE(uniencode_lib, arginfo_uniencode_lib)
	ZEND_FE(unidecode_lib, arginfo_unidecode_lib)
	ZEND_FE(utf8encode_lib, arginfo_utf8encode_lib)
	ZEND_FE(utf8decode_lib, arginfo_utf8decode_lib)
	ZEND_FE(check_uristr_lib, arginfo_check_uristr_lib)
	ZEND_FE(is_email_lib, arginfo_is_email_lib)
	ZEND_FE(is_url_lib, arginfo_is_url_lib)
	ZEND_FE(is_hangul_lib, arginfo_is_hangul_lib)
	ZEND_FE(check_htmltable_lib, arginfo_check_htmltable_lib)
	ZEND_FE(is_iis_lib, arginfo_is_iis_lib)
	ZEND_FE(is_windows_lib, arginfo_is_windows_lib)
	ZEND_FE(perror_lib, arginfo_perror_lib)
	ZEND_FE(pnotice_lib, arginfo_pnotice_lib)
	ZEND_FE(human_fsize_lib, arginfo_human_fsize_lib)
	ZEND_FE(filelist_lib, arginfo_filelist_lib)
	ZEND_FE(putfile_lib, arginfo_putfile_lib)
	ZEND_FE(getfile_lib, arginfo_getfile_lib)
	ZEND_FE(getfiletype_lib, arginfo_getfiletype_lib)
	ZEND_FE(pcregrep_lib, arginfo_pcregrep_lib)
#if HAVE_KRLIBGD
	ZEND_FE(imgresize_lib, arginfo_imgresize_lib)
#endif
	ZEND_FE(mailsource_lib, arginfo_mailsource_lib)
	ZEND_FE(get_hostname_lib, arginfo_get_hostname_lib)
	ZEND_FE(readfile_lib, arginfo_readfile_lib)
	ZEND_FE(sockmail_lib, arginfo_sockmail_lib)
	ZEND_FE(autolink_lib, arginfo_autolink_lib)
	ZEND_FE(substr_lib, arginfo_substr_lib)
	ZEND_FE(agentinfo_lib, arginfo_agentinfo_lib)
	ZEND_FE(postposition_lib, arginfo_postposition_lib)
	ZEND_FE_END
};
