# FIXME we use the EClippedLabel widget from EShortcutBar.  Probably
# it should be moved somewhere else.
if SHORTCUTBAR
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
	e-dateedit.c		\
	e-dateedit.h		\
	e-title-bar.c		\
	e-title-bar.h
endif
