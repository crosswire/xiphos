## Process this file with automake to produce Makefile.in

SUBDIRS =   doc macros po src

EXTRA_DIST = gnomesword1.desktop \
             src/gui/*.h

gnomesworddocdir = $(prefix)/doc/gnomesword
gnomesworddoc_DATA = \
 	   README\
	    COPYING\
	    AUTHORS\
	    ChangeLog\
	    INSTALL\
	    NEWS\
	    TODO



install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
          $(mkinstalldirs) $(datadir)/pixmaps/${PACKAGE}; \
          for pixmap in $(srcdir)/pixmaps/*; do \
            if test -f $$pixmap; then \
              $(INSTALL_DATA) $$pixmap $(datadir)/pixmaps/${PACKAGE}; \
            fi \
          done \
	fi

	$(mkinstalldirs) $(datadir)/gnome/apps/Applications; \
        if test -f gnomesword.desktop; then \
        $(INSTALL_DATA) gnomesword.desktop $(datadir)/gnome/apps/Applications; \
        fi 

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f -r $(datadir)/pixmaps/${PACKAGE}
	rm -f $(datadir)/gnome/apps/Applications/gnomesword.desktop
	rm -f -r $(datadir)/gnome/help/${PACKAGE}

dist-hook:
	if test -d pixmaps; then \
          mkdir $(distdir)/pixmaps; \
          for pixmap in pixmaps/*; do \
            if test -f $$pixmap; then \
              cp -p $$pixmap $(distdir)/pixmaps; \
            fi \
          done \
        fi

