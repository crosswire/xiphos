/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
  /*
     * GnomeSword Bible Study Tool
     * sw_display.h
     * -------------------
     * Fri Mar 16 2001
     * copyright (C) 2001 by Terry Biggs
     * tbiggs@users.sourceforge.net
     *
   */

 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifndef __SW_DISPLAY_H_
#define __SW_DISPLAY_H_

#include <gnome.h>
#include <swmodule.h>

class GtkHTMLEntryDisp:public SWDisplay {
      protected:
	GtkWidget * gtkText;
      public:
	
	bool use_gtkhtml_font;
	GtkHTMLEntryDisp(GtkWidget * gtkText) {
		this->gtkText = gtkText;
	}
	virtual char Display(SWModule & imodule);
	virtual gchar* pick_font(SWModule & imodule);
	//gchar *ProcessText(gchar * text, gint maxlen);
};

//----------------------------------------------------------------------------------------------
class ComEntryDisp:public GtkHTMLEntryDisp { public:
	ComEntryDisp(GtkWidget * gtkText):GtkHTMLEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
class GTKutf8ChapDisp:public GtkHTMLEntryDisp { public:
	GTKutf8ChapDisp(GtkWidget * gtkText):GtkHTMLEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
	void marksearchwords( GString *str );
};

//----------------------------------------------------------------------------------------------
class InterlinearDisp:public GtkHTMLEntryDisp { public:
	InterlinearDisp(GtkWidget * gtkText):GtkHTMLEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
void AboutModsDisplayHTML(char *to, char *text);


class GTKEntryDisp:public SWDisplay {
      protected:
	GtkWidget * gtkText;
      public:
	static GdkColor colourGreen;
	static GdkColor colourBlue;
	static GdkColor colourRed;
	static GdkColor colourCur;

	static void __initialize() { GdkColormap *cmap;

		 cmap = gdk_colormap_get_system();
		 colourGreen.red = 0x0000;
		 colourGreen.green = 0xbbbb;
		 colourGreen.blue = 0x0000;
		 colourBlue.red = 0;
		 colourBlue.green = 0;
		 colourBlue.blue = 0xffff;
		 colourRed.red = 0xffff;
		 colourRed.green = 0;
		 colourRed.blue = 0;
		 colourCur.red = 0x0000;
		 colourCur.green = 0xbbbb;
		 colourCur.blue = 0x0000;
		if (!gdk_color_alloc(cmap, &colourGreen)) {
			g_error("couldn't allocate colour");
		}
		if (!gdk_color_alloc(cmap, &colourBlue)) {
			g_error("couldn't allocate colour");
		}
		if (!gdk_color_alloc(cmap, &colourRed)) {
			g_error("couldn't allocate colour");
		}
		if (!gdk_color_alloc(cmap, &colourCur)) {
			g_error("couldn't allocate colour");
		}
	}
	GTKEntryDisp(GtkWidget * gtkText) {
		this->gtkText = gtkText;
	}
	virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
class GTKPerComDisp:public GTKEntryDisp { public:
	GTKPerComDisp(GtkWidget * gtkText):GTKEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

#endif /* __SW_DISPLAY_H_ */
