/***************************************************************************
                          searchstuff.h  -  description
                             -------------------
    begin                : Wed Jul 26 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@users.sourceforge.net
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>
#include <swdisp.h>
#include <versekey.h>

GtkWidget* create_dlgSearch(void);


class SearchWindow
{
        GtkWidget *dlgSearch;
        GtkWidget *dialog_vbox1;
 
        GtkWidget *frame8;
        GtkWidget *searchPanel;
        GtkWidget *hbox4;
        GtkWidget *toolbar24;
        GtkWidget *tmp_toolbar_icon;
        GtkWidget *btnSearchSaveList;
        GtkWidget *entry1;
        GtkWidget *btnSearch1;
        GtkWidget *searchSettingsPanel;
        GtkWidget *frame15;
        GtkWidget *searchTypePanel;
        GSList *searchType_group;
        GtkWidget *regexSearch;
        GtkWidget *phraseSearch;
        GtkWidget *multiWordSearch;
        GtkWidget *frame16;
        GtkWidget *searchOptionsPanel;
        GtkWidget *caseSensitive;
        GtkWidget *ckbCom;
        GtkWidget *cbpercom;
        GtkWidget *frame20;
        GtkWidget *hbox17;
        GSList *_3_group;
        GtkWidget *rbNoScope;
        GtkWidget *rbUseBounds;
        GtkWidget *rbLastSearch;
        GtkWidget *table6;
        GtkWidget *label82;
        GtkWidget *label83;
        GtkWidget *entryLower;
        GtkWidget *entryUpper;
        GtkWidget *scrolledwindow2;
        GtkWidget *label46;
        GtkWidget *scrolledwindow20;
        GtkWidget *txtSearch;
        GtkWidget *cbContext;
        GtkWidget *hbox15;
        GtkWidget *label57;
        GtkWidget *lbSearchHits;
        GtkWidget *dialog_action_area1;
        GtkWidget *btnSearchOK;
        GtkAdjustment *adj;
        SWDisplay *PLAINsearchDisplay; //--- to display GBF modules using GtkText a verse at a time
        SWDisplay *HTMLsearchDisplay; //--- to display HTML modules using GtkText a verse at a time
        SWDisplay *RWPsearchDisplay; //--- to display RWP module using GtkText a verse at a time
        SWMgr *searchMgr; //-- sword mgr for searchMod - module used for searching
        SWModule *searchMod; //-- module for searching and search window
        VerseKey searchScopeLowUp; //----------- sets lower and upper search bounds
        ListKey	searchScopeList; //----------- search list for searching verses found on last search
        SWKey	*currentScope; //----------- use to set scope of search
        SWFilter *plaintohtml;
        SWFilter *gbftohtml;
        SWFilter *rwphtml;
	//void percentUpdate(char percent, void *userData); 
public:
	SearchWindow();
	~SearchWindow();
	GtkWidget *create();
	GtkWidget *progressbar;
        GtkWidget *resultList;
        void searchSWORD(GtkWidget *searchFrm);  //-- search Bible text or commentaries
        void resultsListSWORD(GtkWidget *searchFrm, gint row, gint column); //-- someone clicked the results list
        void initsearchWindow(GtkWidget *searchDlg); //-- init search dialog
};

extern SearchWindow *searchWindow;

