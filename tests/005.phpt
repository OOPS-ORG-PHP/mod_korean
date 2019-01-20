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

if ( file_exists ('./tests/php_logo.gif') ) {
    imgresize_lib ('./tests/php_logo.gif', 'jpg', 50, 32, './tests/imgresizetest.jpg');
    if ( file_exists ('./tests/imgresizetest.jpg') ) {
        $new = getimagesize ('./tests/imgresizetest.jpg');
        if ( $new[0] == 50 && $new[1] == 32 )
            $chk = true;
        else
            $chk = false;
    } else
        $chk = false;

    @unlink ('./tests/imgresizetest.jpg');
} else {
	$chk = false;
}

result ($chk);
?>
--EXPECT--
OK
