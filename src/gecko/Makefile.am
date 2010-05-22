if HAVE_GECKO
noinst_LIBRARIES = libgecko.a

libgecko_a_SOURCES =  \
	Yelper.cpp		Yelper.h		\
	gecko-html.cpp		gecko-html.h		\
	gecko-print.c		gecko-print.h		\
	gecko-services.h   	gecko-services.cpp 	\
	gecko-utils.cpp		gecko-utils.h			

libgecko_a_CXXFLAGS = \
	-I$(top_srcdir)/src \
	$(GECKO_COMPONENT_CFLAGS) \
	$(GECKO_CFLAGS) \
	$(XIPHOS_CFLAGS) \
	$(WARN_CXXFLAGS)

libgecko_a_CFLAGS = \
	-I$(top_srcdir)/src \
	$(GECKO_COMPONENT_CFLAGS) \
	$(GECKO_CFLAGS) \
	$(XIPHOS_CFLAGS) \
	$(WARN_CFLAGS)

if HAVE_GECKO_HOME
libgecko_a_CPPFLAGS = \
    -DGECKO_HOME=\""$(GECKO_HOME)\""
else
libgecko_a_CPPFLAGS = \
    -DGECKO_HOME=
endif

libgecko_a_CPPFLAGS += $(DISABLE_DEPRECATED)


if HAVE_GECKO_1_9
else
if HAVE_GECKO_1_8_1
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

libgecko_a_CXXFLAGS += \
	$(addprefix -I$(GECKO_INCLUDE_ROOT)/,$(mozilla_include_subdirs))

libgecko_a_CFLAGS += \
	$(addprefix -I$(GECKO_INCLUDE_ROOT)/,$(mozilla_include_subdirs))

endif
endif

endif

EXTRA_DIST = \
	README \
	wscript
