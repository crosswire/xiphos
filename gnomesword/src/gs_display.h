/***************************************************************************
                          gs_display.h  -  description
                             -------------------
    begin                : Fri Mar 16 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

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
 *  You should have received a copy of the GNU Library General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <gnome.h>
#include <swmodule.h>


class GtkHTMLEntryDisp:public SWDisplay {
      protected:
	GtkWidget * gtkText;
      public:
	
	GtkHTMLEntryDisp(GtkWidget * gtkText) {
		this->gtkText = gtkText;
	}
	virtual char Display(SWModule & imodule);
	gchar *gbftohtml(gchar * text, gint maxlen);
};

//----------------------------------------------------------------------------------------------
class ComEntryDisp:public GtkHTMLEntryDisp { public:
	ComEntryDisp(GtkWidget * gtkText):GtkHTMLEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
class GTKhtmlChapDisp:public GtkHTMLEntryDisp { public:
	GTKhtmlChapDisp(GtkWidget * gtkText):GtkHTMLEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
class GTKutf8ChapDisp:public GtkHTMLEntryDisp { public:
	GTKutf8ChapDisp(GtkWidget * gtkText):GtkHTMLEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
class InterlinearDisp:public GtkHTMLEntryDisp { public:
	InterlinearDisp(GtkWidget * gtkText):GtkHTMLEntryDisp(gtkText) {
	} virtual char Display(SWModule & imodule);
};

//----------------------------------------------------------------------------------------------
void AboutModsDisplayHTML(GString *text);

