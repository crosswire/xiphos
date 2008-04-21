/*
 * GnomeSword Bible Study Tool
 * export_passage.cc - 
 *
 * Copyright (C) 2008 GnomeSword Developer Team
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
#ifdef USE_EXPORTER

#include <swmgr.h>
#include <swmodule.h>

#include "backend/sword_main.hh"

#include "gui/export_dialog.h"

#include "main/export_passage.h"
#include "main/settings.h"
#include "main/sword.h"

#define HTML_START "<HTML><HEAD><META HTTP-EQUIV=\"content-type\" CONTENT=\"text/html; CHARSET=utf-8\"><STYLE TYPE=\"text/css\"><!-- A { text-decoration:none } %s --></STYLE></HEAD><BODY>"

static int _save(char *filename, char* text,int len)
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
	int curVerse = 1;
	int curChapter = 1;
	int curBook = key->Book();
	int curTestament = key->Testament();
	int curPos = 0;
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
	_save(filename, str->str,str->len);
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
	_save(filename, str->str,str->len);
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
	
	_save(filename, str->str,str->len);
	
	g_string_free(str,TRUE);
	
	if(book)
		g_free(book);
	
}

void main_export_html(char * filename, int passage_type)
{
	switch(passage_type) {
		case BOOK:
			_export_book(filename, HTML);
			break;
		case CHAPTER:
			_export_chapter(filename, HTML);
			break;
		case VERSE:
			_export_verse(filename, HTML);
			break;
		
	}
	
}

void main_export_plain(char *filename, int what_to_export)
{
	switch(what_to_export) {
		case BOOK:
			_export_book(filename, PLAIN);
			break;
		case CHAPTER:
			_export_chapter(filename, PLAIN);
			break;
		case VERSE:
			_export_verse(filename, PLAIN);
			break;
		
	}
	
}
#endif
