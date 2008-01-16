/*
 * GnomeSword Bible Study Tool
 * display.hh - 
 *
 * Copyright (C) 2000-2008 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>

#ifdef __cplusplus
extern "C" {

#include <gtk/gtk.h>
#include <swmgr.h>
#include <swdisp.h>
#include "main/global_ops.hh"
#include "backend/sword_main.hh"
#include "gui/utilities.h"

using namespace sword;

class GTKEntryDisp : public SWDisplay {
protected:
	GtkWidget *gtkText;
	BackEnd *be;
public:	
	GTKEntryDisp(GtkWidget *gtkText,
		     BackEnd *be) 
	{
		this->gtkText = gtkText; 
		this->be = be;
	}
	virtual char Display(SWModule &imodule);
};

class GTKChapDisp : public GTKEntryDisp {
public:
	GTKChapDisp(GtkWidget *gtkText,
		    BackEnd *be)
	    : GTKEntryDisp(gtkText,be)
	{
	}
	virtual char Display(SWModule &imodule);
	virtual void getVerseBefore(SWModule &imodule,
				    gboolean strongs_or_morph,
				    gboolean strongs_and_morph,
				    uint16_t cache_flags,
				    gint     image_content);
	virtual void getVerseAfter(SWModule &imodule,
				   gboolean strongs_or_morph,
				   gboolean strongs_and_morph,
				   uint16_t cache_flags,
				   gint     image_content);
	MOD_FONT *mf;
	SWBuf swbuf;
	gboolean is_rtol;
};

#ifndef USE_GTKMOZEMBED
class GTKTextviewChapDisp : public GTKEntryDisp {
public:
	GTKTextviewChapDisp(GtkWidget *gtkText,
			    BackEnd *be)
	    : GTKEntryDisp(gtkText,be)
	{
	}
	virtual char Display(SWModule &imodule);
};
#endif

class DialogEntryDisp : public SWDisplay {
protected:
	GtkWidget *gtkText;
	DIALOG_DATA *d;
	BackEnd *be;
	GLOBAL_OPS * ops;
	SWBuf swbuf;
	gboolean is_rtol;
public:
	DialogEntryDisp(GtkWidget *gtkText,
			DIALOG_DATA *d,
			BackEnd *be,
			GLOBAL_OPS * ops)
	{
		this->gtkText = gtkText;
		this->d = d;
		this->be = be;
		this->ops = ops;
	}
	virtual char Display(SWModule &imodule);
};

class DialogChapDisp : public  DialogEntryDisp {
public:
	DialogChapDisp(GtkWidget *gtkText,
		       DIALOG_DATA *d,
		       BackEnd *be,
		       GLOBAL_OPS * ops)
	    : DialogEntryDisp(gtkText,d,be,ops)
	{
	}
	virtual char Display(SWModule &imodule);
};

#ifndef USE_GTKMOZEMBED
class DialogTextviewChapDisp : public  DialogEntryDisp {
public:
	DialogTextviewChapDisp(GtkWidget *gtkText,
			       DIALOG_DATA *d,
			       BackEnd *be,
			       GLOBAL_OPS * ops)
	    : DialogEntryDisp(gtkText,d,be,ops)
	{
	}
	virtual char Display(SWModule &imodule);
};
#endif

class GTKPrintEntryDisp : public SWDisplay {
protected:
	GtkWidget *gtkText;
	BackEnd *be;
public:	
	GTKPrintEntryDisp(GtkWidget *gtkText,
			  BackEnd *be)
	{
		this->gtkText = gtkText;
		this->be = be;
	}
	virtual char Display(SWModule &imodule);
};

class GTKPrintChapDisp : public GTKPrintEntryDisp {
public:
	GTKPrintChapDisp(GtkWidget *gtkText,
			 BackEnd *be)
	    : GTKPrintEntryDisp(gtkText,be)
	{
	}
	virtual char Display(SWModule &imodule);
	MOD_FONT *mf;
};

#endif /* __cplusplus */

void ReadAloud(unsigned int verse, const char *suppliedtext);

#ifdef __cplusplus
}
#endif
