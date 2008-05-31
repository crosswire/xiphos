
mozilla_include_subdirs =	\
	.		\
	gtkembedmoz	\
	xpcom
	
INCLUDES =  \
    $(GNOME_CFLAGS) \
    $(GTKHTML_CFLAGS) \
    $(addprefix -I$(GECKO_INCLUDE_ROOT)/,$(mozilla_include_subdirs))      \
    $(GECKO_COMPONENT_CFLAGS) \
    -DGECKO_HOME=\""$(GECKO_HOME)\""  \
    -I$(top_srcdir)/src \
    -DPREFIX=\""$(prefix)"\" \
    -DSYSCONFDIR=\""$(sysconfdir)"\" \
    -DDATADIR=\""$(datadir)"\" \
    -DLIBDIR=\""$(libdir)"\" \
    -DSHARE_DIR=\"$(pkgdatadir)\"  \
    $(UPRINT_CFLAGS)


bin_PROGRAMS = gnomesword2


gnomesword2_SOURCES =  \
    about_gnomesword.c \
    about_modules.c \
    about_sword.c \
    bibletext.c \
    bibletext_dialog.c \
    bookmark_dialog.c \
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
    export_dialog.c \
    fileselection.c \
    find_dialog.c \
    font_dialog.c \
    gbs.c \
    gbs_dialog.c \
    gnomesword.c \
    gui.c \
    html.c \
    main_menu.c \
    main_window.c \
    mod_mgr.c \
    navbar_book.c \
    navbar_book_dialog.c \
    navbar_versekey.c \
    navbar_versekey_dialog.c \
    navbar_versekey_editor.c \
    navbar_versekey_parallel.c \
    parallel_view.c \
    parallel_dialog.c \
    preferences_cell_renderer.c \
    preferences_dialog.c \
    search_dialog.c \
    search_sidebar.c \
    session.c \
    sidebar.c \
    sidebar_dialog.c \
    splash.c \
    studypad.c \
    toolbar_nav.c \
    tabbed_browser.c \
    treekey-editor.c \
    utilities.c

if USE_GTKMOZEMBED
    LIB_GECKO=$(top_builddir)/src/gecko/libgecko.a 
    gnomesword2_LDFLAGS = -R$(GECKO_HOME)
else
    LIB_GECKO=
endif 


gnomesword2_LDADD = \
    $(top_builddir)/src/main/libmain.a \
    $(top_builddir)/src/backend/libbackend.a \
    $(top_builddir)/src/editor/libeditor.a\
    $(LIB_GECKO)\
    $(GNOME_LIBS) \
    $(GTKHTML_LIBS) \
    $(GECKO_COMPONENT_LIBS) \
    $(GNOME_SPELL_LIBS)\
    $(SWORD_LIBS) \
    $(UPRINT_LIBS)

    
EXTRA_DIST = \
    $(top_builddir)/src/gui/*.h
