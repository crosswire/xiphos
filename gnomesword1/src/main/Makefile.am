INCLUDES =  \
    -I$(top_srcdir)/src \
    $(GNOME_CFLAGS) \
    -DGNOME_DISABLE_DEPRECATED \
    -DGTK_DISABLE_DEPRECATED
    

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
  SPELL_IDL =
endif

libmain_a_SOURCES =  \
    $(IDL_GENERATED)\
    bibletext.c bibletext.h \
    commentary.cc commentary.h \
    configs.c configs.h \
    dictlex.c dictlex.h \
    display.cc display.hh \
    global_ops.cc global_ops.hh \
    key.c key.h \
    lists.cc lists.h \
    main.c \
    module.c module.h \
    module_dialogs.cc module_dialogs.h \
    mod_mgr.c mod_mgr.h \
    percomm.c percomm.h \
    search.c search.h \
    settings.c settings.h \
    sidebar.cc sidebar.h\
    spell_pspell.c \
    spell.h \
    sword.cc sword.h\
    url.cc url.h\
    xml.c xml.h

EXTRA_DIST = \
    $(SPELL_IDL)
    

CLEANFILES = \
    $(IDL_GENERATED)
