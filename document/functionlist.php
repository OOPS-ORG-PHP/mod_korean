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

  # &lt; �� �����ؼ� 3�ٵڿ� &gt; �� ���� ����
  # IMG tag �� A tag �� ��� ��ũ�� �����ٿ� ���� �̷���� ���� ���
  # �̸� ���ٷ� ��ħ (��ġ�鼭 �ΰ� �ɼǵ��� ��� ������)
  $src[] = "/<([^<>\n]*)\n([^<>\n]+)\n([^<>\n]*)>/i";
  $tar[] = "<\\1\\2\\3>";
  $src[] = "/<([^<>\n]*)\n([^\n<>]*)>/i";
  $tar[] = "<\\1\\2>";
  $src[] = "/<(A|IMG)[^>]*(HREF|SRC)[^=]*=[ '\"\n]*($regex[http]|mailto:$regex[mail])[^>]*>/i";
  $tar[] = "<\\1 \\2=\"\\3\">";

  # email �����̳� URL �� ���Ե� ��� URL ��ȣ�� ���� @ �� ġȯ
  $src[] = "/(http|https|ftp|telnet|news|mms):\/\/([^ \n@]+)@/i";
  $tar[] = "\\1://\\2_HTTPAT_\\3";

  # Ư�� ���ڸ� ġȯ �� html���� link ��ȣ
  $src[] = "/&(quot|gt|lt)/i";
  $tar[] = "!\\1";
  $src[] = "/<a([^>]*)href=[\"' ]*($regex[http])[\"']*[^>]*>/i";
  $tar[] = "<A\\1HREF=\"\\3_orig://\\4\" TARGET=\"_blank\">";
  $src[] = "/href=[\"' ]*mailto:($regex[mail])[\"']*>/i";
  $tar[] = "HREF=\"mailto:\\2#-#\\3\">";
  $src[] = "/<([^>]*)(background|codebase|src)[ \n]*=[\n\"' ]*($regex[http])[\"']*/i";
  $tar[] = "<\\1\\2=\"\\4_orig://\\5\"";

  # ��ũ�� �ȵ� url�� email address �ڵ���ũ
  $src[] = "/((SRC|HREF|BASE|GROUND)[ ]*=[ ]*|[^=]|^)($regex[http])/i";
  $tar[] = "\\1<A HREF=\"\\3\" TARGET=\"_blank\">\\3</a>";
  $src[] = "/($regex[mail])/i";
  $tar[] = "<A HREF=\"mailto:\\1\">\\1</a>";
  $src[] = "/<A HREF=[^>]+>(<A HREF=[^>]+>)/i";
  $tar[] = "\\1";
  $src[] = "/<\/A><\/A>/i";
  $tar[] = "</A>";

  # ��ȣ�� ���� ġȯ�� �͵��� ����
  $src[] = "/!(quot|gt|lt)/i";
  $tar[] = "&\\1";
  $src[] = "/(http|https|ftp|telnet|news|mms)_orig/i";
  $tar[] = "\\1";
  $src[] = "'#-#'";
  $tar[] = "@";
  $src[] = "/$regex[file]/i";
  $tar[] = "\\1";

  # email �ּҸ� ������Ŵ
  $src[] = "/$regex[mail]/i";
  $tar[] = "\\1 AT \\2";
  $src[] = "/<A HREF=\"mailto:([^ ]+) at ([^\">]+)/i";
  $tar[] = "<A HREF=\"./mail.php?target=\\1_golbange_\\2";

  # email �ּҸ� ������ �� URL ���� @ �� ����
  $src[] = "/_HTTPAT_/";
  $tar[] = "@";

  # �̹����� ������ 0 �� ����
  $src[] = "/<(IMG SRC=\"[^\"]+\")>/i";
  $tar[] = "<\\1 BORDER=0>";

  # IE �� �ƴ� ��� embed tag �� ������
  if($agent[br] != "MSIE") {
    $src[] = "/<embed/i";
    $tar[] = "&lt;embed";
  }

  $str = preg_replace($src,$tar,$str);
  return $str;
}

?>
