noinst_LIBRARIES = libmain.a

libmain_a_SOURCES =  \
    configs.cc configs.h \
    display.cc display.hh \
    export_passage.cc export_passage.h \
    global_ops.cc global_ops.hh \
    lists.cc lists.h \
    main.c \
    module_dialogs.cc module_dialogs.h \
    modulecache.cc modulecache.hh \
    mod_mgr.cc mod_mgr.h \
    navbar.cc navbar.h \
    navbar_book.cc navbar_book.h \
    navbar_book_dialog.cc navbar_book_dialog.h \
    navbar_versekey.cc navbar_versekey.h \
    parallel_view.cc parallel_view.h \
    prayerlists.cc prayerlists.h \
    previewer.cc previewer.h \
    search_dialog.cc search_dialog.h \
    search_sidebar.cc search_sidebar.h \
    settings.c settings.h \
    sidebar.cc sidebar.h\
    sword.cc sword.h\
    sword_treekey.cc sword_treekey.h \
    tab_history.c tab_history.h\
    url.cc url.hh\
    xml.c xml.h

libmain_a_CXXFLAGS =  \
	-I$(top_srcdir)/src \
	$(XIPHOS_CFLAGS) \
	$(GECKO_COMPONENT_CFLAGS) \
	$(GECKO_CFLAGS)

libmain_a_CFLAGS =  \
	-I$(top_srcdir)/src \
	$(XIPHOS_CFLAGS) \
	$(GECKO_COMPONENT_CFLAGS) \
	$(GECKO_CFLAGS)

libmain_a_CPPFLAGS = $(DISABLE_DEPRECATED) \
	-UGTK_DISABLE_SINGLE_INCLUDES

EXTRA_DIST = \
	wscript
