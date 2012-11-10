<?
# 파일이름에 보안에 취약한 meta character 이 있는지 체크
# 
function check_filename_lib(&$filename) {
  if(preg_match("/[^\xA1-\xFEa-z0-9_-]|\.\./i",trim($s))) {
    return 1;
  }
  return 0;
}

# table tag 가 제대로 사용되었는지 체크
#
function check_htmltable_lib(&$str) {
  if(!preg_match(';</?TABLE[^>]*>;i',$str)) return;

  $from = array(';[\d]+;',     // [0-9]+ to ''
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
  
  $to = array('','<\\1\\2>',1,2,3,4,94,93,92,91,'');
  $check = preg_replace($from,$to,$str);
  
  if(strlen($check)%3) {
    return 1;
  }
  if(!preg_match('/^12(3|4).+9291$/',$check)) {
    return 2;
  } 

  while(preg_match('/([\d])9\1/',$check))
  { $check = preg_replace('/([\d])9\1/','',$check); }

  if($check) {
    return 3;
  }

  return 0;
}

# 경로명이나 번수명에 보안에 취약한 meta character 이 있는지 체크
# 
function check_uristr_lib(&$s) {
  if(preg_match("/[^\xA1-\xFEa-z0-9._-]|\.\./i",trim($s))) {
    return 1;
  }
  return 0;
}

# E-MAIL 주소가 정확한 것인지 검사하는 함수
#
function is_email_lib(&$email) {
  $url = trim($email);

  if (preg_match("/^[\xA1-\xFEa-z0-9_-]+@[\xA1-\xFEa-z0-9_-]+\.[a-z0-9._-]+$/i", $url)) {
    return $url;
  } else {
    return;
  }
}

function is_hangul_lib(&$char) {
  # 특정 문자가 한글의 범위내(0xA1A1 - 0xFEFE)에 있는지 검사
  $char = ord($char);

  if($char >= 0xa1 && $char <= 0xfe)
    return 1;
}

# IIS(isapi) 인지 아닌지 판단 함수
function is_iis_lib() {
  if(php_sapi_name() == "isapi") return 1;
  else return 0;
}

# URL이 정확한 것인지 검사하는 함수
#
function is_url_lib(&$url) {
  $url = trim($url);

  # 프로토콜(http://, ftp://...)을 나타내는 부분이 없을 때 기본값으로
  # http://를 붙임
  if(!preg_match("/^(http|https|ftp|telnet|news):\/\//i", $url))
    $url = "http://$url";

  if(!preg_match("/(http|https|ftp|telnet|news):\/\/[\xA1-\xFEa-z0-9-]+\.[\xA1-\xFEa-zA-Z0-9,:&#@=_~%?\/.+-]+$/i", $url)) {
    return;
  }
    
  return $url;
} 

# PHP의 microtime 함수로 얻어지는 값을 비교하여 경과 시간을 가져오는 함수
#   
function get_microtime_lib($old, $new) {
  $start = explode(" ", $old);
  $end = explode(" ", $new);
  
  return sprintf("%.2f", ($end[1] + $end[0]) - ($start[1] + $start[0]));
}

# 파일 크기를 단위별로 출력하는 함수
#
function human_fsize_lib($bfsize, $sub = "0") {
  $BYTES = number_format($bfsize) . " Bytes"; // 3자리를 기준으로 컴마
  
  if($bfsize < 1024) return $BYTES; # Bytes 범위
  elseif($bfsize < 1048576) $bfsize = number_format($bfsize/1024,1) . " KB"; # KBytes 범위
  elseif($bfsize < 1073741827) $bfsize = number_format($bfsize/1048576,1) . " MB"; # MB 범위
  else $bfsize = number_format($bfsize/1073741827,1) . " GB"; # GB 범위

  if($sub) $bfsize .= "($BYTES)";

  return $bfsize;
}

# 해당 포인트가 2byte 문자인지 체크
#
function multibyte_check($str, $p) {
  if (!$p) { return 0; }

  $chk = ord($str[$p]);
  $chk1 = ord($str[$p-1]);

  if ($chk & 0x81 ||
     /* check of 2byte charactor except KSX 1001 range */
     ($chk1 >= 0x81 && $chk1 <= 0xa0 && $chk >= 0x41 && $chk <=0xfe) ||
     ($chk1 >= 0xa1 && $chk1 <= 0xc6 && $chk >= 0x41 && $chk <=0xa0)) {

    $tmp = preg_replace("/[ ].*/", "", substr($str,$p));
    $l = strlen($tmp);

    for ($i=$p; $i<$p+$l;$i++) {
      $c1 = ord($str[$i]);
      $c2 = ord($str[$i-1]);
      if (ord($str[$i]) & 0x80) { $twobyte++; }
      /* 2th byte of 2 byte charactor is not KSX 1001 range */
      else if (($c2 >= 0x81 && $c2 <= 0xa0 && $c1 >= 0x41 && $c1 <=0xfe) ||
               ($c2 >= 0xa1 && $c2 <= 0xc6 && $c1 >= 0x41 && $c1 <=0xa0))
      { $twobyte++; }
    }

    if ($twobyte % 2 != 0) { return 1; }
  }

  return 0;
}
?>
