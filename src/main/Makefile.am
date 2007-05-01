
mozilla_include_subdirs =	\
	.		\
	commandhandler  \
	content		\
	docshell	\
	browser		\
	dom		\
	fastfind	\
	find		\
	gtkembedmoz	\
	gfx		\
	layout		\
	necko		\
	nspr		\
	pref		\
	string		\
	uriloader	\
	webbrwsr	\
	webshell	\
	widget

	
INCLUDES =  \
    -I$(top_srcdir)/src \
    $(SWORD_CFLAGS) \
    $(GNOME_CFLAGS) \
    $(GTKHTML_CFLAGS) \
    $(addprefix -I$(MOZILLA_INCLUDE_ROOT)/,$(mozilla_include_subdirs))      \
    $(MOZILLA_COMPONENT_CFLAGS) \
    $(UPRINT_CFLAGS)
    
    
noinst_LIBRARIES = libmain.a

libmain_a_SOURCES =  \
    configs.cc configs.h \
    display.cc display.hh \
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
    prayer_list.cc prayer_list.h \
    previewer.cc previewer.h \
    search_dialog.cc search_dialog.h \
    search_sidebar.cc search_sidebar.h \
    settings.c settings.h \
    sidebar.cc sidebar.h\
    sword.cc sword.h\
    tab_history.c tab_history.h\
    url.cc url.hh\
    xml.c xml.h
