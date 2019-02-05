<?php
# 디렉토리의 파일 리스트를 받는 함수
# path  -> 파일리스트를 구할 디렉토리 경로
# t     -> 리스트를 받을 목록
#          f  : 지정한 디렉토리의 파일만 받음
#          d  : 지정한 디렉토리의 디렉토리만 받음
#          l  : 지정한 디렉토리의 링크만 받음
#          fd : 지정한 디렉토리의 파일과 디렉토리만 받음
#          fl : 지정한 디렉토리의 파일과 링크만 받음
#          dl : 지정한 디렉토리의 디렉토리와 링크만 받음
#          아무것도 지정하지 않았을 경우에는 fdl 모두 받음
# regex -> 표현식을 사용할 수 있으며, regex 를 정의하면 t 는
#          e 로 정의되어짐.
#
function filelist_lib ($path='./', $t='', $regex='') {
	$t = $regex ? "e" : $t;

	if ( ! trim ($path) )
		return false;

	if( is_dir ($path) ) {
		$p = opendir ($path);
		if ( ! $p ) { exit; }

		$regex = str_replace ('/', '\/', $regex);

		while( ($i = readdir($p)) ) {
			$fpath = $path . '/' . $i;
			switch ($t) {
				case 'e'  :
					if ( $i != "." && $i != ".." && ($regex && preg_match ("/{$regex}/i",$i)) )
						$file[] = $i;
					break;
				case 'f'  :
					if ( is_file ($fpath) && ! is_link ($fpath) )
						$file[] = $i;
					break;
				case 'd'  :
					if ( $i != "." && $i != ".." && is_dir ($fpath) )
						$file[] = $i;
					break;
				case 'l'  :
					if ( is_link ($fpath) ) $file[] = $i;
					break;
				case 'fd' :
					if ( $i != "." && $i != ".." && (is_dir ($fpath) || is_file ($fpath) && ! is_link ($fpath)) )
						$file[] = $i;
					break;
				case 'fl' :
					if ( is_file ($fpath) ) $file[] = $i;
					break;
				case 'dl' :
					if ( $i != "." && $i != ".." && (is_dir ($fpath) || is_link ($fpath)) ) $file[] = $i;
					break;
				default   :
					if ( $i != "." && $i != ".." ) $file[] = $i;
			}
		}
		closedir ($p);
	} else {
		trigger_error ("Can't open {$path} in read mode", E_USER_ERROR);
		return false;
	}

	return $file;
}

# file 내용을 변수로 받는 함수
#
function getfile_lib ($filename, $size=0) {
	trigger_error ('Use file_get_contents function instead of getfile_lib', E_USER_DEPRECATED);

	if ( strlen ($filename) < 1 )
		return false;

	if ( ! file_exists ($filename) )
		return false;

	return file_get_contents ($filename, false, null, 0, $size);
}

# file 의 확장자를 검사하여 file type 을 구함
#
function getfiletype_lib ($filetype) {
	$tail = preg_replace ('/\.$/', '', $filetype);
	return substr (strrchr ($tail, '.'), 1);
}

function putfile_lib ($filename, $str, $mode = 0) {
	trigger_error ('Use file_put_contents function instead of putfile_lib', E_USER_DEPRECATED);

	if ( $mode ) $mode = 'ab';
	else $mode = 'wb';

	$fp = fopen ($filename, $mode);
	if ( $fp ) {
		fwrite ($fp, $str);
		fclose ($fp);
	}
}

function sockhttp_lib ($url) {
	if ( ! preg_match ('!^https?://!i', $url) )
		$url = 'http://' . $url;

	$purl = (object) parse_url ($url);
	$sock = @fsockopen ($purl->host, $purl->port ? $purl->port : 80, $errno, $errstr);
	if ( ! is_resource ($sock) )
		return false;

	fputs (
		$sock,
		"GET {$purl->path} HTTP/1.1\r\n" .
		"HOST: {$prul->host}\r\n" .
		"Connection: close\r\n\r\n"
	);

	while ( ! feof ($sock) )
		$text .= fgets ($sock, 1024);
	fclose ($sock);

	return $test;
}

function readfile_lib($path, $ipath=0) {
	if ( strlen ($path) < 1 )
		return false;

	if ( ! preg_match (';^https?://;i', $path) && ! file_exists ($path) ) {
		trigger_error ('path is invalid or missing', E_USER_WARNING);
		return false;
	}

	return file_get_contents ($path, $ipath ? true : false);
}

function pcregrep_lib($regex, $text, $opt=0) {

	$buf = explode ("\n", $text);

	for ( $i=0; $i<count ($buf); $i++ ) {
		if ( ! $opt ) {
			if ( preg_match ($regex, $buf[$i]) )
				$str .= $buf[$i] . "\n";
		} else {
			if ( ! preg_match ($regex, $buf[$i]))
				$str .= $buf[$i] . "\n";
		}
	}

	return preg_replace ("/\n$/", '', $str);
}
?>
