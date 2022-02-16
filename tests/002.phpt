--TEST--
test Charset converting API
--SKIPIF--
<?php
if ( ! extension_loaded ('korean') ) {
    print 'skip';
}
?>
--POST--
--GET--
--INI--
extension=iconv.so
--FILE--
<?php
function result($v, $err = '') {
    if ( $v === true )
        echo "OK\n";
    else {
        if ( $err )
            echo $err . "\n";
        else
            echo "Failure\n";
    }
}

$cp949 = iconv ('utf-8', 'cp949', '똠방각하');

$old = ncrencode_lib ($cp949, true);
$new = ncrdecode_lib ($old);
result (($cp949 == $new));

$old = uniencode_lib ($cp949);
$new = unidecode_lib ($old, 'cp949');

result (($cp949 == $new));

$old = utf8encode_lib ($cp949, 'cp949');
$new = utf8decode_lib ($old, 'cp949');

result (($cp949 == $new));

?>
--EXPECT--
OK
OK
OK
