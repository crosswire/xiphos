## Process this file with automake to produce Makefile.in

SUBDIRS =   doc macros po src

BUILT_SOURCES = ${PACKAGE}.desktop

gnomesworddoc_DATA = \
 	   README\
	    COPYING\
	    AUTHORS\
	    ChangeLog\
	    INSTALL\
	    NEWS\
	    TODO


${PACKAGE}.desktop: ${PACKAGE}.desktop.in
	sed -e 's:@pixmapdir@:$(datadir)/pixmaps/${PACKAGE}:g' ${PACKAGE}.desktop.in > ${PACKAGE}.desktop
	
gnomesworddocdir = $(prefix)/doc/gnomesword-1.0

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

	$(mkinstalldirs) $(datadir)/gnome/apps/Applications
	$(INSTALL_DATA) ${PACKAGE}.desktop $(datadir)/gnome/apps/Applications

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f -r $(datadir)/pixmaps/${PACKAGE}
	rm -f $(datadir)/gnome/apps/Applications/gnomesword-1.0.desktop
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

