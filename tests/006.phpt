--TEST--
Test mail API
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

$ln = "utf-8";
$from = "보내는이 <sender@domain.org>";
$to = "받는 이 <reciever@to.org>, \"어드민\" <admin@domain.org>, who@domain.org";
$subject = "subject 안에 \"따옴표\" 가 있습니다.";
$body = <<<EOF
<html>
	<head><title>html 문서</title></head>
	<body>
		html 문서를 작성합니다.
		<table border=1 width=500>
			<tr>
				<td> 1 이죠</td>
				<td> 2 죠 </td>
			</tr>
			<tr>
				<td colspan=2>
				COLSPAN=2 입니다.
				</td>
			</tr>
		</table>
	</body>
</html>
EOF;

$buf = mailsource_lib ($ln, $from, $to, $subject, $body);
$bufr = preg_split ("/\r?\n/", $buf);
echo $bufr[16] . "\n";
?>
--EXPECT--
aHRtbCDrrLjshJzrpbwg7J6R7ISx7ZWp64uI64ukLgoxIOydtOyjoAoyIOyj
