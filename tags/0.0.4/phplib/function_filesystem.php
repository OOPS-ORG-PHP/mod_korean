<?
# ���丮�� ���� ����Ʈ�� �޴� �Լ�
# path  -> ���ϸ���Ʈ�� ���� ���丮 ���
# t     -> ����Ʈ�� ���� ���
#          f  : ������ ���丮�� ���ϸ� ����
#          d  : ������ ���丮�� ���丮�� ����
#          l  : ������ ���丮�� ��ũ�� ����
#          fd : ������ ���丮�� ���ϰ� ���丮�� ����
#          fl : ������ ���丮�� ���ϰ� ��ũ�� ����
#          dl : ������ ���丮�� ���丮�� ��ũ�� ����
#          �ƹ��͵� �������� �ʾ��� ��쿡�� fdl ��� ����
# regex -> ǥ������ ����� �� ������, regex �� �����ϸ� t ��
#          e �� ���ǵǾ���.
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

# file ������ ������ �޴� �Լ�
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

# file �� Ȯ���ڸ� �˻��Ͽ� file type �� ����
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
  if (preg_match("/^http:/i", $path)) {
    $ret = sockhttp_lib($path);
  } else {
    if (!file_exists($path)) {
      $vpath = ini_get("include_path");
      $inpath = explode(":", $vpath);
      for ($i=0;$i<count($inpath);$i++) {
        $inpath[$i] = preg_replace("/\/$/","",$inpath[$i]);
        if (file_exists("$inpath[$i]/$path")) {
          $rpath = "$inpath[$i]/$path";
          break;
        }
      }
      if ($rpath) { $path = $rpath; }
      else { return; }
    }
    $ret = getfile_lib($path, filesize($path));
  }

  return $ret;
}
?>
