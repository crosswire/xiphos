INCLUDES =  \
    $(GNOME_INCLUDEDIR) \
    $(EXTRA_GNOME_CFLAGS) \
    $(EXTRA_PRINT_CFLAGS) \
    $(EXTRA_APP_CFLAGS) \
    $(PIXBUF_CFLAGS) \
    $(SWORD_CPPFLAGS) \
    -I$(top_srcdir)/html \
        -I /usr/include/gtkhtml 
    
    
bin_PROGRAMS = gnomesword

COMMON_SOURCE =  \
    gs_abouts.c \
    gs_abouts.h \
    gs_viewtext_dlg.c \
    gs_viewtext_dlg.h \
    gs_viewcomm_dlg.c \
    gs_viewcomm_dlg.h \
    gs_viewdict_dlg.c \
    gs_viewdict_dlg.h \
    gs_studypad.c \
    gs_studypad.h \
    gs_gnomesword.c \
    gs_gnomesword.h \
    gs_history.c \
    gs_history.h \
    callback.c \
    callback.h \
    display.cpp \
    display.h \
    gs_display.cpp \
    gs_display.h \
    gs_file.c \
    gs_file.h \
    gs_sword.cpp \
    gs_sword.h \
    interface.c \
    interface.h \
    gs_listeditor.c \
    gs_listeditor.h \
    main.c \
    gs_menu.c \
    gs_menu.h \
    gs_popup_cb.c \
    gs_popup_cb.h \
    gs_search.cpp \
    gs_search.h \
    gs_search_cb.cpp \
    gs_search_cb.h \
    support.c \
    support.h \
    gs_html.c \
    gs_html.h \
    sw_utility.cpp\
    sw_utility.h \
    printstuff.c \
    printstuff.h \
    gs_gbfhtml.cpp \
    gs_gbfhtml.h \
   gs_wgsil.cpp \
   gs_wgsil.h

SPELLING_SOURCE =  \
    debug.h \
    spell.c \
    spell.h \
    spell_gui.c \
    spell_gui.h \
    char_table.c \
    char_table.h 

EXTRA_DIST = \
    $(SPELLING_SOURCE) 

gnomesword_LDADD = \
    $(GNOME_LIBDIR) \
    $(GNOMEUI_LIBS) \
    $(EXTRA_GNOME_LIBS) \
    $(SWORD_LIBS)\
    $(GNOME_PRINT_LIBS) \
    $(PIXBUF_LIBS)\
    $(GNOME_APP_LIBS)\
    $(top_builddir)/html/libhtmleditor.a
    
if SPELLING
gnomesword_SOURCES = \
    $(SPELLING_SOURCE) \
    $(COMMON_SOURCE)
else
gnomesword_SOURCES =  \
    $(COMMON_SOURCE)
endif

CXXFLAGS =  \
    $(GTK_CFLAGS) \
    $(SWORD_CPPFLAGS)

