dnl
dnl GNOME_PILOT_HOOK(script if found, fail)
dnl if fail = "failure", abort if gnome-pilot not found
dnl

dnl
dnl 

GNOME_PILOT_CFLAGS=
GNOME_PILOT_LIBS=
PISOCK_CFLAGS=
PISOCK_LIBS=

AC_SUBST(GNOME_PILOT_CFLAGS)
AC_SUBST(GNOME_PILOT_LIBS)
AC_SUBST(PISOCK_CFLAGS)
AC_SUBST(PISOCK_LIBS)

AC_DEFUN([PILOT_LINK_HOOK],[
	AC_ARG_WITH(pisock,
	[  --with-pisock            Specify prefix for pisock files],[
	if test x$withval = xyes; then
	    dnl Note that an empty true branch is not valid sh syntax.
	    ifelse([$1], [], :, [$1])
	else
	    PISOCK_CFLAGS="-I$withval/include"
	    incdir="$withval/include"
	    PISOCK_LIBS="-L$withval/lib -lpisock"
	    AC_MSG_CHECKING("for existance of $withval/lib/libpisock.so")
	    if test -r $withval/lib/libpisock.so; then
		AC_MSG_RESULT(yes)
	    else
		AC_MSG_ERROR([Unable to find libpisock. Try  http://www.pilot-link.org.])
	    fi
	fi
	])

	if test x$PISOCK_CFLAGS = x; then
	    AC_CHECK_HEADER(pi-version.h, [incdir="/usr/include"], [
	    AC_CHECK_HEADER(libpisock/pi-version.h, [PISOCK_CFLAGS="-I/usr/include/libpisock"
	                                             piversion_include="libpisock/pi-version.h"
						     incdir="/usr/include/libpisock"
                                                    ], [
	    AC_CHECK_HEADER($prefix/include/pi-version.h, [PISOCK_CFLAGS="-I$prefix/include/libpisock"
	                                                   piversion_include="$prefix/include/pi-version.h"
						           if test x$PISOCK_LIBDIR = x; then
							      incdir="$prefix/include"
							      PISOCK_LIBS="-L$prefix/lib -lpisock"
                                                           fi							  ],
	    AC_MSG_ERROR([Unable to find pi-version.h])) 
	    ])
	    ])
	fi
		
	if test "x$PISOCK_LIBS" = "x"; then
		AC_CHECK_LIB(pisock, pi_accept, [ PISOCK_LIBS=-lpisock ], 
			[ AC_MSG_ERROR([Unable to find libpisock. Try http://www.pilot-link.org.]) ])
	fi
	
	AC_ARG_ENABLE(pilotlinktest,
		[  --enable-pilotlinktest   Test for correct version of pilot-link],
		[testplversion=$enableval],
		[ testplversion=yes ]
	)

	if test x$piversion_include = x; then
		piversion_include="pi-version.h"
	fi

	pi_version=`cat $incdir/pi-version.h|grep PILOT_LINK_VERSION|sed 's/#define PILOT_LINK_VERSION \([[0-9]]*\)/\1/'`
	pi_major=`cat $incdir/pi-version.h|grep PILOT_LINK_MAJOR|sed 's/#define PILOT_LINK_MAJOR \([[0-9]]*\)/\1/'`
	pi_minor=`cat $incdir/pi-version.h|grep PILOT_LINK_MINOR|sed 's/#define PILOT_LINK_MINOR \([[0-9]]*\)/\1/'`
	pi_patch=`cat $incdir/pi-version.h|grep PILOT_LINK_PATCH|sed 's/#define PILOT_LINK_PATCH \"\(.*\)\"/\1/'`

	PILOT_LINK_VERSION="$pi_version.$pi_major.$pi_minor$pi_patch"

	if test x$testplversion = xyes; then
		AC_MSG_CHECKING([for pilot-link version >= $1])
		pl_ve=`echo $1|sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
		pl_ma=`echo $1|sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
		pl_mi=`echo $1|sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
		CFLAGS_save="$CFLAGS"
		CFLAGS="$CFLAGS $PISOCK_CFLAGS"
		AC_TRY_RUN(
			[
			#include <$piversion_include>
			int main(int argc,char *argv[]) {
				if (PILOT_LINK_VERSION == $pl_ve) {
					if (PILOT_LINK_MAJOR == $pl_ma) {
						if (PILOT_LINK_MINOR >= $pl_mi) {
							return 0;
				       	  	}
					} else if (PILOT_LINK_MAJOR > $pl_ma) {
						return 0;
					}
				} else if (PILOT_LINK_VERSION > $pl_ve) {
					return 0;
				}
				return 1;
			}
			],
			[AC_MSG_RESULT([yes (found $PILOT_LINK_VERSION)])],
			[AC_MSG_ERROR([pilot-link >= $1 required])],
			[AC_MSG_WARN([No action taken for crosscompile])]
		)
		CFLAGS="$CFLAGS_save"
	fi

	unset piversion_include
	unset pi_verion
	unset pi_major
	unset pi_minor
	unset pi_patch
	unset incdir
	unset pl_mi
	unset pl_ma
	unset pl_ve
])

AC_DEFUN([PILOT_LINK_CHECK],[
	PILOT_LINK_HOOK($1,[],nofailure)
])

AC_DEFUN([GNOME_PILOT_HOOK],[
	AC_PATH_PROG(GNOME_CONFIG,gnome-config,no)
	AC_CACHE_CHECK([for gnome-pilot environment],gnome_cv_pilot_found,[
		if test "x$GNOME_CONFIG" = "xno"; then
			gnome_cv_pilot_found=no
		else
			# gnome-config doesn't return a useful error status,
			# so we check if it outputs anything to stderr
			if test "x`$GNOME_CONFIG gpilot 2>&1 > /dev/null`" = "x"; then
				gnome_cv_pilot_found=yes
			else
				gnome_cv_pilot_found=no
			fi
		fi
	])
	if test x$gnome_cv_pilot_found = xyes; then
		if test x$1 != x; then
			gpv_pre_found=`$GNOME_CONFIG --modversion gpilot`
			gpv_found=`echo $gpv_pre_found | sed 's/gnome-pilot-\(.*\)/\1/'`
			AC_MSG_CHECKING([gnome-pilot version ($1)])
			gpv_major=`echo $gpv_found | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
			gpv_minor=`echo $gpv_found | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
			gpv_patch=`echo $gpv_found | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
			gpv_ma=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
			gpv_mi=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
			gpv_pa=`echo $1 | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`

			if test $gpv_major -eq $gpv_ma; then
				if test $gpv_minor -eq $gpv_mi; then
					if test $gpv_patch -ge $gpv_pa; then
						AC_MSG_RESULT([yes (found $gpv_found)])
					else
						AC_MSG_RESULT([too old (found $gpv_found)])
						gnome_cv_pilot_found=no
					fi
				elif test $gpv_minor -gt $gpv_mi; then
					AC_MSG_RESULT([yes (found $gpv_found)])
				else					
					AC_MSG_RESULT([too old (found $gpv_found)])
					gnome_cv_pilot_found=no
				fi
			else
				AC_MSG_RESULT([major version mismatch (found $gpv_found)])
				gnome_cv_pilot_found=no
			fi
			unset gpv_pre_found
			unset gpv_found
			unset gpv_major
			unset gpv_minor
			unset gpv_patch
			unset gpv_ma
			unset gpv_mi
			unset gpv_pa
		fi
	fi

	AM_CONDITIONAL(HAVE_GNOME_PILOT,test x$gnome_cv_pilot_found = xyes)
	if test x$gnome_cv_pilot_found = xyes; then
		PILOT_LINK_CHECK($2)
		GNOME_PILOT_CFLAGS=`gnome-config --cflags gpilot`
		GNOME_PILOT_LIBS=`gnome-config --libs gpilot`
	else
		if test x$3 = xfail; then
			AC_MSG_ERROR([gnome-pilot development package not installed or installation problem])
		else
			AC_MSG_WARN([gnome-pilot development package not installed or installation problem])
		fi
	fi
])

AC_DEFUN([GNOME_PILOT_CHECK],[
	if test x$1 = x; then
		gpv=[]
	else
		gpv=$1
	fi
	if test x$2 = x; then
		plv=0.9.5
	else
		plv=$2
	fi
	if test x$3 = x; then
		good=[]
	else
		good=$3
	fi
		
	GNOME_PILOT_HOOK($gpv, $plv, $good)
])

