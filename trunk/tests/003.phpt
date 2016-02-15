--TEST--
test Check API
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

$buf = '&quot;asdfasdf&quot;';
result (check_uristr_lib ($buf));

$buf = '한글냠냠abc_-';
result (! check_uristr_lib ($buf));

$buf = 'jk.kim@ddongnawa.com';
result (is_email_lib ($buf));

$buf = '&quot;asdfasdf&quot;';
result (! is_email_lib ($buf));

$buf = 'http://a.com/_한글_/?a=fix&b=asdf@gmail.com&c=aa';
result (is_url_lib ($buf));

$buf = 'a.com/_한글_/?a=fix&b=asdf@gmail.com&c[0]=aa..';
result (! is_url_lib ($buf));

$buf = iconv ('utf-8', 'cp949', "돔");
result (is_hangul_lib ($buf));

$buf = iconv ('utf-8', 'cp949', "똠");
result (! is_hangul_lib ($buf));

$buf = '<table><tr><td>asdf</td></tr></table>';
result (! check_htmltable_lib ($buf));

result (! is_iis_lib ());
result (! is_windows_lib ());

?>
--EXPECT--
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
OK
