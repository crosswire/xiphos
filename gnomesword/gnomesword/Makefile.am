## Process this file with automake to produce Makefile.in

INCLUDES = \
	$(GNOME_INCLUDEDIR)\
	-I/usr/include/sword

bin_PROGRAMS = gnomesword

gnomesword_SOURCES = \
	main.cpp\
	support.cpp support.h \
	display.cpp display.h\
	GnomeSword.cpp GnomeSword.h \
	callback.cpp callback.h\
	interface.cpp interface.h\
	filestuff.cpp filestuff.h\
	menustuff.cpp menustuff.h

CXXFLAGS = $(GTK_CFLAGS)  -I/usr/include/sword -g

gnomesword_LDADD = $(GNOME_LIBDIR) $(GNOMEUI_LIBS) 

