## Process this file with automake to produce Makefile.in

SUBDIRS = 	macros \
		e-util \
		widgets \
		gnomesword

EXTRA_DIST = gnomesword.glade gnomesword.kdevprj
install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
	  $(mkinstalldirs) $(datadir)/pixmaps/gnomesword; \
	  for pixmap in $(srcdir)/pixmaps/*; do \
	    if test -f $$pixmap; then \
	      $(INSTALL_DATA) $$pixmap $(datadir)/pixmaps/gnomesword; \
	    fi \
	  done \
	fi

dist-hook:
	if test -d pixmaps; then \
	  mkdir $(distdir)/pixmaps; \
	  for pixmap in pixmaps/*; do \
	    if test -f $$pixmap; then \
	      cp -p $$pixmap $(distdir)/pixmaps; \
	    fi \
	  done \
	fi

