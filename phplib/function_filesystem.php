<?
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
function filelist_lib($path='./',$t='',$regex='') {
  $t = $regex ? "e" : $t;
  if(is_dir($path)) {
    $p = opendir($path);
    if (!$p) { exit; }

    $regex = str_replace("/", "\/", $regex);

    while($i = readdir($p)) {
      switch($t) {
        case 'e'  :
          if($i != "." && $i != ".." && preg_match("/$regex/i",$i)) $file[] = $i;
          break;
        case 'f'  :
          if(is_file("$path/$i") && !is_link("$path/$i")) $file[] = $i;
          break;
        case 'd'  :
          if($i != "." && $i != ".." && is_dir("$path/$i")) $file[] = $i;
          break;
        case 'l'  :
          if(is_link("$path/$i")) $file[] = $i;
          break;
        case 'fd' :
          if($i != "." && $i != ".." && (is_dir("$path/$i") || is_file("$path/$i") && !is_link("$path/$i"))) $file[] = $i;
          break;
        case 'fl' :
          if(is_file("$path/$i")) $file[] = $i;
          break;
        case 'dl' :
          if($i != "." && $i != ".." && (is_dir("$path/$i") || is_link("$path/$i"))) $file[] = $i;
          break;
        default   :
          if($i != "." && $i != "..") $file[] = $i;
      }
    }
    closedir($p);
  } else {
    print_error_lib("$path is not directory");
    return 0;
  }

  return $file;
}

# file 내용을 변수로 받는 함수
#
function getfile_lib($filename,$size=0) {
  $fp = fopen($filename,"r");

  if ($fp) {
    $size = trim($size) ? $size : filesize($filename);
    $getfile = fread($fp,$size);
    fclose($fp);
  }

  return $getfile;
}

# file 의 확장자를 검사하여 file type 을 구함
#
function getfiletype_lib($filetype) {
  $tail = preg_replace("/\.$/", "", $filetype);
  $tail = substr(strrchr($tail,"."),1);
  $tail = strtolower($tail);
  return $tail;
}

function putfile_lib($filename, $str, $mode = 0) {
  if ($mode) $mode = "ab";
  else $mode = "wb";

  $fp = fopen($filename, $mode);
  if ($fp) {
    fwrite($fp, $str);
    fclose($fp);
  }
}

function sockhttp_lib($url) {
  $purl = preg_replace("/^http:\/\//i","",$url);
  $p = @fsockopen($purl, 80, &$errno, &$errstr);

  if ($p) {
    fputs ($p, "GET $url\r\n");
    while (!feof($p)) {
      $text .= fgets($p,1024);
    }
    fclose($p);
  }

  return $text;
}

function readfile_lib($path, $ipath=0) {
  ob_start();
  readfile($path, $ipath);
  $ret = ob_get_contents();
  ob_end_clean();

  return $ret;
}

function pcregrep_lib($regex, $text, $opt=0) {

  $buf = explode ("\n", $text);

  for ($i=0; $i<count($buf); $i++) {
    if (!$opt) {
      if (preg_match("$regex", $buf[$i])) {
        $str .= "{$buf[$i]}\n";
      }
    } else {
      if (!preg_match("$regex", $buf[$i])) {
        $str .= "{$buf[$i]}\n";
      }
    }
  }

  $str = preg_replace("/\n$/", "", $str);

  return $str;
}
?>
