noinst_LIBRARIES = libbackend.a

libbackend_a_SOURCES =  \
    gs_stringmgr.cpp gs_stringmgr.h \
    module_manager.cc module_manager.hh \
    sword_main.cc sword_main.hh

libbackend_a_CXXFLAGS =  \
    -I$(top_srcdir)/src \
    $(XIPHOS_CFLAGS) \
	$(WARN_CXXFLAGS)

EXTRA_DIST = \
   wscript
