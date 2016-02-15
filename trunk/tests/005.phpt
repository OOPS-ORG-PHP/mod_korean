--TEST--
Test image resize API
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
	if ( !$v ) {
        if ( $err )
            echo $err . "\n";
        else
            echo "Failure\n";
	} else
		echo "OK\n";
}

imgresize_lib ('./document/img/php_logo.gif', 'jpg', 50, 32, './imgresizetest.jpg');
if ( file_exists ('./imgresizetest.jpg') ) {
    $new = getimagesize ('./imgresizetest.jpg');
    if ( $new[0] == 50 && $new[1] == 32 )
        $chk = true;
    else
        $chk = false;
} else
    $chk = false;
@unlink ('./imgresizetest.jpg');
result ($chk);

?>
--EXPECT--
OK
