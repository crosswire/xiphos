/*
 * Xiphos Bible Study Tool
 * export_passage.cc -
 *
 * Copyright (C) 2008-2009 Xiphos Developer Team
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
//#ifdef USE_EXPORTER

#include <swmgr.h>
#include <swmodule.h>

#include "backend/sword_main.hh"

#include "gui/export_dialog.h"

#include "main/export_passage.h"
#include "main/settings.h"
#include "main/sword.h"


#define HTML_START "<HTML><HEAD><META HTTP-EQUIV=\"content-type\" CONTENT=\"text/html; CHARSET=utf-8\"><STYLE TYPE=\"text/css\"><!-- A { text-decoration:none } %s --></STYLE></HEAD><BODY>"

enum {
        TARGET_HTML,
        TARGET_UTF8_STRING,
        TARGET_COMPOUND_TEXT,
        TARGET_STRING,
        TARGET_TEXT
};

/* these targets allow fallback to plain text if the target doesn't
   want html */

GtkTargetEntry selection_targets[] = {
        { (gchar *) "text/html", 0, TARGET_HTML },
        { (gchar *) "UTF8_STRING", 0, TARGET_UTF8_STRING },
        { (gchar *) "COMPOUND_TEXT", 0, TARGET_COMPOUND_TEXT },
        { (gchar *) "STRING", 0, TARGET_STRING },
        { (gchar *) "TEXT", 0, TARGET_TEXT}
};

/* there is probably a better way to do this */
static gchar* copy_text;

/**
 * clipboardreq_get:
 * @clipboard:
 * @selection_data:
 * @info:
 * @user_data:
 *
 * This function is called every time a user pastes in another application
 **/
static void clipboardreq_get (GtkClipboard *clipboard,
			      GtkSelectionData *selection_data,
			      guint info, gpointer user_data)
{
	gchar* text = g_strdup(copy_text);
        if (info == TARGET_HTML)
        {
                gtk_selection_data_set(selection_data, gdk_atom_intern("text/html", FALSE),
				       16, (const guchar*)text, strlen(text));

        } else {
                gtk_selection_data_set_text(selection_data, text, strlen(text));
        }

        g_free (text);

        return;
}


int main_get_max_verses (void)
{
	VerseKey key = settings.currentverse;
	return (key.getVerseMax());
}

int main_get_current_verse (void)
{
	VerseKey key = settings.currentverse;
	return key.Verse();
}

/**
 * _copy_to_clipboard:
 * @text: #gchar text to copy
 * @len: #gint length (unused now)
 *
 * Copies the data to the clipboard as HTML if applicable (that is, HTML data,
 * so that smart word processors such as OO.o will preserve markup; at this point,
 * the HTML is *not* preserving the font.
 **/
static void _copy_to_clipboard (char* text,int len)
{
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);

	if (copy_text)
		g_free (copy_text);
	copy_text = g_strdup (text);
	gtk_clipboard_set_with_data (clipboard, selection_targets,
				     G_N_ELEMENTS (selection_targets),
				     (GtkClipboardGetFunc)clipboardreq_get,
				     NULL,
				     NULL);
}

static void _save(char *filename, char* text,int len)
{
	GS_message(("%s",filename));
	g_file_set_contents(filename, text, len, NULL);
	if (filename)
		g_free(filename);
}

static void _export_book(char *filename, int type)
{
	GString *str = g_string_new(NULL);
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = 1;
	int curBook = key->Book();
	int mychapter = 1;
	int myverse = 1;
	key->Chapter(1);
	key->Verse(1);

	if (type == HTML)
		g_string_append_printf(str,
				       "%s<BR><CENTER>%s<BR>Chapter %d</CENTER><BR><BR>",
				       HTML_START,
				       mod->Description(),
				       1);
	else
		g_string_append_printf(str,"\n%s\nChapter %d\n\n",
				       mod->Description(),
				       1);

	while (key->Book() == curBook && !mod->Error()) {
		if (key->Chapter() != curChapter) {
			++mychapter;
			myverse = 1;
			curChapter = key->Chapter();
			if (type == HTML)
				g_string_append_printf(str,"<BR><CENTER>Chapter %d</CENTER><BR><BR>", curChapter);
			else
				g_string_append_printf(str,"\nChapter %d\n\n", curChapter);
		}

		if (settings.showversenum)
			g_string_append_printf(str,
					       ((type == HTML)
						? "&nbsp;&nbsp;[%d]"
						: "  [%d]"),
					       myverse);

		if (type == HTML)
			g_string_append_printf(str, " %s%s",
					       (char*)mod->RenderText(),
					       (settings.versestyle ? "<BR>" : ""));
		else
			g_string_append_printf(str, " %s%s",
					       (char*)mod->StripText(),
					       (settings.versestyle ? "\n" : ""));

		++myverse;
		(*mod)++;
	}
	if (type == HTML)
		g_string_append_printf(str,"%s","</BODY></HTML>");
	if (filename)
		_save(filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	g_string_free(str,TRUE);
}

static void _export_chapter(char *filename, int type)
{
	GString *str = g_string_new(NULL);
	char* book;
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int myverse = 1;

	book = backend->key_get_book(settings.currentverse);
	if (type == HTML)
		g_string_append_printf(str,
				       "%s<BR>%s: %s Chapter %d<BR><BR>",
				       HTML_START,
				       mod->Description(),
				       book,
				       key->Chapter());
	else
		g_string_append_printf(str,
				       "\n%s: %s Chapter %d\n\n",
				       mod->Description(),
				       book,
				       key->Chapter());

	for (key->Verse(1);
	     (key->Book() == curBook) &&
	     (key->Chapter() == curChapter) &&
	     !mod->Error();
	     (*mod)++) {

		if (settings.showversenum)
			g_string_append_printf(str, " [%d]", myverse);

		if (type == HTML)
			g_string_append_printf(str," %s%s",
					       (char*)mod->RenderText(),
					       (settings.versestyle ? "<BR>" : ""));
		else
			g_string_append_printf(str," %s%s",
					       (char*)mod->StripText(),
					       (settings.versestyle ? "\n" : ""));
		++myverse;
	}
	if (type == HTML)
		g_string_append_printf(str,"%s","</BODY></HTML>");
	if (filename)
		_save(filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	g_string_free(str,TRUE);
	if (book)
		g_free(book);
}


static void _export_verse(char *filename, int type)
{
	GString *str = g_string_new(NULL);
	char* book;
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);

	book = backend->key_get_book(settings.currentverse);
	if (type == HTML)
		g_string_append_printf(str,
				       "%s%s: %s %d:%d. %s%s</BODY></HTML>",
				       HTML_START,
				       settings.MainWindowModule,
				       book,
				       key->Chapter(),
				       key->Verse(),
				       (char*)mod->RenderText(),
				       (settings.versestyle ? "<BR>" : ""));
	else
		g_string_append_printf(str,
				       "\n%s: %s %d:%d. %s%s",
				       settings.MainWindowModule,
				       book,
				       key->Chapter(),
				       key->Verse(),
				       (char*)mod->StripText(),
				       (settings.versestyle ? "\n" : ""));

	if (filename)
		_save(filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);

	g_string_free(str,TRUE);

	if (book)
		g_free(book);
}

static void _export_verse_range (EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	char* book;
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = key->Chapter();
	int curBook = key->Book();
	//int myverse = 1;

	book = backend->key_get_book(settings.currentverse);
	if (type == HTML)
		g_string_append_printf(str,
				       (settings.showversenum
					? "%s<BR>%s: %s Chapter %d<BR><BR>"
					: "%s<BR>%s: %s %d:%d-%d<BR><BR>"),
				       HTML_START,
				       settings.MainWindowModule,
				       book,
				       key->Chapter(),
				       data.start_verse,
				       data.end_verse);
	else
		g_string_append_printf(str,
				       (settings.showversenum
					? "\n%s: %s Chapter %d\n\n"
					: "\n%s: %s %d:%d-%d\n\n"),
				       settings.MainWindowModule,
				       book,
				       key->Chapter(),
				       data.start_verse,
				       data.end_verse);

	for (key->Verse(data.start_verse);
	     (key->Verse() <= data.end_verse) &&
	     (key->Book() == curBook) &&
	     (key->Chapter() == curChapter) &&
	     !mod->Error();
	     (*mod)++) {

		if (settings.showversenum)
			g_string_append_printf(str, " [%d]", key->Verse());

		if (type == HTML)
			g_string_append_printf(str," %s%s",
					       (char*)mod->RenderText(),
					       (settings.versestyle ? "<BR>" : ""));
		else
			g_string_append_printf(str," %s%s",
					       (char*)mod->StripText(),
					       (settings.versestyle ? "\n" : ""));
	}
	if (type == HTML)
		g_string_append_printf(str,"%s","</BODY></HTML>");
	if (data.filename)
		_save(data.filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	g_string_free(str,TRUE);
	if (book)
		g_free(book);
}


void main_export_html(EXPORT_DATA data)
{
	switch (data.passage_type) {
		case BOOK:
			_export_book(data.filename, HTML);
			break;
		case CHAPTER:
			_export_chapter(data.filename, HTML);
			break;
		case VERSE:
			_export_verse(data.filename, HTML);
			break;
		case VERSE_RANGE:
			_export_verse_range(data, HTML);
			break;
	}
}

void main_export_plain(EXPORT_DATA data)
{
	switch (data.passage_type) {
		case BOOK:
			_export_book(data.filename, PLAIN);
			break;
		case CHAPTER:
			_export_chapter(data.filename, PLAIN);
			break;
		case VERSE:
			_export_verse(data.filename, PLAIN);
			break;
		case VERSE_RANGE:
			_export_verse_range(data, PLAIN);
			break;
	}
}
//#endif
