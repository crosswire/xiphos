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
#include "sw_sword.h"
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
	*bookmods,
	*percommods,	
	*sbfavoritesmods,
	*sbbiblemods,
	*sbdictmods,
	*sbcommods,
	*sbbookmods,
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
	iquickmarks;	/* number of items in bookmark menu  -- declared in gs_file.c */

/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void
initGnomeSword(SETTINGS *s,
			GList *biblemods, 
			GList *commentarymods ,
			GList *dictionarymods, 
			GList *percommods,
			GtkWidget *splash)
{
	GtkWidget 
		*notebook;
	gint		
		biblepage,
		commpage,
		dictpage;
	
	g_print("%s\n", "Initiating GnomeSword\n");
	/* setup shortcut bar */
	setupSB(s);
	s->settingslist = NULL;
	s->displaySearchResults = FALSE;
	/* set current verse color html */
	mycolor = s->currentverse_color;
	/* add modules to menus -- gs_menu.c */
	addmodstomenus(s, 
			biblemods, 
			commentarymods,
			dictionarymods,
			bookmods,
			percommods);
	/* create popup menus -- gs_menu.c */
	createpopupmenus(s, 
			sbbiblemods,
			sbcommods,
			sbdictmods,
			sbbookmods,
			percommods,
			options);
	additemstooptionsmenu(options,s);			
	/* add pages to module notebooks */
	biblepage = addnotebookpages(lookup_widget(s->app,"nbTextMods"), biblemods, s->MainWindowModule);
	commpage = addnotebookpages(lookup_widget(s->app,"notebook1"), commentarymods, s->CommWindowModule);
	dictpage = addnotebookpages(lookup_widget(s->app,"notebook4"), dictionarymods, s->DictWindowModule);
	
	
	gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(s->app,"nbPerCom")),0);	
	/*  set text windows to word warp */
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(s->app,"textComments")) , TRUE );
	/* set main notebook page */
	//gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(s->app,"notebook3")), s->notebook3page );
	/* store text widgets for spell checker */
	notes =  lookup_widget(s->app,"textComments");		
	/* Add options to Options Menu and get toggle item widget */
	/*autosaveitem = additemtooptionmenu(app, _("_Settings/"), _("Auto Save Personal Comments"),
				(GtkMenuCallback)on_auto_save_notes1_activate);
	*/
	/*notepage  = additemtooptionmenu(app, _("_Settings/"), _("Show Interlinear Page"),
				(GtkMenuCallback)on_show_interlinear_page1_activate);*/
	s->versestyle_item = additemtooptionmenu(s->app, _("_Settings/"), _("Verse Style"),
				(GtkMenuCallback)on_verse_style1_activate);
	/* set dictionary key */
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(s->app,"dictionarySearchText")),s->dictkey);
        loadquickmarks_programstart(s->app); /* add quickmarks to menubar */
	
	
	/* set Bible module to open notebook page */
	/* let's don't do this if we don't have at least one text module */	
	if(havebible){ 	
		if(biblepage == 0)
			changecurModSWORD(s->MainWindowModule, TRUE); 
		/* get notebook */
		notebook = lookup_widget(s->app,"nbTextMods");
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
			changcurdictModSWORD(s->DictWindowModule, s->dictkey);
		/* get notebook */
		notebook = lookup_widget(s->app,"notebook4");
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
		gtk_widget_hide(lookup_widget(s->app,"hbox8"));
	 
	/* set com module to open notebook page */	
	if(havecomm){ /* let's don't do this if we don't have at least one commentary */ 
		if(commpage == 0)
			changcurcomModSWORD(s->CommWindowModule, TRUE);  		
		notebook = lookup_widget(s->app,"notebook1");
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook), commpage);		
                if(settings->comm_tabs) {
			gtk_widget_show(notebook);
		}
                else 
			gtk_widget_hide(notebook);
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                                        GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                                        NULL);	
        }
	
	/* set personal commets notebook label and display module */
	if(usepersonalcomments){
		 /* change personal comments module */
		changepercomModSWORD(s->personalcommentsmod);	
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
	
	if(s->showsplash){	
		while (gtk_events_pending ())
				gtk_main_iteration ();
	}
	
	g_print("done\n");
}

/**********************************************************************************************
 * addnotebookpages - add pages to commentary and dictionary notebooks
 * notebook - notebook to add the pages to
 * list - list of modules - add one page per module
 *********************************************************************************************/
gint
addnotebookpages(GtkWidget *notebook, 
			GList *modlist, 
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
	
	tmp = modlist;
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
void UpdateChecks(SETTINGS *s)
{
	/* does user want verses or paragraphs */
	GTK_CHECK_MENU_ITEM (s->versestyle_item)->active = s->versestyle;		
	
	if(s->footnotesint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Footnotes","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Footnotes","Off" );	/* keep footnotes in sync with menu */	
	
	/* set interlinear Strong's Numbers to last setting used */
	if(s->strongsint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Strong's Numbers","On" );	/* keep Strongs in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Strong's Numbers","Off" );	/* keep Strongs in sync with menu */
	
	/* set interlinear morph tags to last setting used */
	if(s->morphsint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Morphological Tags","On" );	/* keep Morph Tags in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Morphological Tags","Off" );	/* keep Morph Tag in sync with menu */	
	
	/* set interlinear Hebrew Vowel Points to last setting used */
	if(s->hebrewpointsint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW, "Hebrew Vowel Points","On" );	/* keep Hebrew Vowel Points in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW,"Hebrew Vowel Points","Off" );	/* keep Hebrew Vowel Points in sync with menu */	
	
	/* set interlinear Hebrew Cantillation to last setting used */
	if(s->cantillationmarksint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW, "Hebrew Cantillation","On" );	/* keep Hebrew Cantillation in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW, "Hebrew Cantillation","Off" );	/* keep Hebrew Cantillation in sync with menu */	
	
	/* set interlinear Greek Accents to last setting used */
	if(s->greekaccentsint)  
		setglobalopsSWORD(INTERLINEAR_WINDOW, "Greek Accents","On" );	/* keep Greek Accents in sync with menu */		
	else
		setglobalopsSWORD(INTERLINEAR_WINDOW, "Greek Accents","Off" );	/* keep Greek Accents in sync with menu */	
		
	/* set auto save personal comments to last setting */
	autoSave = s->autosavepersonalcomments;
	/* set auto save menu check item */	
	//GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments; 	
        /* fill the dict key clist */
	
	/* show hide shortcut bar - set to options setting */
        if(s->showshortcutbar){
                e_paned_set_position (E_PANED(lookup_widget(s->app,"epaned")), s->shortcutbar_width);
        }else{
                e_paned_set_position (E_PANED(lookup_widget(s->app,"epaned")), 1);
        }
	
	/* set hight of bible and commentary pane */
	e_paned_set_position(E_PANED(lookup_widget(s->app,"vpaned1")), s->upperpane_hight);
	/* set width of bible pane */
	e_paned_set_position(E_PANED(lookup_widget(s->app,"hpaned1")), s->biblepane_width);
	
	if(!s->docked){
		s->docked = TRUE;
		on_btnSBDock_clicked(NULL, s);
	}
  	gtk_widget_show(s->app); /** display the whole thing **/
        if(havedict) FillDictKeysSWORD();
        addhistoryitem = FALSE;
        changeVerseSWORD(s->currentverse);
}

/*****************************************************************************
 *setformatoption
 *button
*****************************************************************************/
void setformatoption(GtkWidget *button)
{
   settings->formatpercom = GTK_TOGGLE_BUTTON(GTK_BUTTON(button))->active;
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
        //if(page_num == 4) changeVerseSWORD(current_verse); /* if we changed to page 0, 1 or 2 */
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
        loadquickmarks_afterSeparator(app); 
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

/*** code from anjuta-0.1.9-bc ***/
/*
	toolbar2 =
		create_extended_toolbar (anjuta_gui,
					 &(appl->widgets.toolbar.
					   extended_toolbar));
	gnome_app_add_toolbar (GNOME_APP (anjuta_gui), GTK_TOOLBAR (toolbar2),
			       ANJUTA_EXTENDED_TOOLBAR,
			       GNOME_DOCK_ITEM_BEH_NORMAL,
			       GNOME_DOCK_TOP, 3, 0, 0);
	gtk_toolbar_set_space_size (GTK_TOOLBAR (toolbar2), 5);
	gtk_toolbar_set_space_style (GTK_TOOLBAR (toolbar2),
				     GTK_TOOLBAR_SPACE_LINE);
	gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar2),
				       GTK_RELIEF_NONE);

	toolbar3 =
		create_tags_toolbar (anjuta_gui,
				     &(appl->widgets.toolbar.tags_toolbar));
	gnome_app_add_toolbar (GNOME_APP (anjuta_gui), GTK_TOOLBAR (toolbar3),
			       ANJUTA_TAGS_TOOLBAR,
			       GNOME_DOCK_ITEM_BEH_NORMAL |
			       GNOME_DOCK_ITEM_BEH_NEVER_VERTICAL,
			       GNOME_DOCK_TOP, 2, 0, 0);
	gtk_toolbar_set_space_size (GTK_TOOLBAR (toolbar3), 5);
	gtk_toolbar_set_space_style (GTK_TOOLBAR (toolbar3),
				     GTK_TOOLBAR_SPACE_LINE);
	gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar3),
				       GTK_RELIEF_NONE);
*/
/*
void
anjuta_toolbar_set_view (gchar* toolbar_name, gboolean view, gboolean resize, gboolean set_in_props)
{
	gchar* key;
	GnomeDock* dock;
	GnomeDockItem* item;

	if (set_in_props)
	{
		key = g_strconcat (toolbar_name, ".visible", NULL);
		preferences_set_int (app->preferences, key, view);
		g_free (key);
	}

	item = gnome_app_get_dock_item_by_name (GNOME_APP (app->widgets.window), toolbar_name);

	g_return_if_fail (item != NULL);
	g_return_if_fail (GNOME_IS_DOCK_ITEM (item));

	if (view)
		gtk_widget_show (GTK_WIDGET (item));
	else
		gtk_widget_hide (GTK_WIDGET (item));
	dock = gnome_app_get_dock(GNOME_APP(app->widgets.window));
	g_return_if_fail (dock != NULL);
	if (resize)
		gtk_widget_queue_resize (GTK_WIDGET (dock));
}
*/

