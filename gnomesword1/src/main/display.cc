/*
 * GnomeSword Bible Study Tool
 * display.cc - 
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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


#include <swmgr.h>
#include <versekey.h>
#include <regex.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif
#include <gtkhtml/gtkhtml.h>

#ifdef __cplusplus
}
#endif
	
#include "main/display.hh"
#include "main/settings.h"

#include "gui/utilities.h"
#include "gui/widgets.h"

#include "backend/module.hh"
#include "backend/tree.hh"

	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

using namespace sword;
using namespace std;

	
char GTKEntryDisp::Display(SWModule &imodule) 
{
	GString *str = g_string_new(NULL);
	const gchar *keytext = NULL;
	int curPos = 0;
	GtkHTML *html = GTK_HTML(gtkText);
	MOD_FONT *mf = get_font(imodule.Name());
	(const char *)imodule;	// snap to entry
	if(backend_get_mod_type(imodule.Name()) == 3)
		keytext = backend_gbs_get_treekey_local_name(
			backend_gbs_get_treekey_offset());
	else
		keytext = imodule.KeyText();
	
	g_string_printf(str, 	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<a href=\"about://%s/%s\">[%s]</a>[%s]<br>"
				"<font face=\"%s\" size=\"%s\">%s"
				"</font></body></html>",
				settings.bible_bg_color, 
				settings.bible_text_color,
				settings.link_color,
				imodule.Name(),
				imodule.Description(),
				imodule.Name(),
				(gchar*)keytext,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(const char *)imodule);	
	
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	if (str->len)
		gtk_html_load_from_string(html,str->str,str->len);
	gtk_html_set_editable(html, was_editable);
	g_string_free(str, TRUE);
	free_font(mf);	
}



char GTKChapDisp::Display(SWModule &imodule) 
{
	char tmpBuf[255];
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;

	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error()); imodule++) {
		
		
	}
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
}
