
mozilla_include_subdirs =	\
	.		\
	commandhandler  \
	content		\
	docshell	\
	browser		\
	dom		\
	fastfind	\
	find		\
	gtkembedmoz	\
	gfx		\
	layout		\
	necko		\
	nspr		\
	pref		\
	string		\
	uriloader	\
	webbrwsr	\
	webshell	\
	widget		\
	xpcom

	
INCLUDES =  \
    -I$(top_srcdir)/src \
    $(SWORD_CFLAGS) \
    $(GNOME_CFLAGS) \
    $(GTKHTML_CFLAGS)	\
    $(addprefix -I$(GECKO_INCLUDE_ROOT)/,$(mozilla_include_subdirs))      \
    $(GECKO_COMPONENT_CFLAGS) \
    -DGECKO_HOME=\""$(GECKO_HOME)\""  \
    $(UPRINT_CFLAGS)
 
if USE_GTKMOZEMBED     
noinst_LIBRARIES = libgecko.a

libgecko_a_SOURCES =  \
	Yelper.cpp		Yelper.h		\
	gecko-html.cpp		gecko-html.h		\
	gecko-print.c		gecko-print.h		\
	gecko-services.h   	gecko-services.cpp 	\
	gecko-utils.cpp		gecko-utils.h			
endif

EXTRA_DIST = \
	README
