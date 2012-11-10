<?
set_time_limit(0);

if (!extension_loaded("korean")) {
  include "{$krext_dlpath}codetables/ksc5601.php";
  include "{$krext_dlpath}function_check.php";
  include "{$krext_dlpath}function_filesystem.php";
  include "{$krext_dlpath}function_html.php";
  include "{$krext_dlpath}function_image.php";
  include "{$krext_dlpath}function_mail.php";
  include "{$krext_dlpath}function_parse.php";
}
?>
