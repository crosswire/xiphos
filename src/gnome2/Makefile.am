bin_PROGRAMS = xiphos

BUILT_SOURCES = ipc-interface.h marshal.h locale_set.c

nodist_xiphos_SOURCES= $(BUILT_SOURCES)

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
    export_bookmarks.c \
    export_dialog.c \
    find_dialog.c \
    font_dialog.c \
    gbs.c \
    gbs_dialog.c \
    gui.c \
    html.c \
    ipc.c \
    main_menu.c \
    main_window.c \
    marshal.c \
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
    preferences_dialog.c \
    search_dialog.c \
    search_sidebar.c \
    sidebar.c \
    sidebar_dialog.c \
    splash.c \
    tabbed_browser.c \
    treekey-editor.c \
    utilities.c \
    xiphos.c

locale_set.c: $(top_srcdir)/po/LINGUAS $(top_srcdir)/src/gnome2/make-locale-set.sh
	sh $(top_srcdir)/src/gnome2/make-locale-set.sh $(top_srcdir)/po/LINGUAS > $@

marshal.c:
	glib-genmarshal --body --prefix=ipc_marshal $(top_srcdir)/src/gnome2/marshal.list \
	$< > $@
	sed -i -e 's;<glib-object.h>;"marshal.h";' $@

marshal.h:
	glib-genmarshal --header --prefix=ipc_marshal $(top_srcdir)/src/gnome2/marshal.list \
	$< > $@

ipc-interface.h:
	dbus-binding-tool --prefix=ipc_object --mode=glib-server $(top_srcdir)/src/gnome2/ipc-interface.xml \
	$< > $@

xiphos_CPPFLAGS =  \
	-DPREFIX=\""$(prefix)"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR="\"$(datadir)/locale\"" \
	-DPACKAGE_PIXMAPS_DIR="\"$(datadir)/pixmaps/$(PACKAGE)\"" \
	-DLIBDIR=\""$(libdir)"\" \
	-DSHARE_DIR=\"$(pkgdatadir)\" \
	$(DISABLE_DEPRECATED) \
	-UGTK_DISABLE_SINGLE_INCLUDES \
	$(AM_CPPFLAGS)

xiphos_CFLAGS = \
	-I$(top_srcdir)/src \
	$(XIPHOS_CFLAGS) \
	$(GECKO_CFLAGS) \
	$(WARN_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(DBUS_GLIB_CFLAGS)

xiphos_CXXFLAGS = \
	-I$(top_srcdir)/src \
	$(GECKO_CFLAGS) \
	$(XIPHOS_CFLAGS) \
	$(WARN_CXXFLAGS) \
	$(DBUS_CXXFLAGS)

if HAVE_GECKO
LIB_GECKO=$(top_builddir)/src/gecko/libgecko.a
endif

xiphos_LDADD = \
	$(LIB_GECKO) \
	$(top_builddir)/src/main/libmain.a \
	$(top_builddir)/src/backend/libbackend.a \
	$(top_builddir)/src/editor/libeditor.a\
	$(XIPHOS_LIBS) \
	$(GNOME_SPELL_LIBS) \
	$(GECKO_LIBS) \
	$(DBUS_LIBS)

EXTRA_DIST = \
    $(top_builddir)/src/gui \
    ipc-interface.xml \
    make-locale-set.sh \
    marshal.list \
    wscript_build

CLEANFILES = \
	$(BUILT_SOURCES)
