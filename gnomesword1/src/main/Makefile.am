INCLUDES =  \
    -I$(top_builddir)/src \
    $(GNOME_CFLAGS) 

noinst_LIBRARIES = libmain.a  

SPELL_IDL = Spell.idl

IDL_GENERATED = 		\
	Spell.h			\
	Spell-common.c		\
	Spell-skels.c		\
	Spell-stubs.c


$(IDL_GENERATED): $(SPELL_IDL)
	$(ORBIT_IDL) $(BONOBO_IDL_INCLUDES) $(srcdir)/Spell.idl

libmain_a_SOURCES =  \
    $(SPELL_IDL_GENERATED)\
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
    spell.h \
    sword.c sword.h\
    xml.c xml.h


EXTRA_DIST = \
    $(SPELL_IDL)
