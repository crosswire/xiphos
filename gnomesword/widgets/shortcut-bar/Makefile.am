
noinst_LIBRARIES = libshortcut-bar.a
noinst_PROGRAMS = test-shortcut-bar

INCLUDES = 					\
	-I$(top_srcdir)				\
	-I$(top_srcdir)/widgets/misc		\
	-DEVOLUTION_VERSION=\""$(VERSION)"\"	\
	$(EXTRA_GNOME_CFLAGS)			\
	-DG_LOG_DOMAIN=\"shortcut-bar\"

libshortcut_bar_a_SOURCES = 	\
	e-group-bar.c		\
	e-group-bar.h		\
	e-icon-bar-bg-item.c	\
	e-icon-bar-bg-item.h	\
	e-icon-bar.c		\
	e-icon-bar.h		\
	e-shortcut-bar.c	\
	e-shortcut-bar.h	\
	e-shortcut-model.c	\
	e-shortcut-model.h	\
	e-vscrolled-bar.c	\
	e-vscrolled-bar.h

test_shortcut_bar_SOURCES = 	\
	test-shortcut-bar.c

test_shortcut_bar_LDADD = 		\
	./libshortcut-bar.a		\
	$(top_builddir)/widgets/e-paned/libepaned.a		\
	../e-text/libetext.a		\
	../misc/libemiscwidgets.a	\
	../../e-util/libeutil.la	\
	$(EXTRA_GNOME_LIBS)

test_shortcut_bar_LDFLAGS =	\
	`gnome-config --libs gdk_pixbuf gnomecanvaspixbuf`
