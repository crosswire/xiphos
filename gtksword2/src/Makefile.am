## Process this file with automake to produce Makefile.in

bin_PROGRAMS = gtksword

gtksword_SOURCES = \
        gtksword.cpp \
        gtksword_cbs.cpp

noinst_HEADERS = \
        gtksword.h \
        gtksword_cbs.h

gtksword_LDADD = -L/usr/local/lib -lsword -L/usr/lib -lgtk-x11-2.0 -lgdk-x11-2.0 -lgdk_pixbuf-2.0 -lm -lpangox-1.0 -lpangoxft-1.0 -lpango-1.0 -latk-1.0 -lgobject-2.0 -lgmodule-2.0 -ldl -lglib-2.0

CXXFLAGS = $(GTK_CFLAGS) -I/usr/local/include/sword -I/usr/include/gtk-2.0 -I/usr/lib/gtk-2.0/include -I/usr/include/glib-2.0 -I/usr/lib/glib-2.0/include -I/usr/include/pango-1.0 -I/usr/X11R6/include -I/usr/include/freetype2 -I/usr/include/atk-1.0  -g
