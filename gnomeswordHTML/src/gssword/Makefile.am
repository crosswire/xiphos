
noinst_LIBRARIES = libgssword.a

INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
        $(SWORD_CPPFLAGS) \
    $(EXTRA_APP_CFLAGS) \
	-I$(top_srcdir) \
	-I$(top_srcdir)/html \
	-I$(top_srcdir)/src \
        -I$(top_srcdir)/src/gsdialogs \
	-I$(top_srcdir)/src/gssword \
        -I /usr/include/gtkhtml \
	$(EXTRA_GNOME_CFLAGS)

libgssword_a_SOURCES =  \
      display.cpp \
      display.h \
      gs_display.cpp \
      gs_display.h \
      gs_sword.cpp \
      gs_sword.h \
      sw_utility.cpp\
      sw_utility.h \
      gs_viewtext_sw.cpp \
       gs_viewtext_sw.h \
       gs_information_sw.cpp \
       gs_information_sw.h\
      gs_wgsil.cpp \
      gs_wgsil.h \
      sw_gbfhtml.cpp \
      sw_gbfhtml.h \
      sw_thmlhtml.cpp \
      sw_thmlhtml.h 



