if USE_GTKMOZEMBED     
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

libgecko_a_CPPFLAGS = \
    -DGECKO_HOME=\""$(GECKO_HOME)\""

endif

EXTRA_DIST = \
	README \
	wscript
