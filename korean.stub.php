<?php
/** @generate-function-entries **/

function buildno_lib(): string { }
function version_lib(): string { }
function movepage_lib(string $url, int $timeout = 0): bool { }
function get_microtime_lib(int $old, int $new): float|false { }

/* in krcahrset.c */
function ncrencode_lib(?string $str, bool $type = false): string { }
function ncrdecode_lib(?string $str): string { }
function uniencode_lib(?string $str, string $prefix = "\\u", string $postfix = ""): string { }
function unidecode_lib(?string $str, string $charset, string $prefix = "\\u", string $postfix = ""): string { }
function utf8encode_lib(?string $str, ?string $charset = "CP949"): string { }
function utf8decode_lib(?string $str, ?string $charset = "CP949"): string { }

/* in krcheck.c */
function check_uristr_lib(string $str): int { }
function is_email_lib(string $email): string { }
function is_url_lib(string $url): string { }
function is_hangul_lib(string $str): bool { }
function check_htmltable_lib(string $str): int { }
function is_iis_lib(): int { }
function is_windows_lib(): int { }

/* in krerror.c */
function perror_lib(string $str, bool $java = false, string $move = "", int $sec = 5): string { }
function pnotice_lib(string $str, bool $java = false): string { }

/* in krfile.c */
function human_fsize_lib(int $size, bool $sub = false, bool $unit = false, bool $cunit = false): string { }
function filelist_lib(string $path, string $mode = "", string $regex = ""): array|false { }
function putfile_lib(string $file, string $str, bool $mode = false): int|false { }
function getfile_lib(string $file, int $size = 0): string|false { }
function getfiletype_lib(string $file): string|false { }
function pcregrep_lib(string $regex, string $str, bool $opt = false): string|false { }

/* in krimage.c */
#if HAVE_KRLIBGD
function imgresize_lib(string $path, string $type = "jpg", int $width = 50, int $height = 50, string $newpath = ""): bool { }
#endif

/* in krmail.c */
function mailsource_lib(string $lang, string $from, string $to, string $subject, string $context, string $plain_context = "", string $attach = ""): string { }

/* in krnetwork.c */
function get_hostname_lib(bool $reverse, string $ip = ""): string { }
function readfile_lib(string $file, bool $use_include_path = false): string { }
function sockmail_lib(string $body, string $from, string $to = "", string $helohost = "", bool $debug): array|false { }

/* in krparse.c */
function autolink_lib(string $str): string { }
function substr_lib(string $str, int $start, int $length = 0, bool $utf8 = false): string|false { }
function agentinfo_lib(): array|false { }
function postposition_lib(string $str, string $postposition, bool $utf8 = false): string { }
