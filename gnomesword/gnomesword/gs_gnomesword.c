/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_gnomesword.c
                             -------------------
    begin                : Tue Dec 05 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif
#include <gnome.h>
#include "gs_gnomesword.h"
#include "GnomeSword.h"
#include "callback.h"
#include "support.h"
#include "interface.h"
#include "filestuff.h"
#include "menustuff.h"
#include "listeditor.h"
//#include "noteeditor.h"
#include  <widgets/shortcut-bar/e-shortcut-bar.h>
#include  <widgets/e-paned/e-paned.h>
#include  <widgets/e-paned/e-hpaned.h>
#include  <widgets/e-paned/e-vpaned.h>


extern GList 	*biblemods,
		*commentarymods,
		*dictionarymods,
		*percommods;
extern gchar *mydictmod;		
extern GtkWidget *versestyle,	//-- widget to access toggle menu - for versestyle
		*footnotes,	//-- widget to access toggle menu - for footnotes
  		*strongsnum,//-- widget to access toggle menu - for strongs numbers
	    	*notepage,	//-- widget to access toggle menu - for interlinear notebook page
	    	*autosaveitem, //-- widget to access toggle menu - for personal comments auto save
		*studypad,  //-- studypad text widget
		*notes;    //-- notes text widget					
extern GdkColor myGreen; //-- current verse color
extern gboolean havedict; //-- let us know if we have at least one lex-dict module
extern gboolean havecomm; //-- let us know if we have at least one commentary module
extern gboolean usepersonalcomments; //-- do we setup for personal comments - default is false
extern gboolean bVerseStyle;
extern gboolean autoSave;

gint dictpages,
	 compages;
SETTINGS *settings;



/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void
initGnomeSword(GtkWidget *app, SETTINGS *settings, GList *biblemods, GList *commentarymods , GList *dictionarymods, GList *percommods)
{
	GtkLabel *label1;
	GtkWidget *notebook;
	
//-- set color for current verse
  	myGreen.red =  settings->currentverse_red; 
	myGreen.green = settings->currentverse_green;
	myGreen.blue =  settings->currentverse_blue;		
//-- add modules to menus -- menu.c
	addmodstomenus(app, settings, biblemods, 
			commentarymods, dictionarymods,
			percommods);
//-- create popup menus -- menu.c
	createpopupmenus(app, settings, biblemods, 
			commentarymods, dictionarymods,
			percommods);
//-- add pages to commentary and  dictionary notebooks			
	addnotebookpages(lookup_widget(app,"notebook1"), commentarymods);
	addnotebookpages(lookup_widget(app,"notebook4"), dictionarymods);	
//--  set text windows to word warp
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"moduleText")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textComp1")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textComp2")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textComp3")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textDict")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textCommentaries")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textComments")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"text3")) , TRUE );
//-- set main notebook page
	gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(app,"notebook3")),settings->notebook3page );        
//-- store text widgets for spell checker
	notes =  lookup_widget(app,"textComments");
	studypad = lookup_widget(app,"text3");				
//-- Add options to Options Menu and get toggle item widget
	autosaveitem = additemtooptionmenu(app, "_Settings/", "Auto Save Personal Comments", (GtkMenuCallback)on_auto_save_notes1_activate); 
	notepage  = additemtooptionmenu(app, "_Settings/", "Show Interlinear Page", (GtkMenuCallback)on_show_interlinear_page1_activate);
	versestyle = additemtooptionmenu(app, "_Settings/", "Verse Style", (GtkMenuCallback)on_verse_style1_activate);
	footnotes   = additemtooptionmenu(app, "_Settings/", "Show Footnotes", (GtkMenuCallback)on_footnotes1_activate);
 	strongsnum   = additemtooptionmenu(app, "_Settings/", "Show Strongs Numbers", (GtkMenuCallback)on_strongs_numbers1_activate); 		
//-- set dictionary key
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(app,"dictionarySearchText")),settings->dictkey);        
        loadbookmarks_programstart(); //--------------------------------- add bookmarks to menubar
        changeVerse(settings->currentverse); //---------------------------------------------- set Text
//-- show hide shortcut bar - set to options setting
        if(settings->showshortcutbar){
                e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), settings->shortcutbarsize);
        }else{
                e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), 1);
        }
//-- load last used file into studypad
        if(settings->studypadfilename != NULL) loadStudyPadFile(settings->studypadfilename); 	

//-- set dict module to open notebook page
	
	if(havedict){ //-- let's don't do this if we don't have at least one dictionary / lexicon			
		gint pagenum = 0; //-- set page to 0 (first page in notebook)
		//-- if save page is less than actual number of pages use saved page number else 0
		if(settings->notebook2page < dictpages) pagenum = settings->notebook2page; 
		notebook = lookup_widget(app,"notebook4"); //-- get notebook
		//-- set notebook page
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),pagenum ); 	
		//-- get the label from the notebook page (module name)		
		label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook), 
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),pagenum));
		changcurdictModSWORD(label1->label, settings->dictkey, pagenum);	
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                      GTK_SIGNAL_FUNC (on_notebook4_switch_page),
                      NULL);
                if(settings->showdicttabs) gtk_widget_show(notebook);
                else gtk_widget_hide(notebook);
	}else gtk_widget_hide(lookup_widget(app,"hbox8")); //-- hide dictionary section of window if we do not have at least one dict/lex
	
//-- set com module to open notebook page	
	if(havecomm){ //-- let's don't do this if we don't have at least one commentary	
		gint pagenum = 0;
		
		if(settings->notebook1page < compages) pagenum = settings->notebook1page;
		notebook = lookup_widget(app,"notebook1");	
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),pagenum );
		label1 = (GtkLabel *)gtk_notebook_get_tab_label (GTK_NOTEBOOK(notebook),
						gtk_notebook_get_nth_page (GTK_NOTEBOOK(notebook),pagenum));
		changcurcomModSWORD(label1->label, pagenum);
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                                        GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                                        NULL);
                if(settings->showcomtabs) gtk_widget_show(notebook);
                else gtk_widget_hide(notebook);
        }else gtk_notebook_remove_page( GTK_NOTEBOOK(lookup_widget(app,"notebook3")) , 0);	
		
//-- set personal commets notebook label and display module
	if(usepersonalcomments){
		 //-- change personal comments module
		changepercomModSWORD(settings->personalcommentsmod);	
	}
//-- set text modules to last used
	changecurModSWORD(settings->MainWindowModule);
	changecomp1ModSWORD(settings->Interlinear1Module);
	changecomp2ModSWORD(settings->Interlinear2Module);
	changecomp3ModSWORD(settings->Interlinear3Module);
	
//-- hide buttons - only show them if their options are enabled
	gtk_widget_hide(lookup_widget(app,"btnPrint"));
	gtk_widget_hide(lookup_widget(app,"btnSpell"));
	gtk_widget_hide(lookup_widget(app,"btnSpellNotes"));

//-- do not show print button if printing not enabled
#ifdef  USE_GNOMEPRINT
        gtk_widget_show(lookup_widget(app,"btnPrint"));
#endif /* USE_GNOMEPRINT */
//-- do not show spell buttons if spellcheck not enabled
#ifdef USE_ASPELL
        gtk_widget_show (lookup_widget(app,"btnSpell"));
        gtk_widget_show (lookup_widget(app,"btnSpellNotes"));
#endif /* USE_ASPELL */	
//-- free module lists
        g_list_free(biblemods);
        g_list_free(commentarymods);
        g_list_free(dictionarymods);
	g_list_free(percommods);	
}  

/********************************************************************************************** 
 * addnotebookpages - add pages to commentary and dictionary notebooks
 * notebook - notebook to add the pages to
 * list - list of modules
 *********************************************************************************************/ 
void 
addnotebookpages(GtkWidget *notebook, GList *list)
{
	GList *tmp;
	gint pg = 0;
	GtkWidget 
		*empty_notebook_page, //-- used to create new pages
		*label;
		
	tmp = list;
	while (tmp != NULL) {
		empty_notebook_page = gtk_vbox_new (FALSE, 0);
		gtk_widget_show (empty_notebook_page);
		gtk_container_add (GTK_CONTAINER (notebook), empty_notebook_page);
		label = gtk_label_new ((gchar *) tmp->data);
		gtk_widget_show (label);
		gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook), 
			gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pg++), label); 
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);	
} 
  

/*****************************************************************************
 * UpdateChecks(GtkWidget *mainform) update chech menu items 
 * and toggle buttons - called on start up
 *****************************************************************************/
void UpdateChecks(GtkWidget *app)
{      
	if(settings->versestyle)	/* set verse style to last setting used */
		bVerseStyle = TRUE; /* keep VerseStyle in sync with menu */
	else
		bVerseStyle = FALSE; /* keep VerseStyle in sync with menu */
	GTK_CHECK_MENU_ITEM (versestyle)->active = settings->versestyle;
	
	
	if(settings->footnotes)  /* set footnotes to last setting used */
		setglobalopsSWORD("Footnotes","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD("Footnotes","Off" );	/* keep footnotes in sync with menu */			
	GTK_CHECK_MENU_ITEM (footnotes)->active = settings->footnotes;
	
	
	if(settings->interlinearpage)	/* set interlinear page to last setting */
		gtk_widget_show(lookup_widget(app,"vbox3"));	
	else
		gtk_widget_hide(lookup_widget(app,"vbox3"));		
	GTK_CHECK_MENU_ITEM (notepage)->active = settings->interlinearpage; /* set interlinear page menu check item */
	
	
	autoSave = settings->autosavepersonalcomments; /* set auto save personal comments to last setting */	
	GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments; /* set auto save menu check item */
	
	
	if(settings->strongs)	/* set Strong's numbers to last setting */
		setglobalopsSWORD("Strong's Numbers","On");  /* " */ /* keep strongs numbers in sync with menu */
	else
		setglobalopsSWORD("Strong's Numbers","Off"); /* ' */ /* keep strongs numbers in sync with menu */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(app,"btnStrongs")), settings->strongs);	/* set strongs toogle button */
	GTK_CHECK_MENU_ITEM (strongsnum)->active = settings->strongs;
	
        /*if(havedict) FillDictKeys(mydictmod);*/  /* fill the dict key clist */
        changeVerse(settings->currentverse);  /* set Text - apply changes */
}

void
applyoptions(GtkWidget *app,gboolean showshortcut, gboolean showcomtabs, gboolean showdicttabs, gboolean showtextgroup,
                gboolean showcomgroup, gboolean showdictgroup, gboolean showhistorygroup)
{
         GtkWidget    *dict,
                      *comm;

        dict = lookup_widget(app,"notebook4");
        comm = lookup_widget(app,"notebook1");

        settings->showshortcutbar = showshortcut;
        settings->showcomtabs = showcomtabs;
        settings->showdicttabs = showdicttabs;

        settings->showtextgroup = showtextgroup;
        settings->showcomgroup =  showcomgroup;
        settings->showdictgroup = showdictgroup;
        settings->showhistorygroup = showhistorygroup;

        if(settings->showshortcutbar){
               e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), settings->shortcutbarsize);
        } else {
               e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), 1); 
        }
        if(settings->showcomtabs) {
                gtk_widget_show(comm);
        } else {
                gtk_widget_hide(comm);
        }
        if(settings->showdicttabs) {
                gtk_widget_show(dict);
        } else {
                gtk_widget_hide(dict);
        }
}

/******************************************************************************
 * add_sb_group - add group to shourtcut bar
 * shortcut_bar - shortcut bar to add group to
 * group_name - name of the group to be added
 *****************************************************************************/
gint
add_sb_group(EShortcutBar *shortcut_bar,gchar *group_name)
{
	gint group_num;
	
	group_num = e_shortcut_model_add_group (shortcut_bar->model, -1, group_name); 	
        e_shortcut_bar_set_view_type (shortcut_bar, group_num, E_ICON_BAR_SMALL_ICONS);
        return group_num;
}
