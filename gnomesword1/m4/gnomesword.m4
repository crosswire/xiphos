
dnl 
dnl code from Evolution configure.in
dnl
AC_DEFUN(GS_CHECK_LIB, [
	dispname="$1"
	pkgname="$2"
	minvers="$3"
	maxvers="$4"

	AC_MSG_CHECKING(for $dispname)

	if gnome-config --libs $pkgname > /dev/null 2>&1; then
		pkgvers=`gnome-config --modversion $pkgname | sed -e 's/^[[^0-9]]*//'`
	else
		pkgvers=not
	fi
	AC_MSG_RESULT($pkgvers found)

	pkgvers=`echo $pkgvers | awk -F. '{ print $[]1 * 1000000 + $[]2 * 10000 + $[]3 * 100 + $[]4;}'`
	cmpminvers=`echo $minvers | awk -F. '{ print $[]1 * 1000000 + $[]2 * 10000 + $[]3 * 100 + $[]4;}'`
	cmpmaxvers=`echo $maxvers | awk -F. '{ print $[]1 * 1000000 + $[]2 * 10000 + $[]3 * 100 + $[]4;}'`
	ok=yes
	if test "$pkgvers" -lt $cmpminvers; then
		ok=no
	elif test -n "$maxvers"; then
		if test "$pkgvers" -gt $cmpmaxvers; then
			ok=no
		elif test "$maxvers" != "$minvers" -a "$cmpmaxvers" -eq "$pkgvers"; then
			ok=no
		fi
	fi
	if test $ok = no; then
		case $maxvers in
		"$(top_builddir)/src")
			dispvers="$minvers or higher"
			;;
		$minvers)
			dispvers="$minvers (exactly)"
			;;
		*)
			dispvers="$minvers or higher, but less than $maxvers,"
			;;
		esac

		AC_MSG_ERROR([
""
"You need $dispname $dispvers to build GnomeSword"
"If you think you already have this installed, consult the README."])
	fi
])


dnl ------------------------------------------------------------------------
dnl Find a file (or one of more files in a list of dirs)
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN(AC_FIND_FILE,
[
$3=NO
for i in $2; do
  for j in $1; do
    if test -r "$i/$j"; then
      $3=$i
      break 2
    fi
  done
done
])


dnl taken from BibleTime

AC_DEFUN(AC_FIND_ZLIB,
[
AC_MSG_CHECKING([for libz])
AC_CACHE_VAL(ac_cv_lib_z,
[
AC_LANG_C
kde_save_LIBS="$LIBS"
LIBS="$all_libraries $USER_LDFLAGS -lz $LIBSOCKET"
kde_save_CFLAGS="$CFLAGS"
CFLAGS="$CFLAGS $all_includes $USER_INCLUDES"
AC_TRY_LINK(dnl
[
#include<zlib.h>
],
            [return (zlibVersion() == ZLIB_VERSION); ],
            eval "ac_cv_lib_z='-lz'",
            eval "ac_cv_lib_z=no")
LIBS="$kde_save_LIBS"
CFLAGS="$kde_save_CFLAGS"
])dnl
if test ! "$ac_cv_lib_z" = no; then
  AC_DEFINE_UNQUOTED(HAVE_LIBZ, 1, [Define if you have libz])
  LIBZ="$ac_cv_lib_z"
  AC_SUBST(LIBZ)
  AC_MSG_RESULT($ac_cv_lib_z)
else
  AC_MSG_ERROR(not found. Check your installation and look into config.log)
  LIBZ=""
  AC_SUBST(LIBZ)
fi
])


dnl This file was created by Joachim Ansorg <joachim@ansorgs.de>
dnl It provides macord for the autoconf package to find the Sword library on your system.

dnl ----------------------------------------------------------------------
dnl		Check wheter to use static linking
dnl		first parameter is the required version
dnl		second is whether to use static sword library
dnl ----------------------------------------------------------------------
AC_DEFUN(AC_CHECK_SWORD,
[
dnl AC_MSG_CHECKING([for a Sword installation])

dnl The option for the configure script
AC_ARG_WITH(sword-dir,
[  --with-sword-dir=DIR     Path where Sword is being installed (default=/usr) ],
[
  ac_sword_dir=$withval
],ac_sword_dir=/usr
)

AC_ARG_ENABLE(static-sword,
[  --enable-static-sword    Link to the static Sword library],
 	ac_static_sword="YES",
 	[ ac_static_sword="$2" ]
)

dnl try to find Sword library files
AC_MSG_CHECKING([for Sword library files])
AC_REQUIRE([AC_FIND_ZLIB])
ac_sword_library_dirs="$ac_sword_dir/lib /usr/lib /usr/lib/sword /usr/local/lib /usr/local/lib/sword /usr/local/sword/lib"

if test "x$ac_static_sword" = "xYES"; then
	SEARCH_LIBS="libsword.a";
else
	SEARCH_LIBS="libsword.a libsword.so";
fi


AC_CACHE_VAL(ac_cv_sword_libdir, AC_FIND_FILE($SEARCH_LIBS, $ac_sword_library_dirs, ac_cv_sword_libdir))

if test "x$ac_cv_sword_libdir" = "xNO"; then
  AC_MSG_ERROR(SWORD library not found. Try to use configure with --with-sword-dir=/your/SWORD/path!);
fi

if test "x$ac_static_sword" = "xYES"; then
	LIB_SWORD="$ac_cv_sword_libdir/libsword.a";
else
	LIB_SWORD="-lsword";
fi

AC_SUBST(SWORD_LIBRARY_PATH)
AC_SUBST(LIB_SWORD)
all_libraries="$all_libraries -L$ac_cv_sword_libdir"

if test "x$ac_static_sword" = "xYES"; then
	MESSAGE="static library $ac_cv_sword_libdir/libsword.a";
else
	MESSAGE="$ac_cv_sword_libdir";
fi
AC_MSG_RESULT([$MESSAGE])

dnl -- try to find Swords include files --
AC_MSG_CHECKING([for Sword include files])
ac_sword_include_dirs="$ac_sword_dir/include/sword $ac_sword_dir/include /usr/include/sword /usr/include /usr/local/include/sword /usr/local/include /usr/local/sword/include /usr/local/sword/include/sword"

AC_CACHE_VAL(ac_cv_sword_incdir, AC_FIND_FILE(swmgr.h, $ac_sword_include_dirs, ac_cv_sword_incdir))

if test "x$ac_cv_sword_incdir" = "xNO"; then
	AC_MSG_ERROR([The Sword include file files were not found.
Please try to use configure with --with-sword-dir=/your/SWORD/path !
])
fi

SWORD_INCLUDES="-I$ac_cv_sword_incdir"
AC_SUBST(SWORD_INCLUDES)

AC_MSG_RESULT([$ac_cv_sword_incdir])
])

dnl -------------------------------------------------------------------------------
dnl		Check wheter to use static linking, first parameter is the result (YES/NO)
dnl -------------------------------------------------------------------------------
AC_DEFUN(AC_CHECK_STATIC_LINKING,
[
$1="NO"

AC_MSG_CHECKING(whether to use static linking)
AC_ARG_ENABLE(static-linking,
      [  --enable-static-linking	use static linking],
      USE_STATIC_LINKING=$enableval, USE_STATIC_LINKING="no")

if test "$USE_STATIC_LINKING" = "yes"; then
dnl	BT_LDFLAGS="-all-static -Wl,-Bstatic";
dnl	AC_SUBST(BT_LDFLAGS)
dnl	CPPFLAGS="-DSTATIC_BUILD $CPPFLAGS";
	$1="YES";
else
	$1="NO";
fi

AC_MSG_RESULT($USE_STATIC_LINKING)
])
