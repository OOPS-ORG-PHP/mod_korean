--TEST--
Test parse API
--SKIPIF--
<?php
if ( ! extension_loaded ('korean') ) {
    print 'skip';
}
?>
--POST--
--GET--
--INI--
--FILE--
<?php
function result($v, $err = '') {
	if ( boolval ($v) )
		echo "OK\n";
	else {
        if ( $err )
            echo $err . "\n";
        else
            echo "Failure\n";
	}
}

error_reporting (E_ALL & ~E_DEPRECATED & ~E_STRICT & ~E_NOTICE);

$old = "나는? <a\n href=\"http://u.com/u\">http://o.org/a.php?a=http://a.txt</a> here";
echo autolink_lib ($old) . "\n";

$old = '한글a와 똠방각하';
$new = substr_lib ($old, 0, 10);
result (($new == '한글a와 똠') ? true : false);

$old = '윤아';
$new = postposition_lib ($old, '은');
result (($new == '는') ? true : false);

?>
--EXPECT--
나는? <a href="http://u.com/u" target="_blank">http://o.org/a.php?a=http://a.txt</a> here
OK
OK
