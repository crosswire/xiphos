INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(PIXBUF_CFLAGS) \
    $(PACKAGE_CFLAGS) \
    $(XML_CFLAGS) \
    -I/usr/include/gnome-1.0/ \
    -I$(top_builddir)/src

EXTRA_DIST =  \
	$(top_builddir)/src/gui/*.h \
	shortcutbar.h

bin_PROGRAMS = gnomesword-1.0

gnomesword_1_0_SOURCES =  \
    about_gnomesword.c \
    about_modules.c \
    about_sword.c \
    bibletext.c \
    bibletext_dialog.c \
    bibletext_menu.c \
    bookmarks.c \
    cipher_key_dialog.c \
    commentary.c \
    commentary_dialog.c \
    commentary_menu.c \
    dialog.c \
    dictlex.c \
    dictlex_dialog.c \
    dictlex_menu.c \
    display_info.c \
    dummy.cpp \
    editor.c \
    editor_menu.c \
    editor_replace.c \
    editor_search.c \
    editor_spell.c \
    fileselection.c \
    find_dialog.c \
    font_dialog.c \
    gbs.c \
    gbs_dialog.c \
    gbs_menu.c \
    gnomesword.c \
    gtkhtml_display.c \
    gui.c \
    history.c \
    html.c \
    interlinear.c \
    interlinear_dialog.c \
    main_menu.c \
    main_window.c \
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
    studypad_dialog.c \
    toolbar_edit.c \
    toolbar_nav.c \
    toolbar_style.c \
    utilities.c
    
gnomesword_1_0_LDADD = \
    $(GNOME_LIBDIR) \
    $(GNOMEUI_LIBS) \
    $(XML_LIBS) \
    $(EXTRA_GNOME_LIBS) \
    $(PACKAGE_LIBS)\
    $(GNOME_APP_LIBS)\
    $(GNOME_PRINT_LIBS) \
    $(PIXBUF_LIBS) \
    $(SWORD_LIBS) \
    $(PSPELL_LIBS) \
    $(top_builddir)/src/main/libmain.a \
    $(top_builddir)/src/backend/libbackend.a 
