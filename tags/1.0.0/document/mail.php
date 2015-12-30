<?php
$to = preg_replace ('/_golbange_/', '@', $_GET['target']);

if ( $_GET['target'] && preg_match ('/@/', $to) 0) {
  Header("Location: mailto:$to");
}

echo '<script type="javascript">history.back()</script>';
exit;
?>
