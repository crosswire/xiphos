INCLUDES =  \
    -I$(top_srcdir)/src \
    $(GNOME_CFLAGS) 

noinst_LIBRARIES = libmain.a  

if GNOME_SPELL
  SPELL_IDL = Spell.idl

  IDL_GENERATED = 		\
	Spell.h			\
	Spell-common.c		\
	Spell-skels.c		\
	Spell-stubs.c

$(IDL_GENERATED): $(SPELL_IDL)
	$(ORBIT_IDL) $(BONOBO_IDL_INCLUDES) $(top_builddir)/src/main/Spell.idl
else
  IDL_GENERATED =
endif

libmain_a_SOURCES =  \
    $(IDL_GENERATED)\
    bibletext.c bibletext.h \
    commentary.c commentary.h \
    configs.c configs.h \
    dictlex.c dictlex.h \
    gbs.c gbs.h \
    key.c key.h \
    lists.c lists.h \
    main.c \
    module.c module.h \
    percomm.c percomm.h \
    search.c search.h \
    settings.c settings.h \
    shortcutbar.c shortcutbar.h \
    spell_gnome_spell.c \
    spell_pspell.c \
    spell.h \
    sword.c sword.h\
    xml.c xml.h


EXTRA_DIST = \
    $(SPELL_IDL)
    
CLEANFILES = \
    $(IDL_GENERATED)
