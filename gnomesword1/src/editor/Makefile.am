# Basic setup

INCLUDES = \
    -I$(top_srcdir)/src \
    $(SWORD_INCLUDES) \
    $(GNOME_CFLAGS) \
    $(GTKHTML_CFLAGS) \
    $(MOZILLA_CFLAGS) 


# CORBA stuff
SPELL_IDLS = Spell.idl
SPELL_IDL_GENERATED_H =		\
	Spell.h
SPELL_IDL_GENERATED_C = 	\
	Spell-common.c		\
	Spell-skels.c		\
	Spell-stubs.c
SPELL_IDL_GENERATED = $(SPELL_IDL_GENERATED_C) $(SPELL_IDL_GENERATED_H)

$(SPELL_IDL_GENERATED_H): $(SPELL_IDLS)
	$(ORBIT_IDL) $(BONOBO_IDL_INCLUDES) $(srcdir)/Spell.idl
$(SPELL_IDL_GENERATED_C): $(SPELL_IDL_GENERATED_H)


noinst_LIBRARIES = libeditor.a

# The Editor Control

libeditor_a_SOURCES =				\
	$(SPELL_IDL_GENERATED)			\
	editor.c editor.h			\
	editor_menu.c editor_menu.h		\
	editor_replace.c editor_replace.h	\
	editor_spell.c editor_spell.h		\
	note_editor.c note_editor.h		\
	note_toolbar_edit.c note_toolbar_edit.h	\
	note_toolbar_style.c note_toolbar_style.h \
	toolbar_edit.c toolbar_edit.h		\
	toolbar_style.c toolbar_style.h		\
	spell.c	 spell.h			\
	gi-utils.h				\
	gi-colors.c gi-colors.h			\
	gi-color-palette.c gi-color-palette.h	\
	gi-color-group.c gi-color-group.h	\
	gi-combo-box.c	 gi-combo-box.h		\
	gi-color-combo.c gi-color-combo.h


# test_editor_LDADD = $(LDADD) libgnome-gtkhtml-editor.la \
# 	@PACKAGE_LIBS@

# Extra stuff to distribute.

EXTRA_DIST = \
    $(SPELL_IDL_GENERATED)
    

CLEANFILES = \
    $(SPELL_IDL_GENERATED)
