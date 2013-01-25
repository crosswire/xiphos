/*
 * Xiphos Bible Study Tool
 * export_passage.cc -
 *
 * Copyright (C) 2008-2011 Xiphos Developer Team
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

#include <swmgr.h>
#include <swmodule.h>

#include "backend/sword_main.hh"

#include "gui/export_dialog.h"

#include "main/export_passage.h"
#include "main/settings.h"
#include "main/sword.h"

#define	HTML_START	"<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\"><!-- A { text-decoration:none } *[dir=rtl] { text-align: right; } --></style></head><body>"

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
    return key.getVerse();
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

static void _export_book(EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = 1;
    int curBook = key->getBook();
	int mychapter = 1;
	int myverse = 1;
    key->setChapter(1);
    key->setVerse(1);

	if (type == HTML)
		g_string_append_printf(str,
				       "%s<center>%s<br/>Chapter %d</center><br/>",
				       HTML_START,
                       mod->getDescription(),
				       1);
	else
		g_string_append_printf(str,"%s\n\nChapter %d\n\n",
                       mod->getDescription(),
				       1);

    while (key->getBook() == curBook && !mod->popError()) {
        if (key->getChapter() != curChapter) {
			++mychapter;
			myverse = 1;
            curChapter = key->getChapter();
			if (type == HTML)
				g_string_append_printf(str,"<br/><center>Chapter %d</center><br/>", curChapter);
			else
				g_string_append_printf(str,"\n\nChapter %d\n\n", curChapter);
		}

		if (data.verse_num)
			g_string_append_printf(str,
					       ((type == HTML)
						? "&nbsp;&nbsp;[%d]"
						: "  [%d]"),
					       myverse);

		if (type == HTML)
			g_string_append_printf(str, " %s%s",
                           (char*)mod->renderText(),
					       (settings.versestyle ? "<br/>" : ""));
		else
			g_string_append_printf(str, " %s%s",
                           (char*)mod->stripText(),
					       (settings.versestyle ? "\n" : ""));

		++myverse;
		(*mod)++;
	}
	if (type == HTML)
		g_string_append_printf(str,"%s","</body></html>");
	if (data.filename)
		_save(data.filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	g_string_free(str,TRUE);
}

static void _export_chapter(EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	char* book;
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
    int curChapter = key->getChapter();
    int curBook = key->getBook();
	int myverse = 1;

	book = backend->key_get_book(settings.currentverse);
	if (type == HTML)
		g_string_append_printf(str,
				       "%s%s: %s Chapter %d<br/><br/>",
				       HTML_START,
                       mod->getDescription(),
				       book,
                       key->getChapter());
	else
		g_string_append_printf(str,
				       "%s: %s Chapter %d\n\n",
                       mod->getDescription(),
				       book,
                       key->getChapter());

    for (key->setVerse(1);
         (key->getBook() == curBook) &&
         (key->getChapter() == curChapter) &&
         !mod->popError();
	     (*mod)++) {

		if (data.verse_num)
			g_string_append_printf(str, " [%d]", myverse);

		if (type == HTML)
			g_string_append_printf(str," %s%s",
                           (char*)mod->renderText(),
					       (settings.versestyle ? "<br/>" : ""));
		else
			g_string_append_printf(str," %s%s",
                           (char*)mod->stripText(),
					       (settings.versestyle ? "\n" : ""));
		++myverse;
	}
	if (type == HTML)
		g_string_append_printf(str,"%s","</body></html>");
	if (data.filename)
		_save(data.filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	g_string_free(str,TRUE);
	if (book)
		g_free(book);
}


static void _export_verse(EXPORT_DATA data, int type)
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
				       "%s%s%s (%s %d:%d [%s])</body></html>",
				       HTML_START,
                       (char*)mod->renderText(),
				       (settings.versestyle ? "<br/>" : ""),
				       book,
                       key->getChapter(),
                       key->getVerse(),
				       settings.MainWindowModule);
	else
		g_string_append_printf(str,
				       "%s%s (%s %d:%d [%s])",
                       (char*)mod->stripText(),
				       (settings.versestyle ? "\n" : ""),
				       book,
                       key->getChapter(),
                       key->getVerse(),
				       settings.MainWindowModule);

	if (data.filename)
		_save(data.filename, str->str,str->len);
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
    int curChapter = key->getChapter();
    int curBook = key->getBook();
	//int myverse = 1;

	// special case: one verse range => single verse export.
	if (data.start_verse == data.end_verse) {
		_export_verse(data, type);
		return;
	}

	book = backend->key_get_book(settings.currentverse);

	if (type == HTML)
		g_string_append_printf(str, "%s", HTML_START);

    for (key->setVerse(data.start_verse);
         (key->getVerse() <= data.end_verse) &&
         (key->getBook() == curBook) &&
         (key->getChapter() == curChapter) &&
         !mod->popError();
	     (*mod)++) {

		if (data.verse_num)
            g_string_append_printf(str, "[%d]", key->getVerse());

		if (type == HTML)
			g_string_append_printf(str," %s%s ",
                           (char*)mod->renderText(),
					       (settings.versestyle ? "<br/>" : ""));
		else
			g_string_append_printf(str," %s%s ",
                           (char*)mod->stripText(),
					       (settings.versestyle ? "\n" : ""));
	}

	g_string_append_printf(str,
			       "%s(%s %d:%d-%d [%s])",
			       ((type == HTML) ? "<br/>" : "\n"),
			       book,
                   key->getChapter(),
			       data.start_verse,
			       data.end_verse,
			       settings.MainWindowModule);

	if (type == HTML)
		g_string_append_printf(str, "</body></html>");
	if (data.filename)
		_save(data.filename, str->str, str->len);
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
			_export_book(data, HTML);
			break;
		case CHAPTER:
			_export_chapter(data, HTML);
			break;
		case VERSE:
			_export_verse(data, HTML);
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
			_export_book(data, PLAIN);
			break;
		case CHAPTER:
			_export_chapter(data, PLAIN);
			break;
		case VERSE:
			_export_verse(data, PLAIN);
			break;
		case VERSE_RANGE:
			_export_verse_range(data, PLAIN);
			break;
	}
}
