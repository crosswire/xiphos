## Process this file with automake to produce Makefile.in

SUBDIRS = doc po src ui help pixmaps

desktopdir = $(datadir)/applications
desktop_in_files = @PACKAGE@.desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)

@INTLTOOL_DESKTOP_RULE@



DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper

dist_doc_DATA = \
	README \
	RELEASE-NOTES \
	COPYING \
	AUTHORS \
	ChangeLog \
	INSTALL \
	NEWS \
	TODO \
	Xiphos.ogg

ACLOCAL_AMFLAGS = -I m4

EXTRA_DIST = \
	${PACKAGE}.desktop.in.in \
	${PACKAGE}.spec.in \
	${PACKAGE}.spec \
	gnome-doc-utils.make \
	xmldocs.make \
	omf.make \
	TRANSLATION-HOWTO \
	m4/gecko.m4 \
	waf \
	wscript \
	po/wscript_build \
	waffles \
        src/examples/ipc_client.c \
	src/examples/ipc_client.py \
	src/examples/Makefile

CLEANFILES = \
	$(desktop_DATA)

rpm: dist
	rpmbuild -ta $(distdir).tar.gz
