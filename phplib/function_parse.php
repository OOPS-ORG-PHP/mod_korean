<?php

// {{{ +-- public substr_lib ($str, $from [, $length = 0, $utf8 = 0])
function substr_lib ($str, $from, $length = 0, $utf8 = 0) {
	// The $utf variable is set automatically. Values passed in function arguments are ignored.
	return $GLOBALS['KSC']->substr ($str, $from, $length);
}
// }}}

/*
 * unicode 한글의 처음은 44032(ac00) 이다.
 * 이 함수는 44032 를 index 0 으로 만드는 역할을 한다.
 */
// {{{ +-- public mchar2dec ($v)
function mchar2dec ($v) {
	$s = iconv ('UTF-8', 'UCS-2BE', $v);
	$c1 = dechex (ord ($s[0]));
	$c2 = dechex (ord ($s[1]));

	if ( strlen ($c1) < 2 ) $c1 = '0' . $c1;
	if ( strlen ($c2) < 2 ) $c2 = '0' . $c2;

	return base_convert ($c1 . $c2, 16, 10) - 44032;
}
// }}}

// {{{ +-- public postposition_lib ($str, $set, $utf = 0)
function postposition_lib ($str, $set, $utf = 0) {
	// The $utf variable is set automatically. Values passed in function arguments are ignored.
	$utf = 0;
	if ( ! $GLOBALS['KSC']->is_utf8 ($str) )
		$str = $GLOBALS['KSC']->utf8 ($str);
	else
		$utf = 1;
	$len = strlen ($str);

	if ( ! $GLOBALS['KSC']->is_utf8 ($set) )
		$set = $GLOBALS['KSC']->utf8 ($set);

	$j = mchar2dec ($set);

	unset ($josa);
	if ( $j == 0 || $j == 7028 ) {  // 가 or 이
		$josa = array ('이', '가');
	} else if ( $j == 6976 || $j == 1684 ) { // 은 or 는
		$josa = array ('은', '는');
	} else if ( $j == 6980 || $j == 3452 ) { // 을 or 를
		$josa = array ('을', '를');
	} else if ( $j == 252  || $j == 6720 ) { // 과 or 와
		$josa = array ('과', '와');
	} else if ( $j == 6468 || $j == 6424 ) { // 아 or 야
		$josa = array ('아', '야');
	} else {
		trigger_error ("'{$set}' is not postposition.", E_USER_ERROR);
		exit;
	}

	// No utf8 case
	if ( $len < 3 || $GLOBALS['KSC']->is_utf8 (substr ($str, -3, 3)) == false ) {
		$buf = substr ($str, -1, 1);
		$ob = ord ($buf);

		// 마지막이 alpabat 일 경우
		if ( ($ob > 64 && $ob < 91) || ($ob > 96 && $ob < 123) ) {
			if ( $len == 1 ) {
				$idx = preg_match ('/[lmnr]/i', $buf) ? 0 : 1;
			} else {
				// tailor..
				if ( preg_match ('/ed|er|or|ar/i', substr ($str, -2, 2)) )
					$idx = 1;
			}
			return $utf ? $josa[$idx] : $GLOBALS['KSC']->utf8 ($josa[$idx], UHC);
		}

		// 마지막이 숫자일 경우
		else if ( $ob > 47 && $ob < 58 ) {
			$idx = (in_array ($buf, array (0, 1, 3, 6, 7, 8)) == true) ? 0 : 1;
			return $utf ? $josa[$idx] : $GLOBALS['KSC']->utf8 ($josa[$idx], UHC);
		}

		else
			return $utf ? $josa[0] : $GLOBALS['KSC']->utf8 ($josa[0], UHC);
	}

	$buf = substr ($str, -3, 3);
	$obuf = mchar2dec ($buf);

	$idx = (($obuf % 28) == 0) ? 1 : 0;
	return $utf ? $josa[$idx] : $GLOBALS['KSC']->utf8 ($josa[$idx], UHC);
}
// }}}

// {{{ +-- public ncrencode_decision_callback ($m)
function ncrencode_decision_callback ($m) {
	$r = base_convert ($m[1], 16, 10);
	return '&#' . $r . ';';
}
// }}}

// {{{ +-- public ncrencode_lib ($str, $type = 0)
function ncrencode_lib ($str, $type = 0) {
	$GLOBALS['KSC']->out_of_ksx1001 ($type ? true : false);
	$r = $GLOBALS['KSC']->ncr ($str, NCR, false);

	return preg_replace_callback ('/&#x([[:alnum:]]{4});/', 'ncrencode_decision_callback', $r);
}
// }}}

// {{{ +-- public ncrdecode_lib ($str)
function ncrdecode_lib ($str) {
	return $GLOBALS['KSC']->ncr ($str, UHC);
}
// }}}

// {{{ +-- public uni_enc_convert ($m)
function uni_enc_convert ($m) {
	$m[1] = base_convert ($m[1], 16, 10);
	return 'U+' . $m[1];
}
// }}}

// {{{ +-- public uniencode_lib ($str, $start='\u', $end='')
function uniencode_lib ($str, $start='\u', $end='') {
	$r = preg_replace_callback (
		'/U\+([[:alnum:]]{4})/', uni_enc_convert, $GLOBALS['KSC']->ucs2 ($str)
	);

	return preg_replace ('/U\+(\d{5})/', $start . '\\1' . $end, $r);
}
// }}}

// {{{ +-- public uni_dec_convert ($m)
function uni_dec_convert ($m) {
	$m[1] = base_convert ($m[1], 10, 16);
	return 'U+' . $m[1];
}
// }}}

// {{{ +-- public unidecode_lib ($str, $set, $start='\u', $end='')
function unidecode_lib ($str, $set, $start='\u', $end='') {
	$start = preg_quote ($start);
	$end   = preg_quote ($end);

	$str = preg_replace_callback ("/{$start}(\d{5}){$end}/", uni_dec_convert, $str);
	$str = preg_replace ("/{$start}([[:alnum:]]{4}){$end}/", 'U+\\1', $str);
	$r = $GLOBALS['KSC']->ucs2 ($str, 'UHC');

	if ( strtolower ($set) == 'euc-kr' || strtolower ($set) == 'euc_kr' )
		$r = ncrencode_lib ($r, true);

	return $r;
}
// }}}

// {{{ +-- public utf8encode_lib ($str)
function utf8encode_lib ($str) {
	return $GLOBALS['KSC']->utf8 ($str, UTF8);
}
// }}}

// {{{ +-- public utf8decode_lib ($str, $set)
function utf8decode_lib ($str, $set) {
	$GLOBALS['KSC']->out_of_ksx1001 (false);
	return $GLOBALS['KSC']->utf8 ($str, UHC);
}
// }}}
?>
