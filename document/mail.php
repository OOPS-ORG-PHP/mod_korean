<?
$to = preg_replace("/_golbange_/","@",$_GET[target]);

if($_GET[target] && preg_match("/@/",$to)) {
  Header("Location: mailto:$to");
}

echo "<SCRIPT>history.back()</SCRIPT>";
exit;
?>
