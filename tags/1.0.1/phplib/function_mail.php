<?
# mail 보내기 함수 2002.07.01 김정균
# OOPS PHP Mail Library v0.2.4

# 서버상의 smtp daemon 에 의존하지 않고 직접 발송하는 smtp class
#
# 특정 배열로 class 에 전달을 하여 메일을 발송한다. 배열은 아래을 참조한다.
#
# debug -> debug 를 할지 안할지를 결정한다.
# ofhtml -> 웹상에서 사용할지 쉘상에서 사용할지를 결정한다.
# from -> 메일을 발송하는 사람의 메일주소
# to -> 메일을 받을 사람의 메일 주소
# text -> 헤더 내용을 포함한 메일 본문
#
class maildaemon_lib {
  var $failed = 0;

  function maildaemon_lib($v) {
    $this->debug = $v->debug ? $v->debug : $v['debug'];
    $this->ofhtml = $v->ofhtml ? $v->ofhtml : $v['ofhtml'];
    if ( ! $this->helo ) {
      if($_SERVER['SERVER_NAME']) $this->helo = $_SERVER['SERVER_NAME'];
      if(preg_match("/^[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}\.[0-9]{1,3}$/",$this->helo))
        $this->helo = "OOPS-PHPLibrarySMTP";
    }
    $this->from = $v->from ? $v->from : $v['from'];
    $this->to   = $v->to ? $v->to : $v['to'];
    $this->body = $v->text ? $v->text."\r\n." : $v['text']."\r\n.";
    $this->newline = $this->ofhtml ? "<br>\n" : "\n";
    $this->errno = 0;

    $to = str_replace("<"," <",$this->to);
    $to = str_replace(">","> ",$to);
    $to = preg_replace("/[ ]+/"," ",$to);
    $to_addr = explode(" ",trim(str_replace(",","",$to)));
    $from_addr = trim(preg_replace("/.*<([^>]+)>/i","\\1",$this->from));

    if ( $ret_r ) unset ($ret_r);
    $ret_r = array ();

    for($i=0;$i<sizeof($to_addr);$i++) {
      $to_addr[$i] = trim(preg_replace("/<|>/","",$to_addr[$i]));
      if(is_email_lib($to_addr[$i])) {
        $this->mx = $this->getMX($to_addr[$i]);

        if($this->debug) {
          echo "DEBUG: ".$this->mx." start\n";
          echo "################################################################\n";
        }
        $this->sockets("open");
        $this->send("HELO ".$this->helo);
        $this->send("MAIL FROM: <$from_addr>");
        $this->send("RCPT TO: <$to_addr[$i]>");
        $this->send("data");
        $this->send($this->body);
        $this->send("quit");
        $this->sockets("close");

        if($this->failed)
          $ret_r[] = $to_addr[$i];
      }
    }

    return $ret_r;
  }

  function getMX($email) {
    $dev = explode("@",$email);
    $account = $dev[0];
    $host = $dev[1];

    if(checkdnsrr($host,"MX") && getmxrr($host,$mx,$weight)) {
      $idx = 0;
      for($i=0;$i<sizeof($mx);$i++) {
        $dest = $dest ? $dest : $weight[$i];
        if($dest > $weight[$i]) {
          $dest = $weight[$i];
          $idx = $i;
        }
      }
    } else return $host;
    return $mx[$idx];
  }

  # 디버그 함수
  #  $t -> 1 (debug of socket open,close)
  #        0 (regular smtp message)
  #  $p -> 1 (print detail debug)
  # 
  # return 1 -> success
  # return 0 -> failed
  #
  function debug($str,$t=0,$p=0) {
    if($t) {
      if(!$str) $this->failed = 1;
      if($this->sock) $returnmsg = trim(fgets($this->sock,1024));
    } else {
      if(!preg_match("/^(220|221|250|251|354)$/",substr(trim($str),0,3)))
        $this->failed = 1;
    }

    # DEBUG mode -> 모든 메세지 출력
    if($p) {
      if($t) {
        $str = "Conncet ".$this->mx;
        $str .= $this->failed ? " Failed" : " Success";
        $str .= $this->newline."DEBUG: $returnmsg";
      }
      echo "DEBUG: $str".$this->newline;
    }

    # DEBUG 모드가 아닐때, 에러 메세지 출력
    if(!$p && $this->failed) {
      $str = ($t) ? "Connect Failed" : $str;
      if($this->ofhtml) echo "<SCRIPT>\nalert('$str')\n</SCRIPT>\n";
      else echo "ERROR: $str\n";
    }
  }

  function sockets($option='') {
    switch($option) {
      case "open" :
        $this->sock = @fsockopen($this->mx,25,&$this->errno,&$this->errstr,30);
        $this->debug($this->sock,1,$this->debug);
        break;
      default :
        if($this->sock) fclose($this->sock);
        break;
    }
  }

  function send($str) {
    if(!$this->failed) {
      if($this->debug) {
        if(preg_match("/\r\n/",trim($str)))
          $str_debug = trim(str_replace("\r\n","\r\n       ",$str));
        else $str_debug = $str;
        echo "DEBUG: $str_debug".$this->newline;
      }
      fputs($this->sock,"$str\r\n");
      $recv = trim(fgets($this->sock,1024));
      $this->debug($recv,0,$this->debug);
    }
  }
}

# 메일을 발송하기 위한 헤더및 본문을 정리
# 해당 값을 넘기기 위해서는 배열을 이용하며, 각 배열의 타입은 아래를
# 따른다.
#
# debug  => mail 발송시 debug mode 작동 (발송 과정을 출력)
# ofhtml => Debug 사용시 html 을 사용할지 안할지. (1-> 사용, 0-> 사용안함)
# ln     => 언어 타입  (ko,en)
# from   => 보내는 사람의 주소
# to     => 받을 사람의 주소
# title  => 메일 제목
# text   => 메일 본문
# htext  => text/html 로 메일을 발솔할 경우, html 을 지원하지 않는 메일 클
#           라이언트를 위한 text/plain 형 메세지
#
class mailparse_lib {
  function mailparse_lib($mails) {
    $this->make_boundary_lib();
    $this->parse_lib($mails);
  }

  function errmsg_lib($s,$ln='') {
    if($ln == "ko")
      $ln = preg_match("/WINDOWS/i",$HTTP_SERVER_VARS['HTTP_USER_AGNET']) ? $ln : "en";
    
    if($ln == "ko") {
      $chkto = "받는이의 주소가 지정되어 있지 않습니다.";
      $chkfrom = "보내는 이의 주소가 지정되어 있지 않습니다";
      $chktitle = "메일 제목이 없습니다";
      $chktext = "메일 내용이 없습니다";
    } else { 
      $chkto = "You didn't specify receiver's email address.";
      $chkfrom = "You didn't specify sender's email address.";
      $chktitle = "Input email title";
      $chktext = "Input email content";
    }
    return ${$s};
  }

  function parse_lib($str) {
    # check debug
    $this->debug = trim($str['debug']) ? trim($str['debug']) : "";

    # check ofhtml
    $this->ofhtml = trim($str['ofhtml']) ? trim($str['ofhtml']) : "";

    # check attach
    $this->attach = trim($str['attach']) ? trim($str['attach']) : "";
    if ($this->attach) $this->generate_attach_lib();

    # check language
    $this->ln = trim($str['ln']) ? trim($str['ln']) : "en";
    if($this->ln == "ko") $this->charset = "EUC-KR";
    else $this->charset = "iso-8859-1";

    # check from
    if(!trim($str['from'])) print_error_lib($this->errmsg_lib("chkfrom",$this->ln),$this->ofhtml);
    else $this->from = trim($str['from']);

    if(preg_match("/<|>| /",$this->from)) {
      $this->from = preg_replace("/[ ]+/"," ",str_replace("<"," <",$this->from));
      $cfrom = explode(" ",$this->from);
      $cfrom[0] = "=?$this->charset?B?".trim(base64_encode(trim($cfrom[0])))."?=";
      $this->from = "$cfrom[0] $cfrom[1]";
    }

    if(!trim($str['to'])) print_error_lib($this->errmsg_lib("chkto",$this->ln),$this->ofhtml);
    else $this->to = trim($str['to']);

    # to 헤더 정리
    if(preg_match("/<|>| /i",$this->to)) {
      $this->to = str_replace("<"," <",$this->to);
      $this->to = str_replace(">","> ",$this->to);
      $this->to = trim(preg_replace("/[ ]+/"," ",$this->to));
      $cto = explode(",",$this->to);
      for($i=0;$i<sizeof($cto);$i++) {
        $toname = $tomail = "";
        $cto[$i] = trim($cto[$i]);
        if (preg_match("/<[^@]+@[^>]*>/i", $cto[$i])) {
          $toname = preg_replace("/<[^>]*>/i", "", $cto[$i]);
        }

        if ($toname) {
          $tomail = preg_replace("/.*(<[^>]*>).*/i", "\\1", $cto[$i]);
          $ccto .= "=?".$this->charset."?B?".trim(base64_encode($toname))."?= $tomail, ";
        } else $ccto .= "$cto[$i], ";
      }
      $this->to = preg_replace("/,$/","",trim($ccto));
    }

    if(!trim($str['title'])) print_error_lib($this->errmsg_lib("chktitle",$this->ln),$this->ofhtml);
    else $this->title = trim($str['title']);
    $this->title = "=?$this->charset?B?".trim(base64_encode($this->title))."?=";
    $this->title = preg_replace("/\n[ \t]*/si"," ",str_replace("\r\n","\n",$this->title));

    if(!trim($str['text'])) print_error_lib($this->errmsg_lib("chktext",$this->ln),$this->ofhtml);
    else $this->text = trim($str['text']);

    $this->htext = trim($str['htext']) ? trim($str['htext']) : "";
    if(!$this->htext)
      $this->htext = $this->html_to_plain_lib($this->text);

    if ($this->attachis) $this->attbound = "001";
    else $this->attbound = "000";

    $this->text = "----=_NextPart_". $this->attbound . $this->boundary . "\r\n".
                  "Content-Type: text/plain; charset=".$this->charset."\r\n".
                  "Content-Transfer-Encoding: base64\r\n\r\n".
                  $this->body_encode_lib($this->htext).
                  "\r\n\r\n----=_NextPart_".$this->attbound.$this->boundary."\r\n".
                  "Content-Type: text/html; charset=".$this->charset."\r\n".
                  "Content-Transfer-Encoding: base64\r\n\r\n".
                  $this->body_encode_lib($this->text).
                  "\r\n\r\n----=_NextPart_".$this->attbound.$this->boundary."--\r\n";

    $this->mail_header_lib();
    $this->text = $this->header.$this->text."\r\n";

    if ($this->attachis) {
      $this->text .= $this->atthead."\r\n----=_NextPart_000".$this->boundary."--\r\n";
    }
  }

  function mail_header_lib() {
    if(preg_match("/<|>/",$this->from)) {
      $msgID = trim(preg_replace("/.*<([^@]+)@.+/i","\\1",$this->from));
    } else {
      $msgID = trim(preg_replace("/([^@]+)@.+/i","\\1",$this->from));
    }
    if(!$msgID) $msgID = "OOPS_PHP_LIB";
    $msgID = $this->generate_mail_id(preg_replace("/@.+$/i","",$msgID));

    if($this->attachis) { $partmode = "mixed"; }
    else { $partmode = "alternative"; }

    $header = "Message-ID: <$msgID>\r\n".
              "From: ".$this->from."\r\n".
              "MIME-Version: 1.0\r\n".
              "Date: ".date ("r",time())."\r\n".
              "To: ".$this->to."\r\n".
              "Subject: ".$this->title."\r\n".
              "Content-Type: multipart/$partmode;\r\n".
              "              boundary=\"--=_NextPart_000".$this->boundary."\"\r\n\r\n\r\n".
              "This is a multi-part message in MIME format.\r\n\r\n";

    if($this->attachis) {
      $header .= "\r\n----=_NextPart_000" . $this->boundary .
                 "\r\nContent-Type: multipart/alternative;\r\n".
                 "              boundary=\"--=_NextPart_001".$this->boundary."\"\r\n\r\n\r\n";
    }

    $this->header = $header;
  }

  function generate_mail_id($uid) {
    $id = date("YmdHis",time());
    mt_srand((float) microtime() * 1000000);
    $randval = mt_rand();
    $id .= $randval."@$uid";
    return $id;
  }

  function make_boundary_lib() {
    $uniqchr = uniqid("");
    $one = strtoupper($uniqchr[0]);
    $two = strtoupper(substr($uniqchr,0,8));
    $three = strtoupper(substr(strrev($uniqchr),0,8));
    $this->boundary = "_000${one}_${two}.${three}";
  }

  function html_to_plain_lib($str) {
    $src = array( "/^.*<BODY[^>]*>/is", "/<\/BODY>.*$/is" );
    $des = array( "", "" );
    $str = strip_tags(preg_replace($src,$des,$str));
    $str = preg_replace("/(\r\n\r\n)(\r\n)*/i","\\1",$str);
    return $str;
  }

  function generate_attach_lib() {
    if (!file_exists($this->attach)) {
      echo "Can't open attach file '$this->attach' in read mode\n";
      exit;
    } else {
      $this->attachis = 1;

      $afile = readfile_lib($this->attach);
      $fname = substr(strrchr($this->attach, "/"), 1);
      $mimetype = $this->generate_mime_lib($fname);

      $entext = $this->body_encode_lib($afile);

      $this->atthead = "----=_NextPart_000" . $this->boundary .
                       "\r\nContent-Type: $mimetype; name=\"$fname\"\r\n".
                       "Content-Transfer-Encoding: base64\r\nContent-Disposition: inline; ".
                       "filename=\"$fname\"\r\n\r\n$entext\r\n";
    }
  }

  function body_encode_lib(&$str) {
    $return = base64_encode($str);
    $len = strlen($return);
    $chk = intval($len/60);

    for($i=1;$i<$chk+1;$i++) {
      if($i < 2) $no = $i*60-1;
      else {
        $pl = $pl + 2;
        $no = $i*60-1+$pl;
      }
      $return = substr_replace($return,"$return[$no]\r\n",$no,1);
    }

    return $return;
  }

  function generate_mime_lib($files) {
    $tails = substr( strrchr($files, "."), 1);

    if ($tails == "ez") { $mime = "application/andrew-inset"; }
    else if ($tails == "hqx") { $mime = "application/mac-binhex40"; }
    else if ($tails == "cpt") { $mime = "application/mac-compactpro"; }
    else if ($tails == "doc") { $mime = "application/msword"; }
    else if ($tails == "oda") { $mime = "application/oda"; }
    else if ($tails == "pdf") { $mime = "application/pdf"; }
    else if ($tails == "rtf") { $mime = "application/rtf"; }
    else if ($tails == "mif") { $mime = "application/vnd.mif"; }
    else if ($tails == "ppt") { $mime = "application/vnd.ms-powerpoint"; }
    else if ($tails == "slc") { $mime = "application/vnd.wap.slc"; }
    else if ($tails == "sic") { $mime = "application/vnd.wap.sic"; }
    else if ($tails == "wmlc") { $mime = "application/vnd.wap.wmlc"; }
    else if ($tails == "wmlsc") { $mime = "application/vnd.wap.wmlscriptc"; }
    else if ($tails == "bcpio") { $mime = "application/x-bcpio"; }
    else if ($tails == "bz2") { $mime = "application/x-bzip2"; }
    else if ($tails == "vcd") { $mime = "application/x-cdlink"; }
    else if ($tails == "pgn") { $mime = "application/x-chess-pgn"; }
    else if ($tails == "cpio") { $mime = "application/x-cpio"; }
    else if ($tails == "csh") { $mime = "application/x-csh"; }
    else if ($tails == "dvi") { $mime = "application/x-dvi"; }
    else if ($tails == "spl") { $mime = "application/x-futuresplash"; }
    else if ($tails == "gtar") { $mime = "application/x-gtar"; }
    else if ($tails == "hdf") { $mime = "application/x-hdf"; }
    else if ($tails == "js") { $mime = "application/x-javascript"; }
    else if ($tails == "ksp") { $mime = "application/x-kspread"; }
    else if ($tails == "kpr" || $tails == "kpt") { $mime = "application/x-kpresenter"; }
    else if ($tails == "chrt") { $mime = "application/x-kchart"; }
    else if ($tails == "kil") { $mime = "application/x-killustrator"; }
    else if ($tails == "skp" || $tails == "skd" || $tails == "skt" ||
             $tails == "skm") { $mime = "application/x-koan"; }
    else if ($tails == "latex") { $mime = "application/x-latex"; }
    else if ($tails == "nc" || $tails == "cdf") { $mime = "application/x-netcdf"; }
    else if ($tails == "rpm") { $mime = "application/x-rpm"; }
    else if ($tails == "sh") { $mime = "application/x-sh"; }
    else if ($tails == "shar") { $mime = "application/x-shar"; }
    else if ($tails == "swf") { $mime = "application/x-shockwave-flash"; }
    else if ($tails == "sit") { $mime = "application/x-stuffit"; }
    else if ($tails == "sv4cpio") { $mime = "application/x-sv4cpio"; }
    else if ($tails == "sv4crc") { $mime = "application/x-sv4crc"; }
    else if ($tails == "tar") { $mime = "application/x-tar"; }
    else if ($tails == "tcl") { $mime = "application/x-tcl"; }
    else if ($tails == "tex") { $mime = "application/x-tex"; }
    else if ($tails == "texinfo" || $tails == "texi") { $mime = "application/x-texinfo"; }
    else if ($tails == "t" || $tails == "tr" ||
             $tails == "roff") { $mime = "application/x-troff"; }
    else if ($tails == "man") { $mime = "application/x-troff-man"; }
    else if ($tails == "me") { $mime = "application/x-troff-me"; }
    else if ($tails == "ms") { $mime = "application/x-troff-ms"; }
    else if ($tails == "ustar") { $mime = "application/x-ustar"; }
    else if ($tails == "src") { $mime = "application/x-wais-source"; }
    else if ($tails == "zip") { $mime = "application/zip"; }
    else if ($tails == "gif") { $mime = "image/gif"; }
    else if ($tails == "ief") { $mime = "image/ief"; }
    else if ($tails == "wbmp") { $mime = "image/vnd.wap.wbmp"; }
    else if ($tails == "ras") { $mime = "image/x-cmu-raster"; }
    else if ($tails == "pnm") { $mime = "image/x-portable-anymap"; }
    else if ($tails == "pbm") { $mime = "image/x-portable-bitmap"; }
    else if ($tails == "pgm") { $mime = "image/x-portable-graymap"; }
    else if ($tails == "ppm") { $mime = "image/x-portable-pixmap"; }
    else if ($tails == "rgb") { $mime = "image/x-rgb"; }
    else if ($tails == "xbm") { $mime = "image/x-xbitmap"; }
    else if ($tails == "xpm") { $mime = "image/x-xpixmap"; }
    else if ($tails == "xwd") { $mime = "image/x-xwindowdump"; }
    else if ($tails == "css") { $mime = "text/css"; }
    else if ($tails == "rtx") { $mime = "text/richtext"; }
    else if ($tails == "rtf") { $mime = "text/rtf"; }
    else if ($tails == "tsv") { $mime = "text/tab-separated-values"; }
    else if ($tails == "sl") { $mime = "text/vnd.wap.sl"; }
    else if ($tails == "si") { $mime = "text/vnd.wap.si"; }
    else if ($tails == "wml") { $mime = "text/vnd.wap.wml"; }
    else if ($tails == "wmls") { $mime = "text/vnd.wap.wmlscript"; }
    else if ($tails == "etx") { $mime = "text/x-setext"; }
    else if ($tails == "xml") { $mime = "text/xml"; }
    else if ($tails == "avi") { $mime = "video/x-msvideo"; }
    else if ($tails == "movie") { $mime = "video/x-sgi-movie"; }
    else if ($tails == "wma") { $mime = "audio/x-ms-wma"; }
    else if ($tails == "wax") { $mime = "audio/x-ms-wax"; }
    else if ($tails == "wmv") { $mime = "video/x-ms-wmv"; }
    else if ($tails == "wvx") { $mime = "video/x-ms-wvx"; }
    else if ($tails == "wm") { $mime = "video/x-ms-wm"; }
    else if ($tails == "wmx") { $mime = "video/x-ms-wmx"; }
    else if ($tails == "wmz") { $mime = "application/x-ms-wmz"; }
    else if ($tails == "wmd") { $mime = "application/x-ms-wmd"; }
    else if ($tails == "ice") { $mime = "x-conference/x-cooltalk"; }
    else if ($tails == "ra") { $mime = "audio/x-realaudio"; }
    else if ($tails == "wav") { $mime = "audio/x-wav"; }
    else if ($tails == "png") { $mime = "image/png"; }
    else if ($tails == "asf" || $tails == "asx") { $mime = "video/x-ms-asf"; }
    else if ($tails == "html" || $tails == "htm") { $mime = "text/html"; }
    else if ($tails == "smi" || $tails == "smil") { $mime = "application/smil"; }
    else if ($tails == "gz" || $tails == "tgz") { $mime = "application/x-gzip"; }
    else if ($tails == "kwd" || $tails == "kwt") { $mime = "application/x-kword"; }
    else if ($tails == "kpr" || $tails == "kpt") { $mime = "application/x-kpresenter"; }
    else if ($tails == "au" || $tails == "snd") { $mime = "audio/basic"; }
    else if ($tails == "ram" || $tails == "rm") { $mime = "audio/x-pn-realaudio"; }
    else if ($tails == "pdb" || $tails == "xyz") { $mime = "chemical/x-pdb"; }
    else if ($tails == "tiff" || $tails == "tif") { $mime = "image/tiff"; }
    else if ($tails == "igs" || $tails == "iges") { $mime = "model/iges"; }
    else if ($tails == "wrl" || $tails == "vrml") { $mime = "model/vrml"; }
    else if ($tails == "asc" || $tails == "txt" || $tails == "php") { $mime = "text/plain"; }
    else if ($tails == "sgml" || $tails == "sgm") { $mime = "text/sgml"; }
    else if ($tails == "qt" || $tails == "mov") { $mime = "video/quicktime"; }
    else if ($tails == "ai" || $tails == "eps" || $tails == "ps") { $mime = "application/postscript"; }
    else if ($tails == "dcr" || $tails == "dir" || $tails == "dxr") { $mime = "application/x-director"; }
    else if ($tails == "mid" || $tails == "midi" || $tails == "kar") { $mime = "audio/midi"; }
    else if ($tails == "mpga" || $tails == "mp2" || $tails == "mp3") { $mime = "audio/mpeg"; }
    else if ($tails == "aif" || $tails == "aiff" || $tails == "aifc") { $mime = "audio/x-aiff"; }
    else if ($tails == "jpeg" || $tails == "jpg" || $tails == "jpe") { $mime = "image/jpeg"; }
    else if ($tails == "msh" || $tails == "mesh" || $tails == "silo") { $mime = "model/mesh"; }
    else if ($tails == "mpeg" || $tails == "mpg" || $tails == "mpe") { $mime = "video/mpeg"; }
    else { $mime = "application/octet-stream"; }

    return $mime;
  }
}

function mailsource_lib($ln,$from,$to,$subject,$body,$pbody="",$attach="") {
  $inputs['ln'] = trim($ln);
  $inputs['from'] = trim($from);
  $inputs['to'] = trim($to);
  $inputs['title'] = trim($subject);
  $inputs['text'] = $body;
  $inputs['htext'] = $pbody;
  $inputs['attach'] = $attach;
  $parse = new mailparse_lib($inputs);

  return $parse->text;
}

function sockmail_lib($source, $from, $to, $helohost = "", $debug = 0) {
  $inputs->debug = $debug;
  $inputs->ofhtml = 0;
  $inputs->from = trim($from);
  $inputs->to = trim($to);
  $inputs->text = trim($source);
  $inputs->helo = trim ($heloshost);

  $ret = new maildaemon_lib($inputs);

  return $ret->errno;
}

function sendmail_lib(&$mails,$ps=0) {
  if(!$ps) $parse = new mailparse_lib($mails);
  else $parse = $mails;
  new maildaemon_lib($parse);
}
?>
