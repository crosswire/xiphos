if USE_GTKHTML_BONOBO_EDITOR
# CORBA stuff
HTML_EDITOR_GENERATED =		\
	Editor.h		\
	Editor-common.c	\
	Editor-skels.c	\
	Editor-stubs.c

# The Editor Control
libeditor_a_SOURCES = $(HTML_EDITOR_GENERATED)		\
	bonobo-editor.c bonobo-editor.h  
else
libeditor_a_SOURCES = slib-editor.c slib-editor.h link_dialog.c link_dialog.h
endif

noinst_LIBRARIES = libeditor.a

libeditor_a_CPPFLAGS = \
	-DINSTALL_PREFIX=\""$(prefix)"\" \
	$(DISABLE_DEPRECATED)

libeditor_a_CFLAGS = \
	-I$(top_srcdir)/src \
	$(XIPHOS_CFLAGS) \
	$(WARN_CFLAGS)

EXTRA_DIST = \
	wscript
