/*
 * Xiphos Bible Study Tool
 * display.hh -
 *
 * Copyright (C) 2000-2026 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdint.h>

#ifdef __cplusplus
#include <gtk/gtk.h>
#include <swmgr.h>
#include <swdisp.h>
#include "main/gtk_compat.h"
#include "main/global_ops.hh"
#include "backend/sword_main.hh"
#include "gui/utilities.h"

using namespace sword;

class GTKEntryDisp : public SWDisplay
{
      public:
	GTKEntryDisp(GtkWidget *_gtkText,
		     BackEnd *_be)
	    : gtkText(_gtkText),
	      be(_be),
	      swbuf(""),
	      mf(NULL),
	      ops(NULL),
	      buf(NULL),
	      mod_column_count(NULL),
	      is_rtol(FALSE),
	      strongs_or_morph(FALSE),
	      strongs_and_morph(FALSE),
	      cache_flags(0)
	{
	}
	virtual char display(SWModule &imodule);
	virtual char displayByChapter(SWModule &imodule,
				      int columns);

      protected:
	GtkWidget *gtkText;
	BackEnd *be;
	SWBuf swbuf;
	MOD_FONT *mf;
	GLOBAL_OPS *ops;

	gboolean is_rtol;

	gboolean strongs_or_morph;
	gboolean strongs_and_morph;

	int cache_flags;

	gchar *buf, *mod_column_count;
};

class GTKChapDisp : public GTKEntryDisp
{
      public:
	GTKChapDisp(GtkWidget *_gtkText,
		    BackEnd *_be)
	    : GTKEntryDisp(_gtkText, _be)
	{
	}
	virtual char display(SWModule &imodule);
	virtual GString *introMaterial(SWModule &imodule, int chapter);
	virtual void getVerseBefore(SWModule &imodule);
	virtual void getVerseAfter(SWModule &imodule);
	virtual void RenderOneChapter(SWModule &imodule, int chapter);
	virtual void RenderWholeBook(SWModule &imodule);

      private:
	int curTest, curBook, curChapter, curVerse;
	VerseKey *key;
};

class DialogEntryDisp : public SWDisplay
{
      public:
	DialogEntryDisp(GtkWidget *_gtkText,
			DIALOG_DATA *_d,
			BackEnd *_be)
	    : gtkText(_gtkText),
	      d(_d),
	      be(_be),
	      swbuf(""),
	      mf(NULL),
	      ops(NULL),
	      buf(NULL),
	      mod_column_count(NULL),
	      is_rtol(FALSE),
	      strongs_or_morph(FALSE),
	      strongs_and_morph(FALSE),
	      cache_flags(0)
	{
	}
	virtual char display(SWModule &imodule);
	virtual char displayByChapter(SWModule &imodule,
				      int columns);

      protected:
	GtkWidget *gtkText;
	DIALOG_DATA *d;
	BackEnd *be;
	SWBuf swbuf;
	MOD_FONT *mf;
	GLOBAL_OPS *ops;

	gboolean is_rtol;

	gboolean strongs_or_morph;
	gboolean strongs_and_morph;

	int cache_flags;

	gchar *buf, *mod_column_count;
};

class DialogChapDisp : public DialogEntryDisp
{
      public:
	DialogChapDisp(GtkWidget *_gtkText,
		       DIALOG_DATA *_d,
		       BackEnd *_be)
	    : DialogEntryDisp(_gtkText, _d, _be)
	{
	}
	virtual char display(SWModule &imodule);

      private:
	int curTest, curBook, curChapter, curVerse;
	VerseKey *key;
};

class GTKPrintEntryDisp : public SWDisplay
{
      public:
	GTKPrintEntryDisp(GtkWidget *_gtkText,
			  BackEnd *_be)
	    : gtkText(_gtkText),
	      be(_be)
	{
	}
	virtual char display(SWModule &imodule);

      protected:
	GtkWidget *gtkText;
	BackEnd *be;
};

class GTKPrintChapDisp : public GTKPrintEntryDisp
{
      public:
	GTKPrintChapDisp(GtkWidget *_gtkText,
			 BackEnd *_be)
	    : GTKPrintEntryDisp(_gtkText, _be)
	{
	}
	virtual char display(SWModule &imodule);
	MOD_FONT *mf;
};

extern "C" {
#endif /* __cplusplus */

void markedCacheFill(const gchar *modname, gchar *key);

#ifdef __cplusplus
}
#endif
