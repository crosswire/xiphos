dnl
dnl PSICONV_INIT 
dnl

AC_DEFUN([PSICONV_INIT],[
	AC_SUBST(PSICONV_LIBS)
	AC_SUBST(PSICONV_CFLAGS)

	AC_ARG_WITH(psiconv-includes,
	[  --with-psiconv-includes Specify location of Psiconv headers],[
	psiconv_cflags="-I$withval"
	])
	
	AC_ARG_WITH(psiconv-libs,
	[  --with-psiconv-libs     Specify location of Psiconv libs],[
	psiconv_libs="-L$withval"
	])

	AC_ARG_WITH(psiconv,
	[  --with-psiconv          Enable/disable Psiconv support],
		if test x$withval = xyes; then
	    		want_psiconv=yes
        	else
	    		if test "x$withval" = xno; then
	        		want_psiconv=no
	    		else
	        		want_psiconv=yes
	    		fi
  		fi,
		want_psiconv=yes)

	
	    if test "$want_psiconv" = "yes"; then
	      ac_save_CFLAGS="$CFLAGS"
	      ac_save_LIBS="$LIBS"
	      CFLAGS="$CFLAGS $psiconv_cflags"
	      LIBS="$LIBS $psiconv_libs"
	      AC_CHECK_LIB(psiconv,psiconv_parse,want_psiconv=yes,want_psiconv=no)
	      CFLAGS="$ac_save_CFLAGS"
	      LIBS="$ac_save_LIBS"
	    fi
	    
	    if test "$want_psiconv" = "yes"; then
	      PSICONV_LIBS="${psiconv_libs} -lpsiconv"
	      PSICONV_CFLAGS="$psiconv_cflags"
	      with_psiconv=true
	    else
	      PSICONV_LIBS=""
	      PSICONV_CFLAGS=""
	      with_psiconv=false
	    fi
	AM_CONDITIONAL(WITH_PSICONV,$with_psiconv)
])
