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
#include <ctype.h>
//#include <gtkhtml/gtkhtml.h>


#include "gs_gnomesword.h"
#include "sw_gnomesword.h"
#include "callback.h"
#include "support.h"
//#include "interface.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_listeditor.h"
#include "gs_shortcutbar.h"
#include "e-splash.h"

#include <gal/e-paned/e-hpaned.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>

/*****************************************************************************
* globals
*****************************************************************************/	
GtkWidget	
	*footnotes,	/* widget to access toggle menu - for footnotes */
  	*strongsnum,/* widget to access toggle menu - for strongs numbers */
	*notepage,	/* widget to access toggle menu - for interlinear notebook page */
	*autosaveitem, /* widget to access toggle menu - for personal comments auto save */
	*studypad,  /* studypad text widget */
	*notes,    /* notes text widget */
	*morphs;
GString 
	*gs_clipboard; /* for copying from the html widgets freed in shutdownSWORD in gs_sword.cpp*/
gboolean 
	isstrongs = FALSE,/* main window selection is not storngs number */
	file_changed = FALSE, /* set to true if text is study pad has changed - and file is not saved  */
	changemain = TRUE; /* change verse of Bible text window */
gchar 
	*current_filename= NULL;	/* filename for open file in study pad window */
gchar 
	current_verse[80]="Romans 8:28",	/* current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window */
	bmarks[50][80];	/* array to store bookmarks - read in form file when program starts - saved to file on edit */
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
	*sbcommods;
extern gchar 
	*mydictmod,
	*shortcut_types[],
	remembersubtree[256],  /* used for bookmark menus declared in filestuff.cpp */
	*mycolor;
extern GdkColor 
	myGreen; /* current verse color */
extern gboolean 
	havedict, /* let us know if we have at least one lex-dict module */
	havecomm, /* let us know if we have at least one commentary module */
	havebible, /* let us know if we have at least one Bible text module */
	usepersonalcomments, /* do we setup for personal comments - default is FALSE */
	autoSave,
	addhistoryitem; /* do we need to add item to history */
extern gint 
	groupnum4,
	ibookmarks;	/* number of items in bookmark menu  -- declared in filestuff.cpp */

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
	
	g_print("Initiating GnomeSword\n");
	/* set the main window size */
	gtk_window_set_default_size(GTK_WINDOW(app), settings->gs_width, settings->gs_hight);
	/* setup shortcut bar */
	setupSB(settings);
	settings->settingslist = NULL;
	/* set current verse color html */
	mycolor = settings->currentverse_color;	
	/* add modules to menus -- menu.c */
	addmodstomenus(app, 
				settings, 
				biblemods, 
				sbbiblemods, 
				commentarymods,
				sbcommods, 
				dictionarymods,
				sbdictmods,
				percommods);
	/* create popup menus -- menu.c */
	createpopupmenus(app, 
				settings, 
				sbbiblemods,
				sbcommods,
				sbdictmods,
				percommods);
	/* add pages to commentary and  dictionary notebooks */
	biblepage = addnotebookpages(lookup_widget(app,"nbTextMods"), biblemods, settings->MainWindowModule);
	//g_warning("%d",biblepage);
	commpage = addnotebookpages(lookup_widget(app,"notebook1"), commentarymods, settings->CommWindowModule);
	dictpage = addnotebookpages(lookup_widget(app,"notebook4"), dictionarymods, settings->DictWindowModule);	
	/*  set text windows to word warp */
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"moduleText")) , TRUE );
	gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(app,"nbPerCom")),0);
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textCommentaries")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"textComments")) , TRUE );
	gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(app,"text3")) , TRUE );
	/* set main notebook page */
	gtk_notebook_set_page(GTK_NOTEBOOK(lookup_widget(app,"notebook3")),
			settings->notebook3page );
	/* store text widgets for spell checker */
	notes =  lookup_widget(app,"textComments");
	studypad = lookup_widget(app,"text3");				
	/* Add options to Options Menu and get toggle item widget */
	autosaveitem = additemtooptionmenu(app, "_Settings/", "Auto Save Personal Comments",
				(GtkMenuCallback)on_auto_save_notes1_activate);
	notepage  = additemtooptionmenu(app, "_Settings/", "Show Interlinear Page",
				(GtkMenuCallback)on_show_interlinear_page1_activate);
	settings->versestyle_item = additemtooptionmenu(app, "_Settings/", "Verse Style",
				(GtkMenuCallback)on_verse_style1_activate);
	footnotes   = additemtooptionmenu(app, "_Settings/", "Show Footnotes",
				(GtkMenuCallback)on_footnotes1_activate);
	morphs   = additemtooptionmenu(app, "_Settings/", "Show Morphological Tags",
				(GtkMenuCallback)on_morphs_activate);
 	strongsnum   = additemtooptionmenu(app, "_Settings/", "Show Strongs Numbers",
 				(GtkMenuCallback)on_strongs_numbers1_activate); 		
	/* set dictionary key */
        gtk_entry_set_text(GTK_ENTRY(lookup_widget(app,"dictionarySearchText")),settings->dictkey);
        loadbookmarks_programstart(); /* add bookmarks to menubar */
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
	/* load last used file into studypad */
        if(settings->studypadfilename != NULL) loadStudyPadFile(settings->studypadfilename); 	
	/* create gs_clipboard */
	gs_clipboard = g_string_new("");
	
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
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                                        GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                                        NULL);		
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook), commpage);		
                if(settings->comm_tabs) 
			gtk_widget_show(notebook);
                else 
			gtk_widget_hide(notebook);
        }else 
		gtk_notebook_remove_page( GTK_NOTEBOOK(lookup_widget(app,"notebook3")) , 0);	
		
	/* set personal commets notebook label and display module */
	if(usepersonalcomments){
		 /* change personal comments module */
		changepercomModSWORD(settings->personalcommentsmod);	
	}
	
	/* hide buttons - only show them if their options are enabled */
	gtk_widget_hide(lookup_widget(app,"btnPrint"));
	gtk_widget_hide(lookup_widget(app,"btnSpell"));
	gtk_widget_hide(lookup_widget(app,"btnSpellNotes"));
	
	/* do not show print button if printing not enabled */
#ifdef USE_GNOMEPRINT
        gtk_widget_show(lookup_widget(app,"btnPrint"));
#endif /* USE_GNOMEPRINT */
	
	/* do not show spell buttons if spellcheck not enabled */
#ifdef USE_SPELL
        gtk_widget_show (lookup_widget(app,"btnSpell"));
        gtk_widget_show (lookup_widget(app,"btnSpellNotes"));
#endif /* USE_SPELL */	

	/* free module lists */
        g_list_free(biblemods);
        g_list_free(commentarymods);
        g_list_free(dictionarymods);
	g_list_free(percommods);	
        g_list_free(sbbiblemods);
        g_list_free(sbcommods);
        g_list_free(sbdictmods);
	
	if(settings->showsplash){
		e_splash_set_icon_highlight (E_SPLASH(splash),3, TRUE);	
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
 * UpdateChecks(GtkWidget *mainform) update chech menu items
 * and toggle buttons - called on start up
 *****************************************************************************/
void UpdateChecks(GtkWidget *app)
{
	GTK_CHECK_MENU_ITEM (settings->versestyle_item)->active = settings->versestyle;	
	/* set footnotes to last setting used */
	if(settings->footnotes)  
		setglobalopsSWORD(0,"Footnotes","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD(0,"Footnotes","Off" );	/* keep footnotes in sync with menu */			
	GTK_CHECK_MENU_ITEM (footnotes)->active = settings->footnotes;	
	
	/* set interlinear page to last setting */
	if(settings->interlinearpage)
		gtk_widget_show(lookup_widget(app,"vbox3"));	
	else
		gtk_widget_hide(lookup_widget(app,"vbox3"));	
	/* set interlinear page menu check item */		
	GTK_CHECK_MENU_ITEM (notepage)->active = settings->interlinearpage; 
	/* set interlinear footnotes to last setting used */
	if(settings->footnotesint)  
		setglobalopsSWORD(1,"Footnotes","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD(1,"Footnotes","Off" );	/* keep footnotes in sync with menu */	
	/* set interlinear Strong's Numbers to last setting used */
	if(settings->strongsint)  
		setglobalopsSWORD(1,"Strong's Numbers","On" );	/* keep footnotes in sync with menu */		
	else
		setglobalopsSWORD(1,"Strong's Numbers","Off" );	/* keep footnotes in sync with menu */	
	/* set auto save personal comments to last setting */
	autoSave = settings->autosavepersonalcomments;
	/* set auto save menu check item */	
	GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments; 	
	/* set Strong's numbers to last setting */
	if(settings->strongs)	
		/* keep strongs numbers in sync with menu */
		setglobalopsSWORD(0,"Strong's Numbers","On");  
	else
		/* keep strongs numbers in sync with menu */
		setglobalopsSWORD(0,"Strong's Numbers","Off");
	/* set strongs toogle button */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(app,"btnStrongs")), settings->strongs);
	GTK_CHECK_MENU_ITEM (strongsnum)->active = settings->strongs;
	
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
 *editbookmarksLoad - load bookmarks into an editor dialog
 *editdlg
*****************************************************************************/
void editbookmarksLoad(GtkWidget *editdlg) 
{
	GtkWidget *text;  /* pointer to text widget for editing */
	gchar buf[255];   /* temp storage of each bookmark */
	gint i=0;         /* counter */

	text = lookup_widget(editdlg,"text4");  /* set text widger pointer */
 	gtk_text_freeze (GTK_TEXT (text));    /* freeze text until all bookmarks are loaded */
  	gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);  /* clear text widget */
	while(i < ibookmarks){  /* loop through bookmarks - ibookmarks the number of bookmarks we have */	 	
		sprintf(buf,"%s\n",bmarks[i]); /* copy bookmark string from array to buf and add newline */
		gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL,buf , -1); /* put buf into text wigdet */
		++i;	/* increment our counter   */   	
	}
	sprintf(buf,"%s\n","-end-");  /* last item to add to text widget '-end-' is used to signal last item */
	gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL,buf , -1); /* add to text widget */
	gtk_text_thaw (GTK_TEXT (text)); /* thaw text widget so we can work */
}

/*****************************************************************************
 *addQuickmark - someone clicked add bookmark to get us here
*****************************************************************************/
void addQuickmark(GtkWidget *app)  
{
	gchar    *bookname;      /* pointer to the Bible book we want to mark */
	gint     iVerse,         /* verse we want to mark */
		 iChap;          /* chapter we want to mark */
	gchar	 buf[255];

	bookname = gtk_entry_get_text(GTK_ENTRY(lookup_widget(app,"cbeBook"))); /* get book name */
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(app,"spbVerse"))); /* get verse number */
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(lookup_widget(app,"spbChapter"))); /* get chapter */
	sprintf(buf,"%s %d:%d%c",bookname, iChap,iVerse, '\0' ); /* put book chapter and verse into bookmarks array */
	 ++ibookmarks;  /* increment number of bookmark item + 1 */	
	savebookmark(buf);  /* save to file so we don't forget -- function in filestuff.cpp */
	removemenuitems(app, "_Quickmarks/<Separator>", ibookmarks); /* remove old bookmarks form menu -- menustuff.cpp */	
        sprintf(buf,"%s%s", "_Quickmarks/",remembersubtree);
        addseparator(app, buf);
        /* loadbookmarkarray(); */ /* load edited bookmarks  -- filestuff.cpp */
        loadbookmarks_afterSeparator(); /* let's show what we did -- GnomeSword.cpp */
}

/*****************************************************************************
 * showIntPage - do we want to see interlinear page?
 * choice
*****************************************************************************/
void showIntPage(GtkWidget *app, gboolean choice)  
{
	GtkWidget *intpage; /* pointer to interlinear notebook page */

	intpage= lookup_widget(app,"vbox3"); /* set pointer to page */
	if(choice){
		gtk_widget_show(intpage);		/* show page */
	} else {
		gtk_widget_hide(intpage);		/* hide page */
	}
	settings->interlinearpage = choice;  /* remember choice for next program startup */
}

/*****************************************************************************
 * setcurrentversecolor - someone change the color setting
 *			  for the current verse
 * arg1 - red
 * arg2 - green
 * arg3 - blue
*****************************************************************************/
void setcurrentversecolor(gint arg1, gint arg2, gint arg3)
{	
        /* set color for current session */
	myGreen.red = arg1;  /* new red setting */
	settings->currentverse_red = arg1; /* remember setting */
	myGreen.green = arg2;//-- new green setting
	settings->currentverse_green= arg2; /* remember setting */
	myGreen.blue = arg3; //-- new blue setting
	settings->currentverse_blue = arg3; /* remember setting  */	
	redisplayTextSWORD(); /* display change */
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
