INCLUDES = \
    $(GNOME_CFLAGS) \
    -I$(top_srcdir)/src

noinst_LIBRARIES = libbackend.a

EXTRA_DIST =  \
    sword_defs.h

libbackend_a_SOURCES =  \
    config.cc config.hh \
    key.cc key.hh \
    mgr.cc mgr.hh \
    module_manager.cc module_manager.hh \
    module_search.cc .hh \
    sword.cpp sword.h\
    sword_main.cc sword_main.hh \
    gs_gbfhtmlhref.cpp gs_gbfhtmlhref.h \
    gs_markupfiltmgr.cpp gs_markupfiltmgr.h \
    gs_osishtmlhref.cpp gs_osishtmlhref.h \
    gs_thmlhtmlhref.cpp gs_thmlhtmlhref.h
