<?
# Agent �� ������ ������
#
function agentinfo_lib() {
  $agent_env = $GLOBALS['HTTP_USER_AGENT'];

  # $agent �迭 ���� [br] ������ ����
  #                  [os] �ü��
  #                  [ln] ��� (�ݽ�������)
  #                  [vr] ������ ����
  #                  [co] ���� ����
  if(preg_match("/MSIE/i", $agent_env)) {
    $agent['br'] = "MSIE";
    # OS �� ����
    if(preg_match("/NT/i", $agent_env)) $agent['os'] = "NT";
    else if(preg_match("/Win/i", $agent_env)) $agent['os'] = "WIN";
    else $agent['os'] = "OTHER";
    # version ����
    $agent['vr'] = trim(preg_replace("/Mo.+MSIE ([^;]+);.+/i","\\1",$agent_env));
    $agent['vr'] = preg_replace("/[a-z]/i","",$agent['vr']);
    # ȣȯ ����
    $agent['co'] = "msie";
  } else if(preg_match("/Opera/i", $agent_env)) {
    $agent['br'] = "OPERA";
    $agent['co'] = "msie";
    # client OS
    if (preg_match("/Linux/i", $agent_env)) $agent['os'] = "LINUX";
    else if (preg_match("/2000|XP/i", $agent_env)) $agent['os'] = "NT";
    else if (preg_match("/Win/i", $agent_env)) $agent['os'] = "WIN";
    else $agent['os'] = "OTHER";
    # version ����
    $agent['vr'] = preg_replace("/Opera\/([0-9.]+).*/i","\\1",$agent_env);
    # language ����
    if (preg_match("/\[ko\]/i", $agent_env)) $agent['ln'] = "ko";
    else if (preg_match("/\[en\]/i", $agent_env)) $agent['ln'] = "en";
    else $agent['ln'] = "other";
  } else if(preg_match("/Gecko|Galeon/i",$agent_env) && !preg_match("/Netscape/i",$agent_env)) {
    echo $agent_env."\n";
    $agent['br'] = "MOZL";
    # client OS ����
    if(preg_match("/NT/i", $agent_env)) $agent['os'] = "NT";
    else if(preg_match("/Win/i", $agent_env)) $agent['os'] = "WIN";
    else if(preg_match("/Linux/i", $agent_env)) $agent['os'] = "LINUX";
    else $agent['os'] = "OTHER";
    # version ����
    $agent['vr'] = preg_replace("/Mozi[^(]+\([^;]+;[^;]+;[^;]+;[^;]+;([^)]+)\).*/i","\\1",$agent_env);
    $agent['vr'] = trim(str_replace("rv:","",$agent['vr']));
    #lang ����
    if (preg_match("/ ko|-KR/", $agent_env)) $agent['ln'] = "ko";
    else if (preg_match("/ en|-US/", $agnet_env)) $agent['ln'] = "en";
    else $agent['ln'] = "other";
    # NS ���� ���� ����
    $agent['co'] = "mozilla";
  } else if(preg_match("/Konqueror/i",$agent_env)) {
    $agent['br'] = "KONQ";
    $agent['co'] = "mozilla";
    # os ����
    if (preg_match("/Linux/i", $agent_env)) $agent['os'] = "LINUX";
    else if (preg_match("/FreeBSD/i", $agent_env)) $agent['os'] = "FreeBSD";
    else $agent['os'] = "OTHER";
    # version ����
    $agent['vr'] = preg_replace("/.*Konqueror\/([0-9.]+).*/i","\\1",$agent_env);
  } else if(preg_match("/Lynx/i", $agent_env)) {
    $agent['br'] = "LYNX";
    $agent['co'] = "TextBR";
    # version ����
    $agent['vr'] = preg_replace("/Lynx\/([^ ]+).*/i","\\1",$agent_env);
  } else if(preg_match("/w3m/i", $agent_env)) {
    $agent['br'] = "W3M";
    $agent['co'] = "TextBR";
    $agent['vr'] = preg_replace("/w3m\/([0-9.]+).*/i","\\1",$agent_env);
  } else if(preg_match("/Links/i", $agent_env)) {
    $agent['br'] = "LINKS";
    $agent['co'] = "TextBR";
    $agent['vr'] = preg_replace("/Links \(([^;]+);.*/i","\\1",$agent_env);
    if (preg_match("/Linux/i", $agent_env)) {
      $agent['os'] = "LINUX";
    } elseif (preg_match("/FreeBSD/i", $agent_env)) {
      $agent['os'] = "FreeBSD";
    } else {
      $agent['os'] = "OTHER";
    }
  } else if(preg_match("/^Mozilla/i", $agent_env)) {
    $agent['br'] = "NS";
    # client OS ����
    if(preg_match("/NT/i", $agent_env)) {
      $agent['os'] = "NT";
    } else if(preg_match("/Win/i", $agent_env)) {
      $agent['os'] = "WIN";
    } else if(preg_match("/Linux/i", $agent_env)) {
      $agent['os'] = "LINUX";
    } else $agent['os'] = "OTHER";
    # language ����
    if(preg_match("/\[ko\]/i", $agent_env)) $agent['ln'] = "ko";
    else if(preg_match("/\[en\]/i", $agent_env)) $agent['ln'] = "en";
    else $agent['ln'] = "other";
    # version ����
    if(preg_match("/Gecko/i",$agent_env)) $agent['vr'] = "6";
    else $agent['vr'] = "4";
    # Mozilla ���� ���� ����
    $agent['co'] = "mozilla";
  } else {
    $agent['br'] = "OTHER";
    $agent['co'] = "OTHER";
  }

  return $agent;
}

# ���� ���뿡 �ִ� URL���� ã�Ƴ��� �ڵ����� ��ũ�� �������ִ� �Լ�
#
# preg_replace  - �� ������ ����ǥ������ �̿��� ġȯ
#                 http://www.php.net/manual/function.preg-replace.php
function autolink_lib($str) {

  $regex['file'] = "gz|tgz|tar|gzip|zip|rar|mpeg|mpg|exe|rpm|dep|rm|ram|asf|ace|viv|avi|mid|gif|jpg|png|bmp|eps|mov";
  $regex['file'] = "(\.({$regex['file']})\") TARGET=\"_blank\"";
  $regex['http'] = "(http|https|ftp|telnet|news|mms):\/\/(([\xA1-\xFEa-z0-9:_\-]+\.[\xA1-\xFEa-z0-9,:;&#=_~%\[\]?\/.,+\-]+)([.]*[\/a-z0-9\[\]]|=[\xA1-\xFE]+))";
  $regex['mail'] = "([\xA1-\xFEa-z0-9_.-]+)@([\xA1-\xFEa-z0-9_-]+\.[\xA1-\xFEa-z0-9._-]*[a-z]{2,3}(\?[\xA1-\xFEa-z0-9=&\?]+)*)";

  # &lt; �� �����ؼ� 3�ٵڿ� &gt; �� ���� ����
  # IMG tag �� A tag �� ��� ��ũ�� �����ٿ� ���� �̷���� ���� ���
  # �̸� ���ٷ� ��ħ (��ġ�鼭 �ΰ� �ɼǵ��� ��� ������)
  $src[] = "/<([^<>\n]*)\n([^<>\n]+)\n([^<>\n]*)>/i";
  $tar[] = "<\\1\\2\\3>";
  $src[] = "/<([^<>\n]*)\n([^\n<>]*)>/i";
  $tar[] = "<\\1\\2>";
  $src[] = "/<(A|IMG)[^>=]*(HREF|SRC)[^=]*=[ '\"\n]*({$regex['http']}|mailto:{$regex['mail']})[^>]*>/i";
  $tar[] = "<\\1 \\2=\"\\3\">";

  # email �����̳� URL �� ���Ե� ��� URL ��ȣ�� ���� @ �� ġȯ
  $src[] = "/(http|https|ftp|telnet|news|mms):\/\/([^ \n@]+)@/i";
  $tar[] = "\\1://\\2_HTTPAT_\\3";

  # Ư�� ���ڸ� ġȯ �� html���� link ��ȣ
  $src[] = "/&(quot|gt|lt)/i";
  $tar[] = "!\\1";
  $src[] = "/<a([^>]*)href=[\"' ]*({$regex['http']})[\"']*[^>]*>/i";
  $tar[] = "<A\\1HREF=\"\\3_orig://\\4\" TARGET=\"_blank\">";
  $src[] = "/href=[\"' ]*mailto:({$regex['mail']})[\"']*>/i";
  $tar[] = "HREF=\"mailto:\\2#-#\\3\">";
  $src[] = "/<([^>]*)(background|codebase|src)[ \n]*=[\n\"' ]*({$regex['http']})[\"']*/i";
  $tar[] = "<\\1\\2=\"\\4_orig://\\5\"";

  # ��ũ�� �ȵ� url�� email address �ڵ���ũ
  $src[] = "/((SRC|HREF|BASE|GROUND)[ ]*=[ ]*|[^=]|^)({$regex['http']})/i";
  $tar[] = "\\1<A HREF=\"\\3\" TARGET=\"_blank\">\\3</a>";
  $src[] = "/({$regex['mail']})/i";
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
  $src[] = "/{$regex['file']}/i";
  $tar[] = "\\1";

  # email �ּҸ� ������ �� URL ���� @ �� ����
  $src[] = "/_HTTPAT_/";
  $tar[] = "@";

  # �̹����� ������ 0 �� ����
  $src[] = "/<(IMG SRC=\"[^\"]+\")>/i";
  $tar[] = "<\\1 BORDER=0>";

  # IE �� �ƴ� ��� embed tag �� ������
  if(!preg_match("/MSIE/i", $GLOBALS['HTTP_USER_AGENT'])) {
    $src[] = "/<embed/i";
    $tar[] = "&lt;embed";
  }

  $str = preg_replace($src,$tar,$str);
  return $str;
}

# �� ���� �������� IP �ּ� Ȥ�� �����θ��� �������� �Լ�
#
function get_hostname_lib($reverse=0, $host=0)
{ 
  if(!$host) {
    if($_SERVER['HTTP_VIA']) {
      $tmp = array('HTTP_CLIENT_IP','HTTP_X_FORWARDED_FOR','HTTP_X_COMING_FROM',
                   'HTTP_X_FORWARDED','HTTP_FORWARDED_FOR','HTTP_FORWARDED',
                   'HTTP_COMING_FROM','HTTP_PROXY','HTTP_SP_HOST');
      foreach($tmp AS $v) if($_SERVER[$v] != $_SERVER['REMOTE_ADDR']) break;
      if($_SERVER[$v]) $host = preg_replace(array('/unknown,/i','/,.*/'),'',$_SERVER[$v]);
      $host = ($host = trim($host)) ? $host : $_SERVER['REMOTE_ADDR'];
    }
    else $host = $_SERVER['REMOTE_ADDR'];
  }
  $check = $reverse ? @gethostbyaddr($host) : '';
  
  return $check ? $check : $host;
}

# ���ϴ� �������� �̵���Ű�� �Լ�
#
function movepage_lib($path,$time = 0) {
  $path = str_replace(" ","%20",$path);
  echo "<META http-equiv=\"refresh\" content=\"$time;URL=$path\">";
}

# ���� �޼����� ����ϴ� �Լ�
#
function perror_lib($s,$java=0,$move='',$time=0) {
  if(!$s) $s = "Error occurrence!";

  $agent = agentinfo_lib();
  if ($agent['co'] = "TextBR") $java = 0;

  if($java) {
    $src = array("/\n/i","/'|#/i");
    $des = array("\\n","\\\\\\0");
    $s = trim($s) ? preg_replace($src,$des,$s) : "Don\'t Specified Error Message";
    echo "<SCRIPT>alert('$s');history.back()</SCRIPT>";
  } else echo "ERROR: $s";

  if ($move) { movepage_lib($move,$time); }
  exit;
}

# ��� �޼����� ����ϴ� �Լ�
#
function pnotice_lib($s='',$java=0) {
  if($java) {
    $src = array("/\n/i","/'|#/i");
    $des = array("\\n","\\\\\\0");
    $s = trim($s) ? preg_replace($src,$des,$s) : "Don\'t permit null string with pnotice_lib function";
    echo "<SCRIPT>alert('$s')</SCRIPT>";
  } else echo "ERROR: $s";
}
?>
