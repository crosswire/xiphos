/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_gnomesword.c
    * -------------------
    * Tue Dec 05 2000
    * copyright (C) 2001 by tbiggs
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
#include <ctype.h>


#include "gs_gnomesword.h"
#include "sw_gnomesword.h"
#include "gs_gui_cb.h"
#include "gs_mainmenu_cb.h"
#include "support.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_shortcutbar.h"
#include "e-splash.h"

#include <gal/e-paned/e-hpaned.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>

/*****************************************************************************
* globals
*****************************************************************************/	
GtkWidget
	*notepage,	/* widget to access toggle menu - for interlinear notebook page */
	*autosaveitem, /* widget to access toggle menu - for personal comments auto save */
	*notes;    /* notes text widget */
	
gboolean 
	file_changed = FALSE, /* set to true if text is study pad has changed - and file is not saved  */
	changemain = TRUE; /* change verse of Bible text window */
	
gchar 
	*current_filename= NULL;	/* filename for open file in study pad window */
	
gchar 
	current_verse[80]=N_("Romans 8:28");	/* current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window */
	
gint 
	dictpages,
	compages,
	textpages;
	
SETTINGS 
	*settings;
		
/*****************************************************************************
* externs
*****************************************************************************/
extern GList 
	*biblemods,
	*commentarymods,
	*dictionarymods,
	*percommods,	
	*sbfavoritesmods,
	*sbbiblemods,
	*sbdictmods,
	*sbcommods,
	*options;
	
extern gchar 
	*mydictmod,
	*shortcut_types[],
	*mycolor,
	rememberlastitem[];
	
extern gboolean 
	havedict, /* let us know if we have at least one lex-dict module */
	havecomm, /* let us know if we have at least one commentary module */
	havebible, /* let us know if we have at least one Bible text module */
	usepersonalcomments, /* do we setup for personal comments - default is FALSE */
	autoSave,
	addhistoryitem; /* do we need to add item to history */
	
extern gint 
	groupnum4,
	iquickmarks;	/* number of items in bookmark menu  -- declared in filestuff.cpp */

/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void
initGnomeSword(GtkWidget *app, SETTINGS *settings,
		GList *biblemods, GList *commentarymods ,
		GList *dictionarymods, GList *percommods,
		GtkWidget *splash)
{
	GtkWidget 
		*notebook;
	gint		
		biblepage,
		commpage,
		dictpage;
	
	g_print("%s\n", "Initiating GnomeSword\n");
	/* set the main window size */
	gtk_window_set_default_size(GTK_WINDOW(app), settings->gs_width, settings->gs_hight);
	/* setup shortcut bar */
	setupSB(settings);
	settings->settingslist = NULL;
	/* set current verse color html */
	mycolor = settings->currentverse_color;	
	/* add modules to menus -- gs_menu.c */
	addmodstomenus(app, 
				settings, 
				biblemods, 
				sbbiblemods, 
				commentarymods,
				sbcommods, 
				dictionarymods,
				sbdictmods,
				percommods);
	/* create popup menus -- gs_menu.c */
	createpopupmenus(app, 
				settings, 
				sbbiblemods,
				sbcommods,
				sbdictmods,
				percommods,
				options);
	additemstooptionsmenu(options,settings);			
	/* add pages to commentary and  dictionary notebooks */
	biblepage = addnotebookpages(lookup_widget(app,"nbTextMods"), biblemods, settings->MainWindowModule);
	commpage = addnotebookpages(lookup_widget(app,"notebook1"), commentarymods, settings->CommWindowModule);
	dictpage = addnotebookpages(lookup_widget(app,"notebook4"), dictionarymods, settings->DictWindowModule);	
	/*  set text windows to word warp */
	gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(app,"nbPerCom")),0);
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textComments")) , TRUE );
	/* set main notebook page */
	gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(app,"notebook3")),
			settings->notebook3page );
	/* store text widgets for spell checker */
	notes =  lookup_widget(app,"textComments");		
	/* Add options to Options Menu and get toggle item widget */
	autosaveitem = additemtooptionmenu(app, _("_Settings/"), _("Auto Save Personal Comments"),
				(GtkMenuCallback)on_auto_save_notes1_activate);
	notepage  = additemtooptionmenu(app, _("_Settings/"), _("Show Interlinear Page"),
				(GtkMenuCallback)on_show_interlinear_page1_activate);
	settings->versestyle_item = additemtooptionmenu(app, _("_Settings/"), _("Verse Style"),
				(GtkMenuCallback)on_verse_style1_activate);
	/* set dictionary key */
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(app,"dictionarySearchText")),settings->dictkey);
        loadquickmarks_programstart(); /* add bookmarks to menubar */
        changeVerseSWORD(settings->currentverse); /* set Text */
	/* show hide shortcut bar - set to options setting */
        if(settings->showshortcutbar){
                e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), settings->shortcutbar_width);
        }else{
                e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), 1);
        }
	/* set hight of bible and commentary pane */
	e_paned_set_position(E_PANED(lookup_widget(app,"vpaned1")), settings->upperpane_hight);
	/* set width of bible pane */
	e_paned_set_position(E_PANED(lookup_widget(app,"hpaned1")), settings->biblepane_width);
	
	/* set Bible module to open notebook page */
	/* let's don't do this if we don't have at least one text module */	
	if(havebible){ 	
		if(biblepage == 0)
			changecurModSWORD(settings->MainWindowModule, TRUE); 
		/* get notebook */
		notebook = lookup_widget(app,"nbTextMods");
		gtk_signal_connect(GTK_OBJECT(notebook), "switch_page",
			   GTK_SIGNAL_FUNC(on_nbTextMods_switch_page),
			   NULL);			
		/* set notebook page */
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook), biblepage); 
                if(settings->text_tabs) 
			gtk_widget_show(notebook);
                else 
			gtk_widget_hide(notebook);
	}
		
	/* set dict module to open notebook page */
	/* let's don't do this if we don't have at least one dictionary / lexicon */	
	if(havedict){ 	
		if(dictpage == 0) 
			changcurdictModSWORD(settings->DictWindowModule, settings->dictkey);
		/* get notebook */
		notebook = lookup_widget(app,"notebook4");
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                      GTK_SIGNAL_FUNC (on_notebook4_switch_page),
                      NULL);		
		/* set notebook page */
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook), dictpage); 	
		if(settings->dict_tabs) 
			gtk_widget_show(notebook);
                else 
			gtk_widget_hide(notebook);
         /* hide dictionary section of window if we do not have at least one dict/lex */
	}else 
		gtk_widget_hide(lookup_widget(app,"hbox8"));
	 
	/* set com module to open notebook page */	
	if(havecomm){ /* let's don't do this if we don't have at least one commentary */ 
		if(commpage == 0)
			changcurcomModSWORD(settings->CommWindowModule, TRUE);  		
		notebook = lookup_widget(app,"notebook1");
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook), commpage);		
                if(settings->comm_tabs) {
			gtk_widget_show(notebook);
		}
                else 
			gtk_widget_hide(notebook);
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                                        GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                                        NULL);	
        }else 
		gtk_notebook_remove_page( GTK_NOTEBOOK(lookup_widget(app,"notebook3")) , 0);	
		
	/* set personal commets notebook label and display module */
	if(usepersonalcomments){
		 /* change personal comments module */
		changepercomModSWORD(settings->personalcommentsmod);	
	}
	
	/* free module lists */
        g_list_free(biblemods);
        g_list_free(commentarymods);
        g_list_free(dictionarymods);
	g_list_free(percommods);	
        g_list_free(sbbiblemods);
        g_list_free(sbcommods);
        g_list_free(sbdictmods);
       // options list freed on exit
	
	if(settings->showsplash){	
		while (gtk_events_pending ())
				gtk_main_iteration ();
	}
	
	g_print("%s\n", "done\n");
}

/**********************************************************************************************
 * addnotebookpages - add pages to commentary and dictionary notebooks
 * notebook - notebook to add the pages to
 * list - list of modules - add one page per module
 *********************************************************************************************/
gint
addnotebookpages(GtkWidget *notebook, 
			GList *list, 
			gchar *modName)
{
	GList 
		*tmp;
	gint 
		pg = 0, 
		retVal = 0;
	GtkWidget
		*empty_notebook_page, /* used to create new pages */
		*label;
	GtkLabel 
		*mylabel;
	
	tmp = list;
	while (tmp != NULL) {
		empty_notebook_page = gtk_vbox_new (FALSE, 0);
		gtk_widget_show (empty_notebook_page);
		gtk_container_add (GTK_CONTAINER (notebook), empty_notebook_page);
		label = gtk_label_new ((gchar *) tmp->data);
		mylabel = GTK_LABEL(label);
		if(!strcmp((gchar*)mylabel->label, modName)) /* set retVal to saved mod's page number */
			retVal = pg;
		gtk_widget_show (label);
		gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook),
			gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook), pg++), label);
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
	return retVal;
}

/*****************************************************************************
 * UpdateChecks(GtkWidget *app) update chech menu items
 * and toggle buttons - called on start up
 *****************************************************************************/
void UpdateChecks(GtkWidget *app)
{
	/* does user want verses or paragraphs */
	GTK_CHECK_MENU_ITEM (settings->versestyle_item)->active = settings->versestyle;		
	/* set interlinear page to last setting */
	if(settings->interlinearpage)
		gtk_widget_show(lookup_widget(app,"vboxInt"));	
	else
		gtk_widget_hide(lookup_widget(app,"vboxInt"));	
	/* set interlinear page menu check item */		
	GTK_CHECK_MENU_ITEM (notepage)->active = settings->interlinearpage; 
	/* set interlinear footnotes to last setting used */
	if(settings->footnotesint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Footnotes","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Footnotes","Off" );	/* keep footnotes in sync with menu */	
	/* set interlinear Strong's Numbers to last setting used */
	if(settings->strongsint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Strong's Numbers","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Strong's Numbers","Off" );	/* keep footnotes in sync with menu */
	/* set interlinear morph tags to last setting used */
	if(settings->morphsint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Morphological Tags","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Morphological Tags","Off" );	/* keep footnotes in sync with menu */	
	/* set auto save personal comments to last setting */
	autoSave = settings->autosavepersonalcomments;
	/* set auto save menu check item */	
	GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments; 	
        /* fill the dict key clist */
        if(havedict) FillDictKeysSWORD();
        /* set Text - apply changes */
        addhistoryitem = FALSE;
        changeVerseSWORD(settings->currentverse);
}

/*****************************************************************************
 *setformatoption
 *button
*****************************************************************************/
void setformatoption(GtkWidget *button)
{
   settings->formatpercom = GTK_TOGGLE_BUTTON(GTK_BUTTON(button))->active;
   /*if(settings->formatpercom){
   		//percomMod->Disp(FPNDisplay);   		
   }*/
}

/*****************************************************************************
 *changepagenotebook - someone changed the page in the main notebook
 *notebook - notebook widget - main notebook
 *page_num - notebook page number
*****************************************************************************/
void changepagenotebook(GtkNotebook *notebook,gint page_num)
{
        settings->notebook3page = page_num; /* store the page number so we can open to it the next time we start */
        changemain = FALSE; /* we don't want to cause the Bible text window to scrool */
        if(page_num < 3) changeVerseSWORD(current_verse); /* if we changed to page 0, 1 or 2 */
}


/*****************************************************************************
 *addQuickmark - someone clicked add quickmark to get us here
*****************************************************************************/
void addQuickmark(GtkWidget *app)  
{
	gchar    *bookname;      /* pointer to the Bible book we want to mark */
	gint     iVerse,         /* verse we want to mark */
		 iChap;          /* chapter we want to mark */
	gchar	 buf[255];
	/* get book name */
	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(app,"cbeBook"))); 
	/* get verse number */
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(app,"spbVerse"))); 
	/* get chapter */
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(app,"spbChapter"))); 
	/* put book chapter and verse into bookmarks array */
	sprintf(buf,"%s %d:%d%c",bookname, iChap,iVerse, '\0' );
	/* increment number of bookmark item + 1 */
	 ++iquickmarks; 
	if(iquickmarks == 1)
		sprintf(rememberlastitem,"%s","<Separator>");
	 /* save to file so we dont forget -- function in gs_file.c */
	savequickmark(buf); 
	//-- remove old bookmarks from menu -- gs_menu.c
	removemenuitems(app, _("_Quickmarks/<Separator>"), iquickmarks); 
        sprintf(buf,"%s", _("_Quickmarks/Clear Quickmarks"));
        addseparator(app, buf);
	
	/* let's show what we did -- gs_file.c */
        loadquickmarks_afterSeparator(); 
}

/*****************************************************************************
 * showIntPage - do we want to see interlinear page?
 * choice
*****************************************************************************/
void showIntPage(GtkWidget *app, gboolean choice)  
{
	GtkWidget *intpage,*frame; /* pointer to interlinear notebook page */

	intpage= lookup_widget(app,"vboxInt"); /* set pointer to page */
	frame= lookup_widget(app,"frame2"); /* set pointer to page */
	if(choice){
		gtk_widget_show(intpage);		/* show page */
		gtk_widget_show(frame);		/* show page */		
	} else {
		gtk_widget_hide(intpage);		/* hide page */
		gtk_widget_hide(frame);
	}
	settings->interlinearpage = choice;  /* remember choice for next program startup */
}


/*****************************************************************************
 * setautosave - someone clicked auto save personal  comments
 * choice
*****************************************************************************/
void setautosave(gboolean choice)    
{
	if(choice){ /* if choice was to autosave */	
		autoSave = TRUE;
	} else {     /* if choice was not to autosave	 */
		autoSave = FALSE;
	}
	settings->autosavepersonalcomments = choice; /* remember our choice for next startup */
}


/*****************************************************************************
 *   -string_is_color- this code is from bluefish-0.6
 *
*****************************************************************************/
gint string_is_color(gchar *color) 
{
	gint i;

	if (!color) {
		g_warning("string_is_color, pointer NULL\n");
		return 0;
	}
	if (strlen(color) != 7) {
		g_warning("string_is_color, strlen(%s) != 7\n", color);
		return 0;
	}
	if (color[0] != '#') {
		g_warning("string_is_color, 0 in %s is not #\n", color);
		return 0;
	}
	for (i = 1; i <7 ; i++) {
		if ((color[i] > 102) 
				|| (color[i] < 48) 
				|| ((color[i] > 57) && (color[i] < 65))
				|| ((color[i] > 70) && (color[i] < 97))) {
			g_warning("string_is_color, %d in %s is not from a color, it is %d\n", i, color, color[i]);
			return 0;
		}
	}
	//g_warning("string_is_color, %s is color\n", color);
	return 1;

}

/*****************************************************************************
 * gdouble_arr_to_hex  -- this code is from bluefish-0.6
 *
*****************************************************************************/
gchar *gdouble_arr_to_hex(gdouble *color, gint websafe)
{
	gchar *tmpstr;
	unsigned int red_int;
	unsigned int green_int;
	unsigned int blue_int;
	gdouble red;
	gdouble green;
	gdouble blue;
	
	red = color[0];
	green = color[1];
	blue = color[2];

	if (websafe) {
		red_int = 0x33*((unsigned int) (red * 255 / 0x33));
		green_int = 0x33*((unsigned int) (green * 255/0x33));
		blue_int = 0x33*((unsigned int) (blue * 255/0x33));
	} else {
		red_int = (unsigned int) (red * 255);
		green_int = (unsigned int) (green * 255);
		blue_int = (unsigned int) (blue * 255);
	}
	tmpstr = g_malloc(8*sizeof(char));
	g_snprintf (tmpstr, 8,"#%.2X%.2X%.2X", red_int, green_int, blue_int);
	return tmpstr;
}

/*****************************************************************************
 *  hex_to_gdouble_arr -- this code is from bluefish-0.6
 *
*****************************************************************************/
gdouble *hex_to_gdouble_arr(gchar *color)
{
	static gdouble tmpcol[4];
	gchar tmpstr[8];
	long tmpl;
	
	
	strncpy(tmpstr, &color[1], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[0] = (gdouble) tmpl;
	
	strncpy(tmpstr, &color[3], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[1] = (gdouble) tmpl;
	
	strncpy(tmpstr, &color[5], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[2] = (gdouble) tmpl;

	//g_warning("hex_to_gdouble_arr, R=%d, G=%d, B=%d\n", color[0], color[1], color[2]);

	tmpcol[3] = 0;
	
	return tmpcol;
}

