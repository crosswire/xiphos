/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

  /*
    * GnomeSword Bible Study Tool
    * sw_properties.cpp
    * -------------------
    * Sat June 30 2001
    * copyright (C) 2001 by Terry Biggs
    * tbiggs@users.sourceforge.net
    *
 */
 
 /*
    *  This program is free software; you can redistribute it and/or modify
    *  it under the terms of the GNU General Public License as published by
    *  the Free Software Foundation; either version 2 of the License, or
    *  (at your option) any later version.
    *
    *  This program is distributed in the hope that it will be useful,
    *  but WITHOUT ANY WARRANTY; without even the implied warranty of
    *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    *  GNU Library General Public License for more details.
    *
    *  You should have received a copy of the GNU General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#ifdef HAVE_CONFIG_H
#  include <config.h>
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
#include "sw_properties.h"
#include "support.h"


/***********************************************************************************************
 externals
***********************************************************************************************/
extern gchar *gSwordDir;
extern SETTINGS *settings;

/******************************************************************************
 * load gnomesword configuration - using sword SWConfig
 ******************************************************************************/
gboolean loadconfig(void)
{
	gchar buf[255];

	sprintf(buf, "%s/preferences.conf", gSwordDir);
	SWConfig settingsInfo(buf);
	settingsInfo.Load();
	
	/* app size on open ,epane sizes and shortcut bar width */
	sprintf(settings->gs_version,"%s",settingsInfo["GnomeSword"]["Version"].c_str());
	settings->shortcutbar_width =
	    atoi(settingsInfo["LAYOUT"]["Shortcutbar"].c_str());
	settings->upperpane_hight =
	    atoi(settingsInfo["LAYOUT"]["UperPane"].c_str());
	settings->biblepane_width =
	    atoi(settingsInfo["LAYOUT"]["BiblePane"].c_str());
	settings->gs_width =
	    atoi(settingsInfo["LAYOUT"]["AppWidth"].c_str());
	settings->gs_hight =
	    atoi(settingsInfo["LAYOUT"]["AppHight"].c_str());
	settings->docked =
	    atoi(settingsInfo["LAYOUT"]["ShortcutbarDocked"].c_str());
	
	/* which lexicon to open when storngs numbers are clicked */
	sprintf(settings->lex_greek, "%s",
		settingsInfo["LEXICONS"]["Greek"].c_str());
	sprintf(settings->lex_hebrew, "%s",
		settingsInfo["LEXICONS"]["Hebrew"].c_str());
		
	/* font sizes html widgets */
	sprintf(settings->bible_font_size, "%s",
		settingsInfo["FontSize"]["BibleWindow"].c_str());
	sprintf(settings->commentary_font_size, "%s",
		settingsInfo["FontSize"]["CommentaryWindow"].c_str());
	sprintf(settings->dictionary_font_size, "%s",
		settingsInfo["FontSize"]["DictionaryWindow"].c_str());
	sprintf(settings->interlinear_font_size, "%s",
		settingsInfo["FontSize"]["InterlinearWindow"].c_str());
	sprintf(settings->verselist_font_size, "%s",
		settingsInfo["FontSize"]["VerseListWindow"].c_str());
	sprintf(settings->verse_num_font_size, "%s",
		settingsInfo["FontSize"]["VerseNum"].c_str());
		
	/*** fonts ***/	
	sprintf(settings->default_font,"%s",settingsInfo["Fonts"]["Default"].c_str());
	sprintf(settings->greek_font,"%s", settingsInfo["Fonts"]["Greek"].c_str());
	sprintf(settings->hebrew_font,"%s",settingsInfo["Fonts"]["Hebrew"].c_str());
	sprintf(settings->unicode_font,"%s", settingsInfo["Fonts"]["Unicode"].c_str());	
	
	/* modules to use on startup */
	sprintf(settings->MainWindowModule, "%s",
		settingsInfo["Modules"]["MainWindow"].c_str());
	sprintf(settings->CommWindowModule, "%s",
		settingsInfo["Modules"]["CommWindow"].c_str());
	sprintf(settings->DictWindowModule, "%s",
		settingsInfo["Modules"]["DictWindow"].c_str());
	sprintf(settings->Interlinear1Module, "%s",
		settingsInfo["Modules"]["Interlinear1"].c_str());
	sprintf(settings->Interlinear2Module, "%s",
		settingsInfo["Modules"]["Interlinear2"].c_str());
	sprintf(settings->Interlinear3Module, "%s",
		settingsInfo["Modules"]["Interlinear3"].c_str());
	sprintf(settings->Interlinear4Module, "%s",
		settingsInfo["Modules"]["Interlinear4"].c_str());
	sprintf(settings->Interlinear5Module, "%s",
		settingsInfo["Modules"]["Interlinear5"].c_str());
	sprintf(settings->personalcommentsmod, "%s",
		settingsInfo["Modules"]["PerComments"].c_str());
	sprintf(settings->devotionalmod, "%s",
		settingsInfo["Modules"]["Devotional"].c_str());
		
	/* main notebook page */
	settings->notebook3page =
	    atoi(settingsInfo["Notebooks"]["notebook3page"].c_str());
	    
	/* Bible verse and dict/lex key to open with */
	sprintf(settings->currentverse, "%s",
		settingsInfo["Keys"]["verse"].c_str());
	sprintf(settings->dictkey, "%s",
		settingsInfo["Keys"]["dictionarykey"].c_str());
		
	/* studypad file to open with if any */
	sprintf(settings->studypadfilename, "%s",
		settingsInfo["StudyPad"]["Lastfile"].c_str());
	sprintf(settings->studypaddir, "%s",
		settingsInfo["StudyPad"]["Directory"].c_str());
		
	/* misc user options */
	sprintf(settings->bible_text_color, "%s",
		settingsInfo["User Options"]["BibleTextColor"].c_str());
	sprintf(settings->bible_bg_color, "%s",
		settingsInfo["User Options"]["BibleBGColor"].c_str());
	sprintf(settings->currentverse_color, "%s",
		settingsInfo["User Options"]["currentVerseColor"].c_str());
	sprintf(settings->bible_verse_num_color, "%s",
		settingsInfo["User Options"]["BibleVerseNumColor"].c_str());
	sprintf(settings->link_color, "%s",
		settingsInfo["User Options"]["LinkColor"].c_str());
	sprintf(settings->found_color, "%s",
		settingsInfo["User Options"]["FoundColor"].c_str());		
	settings->usedefault =
	    atoi(settingsInfo["User Options"]["UseDefault"].c_str());
	settings->strongs =
	    atoi(settingsInfo["User Options"]["strongs"].c_str());
	settings->strongsint =
	    atoi(settingsInfo["User Options"]["strongs interlinear"].c_str());
	settings->morphsint =
	    atoi(settingsInfo["User Options"]["morphs interlinear interlinear"].c_str());  
	settings->hebrewpointsint =
	    atoi(settingsInfo["User Options"]["Hebrew Points Interlinear"].c_str());
	settings->cantillationmarksint =
	    atoi(settingsInfo["User Options"]["Cantillation Marks Interlinear"].c_str());     
	settings->footnotesint =
	    atoi(settingsInfo["User Options"]["footnotes interlinear"].c_str());
	settings->versestyle =
	    atoi(settingsInfo["User Options"]["versestyle"].c_str());
	settings->autosavepersonalcomments =
	    atoi(settingsInfo["User Options"]["autosavepersonalcomments"].c_str());
	settings->formatpercom =
	    atoi(settingsInfo["User Options"]["formatpercom"].c_str());
	settings->showshortcutbar =
	    atoi(settingsInfo["User Options"]["showshortcutbar"].c_str());
	settings-> showfavoritesgroup=
	    atoi(settingsInfo["User Options"]["showfavogroup"].c_str());
	settings->showtextgroup =
	    atoi(settingsInfo["User Options"]["showtextgroup"].c_str());
	settings->showcomgroup =
	    atoi(settingsInfo["User Options"]["showcomgroup"].c_str());
	settings->showdictgroup =
	    atoi(settingsInfo["User Options"]["showdictgroup"].c_str());
	settings->showbookmarksgroup =
	    atoi(settingsInfo["User Options"]["showbookmarksgroup"].c_str());
	settings->interlinearpage =
	    atoi(settingsInfo["User Options"]["interlinearpage"].c_str());
	settings->showhistorygroup =
	    atoi(settingsInfo["User Options"]["showhistorygroup"].c_str());
	settings->showsplash =
	    atoi(settingsInfo["User Options"]["ShowSplash"].c_str());
	settings->showdevotional =
	    atoi(settingsInfo["User Options"]["Daily Devotional"].c_str());
	settings->text_tabs =
	    atoi(settingsInfo["User Options"]["BibleTabs"].c_str());
	settings->comm_tabs =
	    atoi(settingsInfo["User Options"]["CommTabs"].c_str());
	settings->dict_tabs =
	    atoi(settingsInfo["User Options"]["DictTabs"].c_str());
	settings->notefollow =
	    atoi(settingsInfo["User Options"]["NoteScroll"].c_str());


	return true;
}

/******************************************************************************
 * save gnomesword configuration - using sword SWConfig
 ******************************************************************************/
gboolean saveconfig(void)
{
	gchar buf[80], buf2[255];

	sprintf(buf2, "%s/preferences.conf", gSwordDir);
	SWConfig settingsInfo(buf2);
	
	settingsInfo["GnomeSword"]["Version"] = VERSION;
	if (settings->usedefault) {
		settingsInfo["StudyPad"]["Lastfile"] =
		    	settings->studypadfilename;
		settingsInfo["Keys"]["verse"] = 
			settings->currentverse;
		settingsInfo["Keys"]["dictionarykey"] = 
			settings->dictkey;
	} else {
		settingsInfo["Modules"]["MainWindow"] =
		    settings->MainWindowModule;
		settingsInfo["Modules"]["CommWindow"] =
		    settings->CommWindowModule;
		settingsInfo["Modules"]["DictWindow"] =
		    settings->DictWindowModule;
		settingsInfo["Modules"]["Interlinear1"] =
		    settings->Interlinear1Module;
		settingsInfo["Modules"]["Interlinear2"] =
		    settings->Interlinear2Module;
		settingsInfo["Modules"]["Interlinear3"] =
		    settings->Interlinear3Module;
		settingsInfo["Modules"]["Interlinear4"] =
		    settings->Interlinear4Module;
		settingsInfo["Modules"]["Interlinear5"] =
		    settings->Interlinear5Module;
		settingsInfo["Modules"]["PerComments"] =
		    settings->personalcommentsmod;
		settingsInfo["Modules"]["Devotional"] =
		    settings->devotionalmod;

		settingsInfo["LEXICONS"]["Greek"] = settings->lex_greek;
		settingsInfo["LEXICONS"]["Hebrew"] = settings->lex_hebrew;


		sprintf(buf, "%d", settings->notebook3page);
		settingsInfo["Notebooks"]["notebook3page"] = buf;



		settingsInfo["Keys"]["verse"] = settings->currentverse;
		settingsInfo["Keys"]["dictionarykey"] = settings->dictkey;

		settingsInfo["StudyPad"]["Lastfile"] =
		    settings->studypadfilename;
		settingsInfo["StudyPad"]["Directory"] =
		    settings->studypaddir;

		settingsInfo["FontSize"]["BibleWindow"] =
		    settings->bible_font_size;
		settingsInfo["FontSize"]["CommentaryWindow"] =
		    settings->commentary_font_size;
		settingsInfo["FontSize"]["DictionaryWindow"] =
		    settings->dictionary_font_size;
		settingsInfo["FontSize"]["InterlinearWindow"] =
		    settings->interlinear_font_size;
		settingsInfo["FontSize"]["VerseListWindow"] =
		    settings->verselist_font_size;
		settingsInfo["FontSize"]["VerseNum"] =
		    settings->verse_num_font_size;
		 settingsInfo["Fonts"]["Default"] =
		    settings->default_font;   
		 settingsInfo["Fonts"]["Greek"] =
		    settings->greek_font;   
		 settingsInfo["Fonts"]["Hebrew"] =
		    settings->hebrew_font;   
		 settingsInfo["Fonts"]["Unicode"] =
		    settings->unicode_font;   
		    
		    
		sprintf(buf, "%d", settings->shortcutbar_width);
		settingsInfo["LAYOUT"]["Shortcutbar"] = buf;
		sprintf(buf, "%d", settings->upperpane_hight);
		settingsInfo["LAYOUT"]["UperPane"] = buf;
		sprintf(buf, "%d", settings->biblepane_width);
		settingsInfo["LAYOUT"]["BiblePane"] = buf;
		sprintf(buf, "%d", settings->gs_width);
		settingsInfo["LAYOUT"]["AppWidth"] = buf;
		sprintf(buf, "%d", settings->gs_hight);
		settingsInfo["LAYOUT"]["AppHight"] = buf;
		if (settings->docked)
			settingsInfo["LAYOUT"]["ShortcutbarDocked"] = "1";
		else
			settingsInfo["LAYOUT"]["ShortcutbarDocked"] = "0";

		settingsInfo["User Options"]["currentVerseColor"] =
		    settings->currentverse_color;
		settingsInfo["User Options"]["BibleTextColor"] =
		    settings->bible_text_color;
		settingsInfo["User Options"]["BibleBGColor"] =
		    settings->bible_bg_color;
		settingsInfo["User Options"]["BibleVerseNumColor"] =
		    settings->bible_verse_num_color;
		 settingsInfo["User Options"]["LinkColor"] = 
		 	settings->link_color;   
		settingsInfo["User Options"]["FoundColor"] =
			settings->found_color;
		
		if (settings->usedefault)
			settingsInfo["User Options"]["UseDefault"] = "1";
		else
			settingsInfo["User Options"]["UseDefault"] = "0";

		if (settings->strongs)
			settingsInfo["User Options"]["strongs"] = "1";
		else
			settingsInfo["User Options"]["strongs"] = "0";
		
		if (settings->strongsint)
			settingsInfo["User Options"]["strongs interlinear"] = "1";
		else
			settingsInfo["User Options"]["strongs interlinear"] = "0";

		if (settings->morphsint)
			settingsInfo["User Options"]["morphs interlinear"] = "1";
		else
			settingsInfo["User Options"]["morphs interlinear"] = "0";

		if (settings->hebrewpoints)
			settingsInfo["User Options"]["Hebrew Points Interlinear"] = "1";
		else
			settingsInfo["User Options"]["Hebrew Points Interlinear"] = "0";

		if (settings->cantillationmarks)
			settingsInfo["User Options"]["Cantillation Marks Interlinear"] = "1";
		else
			settingsInfo["User Options"]["Cantillation Marks Interlinear"] = "0";

		if (settings->footnotesint)
			settingsInfo["User Options"]["footnotes interlinear"] = "1";
		else
			settingsInfo["User Options"]["footnotes interlinear"] = "0";

		if (settings->versestyle)
			settingsInfo["User Options"]["versestyle"] = "1";
		else
			settingsInfo["User Options"]["versestyle"] = "0";

		if (settings->autosavepersonalcomments)
			settingsInfo["User Options"]
			    ["autosavepersonalcomments"] = "1";
		else
			settingsInfo["User Options"]
			    ["autosavepersonalcomments"] = "0";

		if (settings->formatpercom)
			settingsInfo["User Options"]["formatpercom"] = "1";
		else
			settingsInfo["User Options"]["formatpercom"] = "0";

		if (settings->showshortcutbar)
			settingsInfo["User Options"]["showshortcutbar"] =
			    "1";
		else
			settingsInfo["User Options"]["showshortcutbar"] =
			    "0";

		if (settings->showfavoritesgroup)
			settingsInfo["User Options"]["showfavogroup"] =
			    "1";
		else
			settingsInfo["User Options"]["showfavogroup"] =
			    "0";

		if (settings->showtextgroup)
			settingsInfo["User Options"]["showtextgroup"] =
			    "1";
		else
			settingsInfo["User Options"]["showtextgroup"] =
			    "0";

		if (settings->showcomgroup)
			settingsInfo["User Options"]["showcomgroup"] = "1";
		else
			settingsInfo["User Options"]["showcomgroup"] = "0";

		if (settings->showdictgroup)
			settingsInfo["User Options"]["showdictgroup"] =
			    "1";
		else
			settingsInfo["User Options"]["showdictgroup"] =
			    "0";

		if (settings->showbookmarksgroup)
			settingsInfo["User Options"]["showbookmarksgroup"]
			    = "1";
		else
			settingsInfo["User Options"]["showbookmarksgroup"]
			    = "0";

		if (settings->interlinearpage)
			settingsInfo["User Options"]["interlinearpage"] =
			    "1";
		else
			settingsInfo["User Options"]["interlinearpage"] =
			    "0";

		if (settings->showhistorygroup)
			settingsInfo["User Options"]["showhistorygroup"] =
			    "1";
		else
			settingsInfo["User Options"]["showhistorygroup"] =
			    "0";

		if (settings->showsplash)
			settingsInfo["User Options"]["ShowSplash"] = "1";
		else
			settingsInfo["User Options"]["ShowSplash"] = "0";
		
		if (settings->showdevotional)
			settingsInfo["User Options"]["Daily Devotional"] = "1";
		else
			settingsInfo["User Options"]["Daily Devotional"] = "0";
		
		if (settings->notefollow)
			settingsInfo["User Options"]["NoteScroll"] = "1";
		else
			settingsInfo["User Options"]["NoteScroll"] = "0";
		


		sprintf(buf, "%d", settings->text_tabs);
		settingsInfo["User Options"]["BibleTabs"] = buf;

		sprintf(buf, "%d", settings->comm_tabs);
		settingsInfo["User Options"]["CommTabs"] = buf;
		sprintf(buf, "%d", settings->dict_tabs);
		settingsInfo["User Options"]["DictTabs"] = buf;
	}
	settingsInfo.Save();
	return true;
}

/******************************************************************************
 * create gnomesword configuration - using sword SWConfig
 * and information from the setup dialog
 ******************************************************************************/
gboolean createfromsetupconfig(GtkWidget * setup)
{
	gchar buf[80], buf2[255];

	sprintf(buf2, "%s/preferences.conf", gSwordDir);
	SWConfig settingsInfo(buf2);
	settingsInfo["GnomeSword"]["Version"] = VERSION;
	settingsInfo["Modules"]["MainWindow"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry1")));	/* get mod name */
	settingsInfo["Modules"]["CommWindow"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry12")));	/* get mod name */
	settingsInfo["Modules"]["DictWindow"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry13")));	/* get mod name */
	settingsInfo["Modules"]["Interlinear1"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry2")));	/* get mod name */
	settingsInfo["Modules"]["Interlinear2"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry3")));	/* get mod name */
	settingsInfo["Modules"]["Interlinear3"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry4")));	/* get mod name */
	settingsInfo["Modules"]["Interlinear4"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry10")));	/* get mod name */
	settingsInfo["Modules"]["Interlinear5"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry11")));	/* get mod name */
	settingsInfo["Modules"]["PerComments"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "combo_entry14")));	/* get mod name */
	//settingsInfo["Modules"]["Devotional"] = gtk_entry_get_text(GTK_ENTRY(lookup_widget(setup, "comboDevotional")));	/* get mod name */
	
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
		 
	settingsInfo["Fonts"]["Default"] =  "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";   
	settingsInfo["Fonts"]["Greek"] =	"-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1"; 
	settingsInfo["Fonts"]["Hebrew"] = "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1"; 
	settingsInfo["Fonts"]["Unicode"] = "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1"; 
	
	settingsInfo["LAYOUT"]["Shortcutbar"] = "120";
	settingsInfo["LAYOUT"]["UperPane"] = "296";
	settingsInfo["LAYOUT"]["BiblePane"] = "262";
	settingsInfo["LAYOUT"]["AppWidth"] = "700";
	settingsInfo["LAYOUT"]["AppHight"] = "550";
	settingsInfo["LAYOUT"]["ShortcutbarDocked"] = "1";

	if (GTK_TOGGLE_BUTTON(lookup_widget(setup, "radiobutton1"))->
	    active)
		settingsInfo["User Options"]["UseDefault"] = "1";
	else
		settingsInfo["User Options"]["UseDefault"] = "0";
	if (GTK_TOGGLE_BUTTON(lookup_widget(setup, "checkbutton4"))->
	    active)
		settingsInfo["User Options"]["BibleTabs"] = "1";
	else
		settingsInfo["User Options"]["BibleTabs"] = "0";
	if (GTK_TOGGLE_BUTTON(lookup_widget(setup, "checkbutton5"))->
	    active)
		settingsInfo["User Options"]["CommTabs"] = "1";
	else
		settingsInfo["User Options"]["CommTabs"] = "0";
	if (GTK_TOGGLE_BUTTON(lookup_widget(setup, "checkbutton6"))->
	    active)
		settingsInfo["User Options"]["DictTabs"] = "1";
	else
		settingsInfo["User Options"]["DictTabs"] = "0";
	if (GTK_TOGGLE_BUTTON(lookup_widget(setup, "checkbutton2"))->
	    active)
		settingsInfo["User Options"]["versestyle"] = "1";
	else
		settingsInfo["User Options"]["versestyle"] = "0";
	if (GTK_TOGGLE_BUTTON(lookup_widget(setup, "checkbutton1"))->
	    active)
		settingsInfo["User Options"]["autosavepersonalcomments"] =
		    "1";
	else
		settingsInfo["User Options"]["autosavepersonalcomments"] =
		    "0";
	if (GTK_TOGGLE_BUTTON(lookup_widget(setup, "checkbutton3"))->
	    active)
		settingsInfo["User Options"]["interlinearpage"] = "1";
	else
		settingsInfo["User Options"]["interlinearpage"] = "0";
	settingsInfo["User Options"]["strongs"] = "0";
	settingsInfo["User Options"]["footnotes"] = "0";
	settingsInfo["User Options"]["formatpercom"] = "0";
	settingsInfo["User Options"]["showshortcutbar"] = "1";
	settingsInfo["User Options"]["showfavogroup"] = "0";
	settingsInfo["User Options"]["showtextgroup"] = "1";
	settingsInfo["User Options"]["showcomgroup"] = "1";
	settingsInfo["User Options"]["showdictgroup"] = "1";
	settingsInfo["User Options"]["showbookmarksgroup"] = "1";
	settingsInfo["User Options"]["currentVerseColor"] = "#339966";
	settingsInfo["User Options"]["BibleTextColor"] = "#000000";
	settingsInfo["User Options"]["BibleBGColor"] = "#FFFFFF";
	settingsInfo["User Options"]["BibleVerseNumColor"] = "#000FCF";
	settingsInfo["User Options"]["LinkColor"] = "#898989"; 
	settingsInfo["User Options"]["FoundColor"] = "#D02898";
	settingsInfo["User Options"]["showhistorygroup"] = "1";
	settingsInfo["User Options"]["ShowSplash"] = "1";
	settingsInfo["User Options"]["Daily Devotional"] = "1";
	settingsInfo["User Options"]["NoteScroll"] = "1";
	settingsInfo.Save();
	return true;
}

/******************************************************************************
 * create gnomesword configuration - using sword SWConfig
 ******************************************************************************/
gboolean createconfig(void)
{
	gchar buf[80], buf2[255];

	sprintf(buf2, "%s/preferences.conf", gSwordDir);
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
	
	settingsInfo["Fonts"]["Default"] =  "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1";   
	settingsInfo["Fonts"]["Greek"] =	"-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1"; 
	settingsInfo["Fonts"]["Hebrew"] = "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1"; 
	settingsInfo["Fonts"]["Unicode"] = "-adobe-helvetica-medium-o-normal-*-12-*-*-*-p-*-iso8859-1"; 
	
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
	settingsInfo["User Options"]["showbookmarksgroup"] = "1";
	settingsInfo["User Options"]["interlinearpage"] = "1";
	settingsInfo["User Options"]["showhistorygroup"] = "1";
	settingsInfo["User Options"]["ShowSplash"] = "1";
	settingsInfo["User Options"]["Daily Devotional"] = "1";
	settingsInfo["User Options"]["NoteScroll"] = "1";
	
	settingsInfo.Save();
	return true;
}
