INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(EXTRA_GNOME_CFLAGS) \
    $(EXTRA_PRINT_CFLAGS) \
    $(EXTRA_APP_CFLAGS) \
    $(PIXBUF_CFLAGS) \
    $(SWORD_CPPFLAGS)

bin_PROGRAMS = gnomesword
 
COMMON_SOURCE =  \
    e-splash.c \
    e-splash.h \
    gs_gui.c \
    gs_gui.h \
    gs_setup.c \
    gs_setup.h \
    gs_abouts.c \
    gs_abouts.h \
    gs_info_box.c \
    gs_info_box.h\
    gs_viewcomm_dlg.c \
    gs_viewcomm_dlg.h \
    gs_viewdict_dlg.c \
    gs_viewdict_dlg.h \
    gs_studypad.c \
    gs_studypad.h \
    gs_bookmarks.c \
    gs_bookmarks.h \
    gs_gnomesword.c \
    gs_gnomesword.h \
    gs_history.c \
    gs_history.h \
    callback.c \
    callback.h \
    display.cpp \
    display.h \
    gs_display.cpp \
    gs_display.h \
    gs_file.c \
    gs_file.h \
    gs_sword.cpp \
    gs_sword.h \
    gs_preferences_dlg.c \
    gs_preferences_dlg.h \
    gs_listeditor.c \
    gs_listeditor.h \
    main.c \
    gs_menu.c \
    gs_menu.h \
    gs_popup_cb.c \
    gs_popup_cb.h \
    gs_search.cpp \
    gs_search.h \
    gs_search_cb.cpp \
    gs_search_cb.h \
    support.c \
    support.h \
    gs_html.c \
    gs_html.h \
    gs_verselist_dlg.c \
    gs_verselist_dlg.h \
    gs_viewtext_dlg.c \
    gs_viewtext_dlg.h \
    gs_viewtext_sw.cpp \
    gs_viewtext_sw.h \
    gs_information_dlg.c \
    gs_information_dlg.h \
    gs_information_sw.cpp \
    gs_information_sw.h \
    gs_shortcutbar.c \
    gs_shortcutbar.h \
    sw_utility.cpp\
    sw_utility.h \
    sw_properties.cpp \
    sw_properties.h \
    sw_bookmarks.cpp \
    sw_bookmarks.h \
    sw_gbfhtml.cpp \
    sw_gbfhtml.h \
    sw_thmlhtml.cpp \
    sw_thmlhtml.h\
    sw_latin1utf8.cpp \
    sw_latin1utf8.h \
    sw_search.cpp \
    sw_search.h \
    sw_verselist_dlg.cpp \
    sw_verselist_dlg.h

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

