# FIXME we use the EClippedLabel widget from EShortcutBar.  Probably
# it should be moved somewhere else.

INCLUDES =					\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/widgets/shortcut-bar	\
	$(EXTRA_GNOME_CFLAGS)			\
	-DG_LOG_DOMAIN=__FILE__

noinst_LIBRARIES =	\
	libemiscwidgets.a

libemiscwidgets_a_SOURCES = 	\
	e-calendar.c		\
	e-calendar.h		\
	e-calendar-item.c	\
	e-calendar-item.h	\
	e-clipped-label.c	\
	e-clipped-label.h	\
	e-scroll-frame.c	\
	e-scroll-frame.h	\
	e-title-bar.c		\
	e-title-bar.h

noinst_PROGRAMS = 	\
	test-title-bar		\
	test-calendar

test_title_bar_SOURCES = 	\
	test-title-bar.c

test_title_bar_LDADD = 						\
	./libemiscwidgets.a					\
	$(EXTRA_GNOME_LIBS)

test_calendar_SOURCES = 	\
	test-calendar.c

test_calendar_LDADD = 						\
	./libemiscwidgets.a					\
	../../e-util/libeutil.la				\
	$(EXTRA_GNOME_LIBS)
