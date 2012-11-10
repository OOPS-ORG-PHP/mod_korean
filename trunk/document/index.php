<?php
include "functionlist.php";

if ($_GET[part]) { $files = $list[$_GET[part]]; }

# 함수 리스트 출력
if(!$_GET[func] || preg_match("/^credit|copy|ref$/i",$_GET[func])) {
  for($i=1;$i<sizeof($list);$i++) {
    $plist .= "&nbsp;<IMG SRC=\"./img/box-0.gif\" BORDER=0 WIDTH=11 HEIGHT=7 ALT=\"\">".
              "<A HREF=\"./?part=$i\">$list[$i] function</A><BR>\n";
  }
} else {
  for($i=1;$i<=sizeof(${$list[$_GET[part]]});$i++) {
    $plist .= "&nbsp;<IMG SRC=\"./img/box-0.gif\" BORDER=0 WIDTH=11 HEIGHT=7 ALT=\"\">".
              "<A HREF=\"./?part=$_GET[part]&amp;func=$i\">{${$list[$_GET[part]]}[$i]}</A><BR>\n";
  }
}

# 앞뒤 페이지
$caretl = "<IMG SRC=\"./img/caret-l.gif\" border=0 WIDTH=11 HEIGHT=7 ALT=\"\">";
$caretr = "<IMG SRC=\"./img/caret-r.gif\" border=0 WIDTH=11 HEIGHT=7 ALT=\"\">";
if(!$_GET[part]) {
  $prev = "&nbsp;";
  $next = "<A HREF=\"./?part=1\">$list[1] function $caretr</A>";

  if($_GET[func] == "credit" || $_GET[func] == "copy" || $_GET[func] == "ref") {
    $prev = "$caretl <A HREF=\"./\">main</A>";
    $next = "<A HREF=\"./?func=1\">$list[1]() $caretr</A>";
  }
} else {
  $_GET[func] = ! $_GET[func] ? "0" : $_GET[func];
  $pno = $_GET[func]-1;
  $nno = $_GET[func]+1;

  $ppart = $_GET[part] - 1;

  if($_GET[part] == 1 && !$_GET[func]) {
    $prev = "<A HREF=\"./\">$caretl main</A>";
  } else if($_GET[part] > 1 && !$_GET[func]) {
    $prev = "<A HREF=\"./?part=$ppart\">$caretl {$list[$ppart]}</A>";
  } else if($_GET[part] == 1 && $pno == 0) {
    if($_GET[func] == 1) $prev = "<A HREF=\"./?part=1\">$caretl $list[1]</A>";
    else $prev = "<A HREF=\"./?part=$pno\">$caretl {$list[$pno]} function</A>";
  } else {
    if(!$pno) {
      $prev = "<A HREF=\"./?part=$ppart\">$caretl {$list[$ppart]}</A>";
    } else if($pno > 0) {
      $prev = "<A HREF=\"./?part=$_GET[part]&amp;func=$pno\">$caretl {${$list[$_GET[part]]}[$pno]}()</A>";
    } else $prev = "&nbsp;";
  } 

  if($_GET[part] > 0 && $_GET[func] <= sizeof(${$list[$_GET[part]]}) - 1) {
    if($nno <= sizeof(${$list[$_GET[part]]})) {
      $next = "<A HREF=\"./?part=$_GET[part]&amp;func=$nno\">{${$list[$_GET[part]]}[$nno]}() $caretr</A>";
    } else $next = "&nbsp;";
  } else {
    $nno = $_GET[part] + 1;
    if ($_GET[part] < sizeof($list) - 1) {
      $next = "<A HREF=\"./?part=$nno\">{$list[$nno]} function $caretr</A>";
    } else {
      $next = "&nbsp;";
    }
  }

  if($_GET[func] == "credit" || $_GET[func] == "copy") {
    $prev = "$caretl <A HREF=\"./\">main</A>";
    $next = "<A HREF=\"./?func=1\">$list[1]() $caretr</A>";
  }
}

# 본문 출력
$_GET[part] = ! $_GET[part] ? "0" : $_GET[part];

if (!$_GET[part]) $file = "./text/main";
else {
  if (!$_GET[func]) $file = "./text/$files";
  else $file = "./text/{${$list[$_GET[part]]}[$_GET[func]]}";
}

if($_GET[func] == "credit") $file = "./text/credit";
elseif($_GET[func] == "copy") $file = "./text/copy";

if ($_GET[func] == "ref") {
  for($i=1;$i<=sizeof($list);$i++) {
    $txt .= "\n<FONT TITLE=\"RE\">$list[$i]</FONT>\n\n";
    for($j=1;$j<=sizeof(${$list[$i]});$j++) {
      $txt .= ${$list[$i]}[$j]."()\n";

      $src[] = "/(<FONT[^>]*>)?{${$list[$i]}[$j]}(<\/FONT>)?/i";
      $des[] = "<A HREF=\"./?part=$i&amp;func=$j\">\\0</A>";
    }
  }
  $txt = preg_replace($src,$des,$txt);

} else if(file_exists($file)) {
  $p = fopen($file,"rb");
  $txt = fread($p,filesize($file));
  fclose($p);

  /* func 변수가 없을 경우 리스트 출력 */
  if (!$_GET[func]) {
    for($i=1;$i<=sizeof(${$list[$_GET[part]]});$i++) {
      $flist .= ${$list[$_GET[part]]}[$i]."\n";
    }
  }

  $txt .= $flist;

  for($i=1;$i<=sizeof($list);$i++) {
    for($j=1;$j<=sizeof(${$list[$i]});$j++) {
      $src[] = "/(<FONT[^>]*>)?{${$list[$i]}[$j]}(\(\))?(<\/FONT>)?/i";
      $des[] = "<A HREF=\"./?part=$i&amp;func=$j\">\\0</A>";
    }
  }
  $txt = preg_replace($src,$des,$txt);

} else {
  $txt = "<P>\n<TABLE BORDER=0 WIDTH=\"100%\" HEIGHT=\"100%\" CELLPADDING=10 CELLSPACING=0>\n".
         "<TR><TD ALIGN=\"CENTER\" VALIGN=\"CENTER\">\n".
         "<FONT STYLE=\"font: 20px tahoma; font-weight:bold; color:#000066;\">Function {$list[$_GET[func]]}() is unsupported</FONT>".
         "</TD></TR>\n</TABLE>\n";
}

$rsrc = array("/REP_VERSION_REP/", "/REP_PHPVERSION_REP/");
$rdes = array("$version", "$phpversion");
$txt = preg_replace($rsrc, $rdes, $txt);
$txt = auto_link($txt);

include "html.template";
?>
