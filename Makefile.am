## Process this file with automake to produce Makefile.in

SUBDIRS = m4 doc po src

gnomesworddoc_DATA = \
 	   README\
	    COPYING\
	    COPYING-DOCS\
	    AUTHORS\
	    ChangeLog\
	    INSTALL\
	    NEWS\
	    TODO


gnomesworddocdir = $(prefix)/@NO_PREFIX_PACKAGE_DOC_DIR@

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

	$(mkinstalldirs) $(datadir)/applications
	$(INSTALL_DATA) ${PACKAGE}.desktop $(datadir)/applications

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f -r $(datadir)/pixmaps/${PACKAGE}
	rm -f $(datadir)/applications/${PACKAGE}.desktop
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


ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = ${PACKAGE}.desktop.in ${PACKAGE}.spec.in ${PACKAGE}.spec xmldocs.make omf.make COPYING-DOCS
