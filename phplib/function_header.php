<?php

if ( ! extension_loaded ('korean') ) {

	class KOREAN_LIB {
		const BUILDNO = "202103130156";
		const BUILDVER = "1.0.6";

		const REG_KR_U = '\x{1100}-\x{11FF}\x{3130}-\x{318F}\x{AC00}-\x{D7AF}';
		const REG_KR   = '\xA1-\xFE';
	}
	// }}}

	$KSC = new KSC5601;

	function buildno_lib () {
		return KOREAN_LIB::BUILDNO;
	}

	function version_lib () {
		return KOREAN_LIB::BUILDVER;
	}


	include_once 'codetables/ksc5601.php';
	include_once 'function_check.php';
	include_once 'function_filesystem.php';
	include_once 'function_html.php';
	include_once 'function_image.php';
	include_once 'function_mail.php';
	include_once 'function_parse.php';
}
