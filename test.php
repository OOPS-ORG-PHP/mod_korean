<?php

$mcrochk = 0;

if ( $mcrochk )
	$old = microtime ();

echo <<<EOF
*********************************************************************
                         Check  korean.c
*********************************************************************\n
EOF;

$buf = file_get_contents ('./php_korean.h');
preg_match_all ('/#define (BUILDVER|BUILDNO) "([0-9.]+)"/', $buf, $m);

$ver = (object) array (
	'date' => $m[2][0],
	'no'   => $m[2][1]
);

$chk = ($ver->date == '201512280344') ? 'OK' : 'Failed';
printf (" ** %-40s .. %s\n", 'test buildno_lib()', $chk);

$chk = ($ver->no == '1.0.0') ? 'OK' : 'Failed';
printf (" ** %-40s .. %s\n", 'test version_lib()', $chk);

printf (" ** %-40s .. ", 'test movepage_lib() 3sec');
movepage_lib ('http://a.org', 3);
echo "\n";

if ( $mcrochk ) {
	usleep (1500000);
	$new = microtime ();
}

if ( $mcrochk )
	printf (" ** %-40s .. %s sec\n", 'test get_microtime_lib()', get_microtime_lib ($old, $new));
else
	printf (" ** %-40s .. %s\n", 'test get_microtime_lib()', 'skip');

echo <<<EOF
*********************************************************************
                         Check  krcharset.c
*********************************************************************\n
EOF;

$cp949 = iconv ('utf-8', 'cp949', "똠방각하");

$old = ncrencode_lib ($cp949, true);
$new = ncrdecode_lib ($old);

$chk = ($cp949 == $new) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test ncrencode_lib()|ncrdecode_lib()', $chk);

$old = uniencode_lib ($cp949);
$new = unidecode_lib ($old, 'cp949');

$chk = ($cp949 == $new) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test uniencode_lib()|uniencode_lib()', $chk);

$old = utf8encode_lib ($cp949, 'cp949');
$new = utf8decode_lib ($old, 'cp949');

$chk = ($cp949 == $new) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test utf8encode_lib()|utf8decode_lib()', $chk);

echo <<<EOF
*********************************************************************
                         Check  krcheck.c
*********************************************************************\n
EOF;

$buf = '&quot;asdfasdf&quot;';
$chk = check_uristr_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test check_uristr_lib() unsafe', $chk);

$buf = '한글냠냠abc_-';
$chk = ! check_uristr_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test check_uristr_lib() safe', $chk);

$buf = 'jk.kim@ddongnawa.com';
$chk = is_email_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_email_lib() valid', $chk);

$buf = '&quot;asdfasdf&quot;';
$chk = ! is_email_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_email_lib() invalid', $chk);

$buf = 'http://a.com/_한글_/?a=fix&b=asdf@gmail.com&c=aa';
$chk = is_url_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_url_lib() valid', $chk);

$buf = 'a.com/_한글_/?a=fix&b=asdf@gmail.com&c[0]=aa..';
$chk = ! is_url_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_url_lib() invluad', $chk);

$buf = iconv ('utf-8', 'cp949', "돔");
$chk = is_hangul_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_hangul_lib() vluad', $chk);

$buf = iconv ('utf-8', 'cp949', "똠");
$chk = ! is_hangul_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_hangul_lib() invluad', $chk);

$buf = '<table><tr><td>asdf</td></tr></table>';
$chk = ! check_htmltable_lib ($buf) ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test check_htmltable_lib()', $chk);

$chk = is_iis_lib () ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_iis_lib()', $chk);

$chk = is_windows_lib () ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test is_iis_lib()', $chk);

echo <<<EOF
*********************************************************************
                         Check  krerror.c
*********************************************************************\n
EOF;

printf (" ** %-40s .. ", 'test perror_lib()');
perror_lib ('error message test', false, 'http://a.org', 3);

printf (" ** %-40s .. ", 'test pnotice_lib()');
pnotice_lib ('error message test', false);

echo <<<EOF
*********************************************************************
                         Check  krfile.c
*********************************************************************\n
EOF;

$old = 1239476;
$new = human_fsize_lib ($old, true, true, 1000);
printf (" ** %-40s .. %d => %s\n", 'test human_fsize_lib()', $old, $new);

$old = "./";
$new = filelist_lib ($old, 'f', '^php_[^.]+\.h');
printf (" ** %-40s .. ", 'test filelist_lib()');
print_r ($new);

$old = 'str12345';
putfile_lib ('./putfiletest.txt', $old);
$new = getfile_lib ('./putfiletest.txt');
@unlink ('./putfiletest.txt');
$chk =  ($old == $new) ? 'OK' : 'Filure';
printf (" ** %-40s .. %s\n", 'test putfile_lib()|getfile_lib()', $chk);

$old = 'file.dat';
$new = getfiletype_lib ($old);
$chk = ($new == 'dat') ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test getfiletype_lib()', $chk);


$old = <<<EOF
I'm a boy
You are the student
Am i Oh happy?
Oh no, I'm crasy
Oh no, I'M sexy girl
EOF;

printf (" ** %-40s .. ", 'test pcregrep_lib () with "/i\'m/i"');
echo pcregrep_lib ('/i\'m/i', $old) . "\n";

echo <<<EOF
*********************************************************************
                         Check  krimage.c
*********************************************************************\n
EOF;

imgresize_lib ('./document/img/php_logo.gif', 'jpg', 50, 32, './imgresizetest.jpg');
if ( file_exists ('./imgresizetest.jpg') ) {
	$new = getimagesize ('./imgresizetest.jpg');
	if ( $new[0] == 50 && $new[1] == 32 )
		$chk = 'OK';
	else
		$chk = 'Failure';
} else
	$chk = 'Failure';
@unlink ('./imgresizetest.jpg');
printf (" ** %-40s .. %s\n", 'test imageresize_lib()', $chk);

echo <<<EOF
*********************************************************************
                         Check  krmail.c
*********************************************************************\n
EOF;

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

$old = mailsource_lib ($ln, $from, $to, $subject, $body);
printf (" ** %-40s ..\n\n%s", 'test mailsource_lib()', $old);

echo <<<EOF
*********************************************************************
                         Check  krnetwork.c
*********************************************************************\n
EOF;

echo <<<EOF
*********************************************************************
                         Check  krparse.c
*********************************************************************\n
EOF;

$old = "나는? <a\n href=\"http://u.com/u\">http://o.org/a.php?a=http://a.txt</a> here";
$new = autolink_lib ($old);
printf (" ** %-40s ..\n\n    %s\n\n", 'test autolink_lib()', $new);

$old = '한글a와 똠방각하';
$new = substr_lib ($old, 0, 10);
$chk = ($new == '한글a와 똠') ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test substr_lib()', $chk);


$old = '윤아';
$new = postposition_lib ($old, '은');
$chk = ($new == '는') ? 'OK' : 'Failure';
printf (" ** %-40s .. %s\n", 'test postposition_lib()', $chk);
