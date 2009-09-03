mozilla_include_subdirs =	\
	.		\
	gtkembedmoz	\
	xpcom

INCLUDES =  \
    $(XIPHOS_CFLAGS) \
    $(addprefix -I$(GECKO_INCLUDE_ROOT)/,$(mozilla_include_subdirs))      \
    $(GECKO_COMPONENT_CFLAGS) \
    -DGECKO_HOME=\""$(GECKO_HOME)\""  \
    -I$(top_srcdir)/src \
    -DPREFIX=\""$(prefix)"\" \
    -DSYSCONFDIR=\""$(sysconfdir)"\" \
    -DDATADIR=\""$(datadir)"\" \
    -DPACKAGE_LOCALE_DIR="\"$(datadir)/locale\"" \
    -DPACKAGE_PIXMAPS_DIR="\"$(datadir)/pixmaps/$(PACKAGE)\"" \
    -DLIBDIR=\""$(libdir)"\" \
    -DSHARE_DIR=\"$(pkgdatadir)\"

bin_PROGRAMS = xiphos

bin_SCRIPTS = gnomesword2

xiphos_SOURCES =  \
    about_modules.c \
    about_sword.c \
    about_trans.c \
    about_xiphos.c \
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
    find_dialog.c \
    font_dialog.c \
    gbs.c \
    gbs_dialog.c \
    gui.c \
    html.c \
    main_menu.c \
    main_window.c \
    menu_popup.c \
    mod_mgr.c \
    navbar_book.c \
    navbar_book_dialog.c \
    navbar_versekey.c \
    navbar_versekey_dialog.c \
    navbar_versekey_editor.c \
    navbar_versekey_parallel.c \
    parallel_dialog.c \
    parallel_tab.c \
    parallel_view.c \
    preferences_cell_renderer.c \
    preferences_dialog.c \
    search_dialog.c \
    search_sidebar.c \
    session.c \
    sidebar.c \
    sidebar_dialog.c \
    splash.c \
    tabbed_browser.c \
    treekey-editor.c \
    utilities.c \
    xiphos.c

if USE_GTKMOZEMBED
    LIB_GECKO=$(top_builddir)/src/gecko/libgecko.a 
else
    LIB_GECKO=
endif

if HAVE_GECKO_1_7
    xiphos_LDFLAGS = -R$(GECKO_HOME)
endif
if HAVE_GECKO_1_8
    xiphos_LDFLAGS = -R$(GECKO_HOME)
endif
if HAVE_GECKO_1_8_1
    xiphos_LDFLAGS = -R$(GECKO_HOME)
endif
if HAVE_GECKO_1_9
    xiphos_LDFLAGS = $(AM_LDFLAGS)
endif

xiphos_LDADD = \
    $(top_builddir)/src/main/libmain.a \
    $(top_builddir)/src/backend/libbackend.a \
    $(top_builddir)/src/editor/libeditor.a\
    $(LIB_GECKO)\
    $(XIPHOS_LIBS) \
    $(GECKO_COMPONENT_LIBS) \
    $(GNOME_SPELL_LIBS)

EXTRA_DIST = \
    $(top_builddir)/src/gui/*.h \
    gnomesword2 \
    wscript
