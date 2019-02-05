<?php

$pwd = getcwd ();

$KSCinst = <<<EOL
if [ ! -d "KSC5601" ]; then
	pear download oops/KSC5601
	tar xvfpz KSC5601-*.tgz
	rm -f KSC5601-*.tgz
	mv KSC5601-* KSC5601
fi
EOL;

if ( ! file_exists ('./KSC5601') || ! is_dir ('./KSC5601') ) {
	system (
		'pear download oops/KSC5601 && ' .
		'tar xvfpz KSC5601-*.tgz && ' .
		'rm -f KSC5601-*.tgz && ' .
		'mv KSC5601-* KSC5601 && ' .
		'rm -f pcakage.xml'
	);
}

$inc = <<<EOL
set_include_path (
	get_include_path() . PATH_SEPARATOR . '{$pwd}/KSC5601' . PATH_SEPARATOR . '{$pwd}'
);
include_once 'KSC5601.php';
include_once 'function_header.php';

EOL;

function test_parser ($v) {
	global $inc;

	$r = new stdClass;

	$r->desc = trim (preg_replace ('/^--TEST--(.*)--SKIPIF--.*/s', '\\1', $v));
	$r->code = trim (preg_replace ('/^.*--FILE--(.*)--EXPECT--.*/s', '\\1', $v));
	$r->exp  = ltrim (preg_replace ('/^.*--EXPECT--(.*)/s', '\\1', $v));

	$r->code = preg_replace ('/<\?(php)?/', "\\0\n${inc}", $r->code);

	return $r;
}

function safe_unlink ($v) {
	if ( file_exists ($v) )
		unlink ($v);
}

function line_number ($v) {
	$x = @file ($v);
	return count ($x);
}


if ( ! file_exists ('./tests') )
	mkdir ('./tests');

chdir ('./tests');

if ( ! file_exists ('./php_log.gif') )
	copy ('../../tests/php_logo.gif', 'php_logo.gif');

$dp = opendir ('../../tests/');

while ( ($f = readdir ($dp)) ) {
	if ( ! preg_match ('/\.phpt$/', $f) )
		continue;
	$testlist[] = $f;
}

closedir ($dp);
sort ($testlist);

$sapi = new stdClass;
$sapi->name = PHP_SAPI;
$sapi->version = PHP_VERSION;
$sapi->zend = zend_version ();
$sapi->os = PHP_OS . ' - ' . php_uname ();
$sapi->ini = realpath (get_cfg_var("cfg_file_path"));
$sapi->moreini = function_exists ('php_ini_scanned_files') ?
	str_replace ("\n" , "", php_ini_scanned_files ()) :
	"** not determined **";
$sapi->cwd = getcwd ();
$startt = time ();
$start = date('Y-m-d H:i:s', $startt);

echo <<<EOL

=====================================================================
PHP_SAPI     : {$sapi->name}
PHP_VERSION  : {$sapi->version}
ZEND_VERSION : {$sapi->zend}
PHP_OS       : {$sapi->os}
INI actual   : {$sapi->ini}
More .INIs   : {$sapi->moreini}
CWD          : {$sapi->cwd}

=====================================================================
TIME START {$start}
=====================================================================

EOL;

$total_tests = 0;

$do = 0;
foreach ( $testlist as $test ) {
	$tname = preg_replace ('/\.[^.]+$/', '', $test);

	$v = test_parser (file_get_contents ('../../tests/' . $test));
	$v->base = $tname;

	file_put_contents ($tname . '.php', $v->code);
	file_put_contents ($tname . '.exp', $v->exp);
	chdir ('../');
	system ("php -d allow_url_fopen=1 tests/${tname}.php > tests/${tname}.log 2> /dev/null");
	chdir ('./tests');
	system ("diff ${tname}.log ${tname}.exp > ${tname}.diff 2> /dev/null");

	$total_tests += line_number ("${tname}.exp");

	if ( file_exists ("${tname}.diff") ) {
		$size = filesize ("${tname}.diff");
		$res = ($size == 0) ? 'PASS' : 'FAIL';
	} else {
		$res = 'FAIL';
	}

	if ( $res == 'FAIL' )
		$fail[] = $v;

	printf ("%s %s [tests/%s.phpt]\n", $res, $v->desc, $tname);

	if ( $res == 'PASS' ) {
		foreach ( array ('.php', '.exp', '.log', '.diff') as $ext )
			safe_unlink ($tname . $ext);
	}

	$do++;

	//if ( $do == 2 )
	//	break;
}

safe_unlink ('php_logo.gif');
chdir ('../');
@rmdir ('./tests');

$endt = time ();
$rest = $endt - $startt;
$end = date('Y-m-d H:i:s', $endt);
$total_test_num = count ($testlist);
$fail_count = count ($fail);
$succ_count = $total_test_num - $fail_count;

$fail_per = (100 * $fail_count) / $total_test_num;
$succ_per = (100 * $succ_count) / $total_test_num;

printf ("

=====================================================================
TIME END %s

=====================================================================
TEST RESULT SUMMARY
---------------------------------------------------------------------
Exts skipped    :    0
Exts tested     :  %3d
---------------------------------------------------------------------

Number of tests :   %2d
Tests skipped   :    0 (  0.0%%)
Tests failed    :   %2d (%5.1f%%)
Tests passed    :   %2d (%5.1f%%)
---------------------------------------------------------------------
Time taken      :  %3d seconds
=====================================================================

=====================================================================
FAILED TEST SUMMARY
---------------------------------------------------------------------
",
	$end, $total_tests, $total_test_num, $fail_count, $fail_per,
	$succ_count, $succ_per, $rest
);

if ( count ($fail) ) {
	foreach ( $fail as $f ) {
		printf ("%s [tests/%s.phpt]\n", $f->desc, $f->base);
	}
} else
	echo "All success\n";

echo "=====================================================================\n"

?>
