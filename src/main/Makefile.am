INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(EXTRA_GNOME_CFLAGS) \
    $(EXTRA_PRINT_CFLAGS) \
    $(EXTRA_APP_CFLAGS) \
    $(PIXBUF_CFLAGS) \
    $(SWORD_CPPFLAGS) \
    $(GTKHTML_CFLAGS) \
    -I/usr/include/gnome-1.0/ \
    -I$(top_builddir)/src

bin_PROGRAMS = gnomesword

gnomesword_SOURCES =  \
    bibletext.c \
    bibletext.h \
    bookmarks.c \
    bookmarks.h \
    commentary.c \
    commentary.h \
    dictlex.c \
    dictlex.h \
    gbs.c \
    gbs.h \
    lists.c \
    lists.h \
    main.c \
    percomm.c \
    percomm.h \
    settings.c \
    settings.h \
    shortcutbar.c \
    shortcutbar.h \
    spell_pspell.c \
    spell_pspell.h \
    sword.c \
    sword.h

    
gnomesword_LDADD = \
    $(GNOME_LIBDIR) \
    $(GNOMEUI_LIBS) \
    $(EXTRA_GNOME_LIBS) \
    $(SWORD_LIBS) \
    $(GTKHTML_LIBS)\
    $(GNOME_APP_LIBS)\
    $(GNOME_PRINT_LIBS) \
    $(PSPELL_LIBS) \
    $(PIXBUF_LIBS) \
    $(top_builddir)/src/backend/libbackend.a \
    $(top_builddir)/src/gnome/libgui.a 


CXXFLAGS =  \
    $(GTK_CFLAGS) \
    $(SWORD_CPPFLAGS)

