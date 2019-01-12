--TEST--
test File operation API
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

$old = 1239476;
echo human_fsize_lib ($old, true, true, 1000). "\n";

$old = "./";
$new = filelist_lib ($old, 'f', '^php_[^.]+\.h');
sort ($new);
print_r ($new);

$old = <<<EOF
I'm a boy
You are the student
Am i Oh happy?
Oh no, I'm crasy
Oh no, I'M sexy girl
EOF;

echo pcregrep_lib ('/i\'m/i', $old) . "\n";

?>
--EXPECT--
1.24 Mb (1,239,476 Bits)
Array
(
    [0] => php_korean.h
    [1] => php_kr.h
    [2] => php_krcharset.h
    [3] => php_krcheck.h
    [4] => php_krerror.h
    [5] => php_krfile.h
    [6] => php_krimage.h
    [7] => php_krmail.h
    [8] => php_krmath.h
    [9] => php_krnetwork.h
    [10] => php_krparse.h
)
I'm a boy
Oh no, I'm crasy
Oh no, I'M sexy girl
