dnl config.m4 for extension korean

dnl Comments in this file start with the string 'dnl'.
dnl Remove where necessary. This file will not work
dnl without editing.

dnl If your extension references something external, use with:

PHP_ARG_ENABLE(korean, whether to enable korean support,
[  --enable-korean         Enable korean support])

if test "$PHP_KOREAN" != "no"; then
  AC_DEFINE(HAVE_KOREAN,1,[ ])

  OOPS_PARAMETER="$CFLAGS -Wall -Iext"
  PHP_SUBST(CPPFLAGS)

  AC_MSG_CHECKING(whether to enable gd functoin)

  AC_ARG_ENABLE(korean-gd, [  --enable-korean-gd      Enable external gd functoin [ default=buildin ] ],[
    if test "$enable_korean_gd" != "no" ; then
      AC_MSG_RESULT(external)
    else
      AC_MSG_RESULT(builtin)
    fi ], [ AC_MSG_RESULT(builtin) ])

  if test -z "$PHP_JPEG_DIR"; then
    PHP_ARG_WITH(jpeg-dir, for the location of libjpeg,
    [  --with-jpeg-dir[=DIR]     Set the path to libjpeg install prefix.], "/usr", no)
  fi

  if test -z "$PHP_PNG_DIR"; then
    PHP_ARG_WITH(png-dir, for the location of libpng,
    [  --with-png-dir[=DIR]      Set the path to libpng install prefix.], "/usr", no)
  fi

  if test -z "$PHP_ZLIB_DIR"; then
    PHP_ARG_WITH(zlib-dir, for the location of libz,
    [  --with-zlib-dir[=DIR]     Set the path to libz install prefix.], "/usr", no)
  fi

  AC_DEFINE(KOEAN_GD, 1, [ ])

  if test -z "$enable_korean_gd" ; then
    enable_korean_gd=no
  fi

  if test -z "$with_png_dir"; then
    with_png_dir=no
  fi

  if test -z "$with_zlib_dir"; then
    with_zlib_dir=no
  fi

  if test "$with_libdir" != "$PHP_LIBDIR"; then
    parch=`uname -m`
    if test "$parch" == "x86_64"; then
        if test -d /usr/lib64 || test -d /usr/local/lib64; then
          PHP_LIBDIR="lib64"
        fi
    fi
  fi
  test -z "$PHP_LIBDIR" && PHP_LIBDIR="lib"

  dnl In this case given --enable-korean-gd option
  dnl
  if test "$enable_korean_gd" != "no" ; then
    if test "$enable_korean_gd" = "yes"; then
      GD_SEARCH_PATHS="/usr/local /usr"
    else
      GD_SEARCH_PATHS=$enable_korean_gd
    fi

    for j in $GD_SEARCH_PATHS; do
      for i in include include/gd1.3 include/gd include gd1.3 gd ""; do
        test -f $j/$i/gd.h && KOREAN_INCLUDE=$j/$i
      done

      for i in $PHP_LIBDIR $PHP_LIBDIR/gd1.3 $PHP_LIBDIR/gd $PHP_LIBDIR gd1.3 gd ""; do
        test -f $j/$i/libgd.$SHLIB_SUFFIX_NAME -o -f $j/$i/libgd.a && KOREAN_LIB=$j/$i
      done
    done

    if test -n "$KOREAN_INCLUDE" -a -n "$KOREAN_LIB" ; then
      PHP_ADD_LIBRARY_WITH_PATH(gd, $KOREAN_LIB, KOREAN_SHARED_LIBADD)
    else
      AC_MSG_ERROR([Unable to find libgd.(a|so) anywhere under $GD_SEARCH_PATHS])
    fi 

    PHP_CHECK_LIBRARY(gd, gdImageCreateTrueColor,[AC_DEFINE(HAVE_GD2, 1,[ ])],[],[ -L$KOREAN_LIB $KOREAN_SHARED_LIBADD ])
    PHP_EXPAND_PATH($KOREAN_INCLUDE, KOREAN_INCLUDE)
    PHP_ADD_INCLUDE($KOREAN_INCLUDE)
  else

    dnl
    dnl In this case, don't be given --enable-korean-gd option.
    dnl This case use built in gd library.
    dnl

    dnl case of static build
    if test "$ext_shared" = "no"; then
      if test "$PHP_GD" = "no"; then
        include_gdlib="yes"
      else
        include_gdlib="no"
      fi
    dnl case of dynamic build
    else
      dnl if exists gd bundle library?
      AC_MSG_CHECKING(check gd bundle library for korean extension)
      if test "$PHP_GD" = "no"; then
        AC_MSG_RESULT(needless)
      elif test -d "ext/gd/libgd" ; then
        pushd ext/korean > /dev/null 2>&1
        if test ! -L "./libgd"; then
          mv libgd needless.libgd
          ln -sf ../gd/libgd ./libgd
          AC_MSG_RESULT(link complete)
        else
          AC_MSG_RESULT(already linked)
        fi
        popd > /dev/null 2>&1
      else
        AC_MSG_RESULT(needless)
        AC_DEFINE(HAVE_GD_BUNDLED,1,[ ])
      fi
      include_gdlib="yes"
    fi

    dnl JPEG Libaray Check
    for i in $with_jpeg_dir /usr/local /usr; do
      test -f $i/$PHP_LIBDIR/libjpeg.$SHLIB_SUFFIX_NAME -o -f $i/$PHP_LIBDIR/libjpeg.a && KR_JPEG_DIR=$i && break
    done


    if test -z "$KR_JPEG_DIR" ; then
      AC_MSG_ERROR([libjpeg.(a|so) not found.])
    fi

    PHP_CHECK_LIBRARY(jpeg,jpeg_read_header,
    [
      PHP_ADD_INCLUDE($KR_JPEG_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(jpeg, $KR_JPEG_DIR/$PHP_LIBDIR, KOREAN_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libjpeg.(a|so). Please check config.log for more information.]) 
    ],[
      -L$KR_JPEG_DIR/$PHP_LIBDIR
    ])

    dnl PNG Libaray Check
    for i in $with_png_dir /usr/local /usr; do
      test -f $i/$PHP_LIBDIR/libpng.$SHLIB_SUFFIX_NAME -o -f $i/$PHP_LIBDIR/libpng.a && KR_PNG_DIR=$i && break
    done

    if test -z "$KR_PNG_DIR" ; then
      AC_MSG_ERROR([libpng.(a|so) not found.])
    fi

    if test ! -f $KR_PNG_DIR/include/png.h; then
      AC_MSG_ERROR([png.h not found.])
    fi

    if test ! -f $KR_PNG_DIR/include/png.h; then
      AC_MSG_ERROR([png.h not found.])
    fi

    dnl ZLIB Libaray Check
    if test "$PHP_ZLIB_DIR" = "no"; then
      for i in /usr/local /usr $PHP_ZLIB_DIR; do
        test -f $i/$PHP_LIBDIR/libz.$SHLIB_SUFFIX_NAME -o -f $i/$PHP_LIBDIR/libz.a && KR_ZLIB_DIR=$i && break
      done

      if test -z "$KR_ZLIB_DIR"; then
        AC_MSG_ERROR([PNG support requires ZLIB. Use --with-zlib-dir=<DIR>])
      fi

      if test -z "$KR_ZLIB_DIR" ; then
        AC_MSG_ERROR([libz.(a|so) not found.])
      fi

      if test ! -f $KR_ZLIB_DIR/include/png.h; then
        AC_MSG_ERROR([zlib.h not found.])
      fi

      if test ! -f $KR_ZLIB_DIR/include/png.h; then
        AC_MSG_ERROR([zlib.h not found.])
      fi
    else
      KR_ZLIB_DIR=$PHP_ZLIB_DIR

      if test -z "$KR_ZLIB_DIR" ; then
        AC_MSG_ERROR([libz.(a|so) not found.])
      fi

      if test ! -f $KR_ZLIB_DIR/include/png.h; then
        AC_MSG_ERROR([zlib.h not found.])
      fi

      if test ! -f $KR_ZLIB_DIR/include/png.h; then
        AC_MSG_ERROR([zlib.h not found.])
      fi
    fi

    PHP_CHECK_LIBRARY(png,png_write_image,
    [
      PHP_ADD_INCLUDE($KR_PNG_DIR/include)
      PHP_ADD_LIBRARY_WITH_PATH(z, $KR_ZLIB_DIR/$PHP_LIBDIR, KOREAN_SHARED_LIBADD)
      PHP_ADD_LIBRARY_WITH_PATH(png, $KR_PNG_DIR/$PHP_LIBDIR, KOREAN_SHARED_LIBADD)
    ],[
      AC_MSG_ERROR([Problem with libpng.(a|so) or libz.(a|so). Please check config.log for more information.]) 
    ],[
      -L$KR_ZLIB_DIR/$PHP_LIBDIR -lz -L$KR_PNG_DIR/$PHP_LIBDIR
    ])

    KR_MODULE_TYPE=builtin
    if test "$include_gdlib" = "yes"; then
      krextra_sources="libgd/gd.c libgd/gd_png.c libgd/gd_jpeg.c libgd/gd_gif_in.c libgd/gd_io.c \
                       libgd/gd_gif_out.c libgd/gd_io_file.c libgd/gd_ss.c libgd/gd_io_ss.c \
                       libgd/gdtables.c libgd/gdhelpers.c libgd/gd_io_dp.c libgd/gd_topal.c \
                       libgd/gd_security.c"
    fi

    dnl These are always available with bundled library
    AC_DEFINE(KRGD_BUILTIN,             1, [ ])
    AC_DEFINE(HAVE_GD2,                 1, [ ])
    AC_DEFINE(HAVE_LIBPNG,              1, [ ])
    AC_DEFINE(HAVE_LIBJPEG,             1, [ ])

    if test "$include_gdlib" = "yes"; then
      KOREAN_LIB="./libgd"
      KR_PARAMETER="$OOPS_PARAMETER -I./libgd"
      PHP_ADD_BUILD_DIR(./libgd)
    else
      KR_PARAMETER="$OOPS_PARAMETER"
    fi

    dnl PHP_ADD_LIBRARY_WITH_PATH(gd, $KOREAN_LIB, KOREAN_SHARED_LIBADD)
    PHP_EXPAND_PATH($KOREAN_INCLUDE, KOREAN_INCLUDE)
    PHP_ADD_INCLUDE($KOREAN_INCLUDE)
    PHP_SUBST(KR_PARAMETER)
  fi

  AC_DEFINE(HAVE_KRLIBGD,1,[ ])
  PHP_SUBST(KOREAN_SHARED_LIBADD)

  dnl if php version is under 4.2.x, use PHP_EXTENSION
  dnl bug over php 4.2.x, use PHP_NEW_EXTENSION
  dnl PHP_EXTENSION(korean, $ext_shared)
  PHP_NEW_EXTENSION(korean, krcharset.c krerror.c krimage.c krmath.c krparse.c korean.c krcheck.c krfile.c krmail.c krnetwork.c krregex.c $krextra_sources, $ext_shared,, \\$(KR_PARAMETER))
fi
