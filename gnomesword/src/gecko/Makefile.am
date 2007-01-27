
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
	widget

	
INCLUDES =  \
    -I$(top_srcdir)/src \
    $(SWORD_CFLAGS) \
    $(GNOME_CFLAGS) \
    $(GTKHTML_CFLAGS)	\
    $(addprefix -I$(MOZILLA_INCLUDE_ROOT)/,$(mozilla_include_subdirs))      \
    $(MOZILLA_COMPONENT_CFLAGS) \
    -DMOZILLA_HOME=\""$(MOZILLA_HOME)\"" 
 
     
noinst_LIBRARIES = libgecko.a

libgecko_a_SOURCES =  \
	Yelper.cpp		Yelper.h		\
	gecko-html.cpp		gecko-html.h		\
	gecko-print.c		gecko-print.h		\
	gecko-services.h   	gecko-services.cpp 	\
	gecko-utils.cpp		gecko-utils.h			
	
