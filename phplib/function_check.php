<?php

function set_regex (&$v) {
	$reg = new stdClass;

	if ( $GLOBALS['KSC']->is_utf8 ($v) ) {
		$reg->pat = KOREAN_LIB::REG_KR_U;
		$reg->opt = 'u';
	} else {
		$reg->pat = KOREAN_LIB::REG_KR;
		$reg->opt = '';
	}

	return $reg;
}

# Check whether path name or variable name has vulnerable meta character
# 
function check_uristr_lib (&$s) {
	$reg = set_regex ($s);
	if ( preg_match ("/[^[:alnum:]{$reg->pat} .%_-]|\.\./{$reg->opt}", trim ($s)) )
		return 1;

	return 0;
}

# Check whether the file name has a vulnerable meta character
# 
function check_filename_lib (&$filename) {
	$reg = set_regex ($s);
	if ( preg_match ("/[^[:alnum:]{$reg->pat}_-]|\.\./{$reg->opt}", trim ($s)) )
		return 1;

	return 0;
}

# Function to check if E-MAIL address is correct
#
function is_email_lib (&$email) {
	$url = trim ($email);

	$reg = set_regex ($url);
	$regex = "/^[{$reg->pat}[:alnum:]._-]+@[{$reg->pat}[:alnum:]_-]+\.[[:alnum:]._-]+$/{$reg->opt}";
	return ( preg_match ($regex, $url) ) ? $url : '';
}

# Function to check whether the URL is correct
#
function is_url_lib (&$url) {
	$url = trim ($url);

	if ( strlen ($url) == 0 )
		return '';

	# "Http://" is used by default when there is no part indicating
	# protocol (http://, ftp:// ...)
	if ( ! preg_match ("/^(http|https|ftp|telnet|news):\/\//i", $url) )
		$url = 'http://' . $url;

	$reg = set_regex ($url);
	$regex =
		'/(http|https|ftp|telnet|news):\/\/' . 
		"[{$reg->pat}[:alnum:]-]+\.[{$reg->pat}[:alnum:],:&#@=_~%?\/.+-]+$/{$reg->opt}";

	return ( ! preg_match ($regex, $url) ) ? '' : $url;
} 

# Check whether a certain character is within the range of Hangul (0xA1A1 - 0xFEFE)
#
function is_hangul_lib (&$char) {
	if ( strlen ($char) == 0 )
		return false;

	$char = ord ($char);
	return ( $char >= 0xa1 && $char <= 0xfe ) ? 1 : false;
}

# Check if table tag is used properly
#
function check_htmltable_lib (&$str) {
	if ( ! preg_match (';</?TABLE[^>]*>;i', $str) )
		return;

	$from = array (
		';[\d]+;',     // [0-9]+ to ''
		';<(/?)(TABLE|TH|TR|TD)[^>]*>;i', // to '<\\1\\2>'
		';<TABLE>;i',  // to 1
		';<TR>;i',     // to 2
		';<TH>;i',     // to 3
		';<TD>;i',     // to 4
		';</TD>;i',    // to 94
		';</TH>;i',    // to 93
		';</TR>;i',    // to 92
		';</TABLE>;i', // to 91
		';[\D]+;'      // [^0-9]+ to ''
	);

	$to = array ('', '<\\1\\2>', 1, 2, 3, 4, 94, 93, 92, 91, '');
	$check = preg_replace ($from, $to, $str);

	if ( strlen ($check) % 3 )
		return 1;

	if ( ! preg_match ('/^12(3|4).+9291$/', $check) )
		return 2;

	while ( preg_match ('/([\d])9\1/', $check) )
		$check = preg_replace ('/([\d])9\1/', '', $check);

	if ( $check )
		return 3;

	return 0;
}

# IIS (isapi) or not function
#
function is_iis_lib () {
	return ( php_sapi_name () == 'isapi' ) ? 1 :0; 
}

# Windows check function
#
function is_windows_lib () {
	return strtoupper (substr (PHP_OS, 0, 3)) === 'WIN' ? 1: 0;
}

# Functions that get the elapsed time by comparing the values obtained by PHP's microtime function
#   
function get_microtime_lib ($old, $new) {
	$start = explode (' ', $old);
	$end = explode (' ', $new);

	return sprintf ("%.2f", ($end[1] + $end[0]) - ($start[1] + $start[0]));
}

# Function to output byte by unit
#
function human_fsize_lib ($bfsize, $sub = "0", $unit = 0, $cunit = false) {
	$lunit = $unit ? 'Bits' : 'Bytes';
	$sunit = $unit ? 'b' : 'B';
	$units = array ('', 'K', 'M', 'G', 'T', 'P', 'E');

	$BYTES = number_format ($bfsize) . ' ' . $lunit; # Use a comma in 3 digits.
	$cuint = ($cuint === false) ? 1024 : 1000;

	$idx = 0;
	while ( $bfsize > $cunit ) {
		$bfsize /= $cunit;
		$idx++;
	}

	$bfsize = sprintf ('%.2f %s%s', $bfsize, $units[$idx], $sunit);
	if ($sub)
		$bfsize .= " ($BYTES)";

	return $bfsize;
}

# Check whether the point is 2byte character
#
function multibyte_check ($str, $p) {
	if ( ! $p )
		return 0;

	$chk = ord ($str[$p]);
	$chk1 = ord ($str[$p - 1]);

	if ( $chk & 0x81 ||
		/* check of 2byte charactor except KSX 1001 range */
		($chk1 >= 0x81 && $chk1 <= 0xa0 && $chk >= 0x41 && $chk <=0xfe) ||
		($chk1 >= 0xa1 && $chk1 <= 0xc6 && $chk >= 0x41 && $chk <=0xa0) ) {

		$tmp = preg_replace ("/[ ].*/", "", substr ($str, $p));
		$l = strlen ($tmp);

		for ( $i=$p; $i < $p + $l; $i++ ) {
			$c1 = ord ($str[$i]);
			$c2 = ord ($str[$i - 1]);
			if ( ord ($str[$i]) & 0x80 ) $twobyte++;
			/* 2th byte of 2 byte charactor is not KSX 1001 range */
			else if (
				($c2 >= 0x81 && $c2 <= 0xa0 && $c1 >= 0x41 && $c1 <=0xfe) ||
				($c2 >= 0xa1 && $c2 <= 0xc6 && $c1 >= 0x41 && $c1 <=0xa0)
			) $twobyte++;
		}

		if ( ($twobyte % 2) != 0 )
			return 1;
	}

	return 0;
}

