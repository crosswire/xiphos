/***************************************************************************
                          display.h  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
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

/********************************************************************\
**********************************************************************
**  this code was taken from the Sword Cheatah program							**
**  and modfied to handle some of the BTF stuff. Also added	 				**
**  suport for the x symbol font when using greek modules.     			**
**   																																**                                                                              	 **
**********************************************************************
\********************************************************************/

#include <gnome.h>
#include <swmodule.h>

class GTKEntryDisp : public SWDisplay 
{
protected:
	GtkWidget *gtkText;
public:
	static GdkColor colourGreen;
	static GdkColor colourBlue;
	static GdkColor colourRed;
	static void __initialize()
	{
			GdkColormap *cmap;

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
			if (!gdk_color_alloc(cmap, &colourGreen))
			{
			  g_error("couldn't allocate colour");
			}
			if (!gdk_color_alloc(cmap, &colourBlue))
			{
			  g_error("couldn't allocate colour");
			}
			if (!gdk_color_alloc(cmap, &colourRed))
			{
			  g_error("couldn't allocate colour");
			}
	}
	GTKEntryDisp(GtkWidget *gtkText) { this->gtkText = gtkText; }
	virtual char Display(SWModule &imodule);
};

//----------------------------------------------------------------------------------------------
class GTKPerComDisp : public GTKEntryDisp
{
  public:
	GTKPerComDisp(GtkWidget *gtkText) : GTKEntryDisp(gtkText) {}
	virtual char Display(SWModule &imodule);	
};

//----------------------------------------------------------------------------------------------
class GTKChapDisp : public GTKEntryDisp 
{
  public:
	GTKChapDisp(GtkWidget *gtkText) : GTKEntryDisp(gtkText) {}
	virtual char Display(SWModule &imodule);
};

//----------------------------------------------------------------------------------------------
class GTKRWPDisp : public GTKEntryDisp
{
  public:
	GTKRWPDisp(GtkWidget *gtkText) : GTKEntryDisp(gtkText) {}
	virtual char Display(SWModule &imodule);
};

//----------------------------------------------------------------------------------------------
class HTMLentryDisp : public GTKEntryDisp
{
  public:
	HTMLentryDisp(GtkWidget *gtkText) : GTKEntryDisp(gtkText) {}
	virtual char Display(SWModule &imodule);
};

//----------------------------------------------------------------------------------------------
class GBFentryDisp : public GTKEntryDisp
{
  public:
	GBFentryDisp(GtkWidget *gtkText) : GTKEntryDisp(gtkText) {}
	virtual char Display(SWModule &imodule);
};

//----------------------------------------------------------------------------------------------
class GTKInterlinearDisp : public GTKEntryDisp 
{
  public:
	GTKInterlinearDisp(GtkWidget *gtkText) : GTKEntryDisp(gtkText) {}
	virtual char Display(SWModule &imodule);
};

//----------------------------------------------------------------------------------------------
void
AboutModsDisplay		(GtkWidget* text, gchar *aboutinfo);
