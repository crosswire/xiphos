## Process this file with automake to produce Makefile.in

INCLUDES = \
        $(GNOME_INCLUDEDIR) \
       -I$(top_srcdir)/widgets/misc \
       -I$(top_srcdir)/widgets/shortcut-bar \
       -I$(top_srcdir)/widgets/e-paned


bin_PROGRAMS = gnomesword

COMMON_SOURCE = \
        gs_studypad.c \
        gs_studypad.h \
        gs_gnomesword.c \
        gs_gnomesword.h \
        callback.cpp \
        callback.h \
        display.cpp \
        display.h \
        filestuff.cpp \
        filestuff.h \
        gs_sword.cpp \
        gs_sword.h \
        interface.c \
        interface.h \
        gs_listeditor.c \
        gs_listeditor.h \
        main.cpp \
        menustuff.c \
        menustuff.h \
        noteeditor.cpp \
        noteeditor.h \
        searchstuff.cpp \
        searchstuff.h \
        support.c \
        support.h \
        printstuff.c \
        printstuff.h 

SPELLING_SOURCE= \
        DICTClient.cpp \
        DICTClient.h \
        spellcheck.cpp \
        spellcheck.h 

EXTRA_DIST=$(SPELLING_SOURCE) 


if SPELLING 
gnomesword_SOURCES = \
         $(SPELLING_SOURCE) \
         $(COMMON_SOURCE) 
endif

if NO_OPS
gnomesword_SOURCES = \
        $(COMMON_SOURCE) 
endif

CXXFLAGS = $(GTK_CFLAGS) $(SWORD_CPPFLAGS)          

gnomesword_LDADD = \
        $(GNOME_LIBDIR) $(GNOMEUI_LIBS) \
        $(top_builddir)/widgets/shortcut-bar/libshortcut-bar.a \
        $(top_builddir)/widgets/e-paned/libepaned.a \
        $(top_builddir)/widgets/e-text/libetext.a \
        $(top_builddir)/widgets/misc/libemiscwidgets.a \
        $(top_builddir)/e-util/libeutil.la \
        $(EXTRA_GNOME_LIBS) $(SWORD_LIBS)
