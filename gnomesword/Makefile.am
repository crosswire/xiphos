## Process this file with automake to produce Makefile.in

SUBDIRS = 	macros \
	    src

EXTRA_DIST = gnomesword.glade Gnomesword.prj

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
          $(mkinstalldirs) $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
          for pixmap in $(srcdir)/pixmaps/*; do \
            if test -f $$pixmap; then \
              $(INSTALL_DATA) $$pixmap $(DESTDIR)@PACKAGE_PIXMAPS_DIR@; \
            fi \
          done \
	fi

	$(mkinstalldirs) $(DESTDIR)@PACKAGE_MENU_DIR@/Applications; \
        if test -f Gnomesword.desktop; then \
        $(INSTALL_DATA) Gnomesword.desktop $(DESTDIR)@PACKAGE_MENU_DIR@/Applications; \
        fi 

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f -r $(DESTDIR)@PACKAGE_PIXMAPS_DIR@
	rm -f $(DESTDIR)@PACKAGE_MENU_DIR@/Applications/Gnomesword.desktop
	rm -f -r $(DESTDIR)@PACKAGE_HELP_DIR@

dist-hook:
	if test -d pixmaps; then \
          mkdir $(distdir)/pixmaps; \
          for pixmap in pixmaps/*; do \
            if test -f $$pixmap; then \
              cp -p $$pixmap $(distdir)/pixmaps; \
            fi \
          done \
        fi

