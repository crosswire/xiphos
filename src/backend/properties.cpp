/*
 * GnomeSword Bible Study Tool
 * properties.cpp - save gnomesword properties and settings
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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

#include <gnome.h>
#include <swmgr.h>
#include <swconfig.h>
#include <swmodule.h>
#include <versekey.h>
#include <gbfplain.h>
#include <plainhtml.h>
#include <rwphtml.h>
#include <regex.h>
#include <stdio.h>
#include <sys/stat.h>

#include "gs_gnomesword.h"
#include "properties.h"
#include "support.h"
#include "settings.h"

static
gchar *isON(bool value)
{
	if (value)
		return "1";
	else
		return "0";
}

/******************************************************************************
 * load gnomesword properties - using sword SWConfig
 *****************************************************************************/

gboolean backend_load_properties(SETTINGS * s)
{
	gchar buf[255];

	sprintf(buf, "%s/preferences.conf", settings.gSwordDir);
	SWConfig settingsInfo(buf);
	settingsInfo.Load();

	/* app size on open ,epane sizes and shortcut bar width */
	sprintf(settings.gs_version, "%s",
		settingsInfo["GnomeSword"]["Version"].c_str());
	s->shortcutbar_width =
	    atoi(settingsInfo["LAYOUT"]["Shortcutbar"].c_str());
	s->upperpane_hight =
	    atoi(settingsInfo["LAYOUT"]["UperPane"].c_str());
	s->biblepane_width =
	    atoi(settingsInfo["LAYOUT"]["BiblePane"].c_str());
	s->gs_width = atoi(settingsInfo["LAYOUT"]["AppWidth"].c_str());
	s->gs_hight = atoi(settingsInfo["LAYOUT"]["AppHight"].c_str());
	s->docked =
	    atoi(settingsInfo["LAYOUT"]["ShortcutbarDocked"].c_str());

	/* which lexicon to open when storngs numbers are clicked */
	sprintf(s->lex_greek, "%s",
		settingsInfo["LEXICONS"]["Greek"].c_str());
	sprintf(s->lex_hebrew, "%s",
		settingsInfo["LEXICONS"]["Hebrew"].c_str());
	/* which lexicon to open in viewer when storngs numbers are clicked */
	sprintf(s->lex_greek_viewer, "%s",
		settingsInfo["LEXICONS"]["Greek Viewer"].c_str());
	sprintf(s->lex_hebrew_viewer, "%s",
		settingsInfo["LEXICONS"]["Hebrew Viewer"].c_str());
	/* which dictionary to open in viewer when a word is clicked */
	sprintf(s->DefaultDict, "%s",
		settingsInfo["LEXICONS"]["Default Dictionary"].c_str());

	/* font sizes html widgets */
	sprintf(s->bible_font_size, "%s",
		settingsInfo["FontSize"]["BibleWindow"].c_str());
	sprintf(s->commentary_font_size, "%s",
		settingsInfo["FontSize"]["CommentaryWindow"].c_str());
	sprintf(s->dictionary_font_size, "%s",
		settingsInfo["FontSize"]["DictionaryWindow"].c_str());
	sprintf(s->interlinear_font_size, "%s",
		settingsInfo["FontSize"]["InterlinearWindow"].c_str());
	sprintf(s->verselist_font_size, "%s",
		settingsInfo["FontSize"]["VerseListWindow"].c_str());
	sprintf(s->verse_num_font_size, "%s",
		settingsInfo["FontSize"]["VerseNum"].c_str());

	/*** fonts ***/
	sprintf(s->default_font, "%s",
		settingsInfo["Fonts"]["Default"].c_str());
	sprintf(s->greek_font, "%s",
		settingsInfo["Fonts"]["Greek"].c_str());
	sprintf(s->hebrew_font, "%s",
		settingsInfo["Fonts"]["Hebrew"].c_str());
	sprintf(s->unicode_font, "%s",
		settingsInfo["Fonts"]["Unicode"].c_str());
	sprintf(s->interlinear_font, "%s",
		settingsInfo["Fonts"]["Interlinear"].c_str());

	/* modules to use on startup */
	sprintf(s->MainWindowModule, "%s",
		settingsInfo["Modules"]["MainWindow"].c_str());
	sprintf(s->CommWindowModule, "%s",
		settingsInfo["Modules"]["CommWindow"].c_str());
	sprintf(s->DictWindowModule, "%s",
		settingsInfo["Modules"]["DictWindow"].c_str());
	sprintf(s->Interlinear1Module, "%s",
		settingsInfo["Modules"]["Interlinear1"].c_str());
	sprintf(s->Interlinear2Module, "%s",
		settingsInfo["Modules"]["Interlinear2"].c_str());
	sprintf(s->Interlinear3Module, "%s",
		settingsInfo["Modules"]["Interlinear3"].c_str());
	sprintf(s->Interlinear4Module, "%s",
		settingsInfo["Modules"]["Interlinear4"].c_str());
	sprintf(s->Interlinear5Module, "%s",
		settingsInfo["Modules"]["Interlinear5"].c_str());
	sprintf(s->personalcommentsmod, "%s",
		settingsInfo["Modules"]["PerComments"].c_str());
	sprintf(s->devotionalmod, "%s",
		settingsInfo["Modules"]["Devotional"].c_str());
	strcpy(s->BookWindowModule,
		settingsInfo["Modules"]["BookWindow"].c_str());
	
	/* main notebook page */
	s->notebook3page =
	    atoi(settingsInfo["Notebooks"]["notebook3page"].c_str());

	/* Bible verse and dict/lex key to open with */
	sprintf(s->currentverse, "%s",
		settingsInfo["Keys"]["verse"].c_str());
	sprintf(s->dictkey, "%s",
		settingsInfo["Keys"]["dictionarykey"].c_str());
	/* book key */
	strcpy(s->book_key,settingsInfo["Keys"]["BookKey"].c_str());
	
	/* studypad file to open with if any */
	sprintf(s->studypadfilename, "%s",
		settingsInfo["StudyPad"]["Lastfile"].c_str());
	sprintf(s->studypaddir, "%s",
		settingsInfo["StudyPad"]["Directory"].c_str());

	/* misc user options */
	sprintf(s->bible_text_color, "%s",
		settingsInfo["User Options"]["BibleTextColor"].c_str());
	sprintf(s->bible_bg_color, "%s",
		settingsInfo["User Options"]["BibleBGColor"].c_str());
	sprintf(s->currentverse_color, "%s",
		settingsInfo["User Options"]["currentVerseColor"].
		c_str());
	sprintf(s->bible_verse_num_color, "%s",
		settingsInfo["User Options"]["BibleVerseNumColor"].
		c_str());
	sprintf(s->link_color, "%s",
		settingsInfo["User Options"]["LinkColor"].c_str());
	sprintf(s->found_color, "%s",
		settingsInfo["User Options"]["FoundColor"].c_str());
	s->usedefault =
	    atoi(settingsInfo["User Options"]["UseDefault"].c_str());
	s->strongs =
	    atoi(settingsInfo["User Options"]["strongs"].c_str());
	s->strongsint =
	    atoi(settingsInfo["User Options"]["strongs interlinear"].
		 c_str());
	s->morphsint = atoi(settingsInfo["User Options"]
			    ["morphs interlinear interlinear"].c_str());
	s->hebrewpointsint = atoi(settingsInfo["User Options"]
				  ["Hebrew Points Interlinear"].
				  c_str());
	s->cantillationmarksint = atoi(settingsInfo["User Options"]
				       ["Cantillation Marks Interlinear"].c_str());
	s->footnotesint =
	    atoi(settingsInfo["User Options"]["footnotes interlinear"].
		 c_str());
	s->versestyle =
	    atoi(settingsInfo["User Options"]["versestyle"].c_str());
	s->autosavepersonalcomments = atoi(settingsInfo["User Options"]
					   ["autosavepersonalcomments"].
					   c_str());
	s->formatpercom =
	    atoi(settingsInfo["User Options"]["formatpercom"].c_str());
	s->showshortcutbar =
	    atoi(settingsInfo["User Options"]["showshortcutbar"].
		 c_str());
	s->showfavoritesgroup =
	    atoi(settingsInfo["User Options"]["showfavogroup"].c_str());
	s->showtextgroup =
	    atoi(settingsInfo["User Options"]["showtextgroup"].c_str());
	s->showcomgroup =
	    atoi(settingsInfo["User Options"]["showcomgroup"].c_str());
	s->showdictgroup =
	    atoi(settingsInfo["User Options"]["showdictgroup"].c_str());
	s->showbookgroup =
	    atoi(settingsInfo["User Options"]["showbookgroup"].c_str());
	s->showbookmarksgroup =
	    atoi(settingsInfo["User Options"]["showbookmarksgroup"].
		 c_str());
	s->interlinearpage =
	    atoi(settingsInfo["User Options"]["interlinearpage"].
		 c_str());
	s->showhistorygroup =
	    atoi(settingsInfo["User Options"]["showhistorygroup"].
		 c_str());
	s->showsplash =
	    atoi(settingsInfo["User Options"]["ShowSplash"].c_str());
	s->showdevotional =
	    atoi(settingsInfo["User Options"]["Daily Devotional"].
		 c_str());
	s->text_tabs =
	    atoi(settingsInfo["User Options"]["BibleTabs"].c_str());
	s->text_tool =
	    atoi(settingsInfo["User Options"]["BibleToolbar"].c_str());
	s->comm_tabs =
	    atoi(settingsInfo["User Options"]["CommTabs"].c_str());
	s->comm_tool =
	    atoi(settingsInfo["User Options"]["CommToolbar"].c_str());
	s->dict_tabs =
	    atoi(settingsInfo["User Options"]["DictTabs"].c_str());
	s->book_tabs =
	    atoi(settingsInfo["User Options"]["BookTabs"].c_str());
	s->percomm_tabs =
	    atoi(settingsInfo["User Options"]["PercommTabs"].c_str());
	s->notefollow =
	    atoi(settingsInfo["User Options"]["NoteScroll"].c_str());
	s->inViewer =
	    atoi(settingsInfo["User Options"]["InViewer"].c_str());
	s->inDictpane =
	    atoi(settingsInfo["User Options"]["InDictPane"].c_str());
	s->useDefaultDict =
	    atoi(settingsInfo["User Options"]["UseDefaultDict"].
		 c_str());


	return true;
}

/******************************************************************************
 * save gnomesword properties - using sword SWConfig
 *****************************************************************************/

gboolean backend_save_properties(SETTINGS * s, gboolean shutdown)
{
	gchar buf[80], buf2[255];

	sprintf(buf2, "%s/preferences.conf", settings.gSwordDir);
	SWConfig settingsInfo(buf2);

	settingsInfo["GnomeSword"]["Version"] = VERSION;

	if (s->usedefault && shutdown) {
		settingsInfo["StudyPad"]["Lastfile"] =
		    s->studypadfilename;
		settingsInfo["Keys"]["verse"] = s->currentverse;
		settingsInfo["Keys"]["dictionarykey"] = s->dictkey;
	} else {
		settingsInfo["Modules"]["MainWindow"] =
		    s->MainWindowModule;
		settingsInfo["Modules"]["CommWindow"] =
		    s->CommWindowModule;
		settingsInfo["Modules"]["DictWindow"] =
		    s->DictWindowModule;
		settingsInfo["Modules"]["BookWindow"] =
		    s->BookWindowModule;
		settingsInfo["Modules"]["Interlinear1"] =
		    s->Interlinear1Module;
		settingsInfo["Modules"]["Interlinear2"] =
		    s->Interlinear2Module;
		settingsInfo["Modules"]["Interlinear3"] =
		    s->Interlinear3Module;
		settingsInfo["Modules"]["Interlinear4"] =
		    s->Interlinear4Module;
		settingsInfo["Modules"]["Interlinear5"] =
		    s->Interlinear5Module;
		settingsInfo["Modules"]["PerComments"] =
		    s->personalcommentsmod;
		settingsInfo["Modules"]["Devotional"] =
		    s->devotionalmod;

		settingsInfo["LEXICONS"]["Greek"] = s->lex_greek;
		settingsInfo["LEXICONS"]["Hebrew"] = s->lex_hebrew;
		/* which lexicon to open in viewer when storngs numbers are clicked */
		settingsInfo["LEXICONS"]["Greek Viewer"] =
		    s->lex_greek_viewer;
		settingsInfo["LEXICONS"]["Hebrew Viewer"] =
		    s->lex_hebrew_viewer;
		/* which dictionary to open in viewer when a word is clicked */
		settingsInfo["LEXICONS"]["Default Dictionary"] =
		    s->DefaultDict;

		sprintf(buf, "%d", s->notebook3page);
		settingsInfo["Notebooks"]["notebook3page"] = buf;



		settingsInfo["Keys"]["verse"] = s->currentverse;
		settingsInfo["Keys"]["dictionarykey"] = s->dictkey;
		settingsInfo["Keys"]["BookKey"] = s->book_key;

		settingsInfo["StudyPad"]["Lastfile"] =
		    s->studypadfilename;
		settingsInfo["StudyPad"]["Directory"] = s->studypaddir;

		settingsInfo["FontSize"]["BibleWindow"] =
		    s->bible_font_size;
		settingsInfo["FontSize"]["CommentaryWindow"] =
		    s->commentary_font_size;
		settingsInfo["FontSize"]["DictionaryWindow"] =
		    s->dictionary_font_size;
		settingsInfo["FontSize"]["InterlinearWindow"] =
		    s->interlinear_font_size;
		settingsInfo["FontSize"]["VerseListWindow"] =
		    s->verselist_font_size;
		settingsInfo["FontSize"]["VerseNum"] =
		    s->verse_num_font_size;
		settingsInfo["Fonts"]["Default"] = s->default_font;
		settingsInfo["Fonts"]["Greek"] = s->greek_font;
		settingsInfo["Fonts"]["Hebrew"] = s->hebrew_font;
		settingsInfo["Fonts"]["Unicode"] = s->unicode_font;
		settingsInfo["Fonts"]["Interlinear"] =
		    s->interlinear_font;

		/* layout */
		sprintf(buf, "%d", s->shortcutbar_width);
		settingsInfo["LAYOUT"]["Shortcutbar"] = buf;

		sprintf(buf, "%d", s->upperpane_hight);
		settingsInfo["LAYOUT"]["UperPane"] = buf;

		sprintf(buf, "%d", s->biblepane_width);
		settingsInfo["LAYOUT"]["BiblePane"] = buf;

		sprintf(buf, "%d", s->gs_width);
		settingsInfo["LAYOUT"]["AppWidth"] = buf;

		sprintf(buf, "%d", s->gs_hight);
		settingsInfo["LAYOUT"]["AppHight"] = buf;

		settingsInfo["LAYOUT"]["ShortcutbarDocked"] =
		    isON(s->docked);


		/* User Options */
		settingsInfo["User Options"]["currentVerseColor"] =
		    s->currentverse_color;

		settingsInfo["User Options"]["BibleTextColor"] =
		    s->bible_text_color;

		settingsInfo["User Options"]["BibleBGColor"] =
		    s->bible_bg_color;

		settingsInfo["User Options"]["BibleVerseNumColor"] =
		    s->bible_verse_num_color;

		settingsInfo["User Options"]["LinkColor"] =
		    s->link_color;

		settingsInfo["User Options"]["FoundColor"] =
		    s->found_color;

		settingsInfo["User Options"]["UseDefault"] =
		    isON(s->usedefault);
		settingsInfo["User Options"]["strongs"] =
		    isON(s->strongs);
		settingsInfo["User Options"]["strongs interlinear"] =
		    isON(s->strongsint);
		settingsInfo["User Options"]["morphs interlinear"] =
		    isON(s->morphsint);
		settingsInfo["User Options"]
		    ["Hebrew Points Interlinear"] =
		    isON(s->hebrewpoints);
		settingsInfo["User Options"]
		    ["Cantillation Marks Interlinear"] =
		    isON(s->cantillationmarks);
		settingsInfo["User Options"]["footnotes interlinear"] =
		    isON(s->footnotesint);
		settingsInfo["User Options"]["versestyle"] =
		    isON(s->versestyle);
		settingsInfo["User Options"]["autosavepersonalcomments"]
		    = isON(s->autosavepersonalcomments);
		settingsInfo["User Options"]["formatpercom"] =
		    isON(s->formatpercom);
		settingsInfo["User Options"]["showshortcutbar"] =
		    isON(s->showshortcutbar);
		settingsInfo["User Options"]["showfavogroup"] =
		    isON(s->showfavoritesgroup);
		settingsInfo["User Options"]["showtextgroup"] =
		    isON(s->showtextgroup);
		settingsInfo["User Options"]["showcomgroup"] =
		    isON(s->showcomgroup);
		settingsInfo["User Options"]["showdictgroup"] =
		    isON(s->showdictgroup);
		settingsInfo["User Options"]["showbookgroup"] =
		    isON(s->showbookgroup);
		settingsInfo["User Options"]["showbookmarksgroup"] =
		    isON(s->showbookmarksgroup);
		settingsInfo["User Options"]["interlinearpage"] =
		    isON(s->interlinearpage);
		settingsInfo["User Options"]["showhistorygroup"] =
		    isON(s->showhistorygroup);
		settingsInfo["User Options"]["ShowSplash"] =
		    isON(s->showsplash);
		settingsInfo["User Options"]["Daily Devotional"] =
		    isON(s->showdevotional);
		settingsInfo["User Options"]["NoteScroll"] =
		    isON(s->notefollow);
		settingsInfo["User Options"]["BibleTabs"] =
		    isON(s->text_tabs);
		settingsInfo["User Options"]["BibleToolbar"] =
		    isON(s->text_tool);
		settingsInfo["User Options"]["CommTabs"] =
		    isON(s->comm_tabs);
		settingsInfo["User Options"]["CommToolbar"] =
		    isON(s->comm_tool);
		settingsInfo["User Options"]["DictTabs"] =
		    isON(s->dict_tabs);
		settingsInfo["User Options"]["BookTabs"] = isON(s->book_tabs);
	        settingsInfo["User Options"]["PercommTabs"] = isON(s->percomm_tabs);
		settingsInfo["User Options"]["InViewer"] =
		    isON(s->inViewer);
		settingsInfo["User Options"]["InDictPane"] =
		    isON(s->inDictpane);
		settingsInfo["User Options"]["UseDefaultDict"] =
		    isON(s->useDefaultDict);
	}
	settingsInfo.Save();
	return true;
}

/******************************************************************************
 * create gnomesword properties - using sword SWConfig
 * and information from the setup dialog
 *****************************************************************************/

gboolean backend_create_properties_from_setup(void)
{
	gchar buf[80], buf2[255];

	sprintf(buf2, "%s/preferences.conf", settings.gSwordDir);
	SWConfig settingsInfo(buf2);
	settingsInfo["GnomeSword"]["Version"] = VERSION;
	settingsInfo["Modules"]["MainWindow"] = settings.MainWindowModule;
	settingsInfo["Modules"]["CommWindow"] = settings.CommWindowModule;
	settingsInfo["Modules"]["DictWindow"] = settings.DictWindowModule;
	settingsInfo["Modules"]["Interlinear1"] = settings.Interlinear1Module;
	settingsInfo["Modules"]["Interlinear2"] = settings.Interlinear2Module;
	settingsInfo["Modules"]["Interlinear3"] = settings.Interlinear3Module;
	settingsInfo["Modules"]["Interlinear4"] = settings.Interlinear4Module;
	settingsInfo["Modules"]["Interlinear5"] = settings.Interlinear5Module;
	settingsInfo["Modules"]["PerComments"] = settings.personalcommentsmod;
	//settingsInfo["Modules"]["Devotional"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "comboDevotional")));       /* get mod name */

	settingsInfo["LEXICONS"]["Greek"] = "StrongsGreek";
	settingsInfo["LEXICONS"]["Hebrew"] = "StrongsHebrew";

	settingsInfo["Notebooks"]["notebook3page"] = "0";
	settingsInfo["Keys"]["verse"] = "Romans 8:28";
	settingsInfo["Keys"]["dictionarykey"] = "GRACE";

	settingsInfo["StudyPad"]["Lastfile"] = "";
	settingsInfo["StudyPad"]["Directory"] = "~";

	settingsInfo["FontSize"]["BibleWindow"] = "+0";
	settingsInfo["FontSize"]["CommentaryWindow"] = "+0";
	settingsInfo["FontSize"]["DictionaryWindow"] = "+0";
	settingsInfo["FontSize"]["InterlinearWindow"] = "+0";
	settingsInfo["FontSize"]["VerseListWindow"] = "+0";
	settingsInfo["FontSize"]["VerseNum"] = "+0";

	settingsInfo["Fonts"]["Default"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Greek"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Hebrew"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Unicode"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Interlinear"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["LAYOUT"]["Shortcutbar"] = "120";
	settingsInfo["LAYOUT"]["UperPane"] = "296";
	settingsInfo["LAYOUT"]["BiblePane"] = "262";
	settingsInfo["LAYOUT"]["AppWidth"] = "700";
	settingsInfo["LAYOUT"]["AppHight"] = "550";
	settingsInfo["LAYOUT"]["ShortcutbarDocked"] = "1";

	settingsInfo["User Options"]["UseDefault"] = isON(settings.usedefault);
	settingsInfo["User Options"]["BibleTabs"] = isON(settings.text_tabs);
	settingsInfo["User Options"]["CommTabs"] = isON(settings.comm_tabs);
	settingsInfo["User Options"]["DictTabs"] = isON(settings.dict_tabs);
	settingsInfo["User Options"]["versestyle"] = isON(settings.versestyle);
	settingsInfo["User Options"]["autosavepersonalcomments"] = 
		isON(settings.autosavepersonalcomments);
	settingsInfo["User Options"]["interlinearpage"] = 
		isON(settings.interlinearpage);
	settingsInfo["User Options"]["strongs"] = "0";
	settingsInfo["User Options"]["footnotes"] = "0";
	settingsInfo["User Options"]["formatpercom"] = "0";
	settingsInfo["User Options"]["showshortcutbar"] = "1";
	settingsInfo["User Options"]["showfavogroup"] = "1";
	settingsInfo["User Options"]["showtextgroup"] = "0";
	settingsInfo["User Options"]["showcomgroup"] = "0";
	settingsInfo["User Options"]["showdictgroup"] = "0";
	settingsInfo["User Options"]["showbookgroup"] = "0";
	settingsInfo["User Options"]["showbookmarksgroup"] = "1";
	settingsInfo["User Options"]["currentVerseColor"] = "#339966";
	settingsInfo["User Options"]["BibleTextColor"] = "#000000";
	settingsInfo["User Options"]["BibleBGColor"] = "#FFFFFF";
	settingsInfo["User Options"]["BibleVerseNumColor"] = "#000FCF";
	settingsInfo["User Options"]["LinkColor"] = "#898989";
	settingsInfo["User Options"]["FoundColor"] = "#D02898";
	settingsInfo["User Options"]["showhistorygroup"] = "0";
	settingsInfo["User Options"]["ShowSplash"] = "1";
	settingsInfo["User Options"]["Daily Devotional"] = "0";
	settingsInfo["User Options"]["NoteScroll"] = "1";
	settingsInfo["User Options"]["InViewer"] = "1";
	settingsInfo["User Options"]["InDictPane"] = "1";
	settingsInfo["User Options"]["UseDefaultDict"] = "0";
	settingsInfo.Save();
	return true;
}

/******************************************************************************
 * create gnomesword properties - using sword SWConfig
 *****************************************************************************/

gboolean backend_create_properties(void)
{
	gchar buf[80], buf2[255];

	sprintf(buf2, "%s/preferences.conf", settings.gSwordDir);
	SWConfig settingsInfo(buf2);
	settingsInfo["GnomeSword"]["Version"] = VERSION;
	settingsInfo["Modules"]["MainWindow"] = "KJV";
	settingsInfo["Modules"]["CommWindow"] = "Personal";
	settingsInfo["Modules"]["DictWindow"] = "Eastons";
	settingsInfo["Modules"]["Interlinear1"] = "Byz";
	settingsInfo["Modules"]["Interlinear2"] = "BBE";
	settingsInfo["Modules"]["Interlinear3"] = "Websters";
	settingsInfo["Modules"]["Interlinear4"] = "WEB";
	settingsInfo["Modules"]["Interlinear5"] = "BBE";
	settingsInfo["Modules"]["PerComments"] = "Personal";
	settingsInfo["Modules"]["Devotional"] = "SME";

	settingsInfo["LEXICONS"]["Greek"] = "StrongsGreek";
	settingsInfo["LEXICONS"]["Hebrew"] = "StrongsHebrew";

	settingsInfo["Notebooks"]["notebook3page"] = "0";
	settingsInfo["Keys"]["verse"] = "Romans 8:28";
	settingsInfo["Keys"]["dictionarykey"] = "GRACE";

	settingsInfo["StudyPad"]["Lastfile"] = "";
	settingsInfo["StudyPad"]["Directory"] = "~";

	settingsInfo["FontSize"]["BibleWindow"] = "+0";
	settingsInfo["FontSize"]["CommentaryWindow"] = "+0";
	settingsInfo["FontSize"]["DictionaryWindow"] = "+0";
	settingsInfo["FontSize"]["InterlinearWindow"] = "+0";
	settingsInfo["FontSize"]["VerseListWindow"] = "+0";
	settingsInfo["FontSize"]["VerseNum"] = "+0";

	settingsInfo["Fonts"]["Default"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Greek"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Hebrew"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Unicode"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";
	settingsInfo["Fonts"]["Interlinear"] =
	    "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";

	settingsInfo["LAYOUT"]["Shortcutbar"] = "120";
	settingsInfo["LAYOUT"]["UperPane"] = "296";
	settingsInfo["LAYOUT"]["BiblePane"] = "262";
	settingsInfo["LAYOUT"]["AppWidth"] = "700";
	settingsInfo["LAYOUT"]["AppHight"] = "550";
	settingsInfo["LAYOUT"]["ShortcutbarDocked"] = "1";

	settingsInfo["User Options"]["UseDefault"] = "0";
	settingsInfo["User Options"]["currentVerseColor"] = "#339966";
	settingsInfo["User Options"]["BibleTextColor"] = "#000000";
	settingsInfo["User Options"]["BibleBGColor"] = "#FFFFFF";
	settingsInfo["User Options"]["BibleVerseNumColor"] = "#000FCF";
	settingsInfo["User Options"]["LinkColor"] = "#898989";
	settingsInfo["User Options"]["FoundColor"] = "#D02898";
	settingsInfo["User Options"]["BibleTabs"] = "1";
	settingsInfo["User Options"]["CommTabs"] = "1";
	settingsInfo["User Options"]["DictTabs"] = "1";
	settingsInfo["User Options"]["strongs"] = "0";
	settingsInfo["User Options"]["footnotes"] = "0";
	settingsInfo["User Options"]["versestyle"] = "1";
	settingsInfo["User Options"]["autosavepersonalcomments"] = "1";
	settingsInfo["User Options"]["formatpercom"] = "0";
	settingsInfo["User Options"]["showshortcutbar"] = "1";
	settingsInfo["User Options"]["showfavogroup"] = "1";
	settingsInfo["User Options"]["showtextgroup"] = "1";
	settingsInfo["User Options"]["showcomgroup"] = "1";
	settingsInfo["User Options"]["showdictgroup"] = "1";
	settingsInfo["User Options"]["showbookgroup"] = "1";
	settingsInfo["User Options"]["showbookmarksgroup"] = "1";
	settingsInfo["User Options"]["interlinearpage"] = "1";
	settingsInfo["User Options"]["showhistorygroup"] = "1";
	settingsInfo["User Options"]["ShowSplash"] = "1";
	settingsInfo["User Options"]["Daily Devotional"] = "1";
	settingsInfo["User Options"]["NoteScroll"] = "1";
	settingsInfo["User Options"]["InViewer"] = "1";
	settingsInfo["User Options"]["InDictPane"] = "1";
	settingsInfo["User Options"]["UseDefaultDict"] = "0";

	settingsInfo.Save();
	return true;
}
