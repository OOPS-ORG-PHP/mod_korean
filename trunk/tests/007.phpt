--TEST--
Test network API
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

error_reporting (E_ALL & ~E_DEPRECATED & ~E_STRICT & ~E_NOTICE);

$old = '8.8.8.8';
$new = get_hostname_lib (true, $old);
result (($new == 'google-public-dns-a.google.com'));

$old = readfile_lib ('https://people.kldp.org/robots.txt', false);
result ((preg_match ('/Disallow:/', $old) ? true : false));


?>
--EXPECT--
OK
OK
