INCLUDES = \
    $(PACKAGE_CFLAGS) \
    -I$(top_builddir)/src

noinst_LIBRARIES = libbackend.a

EXTRA_DIST =  \
    sword_defs.h

CXXFLAGS =  \
    $(SWORD_INCLUDES)

libbackend_a_SOURCES =  \
    config.cc config.hh \
    key.cc key.hh \
    mgr.cc mgr.hh \
    module.cc module.hh \
    sword.cpp sword.h\
    tree.cc tree.hh
