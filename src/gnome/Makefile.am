INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(PIXBUF_CFLAGS) \
    $(GTKHTML_CFLAGS) \
    -I/usr/include/gnome-1.0/ \
    -I$(top_builddir)/src

bin_PROGRAMS = gnomesword-1.0

gnomesword_1_0_SOURCES =  \
    display_info.c \
    _editor.c \
    about_gnomesword.c \
    about_modules.c \
    about_sword.c \
    bibletext.c \
    bibletext_dialog.c \
    bookmarks.c \
    cipher_key_dialog.c \
    commentary_dialog.c \
    commentary.c \
    dialog.c \
    dictlex.c \
    dictlex_dialog.c \
    dummy.cpp \
    editor_menu.c \
    editor_replace.c \
    editor_search.c \
    editor_spell.c \
    fileselection.c \
    find_dialog.c \
    font_dialog.c \
    gbs.c \
    gbs_dialog.c \
    gnomesword.c \
    gtkhtml_display.c \
    gui.c \
    history.c \
    html.c \
    interlinear.c \
    interlinear_dialog.c \
    main_window.c \
    main_menu.c \
    percomm.c \
    preferences_dialog.c \
    search_dialog.c \
    setup_druid.c \
    shortcutbar_dialog.c \
    shortcutbar_main.c \
    shortcutbar_search.c \
    shortcutbar_viewer.c \
    splash.c \
    studypad.c \
    toolbar_edit.c \
    toolbar_nav.c \
    toolbar_style.c \
    utilities.c
    
gnomesword_1_0_LDADD = \
    $(GNOME_LIBDIR) \
    $(GNOMEUI_LIBS) \
    $(EXTRA_GNOME_LIBS) \
    $(GTKHTML_LIBS)\
    $(GNOME_APP_LIBS)\
    $(GNOME_PRINT_LIBS) \
    $(PIXBUF_LIBS) \
    $(SWORD_LIBS) \
    $(PSPELL_LIBS) \
    $(top_builddir)/src/main/libmain.a \
    $(top_builddir)/src/backend/libbackend.a 
