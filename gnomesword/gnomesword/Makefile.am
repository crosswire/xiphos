## Process this file with automake to produce Makefile.in

INCLUDES = \
	$(GNOME_INCLUDEDIR)\
	-I/usr/include/sword\
	-I/usr/local/include/sword\
	-I$(top_srcdir)/widgets/misc\
	-I$(top_srcdir)/widgets/shotrcur-bar

bin_PROGRAMS = gnomesword

if SPELLING
gnomesword_SOURCES = \
	main.cpp\
	support.c support.h \
	display.cpp display.h\
	GnomeSword.cpp GnomeSword.h \
	callback.cpp callback.h\
	interface.c interface.h\
	filestuff.cpp filestuff.h\
	menustuff.cpp menustuff.h\
	listeditor.cpp listeditor.h\
	noteeditor.cpp noteeditor.h\
	searchstuff.cpp searchstuff.h\
	spellcheck.cpp spellcheck.h\
	DICTClient.cpp DICTClient.h
endif

if PRINTING
gnomesword_SOURCES = \
	main.cpp\
	support.c support.h \
	display.cpp display.h\
	GnomeSword.cpp GnomeSword.h \
	callback.cpp callback.h\
	interface.c interface.h\
	filestuff.cpp filestuff.h\
	menustuff.cpp menustuff.h\
	listeditor.cpp listeditor.h\
	noteeditor.cpp noteeditor.h\
	searchstuff.cpp searchstuff.h\
	printstuff.c printstuff.h
endif

if SPELLING_PRINTING
gnomesword_SOURCES = \
	main.cpp\
	support.c support.h \
	display.cpp display.h\
	GnomeSword.cpp GnomeSword.h \
	callback.cpp callback.h\
	interface.c interface.h\
	filestuff.cpp filestuff.h\
	menustuff.cpp menustuff.h\
	listeditor.cpp listeditor.h\
	noteeditor.cpp noteeditor.h\
	searchstuff.cpp searchstuff.h\
	spellcheck.cpp spellcheck.h\
	DICTClient.cpp DICTClient.h\
	printstuff.c printstuff.h
endif

if NO_OPS
gnomesword_SOURCES = \
	main.cpp\
	support.c support.h \
	display.cpp display.h\
	GnomeSword.cpp GnomeSword.h \
	callback.cpp callback.h\
	interface.c interface.h\
	filestuff.cpp filestuff.h\
	menustuff.cpp menustuff.h\
	listeditor.cpp listeditor.h\
	noteeditor.cpp noteeditor.h\
	searchstuff.cpp searchstuff.h
endif

CXXFLAGS = $(GTK_CFLAGS)  -I/usr/include/sword -g

gnomesword_LDADD =  $(GNOME_LIBDIR) $(GNOMEUI_LIBS)\
		    $(top_builddir)/widgets/shortcut-bar/libshortcut-bar.a\
			$(top_builddir)/widgets/e-text/libetext.a\
			$(top_builddir)/widgets/misc/libemiscwidgets.a\
			$(top_builddir)/e-util/libeutil.la\
			$(EXTRA_GNOME_LIBS)		
 

