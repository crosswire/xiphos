/*
 * Xiphos Bible Study Tool
 * export_passage.cc - 
 *
 * Copyright (C) 2008 Xiphos Developer Team
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

#include "gecko/gecko-html.h"

#define HTML_START "<HTML><HEAD><META HTTP-EQUIV=\"content-type\" CONTENT=\"text/html; CHARSET=utf-8\"><STYLE TYPE=\"text/css\"><!-- A { text-decoration:none } %s --></STYLE></HEAD><BODY>"

int main_get_max_verses (void)
{
	VerseKey key = settings.currentverse;
#ifdef SWORD_MULTIVERSE
	return (key.getVerseMax());
#else
	return (key.books[key.Testament()-1] [key.Book()-1].
				 versemax[key.Chapter()-1]);
#endif
}

int main_get_current_verse (void)
{
	VerseKey key = settings.currentverse;
	return key.Verse();
}

static void _copy_to_clipboard (char* text,int len)
{
	GtkClipboard *clipboard = gtk_clipboard_get (GDK_SELECTION_CLIPBOARD);
	 
	gtk_clipboard_set_text (clipboard, text, len);
	gecko_html_copy_selection (GECKO_HTML (text));
}

static void _save(char *filename, char* text,int len)
{
	GS_message((filename));
	g_file_set_contents(filename, text, len, NULL);
	if(filename)
		g_free(filename);	
}

static void _export_book(char *filename, int type)
{
	GString *str = g_string_new(NULL);
	SWMgr *mgr = backend->get_main_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = 1;
	int curBook = key->Book();
	int mychapter = 1;
	int myverse = 1;
	key->Chapter(1);
	key->Verse(1);
	
	if(type == HTML)
		g_string_append_printf(str,
				       "%s<BR><CENTER>%s<BR>Chapter %d</CENTER><BR>", 
				       HTML_START,
				       mod->Description(),
				       1);
	else
		g_string_append_printf(str,"\n%s\nChapter %d\n",
				       mod->Description(), 
				       1);
		
	
	while(key->Book() == curBook && !mod->Error()) {
		if(key->Chapter() != curChapter) {
			++mychapter;
			myverse = 1;
			curChapter = key->Chapter();
			if(type == HTML)
				g_string_append_printf(str,"<BR><CENTER>Chapter %d</CENTER><BR>", curChapter);
			else
				g_string_append_printf(str,"\nChapter %d\n", curChapter);
				
		}
		if(type == HTML)
			g_string_append_printf(str,"&nbsp;&nbsp;%d. %s%s",
					       myverse,
					       (char*)mod->RenderText(), 
					       settings.versestyle ? "<BR>" : "");
		else
			g_string_append_printf(str,"  %d. %s%s",
					       myverse,
					       (char*)mod->StripText(), 
					       settings.versestyle ? "\n" : "");
			
		++myverse;
		(*mod)++;
	}
	if(type == HTML)
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
	SWMgr *mgr = backend->get_main_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int myverse = 1;
	
	book = backend->key_get_book(settings.currentverse);
	if(type == HTML)
		g_string_append_printf(str,
				       "%s<BR>%s: %s Chapter %d<BR>", 
				       HTML_START, 
				       settings.MainWindowModule,
				       book, 
				       key->Chapter());
	else
		g_string_append_printf(str,
				       "\n%s: %s Chapter %d\n", 
				       settings.MainWindowModule,
				       book, 
				       key->Chapter());		
	
	for (key->Verse(1);
	     (key->Book() == curBook) &&
	     (key->Chapter() == curChapter) &&
	     !mod->Error();
	     (*mod)++) {
		     
		if(type == HTML)
			g_string_append_printf(str,"%d. %s%s",
					       myverse,
					       (char*)mod->RenderText(), 
					       settings.versestyle ? "<BR>" : "");
		else
			g_string_append_printf(str,"%d. %s%s",
					       myverse,
					       (char*)mod->StripText(), 
					       settings.versestyle ? "\n" : "");
			
		++myverse;
	}
	if(type == HTML)
		g_string_append_printf(str,"%s","</BODY></HTML>");
	if (filename) 
		_save(filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	g_string_free(str,TRUE);
	if(book)
		g_free(book);
	
}


static void _export_verse(char *filename, int type)
{
	GString *str = g_string_new(NULL);
	char* book;
	SWMgr *mgr = backend->get_main_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	
	book = backend->key_get_book(settings.currentverse);
	if(type == HTML)
		g_string_append_printf(str,
				       "%s%s: %s %d:%d. %s%s</BODY></HTML>", 
				       HTML_START, 
				       settings.MainWindowModule,
				       book, 
				       key->Chapter(),
				       key->Verse(),
				       (char*)mod->RenderText(), 
				       settings.versestyle ? "<BR>" : "");
	else
		g_string_append_printf(str,
				       "\n%s: %s %d:%d. %s%s", 
				       settings.MainWindowModule,
				       book, 
				       key->Chapter(),
				       key->Verse(),
				       (char*)mod->StripText(), 
				       settings.versestyle ? "\n" : "");
	
	if (filename) 
		_save(filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	
	g_string_free(str,TRUE);
	
	if(book)
		g_free(book);
	
}

static void _export_verse_range (EXPORT_DATA data, int type)
{
	GString *str = g_string_new(NULL);
	char* book;
	SWMgr *mgr = backend->get_main_mgr();
	SWModule *mod = mgr->Modules[settings.MainWindowModule];
	mod->setKey(settings.currentverse);
	VerseKey *key = (VerseKey *)(SWKey *)(*mod);
	int curChapter = key->Chapter();
	int curBook = key->Book();
	//int myverse = 1;
	
	book = backend->key_get_book(settings.currentverse);
	if(type == HTML)
		g_string_append_printf(str,
				       "%s<BR>%s: %s Chapter %d<BR>", 
				       HTML_START, 
				       settings.MainWindowModule,
				       book, 
				       key->Chapter());
	else
		g_string_append_printf(str,
				       "\n%s: %s Chapter %d\n", 
				       settings.MainWindowModule,
				       book, 
				       key->Chapter());		
	
	for (key->Verse(data.start_verse);
	     (key->Verse() <= data.end_verse) &&
	     (key->Book() == curBook) &&
	     (key->Chapter() == curChapter) &&
	     !mod->Error();
	     (*mod)++) {
		     
		if(type == HTML)
			g_string_append_printf(str,"%d. %s%s",
					       key->Verse(),
					       (char*)mod->RenderText(), 
					       settings.versestyle ? "<BR>" : "");
		else
			g_string_append_printf(str,"%d. %s%s",
					       key->Verse(),
					       (char*)mod->StripText(), 
					       settings.versestyle ? "\n" : "");
	}
	if(type == HTML)
		g_string_append_printf(str,"%s","</BODY></HTML>");
	if (data.filename) 
		_save(data.filename, str->str,str->len);
	else
		_copy_to_clipboard (str->str, str->len);
	g_string_free(str,TRUE);
	if(book)
		g_free(book);
	
}


void main_export_html(EXPORT_DATA data)
{
	switch(data.passage_type) {
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
	switch(data.passage_type) {
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
