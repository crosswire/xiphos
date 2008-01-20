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
public:	
	GTKEntryDisp(GtkWidget *_gtkText,
		     BackEnd *_be) 
	    : gtkText(_gtkText),
	      be(_be),
	      swbuf(""),
	      mf(NULL),
	      ops(NULL),
	      is_rtol(FALSE),
	      strongs_or_morph(FALSE),
	      strongs_and_morph(FALSE),
	      cache_flags(0)
	{
	}
	virtual char Display(SWModule &imodule);

protected:
	GtkWidget  *gtkText;
	BackEnd    *be;
	SWBuf      swbuf;
	MOD_FONT   *mf;
	GLOBAL_OPS *ops;
	gboolean   is_rtol;
	gboolean   strongs_or_morph;
	gboolean   strongs_and_morph;
	uint16_t   cache_flags;
};

class GTKChapDisp : public GTKEntryDisp {
public:
	GTKChapDisp(GtkWidget *_gtkText,
		    BackEnd   *_be)
	    : GTKEntryDisp(_gtkText, _be)
	{
	}
	virtual char Display(SWModule &imodule);
	virtual void getVerseBefore(SWModule &imodule);
	virtual void getVerseAfter(SWModule &imodule);

private:
};

#ifndef USE_GTKMOZEMBED
class GTKTextviewChapDisp : public GTKEntryDisp {
public:
	GTKTextviewChapDisp(GtkWidget *_gtkText,
			    BackEnd   *_be)
	    : GTKEntryDisp(_gtkText, _be)
	{
	}
	virtual char Display(SWModule &imodule);
};
#endif

class DialogEntryDisp : public SWDisplay {
public:
	DialogEntryDisp(GtkWidget   *_gtkText,
			DIALOG_DATA *_d,
			BackEnd     *_be,
			GLOBAL_OPS  *_ops)
	    : gtkText(_gtkText),
	      d(_d),
	      be(_be),
	      ops(_ops),
	      swbuf(""),
	      is_rtol(FALSE)
	{
	}
	virtual char Display(SWModule &imodule);

protected:
	GtkWidget   *gtkText;
	DIALOG_DATA *d;
	BackEnd     *be;
	GLOBAL_OPS  *ops;
	SWBuf       swbuf;
	gboolean    is_rtol;
};

class DialogChapDisp : public  DialogEntryDisp {
public:
	DialogChapDisp(GtkWidget   *_gtkText,
		       DIALOG_DATA *_d,
		       BackEnd     *_be,
		       GLOBAL_OPS  *_ops)
	    : DialogEntryDisp(_gtkText, _d, _be, _ops)
	{
	}
	virtual char Display(SWModule &imodule);
};

#ifndef USE_GTKMOZEMBED
class DialogTextviewChapDisp : public  DialogEntryDisp {
public:
	DialogTextviewChapDisp(GtkWidget   *_gtkText,
			       DIALOG_DATA *_d,
			       BackEnd     *_be,
			       GLOBAL_OPS  *_ops)
	    : DialogEntryDisp(_gtkText, _d, _be, _ops)
	{
	}
	virtual char Display(SWModule &imodule);
};
#endif

class GTKPrintEntryDisp : public SWDisplay {
public:	
	GTKPrintEntryDisp(GtkWidget *_gtkText,
			  BackEnd   *_be)
	    : gtkText(_gtkText),
	      be(_be)
	{
	}
	virtual char Display(SWModule &imodule);

protected:
	GtkWidget *gtkText;
	BackEnd   *be;
};

class GTKPrintChapDisp : public GTKPrintEntryDisp {
public:
	GTKPrintChapDisp(GtkWidget *_gtkText,
			 BackEnd   *_be)
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
