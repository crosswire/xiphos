## Process this file with automake to produce Makefile.in

INCLUDES = \
        $(GNOME_INCLUDEDIR) \
       -I$(top_srcdir)/widgets/misc \
       -I$(top_srcdir)/widgets/shortcut-bar \
       -I$(top_srcdir)/widgets/e-paned


bin_PROGRAMS = gnomesword

COMMON_SOURCE = \
        callback.cpp \
        callback.h \
        display.cpp \
        display.h \
        filestuff.cpp \
        filestuff.h \
        GnomeSword.cpp \
        GnomeSword.h \
        interface.c \
        interface.h \
        listeditor.cpp \
        listeditor.h \
        main.cpp \
        menustuff.c \
        menustuff.h \
        noteeditor.cpp \
        noteeditor.h \
        searchstuff.cpp \
        searchstuff.h \
        support.c \
        support.h 

SPELLING_SOURCE= \
        DICTClient.cpp \
        DICTClient.h \
        spellcheck.cpp \
        spellcheck.h 

PRINT_SOURCE= \
        printstuff.c \
        printstuff.h 

EXTRA_DIST=$(PRINT_SOURCE) $(SPELLING_SOURCE) 

if SPELLING_PRINTING 
gnomesword_SOURCES = \
        $(SPELLING_SOURCE) \
        $(PRINT_SOURCE)	 \
        $(COMMON_SOURCE) 
endif 

if SPELLING 
gnomesword_SOURCES = \
         $(SPELLING_SOURCE) \
         $(COMMON_SOURCE) 
endif

if PRINTING 
gnomesword_SOURCES = \
        $(PRINT_SOURCE) \
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
