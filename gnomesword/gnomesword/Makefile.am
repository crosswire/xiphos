INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(EXTRA_GNOME_CFLAGS) \
    $(EXTRA_PRINT_CFLAGS) \
    $(EXTRA_APP_CFLAGS) \
    $(PIXBUF_CFLAGS) \
    $(SWORD_CPPFLAGS)

bin_PROGRAMS = gnomesword

COMMON_SOURCE =  \
    gs_viewdict.c \
    gs_viewdict.h \
    gs_studypad.c \
    gs_studypad.h \
    gs_gnomesword.c \
    gs_gnomesword.h \
    gs_history.c \
    gs_history.h \
    callback.c \
    callback.h \
    display.cpp \
    display.h \
    gs_file.c \
    gs_file.h \
    gs_sword.cpp \
    gs_sword.h \
    interface.c \
    interface.h \
    gs_listeditor.c \
    gs_listeditor.h \
    main.c \
    gs_menu.c \
    gs_menu.h \
    gs_search.cpp \
    gs_search.h \\
    gs_search_cb.cpp \
    gs_search_cb.h \
    support.c \
    support.h \
    gs_html.c \
    gs_html.h \
    gs_rwphtml.cpp \
    gs_rwphtml.h \
    gs_thmlhtml.cpp \
    gs_thmlhtml.h 

SPELLING_SOURCE =  \
    debug.h \
    spell.c \
    spell.h \
    spell_gui.c \
    spell_gui.h \
    char_table.c \
    char_table.h 

PRINT_SOURCE = \
    printstuff.c \
    printstuff.h

EXTRA_DIST = \
    $(SPELLING_SOURCE) \
    $(PRINT_SOURCE)

COMMON_LDADD = \
    $(GNOME_LIBDIR) \
    $(GNOMEUI_LIBS) \
    $(EXTRA_GNOME_LIBS) \
    $(SWORD_LIBS)

PRINT_LDADD = \
    $(GNOME_PRINT_LIBS) \
    $(PIXBUF_LIBS)

SHORTCUTBAR_LDADD = \
    $(GNOME_APP_LIBS)

if SHORTCUTBAR_SPELL
gnomesword_SOURCES = \
    $(SPELLING_SOURCE) \
    $(PRINT_SOURCE)	 \
    $(COMMON_SOURCE)

gnomesword_LDADD =  \
    $(COMMON_LDADD) \
    $(PRINT_LDADD) \
    $(SHORTCUTBAR_LDADD)
endif

if SHORTCUTBAR
gnomesword_SOURCES = \
    $(PRINT_SOURCE)	 \
    $(COMMON_SOURCE)

gnomesword_LDADD =  \
    $(COMMON_LDADD) \
    $(PRINT_LDADD) \
    $(SHORTCUTBAR_LDADD)
endif
		
if SPELLING_PRINTING
gnomesword_SOURCES = \
    $(SPELLING_SOURCE) \
    $(PRINT_SOURCE) \
    $(COMMON_SOURCE)

gnomesword_LDADD =  \
    $(COMMON_LDADD) \
    $(PRINT_LDADD)
endif

if SPELLING
gnomesword_SOURCES = \
    $(SPELLING_SOURCE) \
    $(COMMON_SOURCE)

gnomesword_LDADD =  \
    $(COMMON_LDADD)
endif

if PRINTING
gnomesword_SOURCES = \
    $(PRINT_SOURCE) \
    $(COMMON_SOURCE)

gnomesword_LDADD =  \
    $(COMMON_LDADD) \
    $(PRINT_LDADD)
endif

if NO_OPS
gnomesword_SOURCES = \
    $(COMMON_SOURCE)

gnomesword_LDADD =  \
    $(COMMON_LDADD)
endif

CXXFLAGS =  \
    $(GTK_CFLAGS) \
    $(SWORD_CPPFLAGS)

