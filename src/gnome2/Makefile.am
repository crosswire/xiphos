bin_PROGRAMS = xiphos

bin_SCRIPTS = gnomesword2

BUILT_SOURCES = ipc-interface.h marshal.h

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
    locale_set.c \
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

locale_set.c: ../../po/LINGUAS make-locale-set.sh
	sh ./make-locale-set.sh > locale_set.c

marshal.c:
	glib-genmarshal --body --prefix=ipc_marshal marshal.list \
	$< > $@
	sed -i -e 's;<glib-object.h>;"marshal.h";' $@

marshal.h:
	glib-genmarshal --header --prefix=ipc_marshal marshal.list \
	$< > $@

ipc-interface.h:
	dbus-binding-tool --prefix=ipc_object --mode=glib-server ipc-interface.xml \
	$< > $@

xiphos_CPPFLAGS =  \
	-DGECKO_HOME=\""$(GECKO_HOME)\""  \
	-DPREFIX=\""$(prefix)"\" \
	-DSYSCONFDIR=\""$(sysconfdir)"\" \
	-DDATADIR=\""$(datadir)"\" \
	-DPACKAGE_LOCALE_DIR="\"$(datadir)/locale\"" \
	-DPACKAGE_PIXMAPS_DIR="\"$(datadir)/pixmaps/$(PACKAGE)\"" \
	-DLIBDIR=\""$(libdir)"\" \
	-DSHARE_DIR=\"$(pkgdatadir)\" \
	$(DISABLE_DEPRECATED) \
	$(AM_CPPFLAGS)

xiphos_CFLAGS = \
	-I$(top_srcdir)/src \
	$(XIPHOS_CFLAGS) \
	$(GECKO_CFLAGS) \
	$(GECKO_COMPONENT_CFLAGS) \
	$(WARN_CFLAGS) \
	$(DBUS_CFLAGS) \
	$(DBUS_GLIB_CFLAGS)

xiphos_CXXFLAGS = \
	-I$(top_srcdir)/src \
	$(GECKO_CFLAGS) \
	$(GECKO_COMPONENT_CFLAGS) \
	$(XIPHOS_CFLAGS) \
	$(WARN_CXXFLAGS) \
	$(DBUS_CXXFLAGS)

if HAVE_GECKO
LIB_GECKO=$(top_builddir)/src/gecko/libgecko.a
endif

if HAVE_GECKO_1_9_1
else
if HAVE_GECKO_1_9
else
if HAVE_GECKO
    xiphos_LDFLAGS = -R$(GECKO_HOME)
endif
endif
endif


xiphos_LDADD = \
	$(LIB_GECKO) \
	$(top_builddir)/src/main/libmain.a \
	$(top_builddir)/src/backend/libbackend.a \
	$(top_builddir)/src/editor/libeditor.a\
	$(XIPHOS_LIBS) \
	$(GNOME_SPELL_LIBS) \
	$(GECKO_COMPONENT_LIBS) \
	$(GECKO_LIBS) \
	$(DBUS_LIBS)

EXTRA_DIST = \
    $(top_builddir)/src/gui/*.h \
    gnomesword2 \
    wscript
