/*
 * Xiphos Bible Study Tool
 * export_passage.cc -
 *
 * Copyright (C) 2008-2017 Xiphos Developer Team
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
#include "main/global_ops.hh"

#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><style type=\"text/css\"><!-- A { text-decoration:none } *[dir=rtl] { text-align: right; } .transChangeSupplied { font-style: italic; } --></style></head><body>"

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
    {(gchar *)"text/html", 0, TARGET_HTML},
    {(gchar *)"UTF8_STRING", 0, TARGET_UTF8_STRING},
    {(gchar *)"COMPOUND_TEXT", 0, TARGET_COMPOUND_TEXT},
    {(gchar *)"STRING", 0, TARGET_STRING},
    {(gchar *)"TEXT", 0, TARGET_TEXT}};

/* there is probably a better way to do this */
static gchar *copy_text;

/**
 * clipboardreq_get:
 * @clipboard:
 * @selection_data:
 * @info:
 * @user_data:
 *
 * This function is called every time a user pastes in another application
 **/
static void clipboardreq_get(GtkClipboard *clipboard,
			     GtkSelectionData *selection_data,
			     guint info, gpointer user_data)
{
	gchar *text = g_strdup(copy_text);
	if (info == TARGET_HTML) {
		gtk_selection_data_set(selection_data, gdk_atom_intern("text/html", FALSE),
				       16, (const guchar *)text, strlen(text));
	} else {
		gtk_selection_data_set_text(selection_data, text, strlen(text));
	}

	g_free(text);

	return;
}

int main_get_max_verses(const char *name)
{
	SWModule *mod = backend->get_SWModule(name);
	if (!mod)
		return 1;

	VerseKey *key = (VerseKey *)mod->createKey();
	key->setText(settings.currentverse);
	int max = key->getVerseMax();
	delete key;
	return max;
}

int main_get_current_verse(const char *name)
{
	SWModule *mod = backend->get_SWModule(name);
	if (!mod)
		return 1;

	VerseKey *key = (VerseKey *)mod->createKey();
	key->setText(settings.currentverse);
	int v = key->getVerse();
	delete key;
	return v;
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
static void _copy_to_clipboard(EXPORT_DATA data, char *text, int len)
{
	GtkClipboard *clipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);

	if (copy_text)
		g_free(copy_text);
	copy_text = g_strdup(text);
	gtk_clipboard_set_with_data(clipboard, selection_targets,
				    G_N_ELEMENTS(selection_targets),
				    (GtkClipboardGetFunc)clipboardreq_get,
				    NULL,
				    NULL);
	if (data.bookheader)
		g_free(data.bookheader);
	if (data.chapterheader_book)
		g_free(data.chapterheader_book);
	if (data.chapterheader_chapter)
		g_free(data.chapterheader_chapter);
	if (data.versenumber)
		g_free(data.versenumber);
	if (data.verselayout_single_verse_ref_last)
		g_free(data.verselayout_single_verse_ref_last);
	if (data.verselayout_single_verse_ref_first)
		g_free(data.verselayout_single_verse_ref_first);
	if (data.verse_range_verse)
		g_free(data.verse_range_verse);
}

static void _save(EXPORT_DATA data, char *text, int len)
{
	XI_message(("%s", data.filename));
	g_file_set_contents(data.filename, text, len, NULL);
	if (data.filename)
		g_free(data.filename);
	if (data.bookheader)
		g_free(data.bookheader);
	if (data.chapterheader_book)
		g_free(data.chapterheader_book);
	if (data.chapterheader_chapter)
		g_free(data.chapterheader_chapter);
	if (data.versenumber)
		g_free(data.versenumber);
	if (data.verselayout_single_verse_ref_last)
		g_free(data.verselayout_single_verse_ref_last);
	if (data.verselayout_single_verse_ref_first)
		g_free(data.verselayout_single_verse_ref_first);
	if (data.verse_range_verse)
		g_free(data.verse_range_verse);
}

static void _export_book(EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	//gchar *buf;
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
				       data.bookheader,
				       HTML_START,
				       (data.version ? mod->getDescription() : ""),
				       1);
	else
		g_string_append_printf(str, data.plain_bookheader,
				       mod->getDescription(),
				       1);
	//g_free(buf);
	while (key->getBook() == curBook && !mod->popError()) {
		if (key->getChapter() != curChapter) {
			++mychapter;
			myverse = 1;
			curChapter = key->getChapter();
			if (type == HTML)
				g_string_append_printf(str, data.chapterheader_book, curChapter);
			else
				g_string_append_printf(str, data.plain_chapterheader_book, curChapter);
		}

		if (data.verse_num)
			g_string_append_printf(str,
					       ((type == HTML)
						    ? data.versenumber
						    : data.plain_versenumber),
					       myverse);

		if (type == HTML)
			g_string_append_printf(str, " %s%s",
					       mod->renderText().c_str(),
					       (settings.versestyle ? "<br>" : ""));
		else
			g_string_append_printf(str, " %s%s",
					       (char *)mod->stripText(),
					       (settings.versestyle ? "\n" : ""));

		++myverse;
		(*mod)++;
	}
	if (type == HTML)
		g_string_append_printf(str, "%s", "</body></html>");
	if (data.filename)
		_save(data, str->str, str->len);
	else
		_copy_to_clipboard(data, str->str, str->len);
	g_string_free(str, TRUE);
}

static void _export_chapter(EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	char *book;
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = key->getChapter();
	int curBook = key->getBook();
	int myverse = 1;

	book = backend->key_get_book(settings.MainWindowModule, settings.currentverse);
	if (type == HTML)
		g_string_append_printf(str,
				       data.chapterheader_chapter,
				       HTML_START,
				       (data.version ? mod->getDescription() : ""),
				       book,
				       key->getChapter());
	else
		g_string_append_printf(str,
				       data.plain_chapterheader_chapter,
				       mod->getDescription(),
				       book,
				       key->getChapter());

	for (key->setVerse(1);
	     (key->getBook() == curBook) && (key->getChapter() == curChapter) && !mod->popError();
	     (*mod)++) {

		if (data.verse_num)
			g_string_append_printf(str,
					       ((type == HTML)
						    ? data.versenumber
						    : data.plain_versenumber),
					       myverse);

		if (type == HTML)
			g_string_append_printf(str, " %s%s",
					       mod->renderText().c_str(),
					       (settings.versestyle ? "<br>" : ""));
		else
			g_string_append_printf(str, " %s%s",
					       (char *)mod->stripText(),
					       (settings.versestyle ? "\n" : ""));
		++myverse;
	}
	if (type == HTML)
		g_string_append_printf(str, "%s", "</body></html>");
	if (data.filename)
		_save(data, str->str, str->len);
	else
		_copy_to_clipboard(data, str->str, str->len);
	g_string_free(str, TRUE);
	if (book)
		g_free(book);
}

static void _export_verse(EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	char *book;
	char *modstr = g_strdup_printf(" [%s]", settings.MainWindowModule);
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);

	book = backend->key_get_book(settings.MainWindowModule, settings.currentverse);
	if (type == HTML)
		if (data.reference_last)
			g_string_append_printf(str,
					       data.verselayout_single_verse_ref_last,
					       HTML_START,
					       mod->renderText().c_str(),
					       book,
					       key->getChapter(),
					       key->getVerse(),
					       (data.version ? modstr : ""));
		else
			g_string_append_printf(str,
					       data.verselayout_single_verse_ref_first,
					       HTML_START,
					       book,
					       key->getChapter(),
					       key->getVerse(),
					       (data.version ? modstr : ""),
					       mod->renderText().c_str());
	else if (data.reference_last)
		g_string_append_printf(str,
				       data.plain_verselayout_single_verse_ref_last,
				       (char *)mod->stripText(),
				       book,
				       key->getChapter(),
				       key->getVerse(),
				       (data.version ? modstr : ""));
	else
		g_string_append_printf(str,
				       data.plain_verselayout_single_verse_ref_first,
				       book,
				       key->getChapter(),
				       key->getVerse(),
				       (data.version ? modstr : ""),
				       (char *)mod->stripText());

	if (data.filename)
		_save(data, str->str, str->len);
	else
		_copy_to_clipboard(data, str->str, str->len);

	g_string_free(str, TRUE);

	if (book)
		g_free(book);
	if (modstr)
		g_free(modstr);
}

static void _export_verse_range(EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	char *book;
	SWMgr *mgr = backend->get_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = key->getChapter();
	int curBook = key->getBook();
	char *modstr = g_strdup_printf(" [%s]", settings.MainWindowModule);
	//int myverse = 1;

	// special case: one verse range => single verse export.
	if (data.start_verse == data.end_verse) {
		_export_verse(data, type);
		return;
	}

	book = backend->key_get_book(settings.MainWindowModule, settings.currentverse);

	if (type == HTML)
		g_string_append_printf(str, "%s", HTML_START);

	if (!data.reference_last)
		g_string_append_printf(str,
				       ((type == HTML)
					    ? data.verse_range_ref_first
					    : data.plain_verse_range_ref_first),
				       book,
				       key->getChapter(),
				       data.start_verse,
				       data.end_verse,
				       (data.version ? modstr : ""));

	for (key->setVerse(data.start_verse);
	     (key->getVerse() <= data.end_verse) &&
		 (key->getBook() == curBook) &&
		 (key->getChapter() == curChapter) &&
		 !mod->popError();
	     (*mod)++) {

		if (data.verse_num)
			g_string_append_printf(str,
					       ((type == HTML)
						    ? data.versenumber
						    : data.plain_versenumber),
					       key->getVerse());

		if (type == HTML)
			g_string_append_printf(str, data.verse_range_verse,
					       mod->renderText().c_str(),
					       (settings.versestyle ? "<br>" : " "));
		else
			g_string_append_printf(str, data.verse_range_verse,
					       (char *)mod->stripText(),
					       (settings.versestyle ? "\n" : " "));
	}
	// back up one verse.
	// if we have dumped the last verse of a chapter, then we will have stepped
	// into next chapter in order to end loop above.  this reverts us correctly.
	(*mod)--;

	if (data.reference_last)
		g_string_append_printf(str,
				       ((type == HTML) ? data.verse_range_ref_last
						       : data.plain_verse_range_ref_last),
				       ((type == HTML) ? "<br>" : "\n"),
				       book,
				       key->getChapter(),
				       data.start_verse,
				       data.end_verse,
				       (data.version ? modstr : ""));

	if (type == HTML)
		g_string_append_printf(str, "</body></html> ");
	if (data.filename)
		_save(data, str->str, str->len);
	else
		_copy_to_clipboard(data, str->str, str->len);
	g_string_free(str, TRUE);
	if (book)
		g_free(book);
	if (modstr)
		g_free(modstr);
}

void main_export_content(EXPORT_DATA data, gint format)
{
	int style = (format ? HTML : PLAIN);

	// no markers are left in exported text.
	_set_global_textual("Cross-references", "Off");
	_set_global_textual("Footnotes", "Off");

	switch (data.passage_type) {
	case BOOK:
		_export_book(data, style);
		break;
	case CHAPTER:
		_export_chapter(data, style);
		break;
	case VERSE:
		_export_verse(data, style);
		break;
	case VERSE_RANGE:
		_export_verse_range(data, style);
		break;
	}
}
