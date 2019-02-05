<?php
# 이미지 사이즈를 조정하는 함수
# Usage : ImgResize_lib("original image path","image type","new width",'new height","newpath");
# image type -> create new image as jpeg or gif or wbmp or png type
#
function imgresize_lib ($original, $type = 'jpg', $width = 50, $height = 50, $newpath = null) {
	# gd extension 이 load 되었는지 확인
	if ( ! extension_loaded ('gd') ) {
		trigger_error ('imgresize_lib api required gd extension', E_USER_ERROR);
	}

	if ( ! strlen ($original) || ! file_exists ($original) )
		return false;

	# 원본 이미지로 부터 JPEG 파일을 생성
	$otype = getimagesize ($original);
	switch ($otype[2]) {
		case 1:
			$img = ImageCreateFromGIF ($original);
			break;
		case 2:
			$img = ImageCreateFromJPEG ($original);
			break;
		case 3:
			$img = ImageCreateFromPNG ($original);
			break;
		default:
			trigger_error ('support only GIF, JPG and PNG format', E_USER_WARNING);
			return false;
	}

	# 원본 이미지의 width, height 를 구함
	$owidth = ImagesX ($img);
	$oheight = ImagesY ($img);

	# width 와 height 를 모두 0 으로 주었을 경우 기본값 50
	if ( ! $width && ! $height)
		$width = $height = 50;

	# width 가 없을 경우 height 의 축소/확대 비율로 width 를 구함
	if ( ! $width ) {
		$ratio = (real) ($height / $oheight);
		$width = (int) ($owidth * $ratio);
	}

	# height 가 없을 경우 width 의 축소/확대 비율로 height 를 구함
	if( ! $height ) {
		$ratio = (real) ($width / $owidth);
		$height = (int) ($oheight * $ratio);
	}

	# 새로운 이미지를 생성
	if ( function_exists ('imagecreatetruecolor') ) {
		$newimg = imagecreatetruecolor ($width, $height);
		# 새로운 이미지에 원본 이미지를 사이즈 조정하여 복사.
		ImageCopyResampled ($newimg, $img, 0, 0, 0, 0, $width, $height, $owidth, $oheight);
	} else {
		$newimg = ImageCreate ($width, $height);
		# 새로운 이미지에 원본 이미지를 사이즈 조정하여 복사.
		ImageCopyResized ($newimg, $img, 0, 0, 0, 0, $width, $height, $owidth, $oheight);
	}

	# 타입에 따라 헤더를 출력
	switch ($type) {
		case "wbmp" :
			$type_header = "vnd.wap.wbmp";
			break;
		default :
			$type = ($type == "jpg") ? "jpeg" : $type;
			$type_header = $type;
	}
	if ( ! $newpath ) Header( "Content-type: image/{$type_header}");

	switch ($type) {
		case "png" :
			if ( $newpath ) ImagePNG ($newimg, $newpath);
			else ImagePNG ($newimg);
			break;
		case "wbmp" :
			if ( $newpath ) ImageWBMP ($newimg, $newpath);
			else ImageWBMP ($newimg);
			break;
		case "gif" :
			if ( $newpath ) ImageGIF ($newimg, $newpath);
			else ImageGIF ($newimg);
		default :
			ImageJPEG ($newimg, $newpath, -1);
	}
	ImageDestroy ($newimg);
	if ( $dbchk && file_exists ($original) ) unlink ($original);
}
?>
