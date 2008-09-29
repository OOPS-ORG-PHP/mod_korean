<?
function substr_lib($str,$from,$length=0,$utf8=0) {

  if ($utf8) { $str = utf8decode_lib($str, "cp949"); }
  else { $str = ncrdecode_lib($str); }

  $len = strlen($str);

  if (!$length) $length = strlen($str);

  if ($from < 0) {
    $from += $length;
    if ($from < 0) $from = 0;
  }

  if ($length < 0) {
    $length = ($len - $from) + $length;
    if ($length < 0) $length = 0;
  }

  if ($from >= $len) { return; }
  if (($from + $length) > $len) { $length = $len - $from; }

  if (multibyte_check($str, $from)) {
    $from++;
    $length--;
  }

  if (multibyte_check($str, $from + $length)) { $length++; }

  $ret = substr($str, $from, $length);

  if ($utf8) { $ret = utf8encode_lib($ret); }
  else { $ret = ncrencode_lib($ret, 1); }

  return $ret;
}

function postposition_lib($str,$set,$utf=0) {
  if ($utf) {
    $str = utf8decode_lib($str, "cp949");
    $set = utf8decode_lib($set, "cp949");
  } else { $str = ncrdecode_lib($str,"cp949"); }

  if (strlen($set) != 2) {
    if (!$set) { echo "postposition value is not found"; }
    else { echo "$set is not postposition"; }
    exit;
  }

  $one = ord($set[0]);
  $two = ord($set[1]);
  $len = strlen($str);

  if ($len < 1) {
    echo "original string is not found";
    exit;
  }

  if ( ($one == 0xc0 && $two == 0xcc) || ($one == 0xb0 && $two == 0xa1) ) {
    $chkjosa[0] = "이";
    $chkjosa[1] = "가";
  } else if ( ($one == 0xc0 && $two == 0xba) || ($one == 0xb4 && $two == 0xc2) ) {
    $chkjosa[0] = "은";
    $chkjosa[1] = "는";
  } else if ( ($one == 0xc0 && $two == 0xbb) || ($one == 0xb8 && $two == 0xa6) ) {
    $chkjosa[0] = "을";
    $chkjosa[1] = "를";
  } else if ( ($one == 0xb0 && $two == 0xfa) || ($one == 0xbf && $two == 0xcd) ) {
    $chkjosa[0] = "과";
    $chkjosa[1] = "와";
  } else if ( ($one == 0xbe && $two == 0xc6) || ($one == 0xbe && $two == 0xdf) ) {
    $chkjosa[0] = "아";
    $chkjosa[1] = "야";
  } else {
    echo "$set is not postposition";
    exit;
  }

  if ($len > 1) $chkstr = ($len > 1) ? $str[$len-2].$str[$len-1] : $str;

  $one = ord($chkstr[0]);
  $two = (strlen($chkstr) > 1) ? ord($chkstr[0]) : $one;

  # if 한
  if ($one & 0x80) {
    if ($one > 0x7a) $two -= 6;
    if ($one > 0x5a) $two -= 6;
    $ncr = ($one - 0x81) * 178 + ($two - 0x41);

    if (($GLOBALS['table_ksc5601'][$ncr] - 16) % 28 == 0) { $post = $chkjosa[1]; }
    else { $post = $chkjosa[0]; }
  # number area
  } else if ($two > 47 && $two < 58) {
    if ($one == 50 || $one == 52 || $one == 53 || $one == 57) { $post = $chkjosa[0]; }
    else { $post = $chkjosa[1]; }
  # only 1 charactor
  } else if ($one == $two) {
    if ( $one == 114 || ($one > 108 && $one < 111) ) { $post = $chkjosa[0]; }
    else { $post = $chkjosa[1]; }
  # if 한a
  } else if ($one != $second && $one > 127 && ($two < 123 && $two >96)) {
    if ( $two == 114 || ($two > 108 && $two < 111) ) { $post = $chkjosa[0]; }
    else { $post = $chkjosa[1]; }
  # if aa
  } else {
    # last charactor is a or e or i or o or u or w or y
    if ($two == 0x61 || $two == 0x65 || $two == 0x69 || $two == 0x6f ||
        $two == 0x75 || $two == 0x77 || $two == 0x79) {
        $post = $chkjosa[1];
    /* last charactor is ed or er or or */
    } else if ( ($one == 0x65 && $one == 0x72) || ($one == 0x6f && $one == 0x72) ||
              ($one == 0x65 && $one == 0x64) ) {
        $post = $chkjosa[1];
    } else {
        $post = $chkjosa[0];
    }
  }

  if ($utf) { $post = utf8encode_lib($post); }

  return $post;
}

function charencode_lib($one, $two, $opt=0, $hanja=0, $start='\u', $end='') {
  if (!$hanja) {
    if ($two < 0x41 || $two > 0xfe) { $ret = "?"; }
    elseif (0x5a < $two && $two < 0x61) { $ret = "?"; }
    elseif (0x7a < $two && $two < 0x81) { $ret = "?"; }
    else {
      if ($two > 0x7a) { $two -= 6; }
      if ($two > 0x5a) { $two -= 6; }
      $ncr = ($one - 0x81) * 178 + ($two - 0x41);

      switch ($opt) {
        case 1: $ret = "&#".$GLOBALS['table_ksc5601'][$ncr].";"; break;
        case 2: $ret = $start.strtoupper(dechex($GLOBALS['table_ksc5601'][$ncr])).$end; break;
        default: $ret = $GLOBALS['table_ksc5601'][$ncr];
      }
    }
  } else {
    if ($two < 0xa1 || $two > 0xfe) { $ret = "?"; }
    else {
      $ncr = ($one - 0xca) * 94 + ($two - 0xa1);

      switch ($opt) {
        case 1: $ret = "&#".$GLOBALS['table_ksc5601_hanja'][$ncr].";"; break;
        case 2: $ret = $start.strtoupper(dechex($GLOBALS['table_ksc5601_hanja'][$ncr])).$end; break;
        default: $ret = $GLOBALS['table_ksc5601_hanja'][$ncr];
      }
    }
  }
  return $ret;
}

function ncrencode_lib($str, $type = 0) {
  $len = strlen($str);

  for ($i=0; $i<$len; $i++) {
    $one = ord($str[$i]);
    $two = ord($str[$i+1]);
    if ( $one & 0x80 ) {
      # if type 1, check range of KSX 1001
      switch ($type) {
        case 1:
          if ( ($one >= 0x81 && $one <= 0xa0 && $two >= 0x41 && $two <= 0xfe) ||
               ($one >= 0xa1 && $one <= 0xc6 && $two >= 0x41 && $two <= 0xa0) ) {
            $ret .= charencode_lib($one, $two, 1, 0);
            $i++;
          } else {
            $ret .= $str[$i];
          }
          break;
        # range of whole string
        default:
          if ( 0x81 <= $one && $one <= 0xc8 ) {
            $ret .= charencode_lib($one, $two, 1, 0);
          } else if (0xca <= $one && $one <= 0xfd) {
            $ret .= charencode_lib($one, $two, 1, 1);
          } else {
             $ret .= $str[$i];
          }
          $i++;
      }
    # 1 byte charactor
    } else {
      $ret .= $str[$i];
    }
  }
  return $ret;
}

function ncrdecode_lib($str) {
  global $table_ksc5601;
  $len = strlen($str);

  for ($i=0; $i<$len; $i++) {
    $c1 = 0x81;
    $c2 = 0x41;
    if ($str[$i] == '&' && $str[$i+1] == '#' && $str[$i+7] == ';') {
      $tmp = $str[$i+2].$str[$i+3].$str[$i+4].$str[$i+5].$str[$i+6];

      $point = array_search($tmp, $GLOBALS[table_ksc5601]);
      for($j=0; $j<=$point; $j++) {
        if ($j != 0) {
          $c2++;
          if ($c2 == 0x5b) $c2 = 0x61;
          else if ($c2 == 0x7b) $c2 = 0x81;
          else if ($c2 == 0xff) {
            $c2 = 0x41;
            $c1++;
          }
        }
      }

      $tmp = ($c1 << 8) + $c2;

      $first = $tmp >> 8;
      $second = $tmp & 0x00FF;

      $ret .= chr($first).chr($second);
      $i += 7;
    } else { $ret .= $str[$i]; }
  }

  return $ret;
}

function uniencode_lib ($str, $start='\u', $end='') {
  $len = strlen($str);

  for ($i=0; $i<$len; $i++) {
    $one = ord($str[$i]);
    $two = ord($str[$i+1]);

    # if 2byte charactor
    if ( 0x81 <= $one && $one <= 0xc8) {
      $ret .= charencode_lib($one, $two, 2, 0, $start, $end);
      $i++;
    } else if (0xca <= $one && $one <= 0xfd) {
      $ret .= charencode_lib($one, $two, 2, 1, $start, $end);
      $i++;
    } else { $ret .= $str[$i]; }
  }

  return $ret;
}

function unidecode_lib ($str, $set, $start='\u', $end='') {
  $len = strlen($str);
  $slen = strlen($start);
  $elen = strlen($end);

  if ($slen > 10 || $elen > 10) {
    echo "Can't use string over 10 charactors <br />\n".
         "on unicode start string or end string";
    exit;
  }

  if (preg_match("/^(euc-kr|euc_kr)$/i", $set)) {
    $ncr = 1;
  } else if (preg_match("/^cp949$/i", $set)) {
    $ncr = 0;
  } else {
    echo "Unknown charactor set $set";
    exit;
  }

  for ($i=0; $i<$len; $i++) {
    $c1 = 0x81;
    $c2 = 0x41;

    for ($l=0; $l<$slen; $l++) $svalue .= $str[$i+$l];
    for ($l=0; $l<$elen; $l++) $evalue .= $str[$i+$slen+4+$l];
    for ($l=0; $l<4; $l++) $value .= $str[$i+$slen+$l];

    if (!strncmp($svalue, $start, $slen) && !strncmp($evalue, $end, $elen) &&
        preg_match("/[0-9a-z]{4}/i", $value)) {
      $tmp = hexdec($str[$i+$slen].$str[$i+$slen+1].$str[$i+$slen+2].$str[$i+$slen+3]);
      $point = array_search($tmp, $GLOBALS[table_ksc5601]);

      for($j=0; $j<=$point; $j++) {
        if ($j != 0) {
          $c2++;
          if ($c2 == 0x5b) $c2 = 0x61;
          else if ($c2 == 0x7b) $c2 = 0x81;
          else if ($c2 == 0xff) {
            $c2 = 0x41;
            $c1++;
          }
        }
      }

      $tmp = ($c1 << 8) + $c2;

      $first = $tmp >> 8;
      $second = $tmp & 0x00FF;

      $ret .= chr($first).chr($second);
      $i += $slen + $elen + 3;

    } else { $ret .= $str[$i]; }
    unset($value);
    unset($svalue);
    unset($evalue);
  }

  if ($ncr) $ret = ncrencode_lib($ret, $ncr);
  return $ret;
}

function utf8encode_lib($str) {
  $len = strlen($str);

  for ($i=0;$i<$len;$i++) {
    $one = ord($str[$i]);
    $two = ord($str[$i+1]);

    if ($one & 0x81) {
      $uni = dechex(charencode_lib($one, $two));
      $a = checkLen_lib(decbin(hexdec($uni[0])),4);
      $b = checkLen_lib(decbin(hexdec($uni[1])),4);
      $c = checkLen_lib(decbin(hexdec($uni[2])),4);
      $d = checkLen_lib(decbin(hexdec($uni[3])),4);

      $utf = array("1110".$a[0].$a[1].$a[2].$a[3],
                   "10".$b[0].$b[1].$b[2].$b[3].$c[0].$c[1],
                   "10".$c[2].$c[3].$d);

      $ret .= chr(bindec($utf[0])).chr(bindec($utf[1])).chr(bindec($utf[2]));
      $i++;
    } else { $ret .= $str[$i]; }
  }

  return $ret;
}

function utf8decode_lib($str, $set) {
  $len = strlen($str);

  if (!preg_match("/euc-kr|euc_kr|cp949/i",$set)) {
    if (!$set) echo "charactor set in no value";
    else echo "Unsupported charactor $set!";
    exit;
  }

  for ($i=0; $i<$len; $i++) {
    if (ord($str[$i]) & 0x80) {
      $chr1 = decbin(ord($str[$i]));
      $chr2 = decbin(ord($str[$i+1]));
      $chr3 = decbin(ord($str[$i+2]));

      $chrCode1 = dechex(bindec($chr1[4].$chr1[5].$chr1[6].$chr1[7]));
      $chrCode2 = dechex(bindec($chr2[2].$chr2[3].$chr2[4].$chr2[5]));
      $chrCode3 = dechex(bindec($chr2[6].$chr2[7].$chr3[2].$chr3[3]));
      $chrCode4 = dechex(bindec($chr3[4].$chr3[5].$chr3[6].$chr3[7]));

      $tmp = "\u".$chrCode1.$chrCode2.$chrCode3.$chrCode4;
      $ret .= unidecode_lib ($tmp, $set);
      unset($tmp);

      $i += 2;
    } else { $ret .= $str[$i]; }
  }

  return $ret;
}

function checkLen_lib($str,$len,$chr='0') {
  $add = $len - strlen($str);
  for($i=0;$i<$add;$i++) {
    $ret .= "$chr";
  }
  $str = $ret.$str;
  return $str;
}

?>
