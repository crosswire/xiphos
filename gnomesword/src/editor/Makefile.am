# Basic setup

INCLUDES = \
    -I$(top_srcdir)/src \
    $(SWORD_INCLUDES) \
    $(GNOME_CFLAGS) \
    $(GTKHTML_CFLAGS) \
    $(MOZILLA_CFLAGS) 


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
	html-editor.c html-editor.h     


# Extra stuff to distribute.

EXTRA_DIST = \
    $(HTML_EDITOR_GENERATED)
    

CLEANFILES = \
    $(HTML_EDITOR_GENERATED)
