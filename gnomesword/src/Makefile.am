INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(EXTRA_GNOME_CFLAGS) \
    $(EXTRA_PRINT_CFLAGS) \
    $(EXTRA_APP_CFLAGS) \
    $(PIXBUF_CFLAGS) \
    $(SWORD_CPPFLAGS) \
    $(GTKHTML_CFLAGS)

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
    sw_display.cpp \
    sw_display.h \
    gs_file.c \
    gs_file.h \
    gs_preferences_dlg.c \
    gs_preferences_dlg.h \
    gs_unlockkey_dlg.c \
    gs_unlockkey_dlg.h \
    gs_listeditor.c \
    gs_listeditor.h \
    main.c \
    gs_menu.c \
    gs_menu.h \
    gs_popup_cb.c \
    gs_popup_cb.h \
    support.c \
    support.h \
    gs_html.c \
    gs_html.h \
    gs_viewtext_dlg.c \
    gs_viewtext_dlg.h \
    sw_viewtext_dlg.cpp \
    sw_viewtext_dlg.h \
    gs_information_dlg.c \
    gs_information_dlg.h \
    gs_information_sw.cpp \
    gs_information_sw.h \
    gs_shortcutbar.c \
    gs_shortcutbar.h \
    sw_gnomesword.cpp \
    sw_gnomesword.h \
    sw_utility.cpp\
    sw_utility.h \
    sw_properties.cpp \
    sw_properties.h \
    sw_bookmarks.cpp \
    sw_bookmarks.h \
    sw_search.cpp \
    sw_search.h \
    sw_shortcutbar.cpp \
    sw_shortcutbar.h \
    sw_verselist_sb.cpp \
    sw_verselist_sb.h\
    sw_viewcomm_dlg.cpp \
    sw_viewcomm_dlg.h\
    sw_viewdict_dlg.cpp \
    sw_viewdict_dlg.h \
    gbfhtmlhref.cpp \
    gbfhtmlhref.h\
    thmlhtmlhref.cpp \
    thmlhtmlhref.h\
    sw_latin1utf8.cpp \
    sw_latin1utf8.h

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
    $(SWORD_LIBS) \
    $(GTKHTML_LIBS)\
    $(GNOME_APP_LIBS)
    
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

CXXFLAGS =  \
    $(GTK_CFLAGS) \
    $(SWORD_CPPFLAGS)

