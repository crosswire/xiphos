INCLUDES =  \
    $(GNOME_CFLAGS) \
    -I$(top_srcdir)/src \
    -DPREFIX=\""$(prefix)"\" \
    -DSYSCONFDIR=\""$(sysconfdir)"\" \
    -DDATADIR=\""$(datadir)"\" \
    -DLIBDIR=\""$(libdir)"\" 


bin_PROGRAMS = gnomesword2


gnomesword2_SOURCES =  \
    about_gnomesword.c \
    about_modules.c \
    about_sword.c \
    bibletext.c \
    bibletext_dialog.c \
    bookmarks_menu.c \
    bookmarks_treeview.c \
    cipher_key_dialog.c \
    commentary.c \
    commentary_dialog.c \
    dialog.c \
    dictlex.c \
    dictlex_dialog.c \
    display_info.c \
    dummy.cpp \
    editor.c \
    editor_menu.c \
    editor_replace.c \
    editor_spell.c \
    fileselection.c \
    find_dialog.c \
    font_dialog.c \
    gbs.c \
    gbs_dialog.c \
    gnomesword.c \
    gui.c \
    hints.c \
    history.c \
    html.c \
    main_menu.c \
    main_window.c \
    mod_mgr.c \
    note_editor.c \
    note_toolbar_edit.c \
    note_toolbar_style.c \
    parallel_view.c \
    parallel_dialog.c \
    preferences_cell_renderer.c \
    preferences_dialog.c \
    search_dialog.c \
    search_sidebar.c \
    session.c \
    setup_druid.c \
    sidebar.c \
    sidebar_dialog.c \
    spell_gnome_spell.c \
    splash.c \
    studypad.c \
    studypad_dialog.c \
    tabbed_browser.c \
    toolbar_edit.c \
    toolbar_nav.c \
    toolbar_style.c \
    utilities.c
    
gnomesword2_LDADD = \
    $(GNOME_LIBS)\
    $(SPELL_LIBS) \
    $(GNOME_SPELL_LIBS)\
    $(top_builddir)/src/main/libmain.a \
    $(top_builddir)/src/backend/libbackend.a

EXTRA_DIST = \
    $(top_builddir)/src/gui/*.h
