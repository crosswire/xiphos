dnl
dnl GNOME_PLATFORM_GNOME_2(default, [force])
dnl
dnl   If the first parameter is `yes', then the default is
dnl   the GNOME 2.x platform, otherwise the GNOME 1.x one.
dnl
dnl   If the optional second parameter is `force', then use
dnl   the default value without command line argument.
dnl

AC_DEFUN([GNOME_PLATFORM_GNOME_2],[
	AC_REQUIRE([GNOME_REQUIRE_PKGCONFIG])

	if test x$1 = xyes ; then
	    platform_gnome_2_default=yes
	else
	    platform_gnome_2_default=no
	fi
	if test x$2 = xforce ; then
	    platform_gnome_2="$platform_gnome_2_default";
	else
	    AC_ARG_ENABLE(platform-gnome-2, [  --enable-platform-gnome-2 enable GNOME 2.x platform [default=no]],[platform_gnome_2="$enableval"],[platform_gnome_2="$platform_gnome_2_default"])
	fi

	AM_CONDITIONAL(PLATFORM_GNOME_2, test $platform_gnome_2 = yes)

	AC_MSG_CHECKING(for GNOME Platform)
	if test $platform_gnome_2 = yes; then
	    AC_MSG_RESULT(GNOME 2.x)
	    GNOME_INTERFACE_VERSION=2
	    GNOME_REQUIRE_PKGCONFIG
	else
	    AC_MSG_RESULT(GNOME 1.x)
	    GNOME_INTERFACE_VERSION=1
	fi
	AC_SUBST(GNOME_INTERFACE_VERSION)
])
