INCLUDES = \
	-I$(top_builddir)/src

noinst_LIBRARIES = libbackend.a

EXTRA_DIST =  \
    sword_defs.h

CXXFLAGS =  \
    $(SWORD_CPPFLAGS)

libbackend_a_SOURCES =  \
    bookmarks.cpp bookmarks.h \
    config.cc config.hh \
    key.cc key.hh \
    mgr.cc mgr.hh \
    module.cc module.hh \
    percomm_.cpp percomm_.h \
    sword.cpp sword.h\
    tree.cc tree.hh
