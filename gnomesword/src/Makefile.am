INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(EXTRA_GNOME_CFLAGS) \
    $(EXTRA_PRINT_CFLAGS) \
    $(EXTRA_APP_CFLAGS) \
    $(PIXBUF_CFLAGS) \
    $(SWORD_CPPFLAGS) \
    $(GTKHTML_CFLAGS) \
    -I/usr/include/gnome-1.0/

bin_PROGRAMS = gnomesword
 
COMMON_SOURCE =  \
    e-splash.c \
    e-splash.h \
    gs_abouts.c \
    gs_abouts.h \
    gs_bookmarks.c \
    gs_bookmarks.h \
    gs_detach_int.c \
    gs_detach_int.h \
    gs_detach_sb.c \
    gs_detach_sb.h \
    gs_editor.c \
    gs_editor.h \
    gs_editor_replace.c \
    gs_editor_replace.h \
    gs_editor_search.c \
    gs_editor_search.h \
    gs_editor_toolbar.c \
    gs_editor_toolbar.h \
    gs_file.c \
    gs_file.h \
    gs_find_dlg.c \
    gs_find_dlg.h \
    gs_getinfo_dlg.c \
    gs_getinfo_dlg.h \
    gs_gnomesword.c \
    gs_gnomesword.h \
    gs_gui.c \
    gs_gui.h \
    gs_gui_cb.c \
    gs_gui_cb.h \
    gs_history.c \
    gs_history.h \
    gs_html.c \
    gs_html.h \
    gs_info_box.c \
    gs_info_box.h\
    gs_information_dlg.c \
    gs_information_dlg.h \
    gs_mainmenu_cb.c \
    gs_mainmenu_cb.h\
    gs_menu.c \
    gs_menu.h \
    gs_preferences_dlg.c \
    gs_preferences_dlg.h \
    gs_popup_cb.c \
    gs_popup_cb.h \
    gs_setup.c \
    gs_setup.h \
    gs_shortcutbar.c \
    gs_shortcutbar.h \
    gs_unlockkey_dlg.c \
    gs_unlockkey_dlg.h \
    gs_viewcomm_dlg.c \
    gs_viewcomm_dlg.h \
    gs_viewdict_dlg.c \
    gs_viewdict_dlg.h \
    gs_viewtext_dlg.c \
    gs_viewtext_dlg.h \
    main.c \
    support.c \
    support.h \
    sw_bookmarks.cpp \
    sw_bookmarks.h \
    sw_display.cpp \
    sw_display.h \
    sw_gbs.cpp \
    sw_gbs.h \
    sw_information_dlg.cpp \
    sw_information_dlg.h \
    sw_module_options.cpp \
    sw_module_options.h \
    sw_properties.cpp \
    sw_properties.h \
    sw_search.cpp \
    sw_search.h \
    sw_shortcutbar.cpp \
    sw_shortcutbar.h \
    sw_sword.cpp \
    sw_sword.h \
    sw_utility.cpp\
    sw_utility.h \
    sw_verselist_sb.cpp \
    sw_verselist_sb.h\
    sw_viewcomm_dlg.cpp \
    sw_viewcomm_dlg.h\
    sw_viewdict_dlg.cpp \
    sw_viewdict_dlg.h \
    sw_viewtext_dlg.cpp \
    sw_viewtext_dlg.h 

SPELLING_SOURCE =  \
    debug.h \
    spell.c \
    spell.h \
    spell_gui.c \
    spell_gui.h \
    char_table.c \
    char_table.h 

EXTRA_DIST = \
    $(SPELLING_SOURCE)\
    tt.xpm
    
gnomesword_LDADD = \
    $(GNOME_LIBDIR) \
    $(GNOMEUI_LIBS) \
    $(EXTRA_GNOME_LIBS) \
    $(SWORD_LIBS) \
    $(GTKHTML_LIBS)\
    $(GNOME_APP_LIBS)\
    $(GNOME_PRINT_LIBS) \
    $(PIXBUF_LIBS)
	
gnomesword_SOURCES = \
    $(SPELLING_SOURCE) \
    $(COMMON_SOURCE)

CXXFLAGS =  \
    $(GTK_CFLAGS) \
    $(SWORD_CPPFLAGS)

