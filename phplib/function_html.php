<?php
# Agent 의 정보를 가져옴
#
function agentinfo_lib () {
	$agent_env = $GLOBALS['HTTP_USER_AGENT'];

	# $agent 배열 정보 [br] 브라우져 종류
	#                  [os] 운영체제
	#                  [ln] 언어 (넷스케이프)
	#                  [vr] 브라우져 버젼
	#                  [co] 예외 정보
	if ( preg_match ("/MSIE/i", $agent_env) ) {
		$agent['br'] = "MSIE";
		# OS 별 구분
		if ( preg_match ("/NT/i", $agent_env) ) $agent['os'] = "NT";
		else if ( preg_match ("/Win/i", $agent_env) ) $agent['os'] = "WIN";
		else $agent['os'] = "OTHER";
		# version 정보
		$agent['vr'] = trim (preg_replace ("/Mo.+MSIE ([^;]+);.+/i", "\\1", $agent_env));
		$agent['vr'] = preg_replace ("/[a-z]/i", "", $agent['vr']);
		# 호환 정보
		$agent['co'] = "msie";
	} else if ( preg_match ("/Opera/i", $agent_env) ) {
		$agent['br'] = "OPERA";
		$agent['co'] = "msie";
		# client OS
		if ( preg_match ("/Linux/i", $agent_env) ) $agent['os'] = "LINUX";
		else if ( preg_match ("/2000|XP/i", $agent_env) ) $agent['os'] = "NT";
		else if ( preg_match ("/Win/i", $agent_env) ) $agent['os'] = "WIN";
		else $agent['os'] = "OTHER";
		# version 정보
		$agent['vr'] = preg_replace ("/Opera\/([0-9.]+).*/i", "\\1", $agent_env);
		# language 정보
		if ( preg_match ("/\[ko\]/i", $agent_env) ) $agent['ln'] = "ko";
		else if ( preg_match ("/\[en\]/i", $agent_env) ) $agent['ln'] = "en";
		else $agent['ln'] = "other";
	} else if ( preg_match ("/Gecko|Galeon/i", $agent_env) && !preg_match ("/Netscape/i", $agent_env) ) {
		echo $agent_env."\n";
		$agent['br'] = "MOZL";
		# client OS 구분
		if ( preg_match ("/NT/i", $agent_env) ) $agent['os'] = "NT";
		else if ( preg_match ("/Win/i", $agent_env) ) $agent['os'] = "WIN";
		else if ( preg_match ("/Linux/i", $agent_env) ) $agent['os'] = "LINUX";
		else $agent['os'] = "OTHER";
		# version 정보
		$agent['vr'] = preg_replace ("/Mozi[^(]+\([^;]+;[^;]+;[^;]+;[^;]+;([^)]+)\).*/i", "\\1", $agent_env);
		$agent['vr'] = trim (str_replace ("rv:", "", $agent['vr']));
		#lang 정보
		if ( preg_match ("/ ko|-KR/", $agent_env) ) $agent['ln'] = "ko";
		else if ( preg_match ("/ en|-US/", $agnet_env) ) $agent['ln'] = "en";
		else $agent['ln'] = "other";
		# NS 와의 공통 정보
		$agent['co'] = "mozilla";
	} else if ( preg_match ("/Konqueror/i", $agent_env) ) {
		$agent['br'] = "KONQ";
		$agent['co'] = "mozilla";
		# os 정보
		if ( preg_match ("/Linux/i", $agent_env) ) $agent['os'] = "LINUX";
		else if ( preg_match ("/FreeBSD/i", $agent_env) ) $agent['os'] = "FreeBSD";
		else $agent['os'] = "OTHER";
		# version 정보
		$agent['vr'] = preg_replace ("/.*Konqueror\/([0-9.]+).*/i", "\\1", $agent_env);
	} else if ( preg_match ("/Lynx/i", $agent_env) ) {
		$agent['br'] = "LYNX";
		$agent['co'] = "TextBR";
		# version 정보
		$agent['vr'] = preg_replace ("/Lynx\/([^ ]+).*/i", "\\1", $agent_env);
	} else if ( preg_match ("/w3m/i", $agent_env) ) {
		$agent['br'] = "W3M";
		$agent['co'] = "TextBR";
		$agent['vr'] = preg_replace ("/w3m\/([0-9.]+).*/i", "\\1", $agent_env);
	} else if ( preg_match ("/Links/i", $agent_env) ) {
		$agent['br'] = "LINKS";
		$agent['co'] = "TextBR";
		$agent['vr'] = preg_replace ("/Links \(([^;]+);.*/i", "\\1", $agent_env);
		if ( preg_match ("/Linux/i", $agent_env) ) {
			$agent['os'] = "LINUX";
		} elseif ( preg_match ("/FreeBSD/i", $agent_env) ) {
			$agent['os'] = "FreeBSD";
		} else {
			$agent['os'] = "OTHER";
		}
	} else if ( preg_match ("/^Mozilla/i", $agent_env) ) {
		$agent['br'] = "NS";
		# client OS 구분
		if ( preg_match ("/NT/i", $agent_env) ) {
			$agent['os'] = "NT";
		} else if ( preg_match ("/Win/i", $agent_env) ) {
			$agent['os'] = "WIN";
		} else if ( preg_match ("/Linux/i", $agent_env) ) {
			$agent['os'] = "LINUX";
		} else $agent['os'] = "OTHER";
		# language 정보
		if ( preg_match ("/\[ko\]/i", $agent_env) ) $agent['ln'] = "ko";
		else if ( preg_match ("/\[en\]/i", $agent_env) ) $agent['ln'] = "en";
		else $agent['ln'] = "other";
		# version 정보
		if ( preg_match ("/Gecko/i", $agent_env) ) $agent['vr'] = "6";
		else $agent['vr'] = "4";
		# Mozilla 와의 공통 정보
		$agent['co'] = "mozilla";
	} else {
		$agent['br'] = "OTHER";
		$agent['co'] = "OTHER";
	}

	return $agent;
}

# 문서 내용에 있는 URL들을 찾아내어 자동으로 링크를 구성해주는 함수
#
# preg_replace  - 펄 형식의 정규표현식을 이용한 치환
#                 http://www.php.net/manual/function.preg-replace.php
function autolink_lib ($str) {

	$regex['file'] = 'gz|tgz|tar|gzip|zip|rar|mpeg|mpg|exe|rpm|dep|rm|ram|asf|ace|viv|avi|mid|gif|jpg|png|bmp|eps|mov';
	$regex['file'] = "(\.({$regex['file']})\") TARGET=\"_blank\"";
	$regex['http'] = '(http|https|ftp|telnet|news|mms):\/\/(([\xA1-\xFEa-z0-9:_\-]+\.[\xA1-\xFEa-z0-9,:;&#=_~%\[\]?\/.,+\-]+)([.]*[\/a-z0-9\[\]]|=[\xA1-\xFE]+))';
	$regex['mail'] = '([\xA1-\xFEa-z0-9_.-]+)@([\xA1-\xFEa-z0-9_-]+\.[\xA1-\xFEa-z0-9._-]*[a-z]{2,3}(\?[\xA1-\xFEa-z0-9=&\?]+)*)';

	# &lt; 로 시작해서 3줄뒤에 &gt; 가 나올 경우와
	# IMG tag 와 A tag 의 경우 링크가 여러줄에 걸쳐 이루어져 있을 경우
	# 이를 한줄로 합침 (합치면서 부가 옵션들은 모두 삭제함)
	$src[] = '/<([^<>\n]*)\n([^<>\n]+)\n([^<>\n]*)>/i';
	$tar[] = '<\\1\\2\\3>';
	$src[] = '/<([^<>\n]*)\n([^\n<>]*)>/i';
	$tar[] = '<\\1\\2>';
	$src[] = "/<(a|img)[^>=]*(href|src)[^=]*=[ '\"\n]*({$regex['http']}|mailto:{$regex['mail']})[^>]*>/i";
	$tar[] = '<\\1 \\2="\\3">';

	# email 형식이나 URL 에 포함될 경우 URL 보호를 위해 @ 을 치환
	$src[] = '/(http|https|ftp|telnet|news|mms):\/\/([^ \n@]+)@/i';
	$tar[] = '\\1://\\2_HTTPAT_\\3';

	# 특수 문자를 치환 및 html사용시 link 보호
	$src[] = '/&(quot|gt|lt)/i';
	$tar[] = '!\\1';
	$src[] = "/<a([^>]*)href=[\"' ]*({$regex['http']})[\"']*[^>]*>/i";
	$tar[] = '<a\\1href="\\3_orig://\\4" target="_blank">';
	$src[] = "/href=[\"' ]*mailto:({$regex['mail']})[\"']*>/i";
	$tar[] = "href=\"mailto:\\2#-#\\3\">";
	$src[] = "/<([^>]*)(background|codebase|src)[ \n]*=[\n\"' ]*({$regex['http']})[\"']*/i";
	$tar[] = '<\\1\\2="\\4_orig://\\5"';

	# 링크가 안된 url및 email address 자동링크
	$src[] = "/((src|href|base|ground)[ ]*=[ ]*|[^=]|^)({$regex['http']})/i";
	$tar[] = '\\1<a href="\\3" target="_blank">\\3</a>';
	$src[] = "/({$regex['mail']})/i";
	$tar[] = '<a href="mailto:\\1">\\1</a>';
	$src[] = '/(<a href=[^>]+>)<a href=[^>]+>/i';
	$tar[] = '\\1';
	$src[] = '/<\/a><\/a>/i';
	$tar[] = '</a>';

	# 보호를 위해 치환한 것들을 복구
	$src[] = '/!(quot|gt|lt)/i';
	$tar[] = '&\\1';
	$src[] = '/(http|https|ftp|telnet|news|mms)_orig/i';
	$tar[] = '\\1';
	$src[] = "'#-#'";
	$tar[] = '@';
	$src[] = "/{$regex['file']}/i";
	$tar[] = '\\1';

	# email 주소를 변형한 뒤 URL 속의 @ 을 복구
	$src[] = "/_HTTPAT_/";
	$tar[] = "@";

	# 이미지에 보더값 0 을 삽입
	$src[] = '/<(img src=\"[^\"]+\")>/i';
	$tar[] = '<\\1 border="0">';

	# IE 가 아닌 경우 embed tag 를 삭제함
	if (!preg_match ('/MSIE/i', $GLOBALS['HTTP_USER_AGENT'])) {
		$src[] = '/<embed/i';
		$tar[] = '&lt;embed';
	}

	$str = preg_replace ($src, $tar, $str);
	return $str;
}

# 웹 서버 접속자의 IP 주소 혹은 도메인명을 가져오는 함수
#
function get_hostname_lib ($reverse = 0, $host = 0) { 
	if ( ! $host ) {
		if ( $_SERVER['HTTP_VIA'] ) {
			$tmp = array (
				'HTTP_CLIENT_IP', 'HTTP_X_FORWARDED_FOR', 'HTTP_X_COMING_FROM',
				'HTTP_X_FORWARDED', 'HTTP_FORWARDED_FOR', 'HTTP_FORWARDED',
				'HTTP_COMING_FROM', 'HTTP_PROXY', 'HTTP_SP_HOST'
			);
			foreach ( $tmp AS $v ) {
				if ( $_SERVER[$v] != $_SERVER['REMOTE_ADDR'] )
					break;
			}
			if ( $_SERVER[$v] )
				$host = preg_replace (array ('/unknown,/i', '/,.*/'), '', $_SERVER[$v]);
			$host = ($host = trim ($host)) ? $host : $_SERVER['REMOTE_ADDR'];
		} else
			$host = $_SERVER['REMOTE_ADDR'];
	}
	$check = $reverse ? @gethostbyaddr ($host) : '';

	return $check ? $check : $host;
}

# 원하는 페이지로 이동시키는 함수
#
function movepage_lib ($path, $time = 0) {
	$path = str_replace (" ", "%20", $path);
	printf ('<meta http-equiv="refresh" content="%s;url=%s">', $time, $path);
}

# 에러 메세지를 출력하는 함수
#
function perror_lib ($s, $java=0, $move='', $time=0) {
	if ( ! $s ) $s = 'Error occurrence!';

	$agent = agentinfo_lib ();
	if ( $agent['co'] = "TextBR")  $java = 0;

	if ( $java ) {
		$src = array ("/\n/i", "/'|#/i");
		$des = array ("\\n", "\\\\\\0");
		$s = trim ($s) ? preg_replace ($src, $des, $s) : "Don\'t Specified Error Message";
		echo "<script>alert('{$s}');history.back()</script>";
	} else echo "ERROR: {$s}";

	if ( $move ) { movepage_lib ($move, $time); }
	exit;
}

# 경고 메세지를 출력하는 함수
#
function pnotice_lib ($s='', $java=0) {
	if ( $java ) {
		$src = array ("/\n/i", "/'|#/i");
		$des = array ("\\n", "\\\\\\0");
		$s = trim ($s) ? preg_replace ($src, $des, $s) : "Don\'t permit null string with pnotice_lib function";
		echo "<script>alert('{$s}')</script>";
	} else echo "ERROR: {$s}";
}
?>
