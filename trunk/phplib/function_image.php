<?
# �̹��� ����� �����ϴ� �Լ�
# Usage : ImgResize_lib("original image path","image type","new width",'new height","newpath");
# image type -> create new image as jpeg or gif or wbmp or png type
#
function ImgResize_lib($original,$type="jpg",$width=0,$height=0,$newpath="") {
  # gd extension �� load �Ǿ����� Ȯ��
  if(!extension_loaded("gd")) {
    echo "ImgResize_lib() funtion required gd extension in PHP";
    exit;
  }

  # gd versoin Ȯ��
  ob_start();
  phpinfo(8);
  $gdinfo = ob_get_contents();
  ob_end_clean();
  $gdinfo = preg_replace("/.*(GD Version[^\n]*)\n.*/s","\\1",$gdinfo);
  if (preg_match("/2\.[0-9]/", $gdinfo)) $gdver = 2;
  else $gdver = 1;

  # ���� �̹����� ���� JPEG ������ ����
  $otype = GetImageSize($original);
  switch($otype[2]) {
    case 1:
      $img = ImageCreateFromGIF($original);
      break;
    case 2:
      $img = ImageCreateFromJPEG($original);
      break;
    case 3:
      $img = ImageCreateFromPNG($original);
      break;
    default:
      print_error_lib("Enable original file is type of GIF,JPG,PNG");
  }

  # ���� �̹����� width, height �� ����
  $owidth = ImagesX($img);
  $oheight = ImagesY($img);

  # width �� height �� ��� 0 ���� �־��� ��� �⺻�� 50
  if(!$width && !$height) $width = $height = 50;

  # width �� ���� ��� height �� ���/Ȯ�� ������ width �� ����
  if(!$width) {
    $ratio = ((real)$height/$oheight);
    $width = ((int)$owidth*$ratio);
  }

  # height �� ���� ��� width �� ���/Ȯ�� ������ height �� ����
  if(!$height) {
    $ratio = ((real)$width/$owidth);
    $height = ((int)$oheight*$ratio);
  }

  # ���ο� �̹����� ����
  if ($gdver == 2) {
    $newimg = imagecreatetruecolor($width,$height);
  } else {
    $newimg = ImageCreate($width,$height);
  }
  # ���ο� �̹����� ���� �̹����� ������ �����Ͽ� ����.
  if ($gdver == 2) {
    ImageCopyResampled($newimg,$img,0,0,0,0,$width,$height,$owidth,$oheight);
  } else {
    ImageCopyResized($newimg,$img,0,0,0,0,$width,$height,$owidth,$oheight);
  }

  # Ÿ�Կ� ���� ����� ���
  switch($type) {
    case "wbmp" :
      $type_header = "vnd.wap.wbmp";
      break;
    default :
      $type = ($type == "jpg") ? "jpeg" : $type;
      $type_header = $type;
  }
  if(!$newpath) Header("Content-type: image/$type_header");

  switch($type) {
    case "png" :
      if($newpath) ImagePNG($newimg,$newpath);
      else ImagePNG($newimg);
      break;
    case "wbmp" :
      if($newpath) ImageWBMP($newimg,$newpath);
      else ImageWBMP($newimg);
      break;
    case "gif" :
      if($newpath) ImageGIF($newimg,$newpath);
      else ImageGIF($newimg);
    default :
      ImageJPEG($newimg,$newpath,-1);
  }
  ImageDestroy($newimg);
  if($dbchk && file_exists($original)) unlink($original);
}
?>
