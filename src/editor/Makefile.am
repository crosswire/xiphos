if USE_GTKHTML_BONOBO_EDITOR
# CORBA stuff

HTML_EDITOR_GENERATED =		\
	Editor.h		\
	Editor-common.c	\
	Editor-skels.c	\
	Editor-stubs.c

Editor-commmon.c: $(GTKHTML_DATA_DIR)/Editor.idl

$(HTML_EDITOR_GENERATED): $(GTKHTML_DATA_DIR)/Editor.idl
	$(ORBIT_IDL) -I $(srcdir) $(BONOBO_IDL_INCLUDES) -I $(GTKHTML_DATA_DIR)/gtkhtml $(GTKHTML_DATA_DIR)/Editor.idl

noinst_LIBRARIES = libeditor.a

# The Editor Control
libeditor_a_SOURCES = $(HTML_EDITOR_GENERATED)		\
	bonobo-editor.c bonobo-editor.h  
# Extra stuff to distribute.

CLEANFILES = \
    $(HTML_EDITOR_GENERATED)

else

noinst_LIBRARIES = libeditor.a
libeditor_a_SOURCES = slib-editor.c slib-editor.h link_dialog.c link_dialog.h

endif

libeditor_a_CPPFLAGS = -DINSTALL_PREFIX=\""$(prefix)"\" \
	$(DISABLE_DEPRECATED)
libeditor_a_CFLAGS = \
	-I$(top_srcdir)/src \
	$(XIPHOS_CFLAGS) \
	$(WARN_CFLAGS)

EXTRA_DIST = \
	wscript
