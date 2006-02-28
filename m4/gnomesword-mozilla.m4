dnl Check for Mozilla
dnl taken from the gtkmozedit project

AC_DEFUN([GS_CHECK_MOZILLA], [

mozilla_libs=""
mozilla_includes=""
AC_ARG_WITH(mozilla-libs,
	[  --with-mozilla-libs=dir     Set mozilla 'lib' place ],
	[ mozilla_libs="$withval" ])
AC_ARG_WITH(mozilla-includes,
	[  --with-mozilla-includes=dir Set mozilla 'include' place ],
	[ mozilla_includes="$withval" ])
AC_ARG_WITH(mozilla-home,
	[  --with-mozilla-home=dir Where mozilla libs are at runtime ],
	[ MOZILLA_HOME="$withval" ])
AC_ARG_WITH(firefox,
	[  --with-firefox Build against firefox ],
	[ use_firefox="yes" ])
AC_ARG_WITH(seamonkey,
	[  --with-seamonkey Build against seamonkey ],
	[ use_seamonkey="yes" ])

if test "x$mozilla_includes" != "x" ; then
    dnl Don't detect mozilla, use given version
    if test "x$mozilla_libs" = "x" ; then
        AC_MSG_ERROR([Must specify both --with-mozilla-includes and --with-mozilla-libs])
    fi
    MOZILLA_CFLAGS="-I$mozilla_includes -I$mozilla_includes/xpcom -I$mozilla_includes/string -I$mozilla_includes/nspr"

    MOZILLA_LIBS="-L$mozilla_libs -lxpcom -lplds4 -lnspr4 -lplc4"
    case "${host}" in
      *-*-freebsd* )
          MOZILLA_LIBS="$MOZILLA_LIBS -pthread"
      ;;
      * )
          MOZILLA_LIBS="$MOZILLA_LIBS -lpthread -ldl"
      ;;
    esac

    MOZILLA_CFLAGS="$MOZILLA_CFLAGS -I$mozilla_includes/gtkembedmoz"
    MOZILLA_LIBS="$MOZILLA_LIBS -lgtkembedmoz -lgtksuperwin"
    LIZARD=mozilla
else
    if test "$use_firefox" = "yes" ; then
        PKG_CHECK_MODULES([MOZILLA], 
            [firefox-nspr >= 1.0 firefox-xpcom])
        mozilla_includes=`$PKG_CONFIG --variable=includedir firefox-xpcom`
        mozilla_libs=`$PKG_CONFIG --variable=libdir firefox-xpcom`

        PKG_CHECK_MODULES([MOZILLA_GTK], [firefox-gtkmozembed >= 1.0])
        MOZILLA_CFLAGS="$MOZILLA_CFLAGS $MOZILLA_GTK_CFLAGS"
        MOZILLA_LIBS="$MOZILLA_LIBS $MOZILLA_GTK_LIBS"
        LIZARD=firefox
	GTKMOZEMBED_VERSION=`pkg-config --modversion firefox-gtkmozembed`
    elif test "$use_seamonkey" = "yes" ; then
        PKG_CHECK_MODULES([MOZILLA], 
            [seamonkey-nspr >= 4.0 seamonkey-xpcom])
        mozilla_includes=`$PKG_CONFIG --variable=includedir seamonkey-xpcom`
        mozilla_libs=`$PKG_CONFIG --variable=libdir seamonkey-xpcom`

        PKG_CHECK_MODULES([MOZILLA_GTK], [seamonkey-gtkmozembed >= 1.0])
        MOZILLA_CFLAGS="$MOZILLA_CFLAGS $MOZILLA_GTK_CFLAGS"
        MOZILLA_LIBS="$MOZILLA_LIBS $MOZILLA_GTK_LIBS"
        LIZARD=seamonkey
	GTKMOZEMBED_VERSION=`pkg-config --modversion seamonkey-gtkmozembed`
    else
        PKG_CHECK_MODULES([MOZILLA], 
            [mozilla-nspr >= 1.3 mozilla-xpcom])
        mozilla_includes=`$PKG_CONFIG --variable=includedir mozilla-xpcom`
        mozilla_libs=`$PKG_CONFIG --variable=libdir mozilla-xpcom`

        PKG_CHECK_MODULES([MOZILLA_GTK], [mozilla-gtkmozembed >= 1.3])
        MOZILLA_CFLAGS="$MOZILLA_CFLAGS $MOZILLA_GTK_CFLAGS"
        MOZILLA_LIBS="$MOZILLA_LIBS $MOZILLA_GTK_LIBS"
        LIZARD=mozilla
	GTKMOZEMBED_VERSION=`pkg-config --modversion mozilla-gtkmozembed`
    fi
fi
GTKMOZEMBED_VERSION=${GTKMOZEMBED_VERSION:0:3} # TODO improve extraction of x.y
GTKMOZEMBED_VERSION=${GTKMOZEMBED_VERSION//./_}
GTKMOZEMBED_VERSION=${GTKMOZEMBED_VERSION//[[:alpha:]]/}
GTKMOZEMBED_VERSION=GTKMOZEMBED_VERSION_$GTKMOZEMBED_VERSION
AC_SUBST(GTKMOZEMBED_VERSION)
AC_SUBST(LIZARD)

dnl Additional mozilla settings:
if test "x$MOZILLA_HOME" = "x"; then
    MOZILLA_HOME=$mozilla_libs
fi
MOZILLA_CFLAGS="$MOZILLA_CFLAGS -I$mozilla_includes"
for subdir in unix dom windowwatcher webbrowserpersist appcomps find txtsvc \
              docshell shistory webshell gfx necko widget content layout \
              browser webbrwsr uconv embed_base uriloader editor htmlparser \
              commandhandler composer pref profdirserviceprovider unicharutil ; do
    MOZILLA_CFLAGS="$MOZILLA_CFLAGS -I$mozilla_includes/$subdir"
done
MOZILLA_LIBS="$MOZILLA_LIBS -Wl,-rpath,$MOZILLA_HOME"
AC_SUBST(MOZILLA_LIBS)
AC_SUBST(MOZILLA_CFLAGS)
AC_SUBST(MOZILLA_HOME)
AC_DEFINE(USE_GTKMOZEMBED, 1, [Define if you want to use gtkmozembed])

])
