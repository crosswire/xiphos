INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(EXTRA_GNOME_CFLAGS) \
    $(EXTRA_PRINT_CFLAGS) \
    $(EXTRA_APP_CFLAGS) \
    $(PIXBUF_CFLAGS) \
    $(SWORD_CPPFLAGS) \
    $(GTKHTML_CFLAGS) \
    -I/usr/include/gnome-1.0/ \
    -I$(top_builddir)/src/backend/ \
    -I$(top_builddir)/src/gnome/

bin_PROGRAMS = gnomesword

COMMON_SOURCE =  \
    bibletext.c \
    bibletext.h \
    bibletext_view.c \
    bibletext_view.h \
    commentary.c \
    commentary.h \
    dictlex.c \
    dictlex.h \
    find_dlg_defs.h \
    gbs.c \
    gbs.h \
    gs_bookmarks.c \
    gs_bookmarks.h \
    gs_gnomesword.c \
    gs_gnomesword.h \
    gs_gui.c \
    gs_gui.h \
    gs_history.c \
    gs_history.h \
    gs_html.c \
    gs_html.h \
    gs_info_box.c \
    gs_info_box.h\
    gs_information_dlg.c \
    gs_information_dlg.h \
    gs_interlinear.c \
    gs_interlinear.h \
    gs_menu.c \
    gs_menu.h \
    gs_popup_cb.c \
    gs_popup_cb.h \
    gs_preferences_dlg.c \
    gs_preferences_dlg.h \
    setup.c \
    setup.h \
    gs_shortcutbar.c \
    gs_shortcutbar.h \
    gs_viewcomm_dlg.c \
    gs_viewcomm_dlg.h \
    gs_viewdict_dlg.c \
    gs_viewdict_dlg.h \
    lists.c \
    lists.h \
    main.c \
    percomm.c \
    percomm.h \
    support.c \
    support.h \
    settings.c \
    settings.h 

SPELLING_SOURCE =  \
    debug.h \
    spell.c \
    spell.h \
    spell_gui.c \
    spell_gui.h \
    char_table.c \
    char_table.h 

EXTRA_DIST = \
    $(SPELLING_SOURCE)
    
gnomesword_LDADD = \
    $(GNOME_LIBDIR) \
    $(GNOMEUI_LIBS) \
    $(EXTRA_GNOME_LIBS) \
    $(SWORD_LIBS) \
    $(GTKHTML_LIBS)\
    $(GNOME_APP_LIBS)\
    $(GNOME_PRINT_LIBS) \
    $(PIXBUF_LIBS) \
    $(top_builddir)/src/backend/libbackend.a \
    $(top_builddir)/src/gnome/libgui.a 


gnomesword_SOURCES = \
    $(SPELLING_SOURCE) \
    $(COMMON_SOURCE)

CXXFLAGS =  \
    $(GTK_CFLAGS) \
    $(SWORD_CPPFLAGS)

