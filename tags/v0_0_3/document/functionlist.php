<?
$version = "0.0.1";

$list[] = "main";
$list[] = "check";
$list[] = "filesystem";
$list[] = "html";
$list[] = "parse";
$list[] = "image";
$list[] = "mail";

$check[1] = "check_filename_lib";
$check[2] = "check_htmltable_lib";
$check[3] = "check_uristr_lib";
$check[4] = "get_microtime_lib";
$check[5] = "human_fsize_lib";
$check[6] = "is_email_lib";
$check[7] = "is_hangul_lib";
$check[8] = "is_iis_lib";
$check[9] = "is_url_lib";
$check[10] = "is_windows_lib";
$check[11] = "buildno_lib";
$check[12] = "version_lib";

$filesystem[1] = "filelist_lib";
$filesystem[2] = "getfile_lib";
$filesystem[3] = "getfiletype_lib";
$filesystem[4] = "putfile_lib";
$filesystem[5] = "readfile_lib";

$html[1] = "agentinfo_lib";
$html[2] = "autolink_lib";
$html[3] = "get_hostname_lib";
$html[4] = "movepage_lib";
$html[5] = "perror_lib";
$html[6] = "pnotice_lib";

$parse[1] = "ncrencode_lib";
$parse[2] = "ncrdecode_lib";
$parse[3] = "uniencode_lib";
$parse[4] = "unidecode_lib";
$parse[5] = "utf8encode_lib";
$parse[6] = "utf8decode_lib";
$parse[7] = "substr_lib";
$parse[8] = "postposition_lib";

$image[1] = "imgresize_lib";

$mail[1] = "mailsource_lib";
$mail[2] = "sockmail_lib";

function auto_link($str) {
  global $agent,$rmail;

  $regex[file] = "gz|tgz|tar|gzip|zip|rar|mpeg|mpg|exe|rpm|dep|rm|ram|asf|ace|viv|avi|mid|gif|jpg|png|bmp|eps|mov";
  $regex[file] = "(\.($regex[file])\") TARGET=\"_blank\"";
  $regex[http] = "(http|https|ftp|telnet|news|mms):\/\/(([\xA1-\xFEa-z0-9:_\-]+\.[\xA1-\xFEa-z0-9,:;&#=_~%\[\]?\/.,+\-]+)([.]*[\/a-z0-9\[\]]|=[\xA1-\xFE]+))";
  $regex[mail] = "([\xA1-\xFEa-z0-9_.-]+)@([\xA1-\xFEa-z0-9_-]+\.[\xA1-\xFEa-z0-9._-]*[a-z]{2,3}(\?[\xA1-\xFEa-z0-9=&\?]+)*)";

  # &lt; 로 시작해서 3줄뒤에 &gt; 가 나올 경우와
  # IMG tag 와 A tag 의 경우 링크가 여러줄에 걸쳐 이루어져 있을 경우
  # 이를 한줄로 합침 (합치면서 부가 옵션들은 모두 삭제함)
  $src[] = "/<([^<>\n]*)\n([^<>\n]+)\n([^<>\n]*)>/i";
  $tar[] = "<\\1\\2\\3>";
  $src[] = "/<([^<>\n]*)\n([^\n<>]*)>/i";
  $tar[] = "<\\1\\2>";
  $src[] = "/<(A|IMG)[^>]*(HREF|SRC)[^=]*=[ '\"\n]*($regex[http]|mailto:$regex[mail])[^>]*>/i";
  $tar[] = "<\\1 \\2=\"\\3\">";

  # email 형식이나 URL 에 포함될 경우 URL 보호를 위해 @ 을 치환
  $src[] = "/(http|https|ftp|telnet|news|mms):\/\/([^ \n@]+)@/i";
  $tar[] = "\\1://\\2_HTTPAT_\\3";

  # 특수 문자를 치환 및 html사용시 link 보호
  $src[] = "/&(quot|gt|lt)/i";
  $tar[] = "!\\1";
  $src[] = "/<a([^>]*)href=[\"' ]*($regex[http])[\"']*[^>]*>/i";
  $tar[] = "<A\\1HREF=\"\\3_orig://\\4\" TARGET=\"_blank\">";
  $src[] = "/href=[\"' ]*mailto:($regex[mail])[\"']*>/i";
  $tar[] = "HREF=\"mailto:\\2#-#\\3\">";
  $src[] = "/<([^>]*)(background|codebase|src)[ \n]*=[\n\"' ]*($regex[http])[\"']*/i";
  $tar[] = "<\\1\\2=\"\\4_orig://\\5\"";

  # 링크가 안된 url및 email address 자동링크
  $src[] = "/((SRC|HREF|BASE|GROUND)[ ]*=[ ]*|[^=]|^)($regex[http])/i";
  $tar[] = "\\1<A HREF=\"\\3\" TARGET=\"_blank\">\\3</a>";
  $src[] = "/($regex[mail])/i";
  $tar[] = "<A HREF=\"mailto:\\1\">\\1</a>";
  $src[] = "/<A HREF=[^>]+>(<A HREF=[^>]+>)/i";
  $tar[] = "\\1";
  $src[] = "/<\/A><\/A>/i";
  $tar[] = "</A>";

  # 보호를 위해 치환한 것들을 복구
  $src[] = "/!(quot|gt|lt)/i";
  $tar[] = "&\\1";
  $src[] = "/(http|https|ftp|telnet|news|mms)_orig/i";
  $tar[] = "\\1";
  $src[] = "'#-#'";
  $tar[] = "@";
  $src[] = "/$regex[file]/i";
  $tar[] = "\\1";

  # email 주소를 변형시킴
  $src[] = "/$regex[mail]/i";
  $tar[] = "\\1 AT \\2";
  $src[] = "/<A HREF=\"mailto:([^ ]+) at ([^\">]+)/i";
  $tar[] = "<A HREF=\"./mail.php?target=\\1_golbange_\\2";

  # email 주소를 변형한 뒤 URL 속의 @ 을 복구
  $src[] = "/_HTTPAT_/";
  $tar[] = "@";

  # 이미지에 보더값 0 을 삽입
  $src[] = "/<(IMG SRC=\"[^\"]+\")>/i";
  $tar[] = "<\\1 BORDER=0>";

  # IE 가 아닌 경우 embed tag 를 삭제함
  if($agent[br] != "MSIE") {
    $src[] = "/<embed/i";
    $tar[] = "&lt;embed";
  }

  $str = preg_replace($src,$tar,$str);
  return $str;
}

?>
