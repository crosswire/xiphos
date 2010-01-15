libeditor_a_SOURCES = slib-editor.c slib-editor.h link_dialog.c link_dialog.h
noinst_LIBRARIES = libeditor.a

libeditor_a_CPPFLAGS = \
	-DINSTALL_PREFIX=\""$(prefix)"\" \
	$(DISABLE_DEPRECATED)

libeditor_a_CFLAGS = \
	-I$(top_srcdir)/src \
	$(XIPHOS_CFLAGS) \
	$(WARN_CFLAGS)

EXTRA_DIST = \
	wscript_build
