--TEST--
Check build number API
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
printf ("%s\n", buildno_lib());
printf ("%s\n", version_lib());
?>
--EXPECT--
201901180110
0.1.6
