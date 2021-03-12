--TEST--
Test network API
--SKIPIF--
<?php
if ( ! extension_loaded ('korean') ) {
    print 'skip';
}

$iniget = 'ini_get';
if ( function_exists ('___ini_get') ) $iniget = '___ini_get';
if ( ! $iniget ('allow_url_fopen') ) {
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
result (($new == 'dns.google'));

$old = readfile_lib ('https://people.kldp.org/robots.txt', false);
result ((preg_match ('/Disallow:/', $old) ? true : false));


?>
--EXPECT--
OK
OK
