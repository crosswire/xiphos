## Process this file with automake to produce Makefile.in

INCLUDES = \
	$(GNOME_INCLUDEDIR)\
	-I/usr/include/sword

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
	thmlgbf.cpp\
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
	thmlgbf.cpp\
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
	thmlgbf.cpp\
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
	thmlgbf.cpp
endif

CXXFLAGS = $(GTK_CFLAGS)  -I/usr/include/sword -g

gnomesword_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) 

