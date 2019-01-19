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
201901191955
1.0.4
