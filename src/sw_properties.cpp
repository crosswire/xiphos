/* -*- Mode: C++; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                                 sw_properties.cpp
                             -------------------
     begin                : Sat June 30 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@users.sf.net   
 ***************************************************************************/
 
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
 *  You should have received a copy of the GNU Library General Public License
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

extern gchar *gSwordDir;
extern SETTINGS *settings;

GS_FONTS *gsfonts,
		mfonts;
		
extern GS_LAYOUT 		gslayout;
extern GS_NB_PAGES 	npages,	
					*nbpages;
extern GS_TABS	tabs,
				*p_tabs;
extern GS_LEXICON *p_gslexicon,
				gslexicon;
/******************************************************************************
 * load gnomesword configuration - using sword SWConfig
 ******************************************************************************/
gboolean loadconfig(void)
{	
	gchar buf[255];
	
	sprintf(buf,"%s/preferences.conf",gSwordDir); 
	SWConfig settingsInfo(buf); 
	settingsInfo.Load();
	//g_warning("buf = %s",buf);
	
	gslayout.shortcutbar_width = atoi(settingsInfo["LAYOUT"]["Shortcutbar"].c_str()); 
	gslayout.upperpane_hight = atoi(settingsInfo["LAYOUT"]["UperPane"].c_str()); 
	gslayout.biblepane_width = atoi(settingsInfo["LAYOUT"]["BiblePane"].c_str()); 
	gslayout.gs_width = atoi(settingsInfo["LAYOUT"]["AppWidth"].c_str()); 
	gslayout.gs_hight = atoi(settingsInfo["LAYOUT"]["AppHight"].c_str()); 
	/* which lexicon to open when storngs numbers are clicked */
	sprintf(gslexicon.greek, "%s", settingsInfo["LEXICONS"]["Greek"].c_str()); 
	sprintf(gslexicon.hebrew, "%s", settingsInfo["LEXICONS"]["Hebrew"].c_str()); 
	
	sprintf(mfonts.bible_font_size, "%s", settingsInfo["FontSize"]["BibleWindow"].c_str()); 
	sprintf(mfonts.commentary_font_size, "%s", settingsInfo["FontSize"]["CommentaryWindow"].c_str()); 
    	sprintf(mfonts.dictionary_font_size, "%s", settingsInfo["FontSize"]["DictionaryWindow"].c_str()); 
	sprintf(mfonts.interlinear_font_size, "%s", settingsInfo["FontSize"]["InterlinearWindow"].c_str()); 
	sprintf(mfonts.verselist_font_size, "%s", settingsInfo["FontSize"]["VerseListWindow"].c_str()); 
	
    	sprintf(settings->MainWindowModule, "%s",settingsInfo["Modules"]["MainWindow"].c_str());
    	sprintf(settings->Interlinear1Module, "%s",settingsInfo["Modules"]["Interlinear1"].c_str());
	sprintf(settings->Interlinear2Module, "%s",settingsInfo["Modules"]["Interlinear2"].c_str() );
	sprintf(settings->Interlinear3Module, "%s",settingsInfo["Modules"]["Interlinear3"].c_str());
	sprintf(settings->Interlinear4Module, "%s",settingsInfo["Modules"]["Interlinear4"].c_str()); 
	sprintf(settings->Interlinear5Module, "%s",settingsInfo["Modules"]["Interlinear5"].c_str());
    	sprintf(settings->personalcommentsmod, "%s",settingsInfo["Modules"]["PerComments"].c_str()); 
	/* notebook pages */
	npages.nbTextModspage = atoi(settingsInfo["Notebooks"]["BiblePage"].c_str());
	npages.notebook1page = atoi(settingsInfo["Notebooks"]["CommentaryPage"].c_str());
	npages.notebook2page = atoi(settingsInfo["Notebooks"]["Dict/LexPage"].c_str()); 
	npages.notebook3page = atoi(settingsInfo["Notebooks"]["notebook3page"].c_str());
	
	sprintf(settings->currentverse, "%s",settingsInfo["Keys"]["verse"].c_str() ); 
	sprintf(settings->dictkey, "%s",settingsInfo["Keys"]["dictionarykey"].c_str()); 
	
	//settingsInfo["StudyPad"]["lastfile"] = settings->studypadfilename;
	
	sprintf(settings->currentverse_color, "%s",settingsInfo["User Options"]["currentVerseColor"].c_str()); 
	 
	settings->strongs = atoi(settingsInfo["User Options"]["strongs"].c_str());
	settings->footnotes = atoi(settingsInfo["User Options"]["footnotes"].c_str());	
	settings->versestyle = atoi(settingsInfo["User Options"]["versestyle"].c_str());
	settings->autosavepersonalcomments = atoi(settingsInfo["User Options"]["autosavepersonalcomments"].c_str());
	settings->formatpercom = atoi(settingsInfo["User Options"]["formatpercom"].c_str());
	settings->showshortcutbar = atoi(settingsInfo["User Options"]["showshortcutbar"].c_str());
	settings->showtextgroup = atoi(settingsInfo["User Options"]["showtextgroup"].c_str());	
	settings->showcomgroup = atoi(settingsInfo["User Options"]["showcomgroup"].c_str());
	settings->showdictgroup = atoi(settingsInfo["User Options"]["showdictgroup"].c_str());
	settings->showbookmarksgroup = atoi(settingsInfo["User Options"]["showbookmarksgroup"].c_str());	
	settings->interlinearpage = atoi(settingsInfo["User Options"]["interlinearpage"].c_str());
	settings->showhistorygroup = atoi(settingsInfo["User Options"]["showhistorygroup"].c_str());
	settings->shortcutbarsize = atoi(settingsInfo["User Options"]["shortcutbarsize"].c_str());  
	
	tabs.textwindow= atoi(settingsInfo["User Options"]["BibleTabs"].c_str());
	tabs.commwindow= atoi(settingsInfo["User Options"]["CommTabs"].c_str());
	tabs.dictwindow= atoi(settingsInfo["User Options"]["DictTabs"].c_str());
  	//g_warning(settingsInfo["TEST"]["mytest"].c_str());
	p_tabs = &tabs;
	nbpages = &npages;
	gsfonts = &mfonts;
	p_gslexicon = &gslexicon;
	return true;
}

/******************************************************************************
 * save gnomesword configuration - using sword SWConfig
 ******************************************************************************/
gboolean saveconfig(void)
{
	gchar buf[80], buf2[255];	
	
	sprintf(buf2,"%s/preferences.conf",gSwordDir); 
	SWConfig settingsInfo(buf2); 
    	settingsInfo["Modules"]["MainWindow"] = settings->MainWindowModule;
    	settingsInfo["Modules"]["Interlinear1"] = settings->Interlinear1Module;
	settingsInfo["Modules"]["Interlinear2"] = settings->Interlinear2Module;
	settingsInfo["Modules"]["Interlinear3"] = settings->Interlinear3Module; 
	settingsInfo["Modules"]["Interlinear4"] = settings->Interlinear4Module; 
	settingsInfo["Modules"]["Interlinear5"] = settings->Interlinear5Module; 
    	settingsInfo["Modules"]["PerComments"] = settings->personalcommentsmod; 
	
	settingsInfo["LEXICONS"]["Greek"]=p_gslexicon->greek; 
	settingsInfo["LEXICONS"]["Hebrew"]=p_gslexicon->hebrew; 
	
	
	sprintf(buf, "%d",nbpages->nbTextModspage);
	settingsInfo["Notebooks"]["BiblePage"] = buf; //-- commentaries notebook
	sprintf(buf, "%d",nbpages->notebook1page);
	settingsInfo["Notebooks"]["CommentaryPage"] = buf; //-- commentaries notebook
	sprintf(buf, "%d",nbpages->notebook2page);
	settingsInfo["Notebooks"]["Dict/LexPage"] = buf; //-- dict - lex notebook; 
	sprintf(buf, "%d",nbpages->notebook3page);
	settingsInfo["Notebooks"]["notebook3page"] = buf; 
	
	sprintf(buf, "%d",p_tabs->textwindow);
	settingsInfo["User Options"]["BibleTabs"]=buf;
	sprintf(buf, "%d",p_tabs->commwindow);
	settingsInfo["User Options"]["CommTabs"]=buf;
	sprintf(buf, "%d",p_tabs->dictwindow);
	settingsInfo["User Options"]["DictTabs"]=buf;
	 
	settingsInfo["Keys"]["verse"] = settings->currentverse; 
	settingsInfo["Keys"]["dictionarykey"] = settings->dictkey; 
	
	settingsInfo["StudyPad"]["lastfile"] = settings->studypadfilename;
	
	settingsInfo["FontSize"]["BibleWindow"] = gsfonts->bible_font_size; 
	settingsInfo["FontSize"]["CommentaryWindow"] = gsfonts->commentary_font_size; 
    	settingsInfo["FontSize"]["DictionaryWindow"] = gsfonts->dictionary_font_size; 
	settingsInfo["FontSize"]["InterlinearWindow"] = gsfonts->interlinear_font_size;
	settingsInfo["FontSize"]["VerseListWindow"] = gsfonts->verselist_font_size;
	
	sprintf(buf,"%d",gslayout.shortcutbar_width);	
	settingsInfo["LAYOUT"]["Shortcutbar"] = buf ;
	sprintf(buf,"%d",gslayout.upperpane_hight);
	settingsInfo["LAYOUT"]["UperPane"] = buf ;
	sprintf(buf,"%d",gslayout.biblepane_width);
	settingsInfo["LAYOUT"]["BiblePane"] = buf ;
	sprintf(buf,"%d",gslayout.gs_width); 
	settingsInfo["LAYOUT"]["AppWidth"] = buf ;
	sprintf(buf,"%d",gslayout.gs_hight);
	settingsInfo["LAYOUT"]["AppHight"] = buf ;
	
	settingsInfo["User Options"]["currentVerseColor"] = settings->currentverse_color;
	
	if(settings->strongs)
		settingsInfo["User Options"]["strongs"] = "1";
	else
		settingsInfo["User Options"]["strongs"] = "0";
	
	if(settings->footnotes)
		settingsInfo["User Options"]["footnotes"] = "1";
	else
		settingsInfo["User Options"]["footnotes"] = "0";
	
	if(settings->versestyle)
		settingsInfo["User Options"]["versestyle"] = "1";
	else
		settingsInfo["User Options"]["versestyle"] = "0";
	
	if(settings->autosavepersonalcomments)
		settingsInfo["User Options"]["autosavepersonalcomments"] = "1";
	else
		settingsInfo["User Options"]["autosavepersonalcomments"] = "0";
	
	if(settings->formatpercom)
		settingsInfo["User Options"]["formatpercom"] = "1";
	else
		settingsInfo["User Options"]["formatpercom"] = "0";
	
	if(settings->showshortcutbar)
		settingsInfo["User Options"]["showshortcutbar"] = "1";
	else
		settingsInfo["User Options"]["showshortcutbar"] = "0";
	
	if(settings->showtextgroup)
		settingsInfo["User Options"]["showtextgroup"] = "1";
	else
		settingsInfo["User Options"]["showtextgroup"] = "0";
	
	if(settings->showcomgroup)
		settingsInfo["User Options"]["showcomgroup"] = "1";
	else
		settingsInfo["User Options"]["showcomgroup"] = "0";
	
	if(settings->showdictgroup)
		settingsInfo["User Options"]["showdictgroup"] = "1";
	else
		settingsInfo["User Options"]["showdictgroup"] = "0";
	
	if(settings->showbookmarksgroup)
		settingsInfo["User Options"]["showbookmarksgroup"] = "1";
	else
		settingsInfo["User Options"]["showbookmarksgroup"] = "0";
	
	if(settings->interlinearpage)
		settingsInfo["User Options"]["interlinearpage"] = "1";
	else
		settingsInfo["User Options"]["interlinearpage"] = "0";
	
	if(settings->showhistorygroup)
		settingsInfo["User Options"]["showhistorygroup"] = "1";
	else
		settingsInfo["User Options"]["showhistorygroup"] = "0";
		
	sprintf(buf, "%d",settings->shortcutbarsize);
	settingsInfo["User Options"]["shortcutbarsize"] = buf; 
	
	
	//settingsInfo["TEST"]["mytest"] = "KJV,ROM 3:23, MHC, John 3:16";
	
    	settingsInfo.Save();
	return true;
}
