## Process this file with automake to produce Makefile.in

@INTLTOOL_DESKTOP_RULE@

SUBDIRS = m4 doc po src

gnomesworddoc_DATA = \
	README \
	COPYING \
	COPYING-DOCS \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS \
	TODO

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	${PACKAGE}.desktop.in.in \
	${PACKAGE}.spec.in \
	${PACKAGE}.spec \
	xmldocs.make \
	omf.make \
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	COPYING-DOCS

gnomesworddocdir = $(prefix)/@NO_PREFIX_PACKAGE_DOC_DIR@

gnomemenudir = $(datadir)/applications
gnomemenu_premerge_file = gnomesword.desktop.in
gnomemenu_DATA = $(gnomemenu_premerge_file:.desktop.in=.desktop)

install-data-local:
	@$(NORMAL_INSTALL)
	if test -d $(srcdir)/pixmaps; then \
          $(mkinstalldirs) $(DESTDIR)$(datadir)/pixmaps/${PACKAGE}; \
          for pixmap in $(srcdir)/pixmaps/*; do \
            if test -f $$pixmap; then \
              $(INSTALL_DATA) $$pixmap $(DESTDIR)$(datadir)/pixmaps/${PACKAGE}; \
            fi \
          done \
	fi

	$(mkinstalldirs) $(DESTDIR)$(datadir)/applications
	$(INSTALL_DATA) ${PACKAGE}.desktop $(DESTDIR)$(datadir)/applications

uninstall-local:
	@$(NORMAL_UNINSTALL)
	rm -f -r $(DESTDIR)$(datadir)/pixmaps/${PACKAGE}
	rm -f $(DESTDIR)$(datadir)/applications/${PACKAGE}.desktop
	rm -f -r $(DESTDIR)$(datadir)/gnome/help/${PACKAGE}

dist-hook:
	if test -d pixmaps; then \
          mkdir $(DESTDIR)$(distdir)/pixmaps; \
          for pixmap in pixmaps/*; do \
            if test -f $$pixmap; then \
              cp -p $$pixmap $(DESTDIR)$(distdir)/pixmaps; \
            fi \
          done \
        fi

rpm: dist
	rpmbuild -ta $(distdir).tar.gz
