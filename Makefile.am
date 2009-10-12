## Process this file with automake to produce Makefile.in

desktopdir = $(datadir)/applications
desktop_in_files = @PACKAGE@.desktop.in.in
desktop_DATA = $(desktop_in_files:.desktop.in.in=.desktop)

@INTLTOOL_DESKTOP_RULE@

SUBDIRS = doc po src ui help pixmaps

DISTCHECK_CONFIGURE_FLAGS = --disable-scrollkeeper

dist_doc_DATA = \
	README \
	RELEASE-NOTES \
	COPYING \
	COPYING-DOCS \
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
	intltool-extract.in \
	intltool-merge.in \
	intltool-update.in \
	TRANSLATION-HOWTO \
	m4 \
	waf \
	wscript \
	waffles/__init__.py \
	waffles/editor.py \
	waffles/gecko.py \
	waffles/gtkhtml.py \
	waffles/misc.py \
	src/examples/ipc_client.c \
	src/examples/ipc_client.py \
	src/examples/Makefile

DISTCLEANFILES = gnome-doc-utils.make

rpm: dist
	rpmbuild -ta $(distdir).tar.gz
