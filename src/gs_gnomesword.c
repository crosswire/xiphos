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
#include "gs_sword.h"
#include "callback.h"
#include "support.h"
#include "interface.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_listeditor.h"
#include "gs_shortcutbar.h"

#if USE_SHORTCUTBAR
#include <gal/e-paned/e-hpaned.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#endif /* USE_SHORTCUTBAR */

/*****************************************************************************
* globals
*****************************************************************************/	
GtkWidget	*versestyle,	/* widget to access toggle menu - for versestyle */
		*footnotes,	/* widget to access toggle menu - for footnotes */
  		*strongsnum,/* widget to access toggle menu - for strongs numbers */
	    	*notepage,	/* widget to access toggle menu - for interlinear notebook page */
	    	*autosaveitem, /* widget to access toggle menu - for personal comments auto save */
		*studypad,  /* studypad text widget */
		*notes,    /* notes text widget */
		*morphs;

GString *gs_clipboard; /* for copying from the html widgets freed in shutdownSWORD in gs_sword.cpp*/

gboolean isstrongs = FALSE; /* main window selection is not storngs number */
gchar *current_filename= NULL;	/* filename for open file in study pad window */
gboolean file_changed = FALSE;	/* set to true if text is study pad has changed - and file is not saved  */
gchar current_verse[80]="Romans 8:28";	/* current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window */
gboolean changemain = TRUE; /* change verse of Bible text window */
gint dictpages,
	 compages,
	 textpages;
SETTINGS *settings;
GS_APP gs;
gchar 	 bmarks[50][80];	/* array to store bookmarks - read in form file when program starts - saved to file on edit */
GS_LAYOUT gslayout,
		   *p_gslayout;
GS_NB_PAGES *nbpages,
			npages;
GS_TABS		tabs,
			*p_tabs;
GS_LEXICON 	*p_gslexicon,
			gslexicon;
/*****************************************************************************
* externs
*****************************************************************************/
extern GList 	*biblemods,
			*commentarymods,
			*dictionarymods,
			*percommods,
			*sbbiblemods,
			*sbdictmods,
			*sbcommods;
extern gchar *mydictmod;						
extern GdkColor myGreen; /* current verse color */
extern gboolean havedict; /* let us know if we have at least one lex-dict module */
extern gboolean havecomm; /* let us know if we have at least one commentary module */
extern gboolean havebible; /* let us know if we have at least one Bible text module */
extern gboolean usepersonalcomments; /* do we setup for personal comments - default is FALSE */
extern gboolean bVerseStyle;
extern gboolean autoSave;
extern gint groupnum4;
extern gint ibookmarks;	/* number of items in bookmark menu  -- declared in filestuff.cpp */
extern gchar remembersubtree[256];  /* used for bookmark menus declared in filestuff.cpp */
extern gchar *shortcut_types[];
extern gboolean addhistoryitem; /* do we need to add item to history */
extern gchar *mycolor;
//gboolean firstbackclick = TRUE;

/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void
initGnomeSword(GtkWidget *app, SETTINGS *settings,
		GList *biblemods, GList *commentarymods ,
		GList *dictionarymods, GList *percommods)
{
	GtkWidget *notebook;
	gint		biblepage,
			commpage,
			dictpage;
	p_gslayout = &gslayout;
	
/* set the main window size */
	gtk_window_set_default_size(GTK_WINDOW(app), p_gslayout->gs_width, p_gslayout->gs_hight);
	//g_warning("width = %d hight = %d",p_gslayout->gs_width, p_gslayout->gs_hight);
/* setup shortcut bar */
	setupSB(sbbiblemods, sbcommods ,sbdictmods);
	mycolor = settings->currentverse_color;
/* set color for current verse for gtktext widgets */
  	//myGreen.red =  settings->currentverse_red;
	//myGreen.green = settings->currentverse_green;
	//myGreen.blue =  settings->currentverse_blue;		
/* add modules to menus -- menu.c */
	addmodstomenus(app, 
				settings, 
				biblemods, /* list of mod names */
				sbbiblemods, /* list of mod descriptions */
				commentarymods,
				sbcommods, 
				dictionarymods,
				sbdictmods,
				percommods);
/* create popup menus -- menu.c */
	createpopupmenus(app, 
				settings, 
				biblemods,
				sbbiblemods,
				commentarymods,
				sbcommods,
				dictionarymods,
				sbdictmods,
				percommods);
/* add pages to commentary and  dictionary notebooks */
	biblepage = addnotebookpages(lookup_widget(app,"nbTextMods"), biblemods, settings->MainWindowModule);
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
			nbpages->notebook3page );
/* store text widgets for spell checker */
	notes =  lookup_widget(app,"textComments");
	studypad = lookup_widget(app,"text3");				
/* Add options to Options Menu and get toggle item widget */
	autosaveitem = additemtooptionmenu(app, "_Settings/", "Auto Save Personal Comments",
				(GtkMenuCallback)on_auto_save_notes1_activate);
	notepage  = additemtooptionmenu(app, "_Settings/", "Show Interlinear Page",
				(GtkMenuCallback)on_show_interlinear_page1_activate);
	versestyle = additemtooptionmenu(app, "_Settings/", "Verse Style",
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
                e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), p_gslayout->shortcutbar_width);
        }else{
                e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), 1);
        }
/* set hight of bible and commentary pane */
	e_paned_set_position(E_PANED(lookup_widget(app,"vpaned1")), p_gslayout->upperpane_hight);
/* set width of bible pane */
	e_paned_set_position(E_PANED(lookup_widget(app,"hpaned1")),p_gslayout->biblepane_width);
/* load last used file into studypad */
        if(settings->studypadfilename != NULL) loadStudyPadFile(settings->studypadfilename); 	
/* create gs_clipboard */
	gs_clipboard = g_string_new("");
	
/* set Bible module to open notebook page */
	/* let's don't do this if we don't have at least one text module */	
	if(havebible){ 			
		gint pagenum;
		
		pagenum = biblepage; 
		/* get notebook */
		notebook = lookup_widget(app,"nbTextMods");
		gtk_signal_connect(GTK_OBJECT(notebook), "switch_page",
			   GTK_SIGNAL_FUNC(on_nbTextMods_switch_page),
			   NULL);			
		/* set notebook page */
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),pagenum ); 
                if(p_tabs->textwindow) 
			gtk_widget_show(notebook);
                else 
			gtk_widget_hide(notebook);
	}
		
/* set dict module to open notebook page */
	/* let's don't do this if we don't have at least one dictionary / lexicon */	
	if(havedict){ 			
		/* set page to 0 (first page in notebook) */
		gint pagenum = 0;
		pagenum = dictpage;
		/* get notebook */
		notebook = lookup_widget(app,"notebook4");
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                      GTK_SIGNAL_FUNC (on_notebook4_switch_page),
                      NULL);		
		/* set notebook page */
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),pagenum ); 	
		if(p_tabs->dictwindow) gtk_widget_show(notebook);
                else gtk_widget_hide(notebook);
         /* hide dictionary section of window if we do not have at least one dict/lex */
	}else 
		gtk_widget_hide(lookup_widget(app,"hbox8"));
	
/* set com module to open notebook page */	
	if(havecomm){ /* let's don't do this if we don't have at least one commentary */  	
		gint pagenum = 0;
		notebook = lookup_widget(app,"notebook1");
		gtk_signal_connect (GTK_OBJECT (notebook), "switch_page",
                                        GTK_SIGNAL_FUNC (on_notebook1_switch_page),
                                        NULL);
		pagenum = commpage;			
		gtk_notebook_set_page(GTK_NOTEBOOK(notebook),pagenum );		
                if(p_tabs->commwindow) gtk_widget_show(notebook);
                else gtk_widget_hide(notebook);
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
}

/**********************************************************************************************
 * addnotebookpages - add pages to commentary and dictionary notebooks
 * notebook - notebook to add the pages to
 * list - list of modules - add one page per module
 *********************************************************************************************/
gint
addnotebookpages(GtkWidget *notebook, GList *list, gchar *modName)
{
	GList *tmp;
	gint pg = 0, retVal;
	GtkWidget
		*empty_notebook_page, /* used to create new pages */
		*label;
	GtkLabel *mylabel;
	retVal = 0;	
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
	
	/* set interlinear page to last setting */
	if(settings->interlinearpage)
		gtk_widget_show(lookup_widget(app,"vbox3"));	
	else
		gtk_widget_hide(lookup_widget(app,"vbox3"));	
	/* set interlinear page menu check item */		
	GTK_CHECK_MENU_ITEM (notepage)->active = settings->interlinearpage; 	
	/* set auto save personal comments to last setting */
	autoSave = settings->autosavepersonalcomments;
	/* set auto save menu check item */	
	GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments; 	
	/* set Strong's numbers to last setting */
	if(settings->strongs)	
		/* keep strongs numbers in sync with menu */
		setglobalopsSWORD("Strong's Numbers","On");  /* " */
	else
		/* keep strongs numbers in sync with menu */
		setglobalopsSWORD("Strong's Numbers","Off"); /* ' */
	/* set strongs toogle button */
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(lookup_widget(app,"btnStrongs")), settings->strongs);
	GTK_CHECK_MENU_ITEM (strongsnum)->active = settings->strongs;	
        /* fill the dict key clist */
        if(havedict) FillDictKeysSWORD();
        /* set Text - apply changes */
        addhistoryitem = FALSE;
        changeVerseSWORD(settings->currentverse);
        //addHistoryItem(app, lookup_widget(app, "shortcut_bar"), settings->currentverse);
}

void
applyoptions(GtkWidget *app,
		gboolean showshortcut, 
		gboolean showtexttabs,
		gboolean showcomtabs,
		gboolean showdicttabs, 
		gboolean showtextgroup,
                gboolean showcomgroup, 
		gboolean showdictgroup,
                gboolean showhistorygroup)
{
         GtkWidget    	*text,
				*dict,
                      		*comm;
	text = lookup_widget(app,"nbTextMods");
        dict = lookup_widget(app,"notebook4");
        comm = lookup_widget(app,"notebook1");
/*  */
        settings->showshortcutbar = showshortcut;
	
	p_tabs->textwindow = showtexttabs;
        p_tabs->commwindow = showcomtabs;
        p_tabs->dictwindow = showdicttabs;

        settings->showtextgroup = showtextgroup;
        settings->showcomgroup =  showcomgroup;
        settings->showdictgroup = showdictgroup;
        settings->showhistorygroup = showhistorygroup;
	
	if(p_tabs->textwindow) {
                gtk_widget_show(text);
        } else {
                gtk_widget_hide(text);
        }
        if(p_tabs->commwindow) {
                gtk_widget_show(comm);
        } else {
                gtk_widget_hide(comm);
        }
        if(p_tabs->dictwindow) {
                gtk_widget_show(dict);
        } else {
                gtk_widget_hide(dict);
        }
        if(settings->showshortcutbar){
               e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), p_gslayout->shortcutbar_width);
        } else {
               e_paned_set_position (E_PANED(lookup_widget(app,"epaned")), 1);
        }
}

/*****************************************************************************		
 * return verse number form verse in main Bible window
 * starting index must be in the verse number
 * else we return 0			
******************************************************************************/
gint getversenumber(GtkWidget *text)
{
	gchar   *buf, /* buffer for storing the verse number */
	        cbuf; /* char for checking for numbers (isdigit) */
	gint	startindex, /* first digit in verse number */
	        endindex,   /* last digit in verse number */
	        index;      /* current position in text widget */
						
	 index = gtk_editable_get_position(GTK_EDITABLE(text));	/* get current position for a starting point */
	 cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), index); /* get char at current position (index) */
	 if(!isdigit(cbuf)) return 0; /* if cbuf is not a number stop - do no more */
	 endindex = index;  /* set endindex to index */
	 while(isdigit(cbuf)){ /* loop until cbuf is not a number */	
	 	cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), endindex); /* get next char */
	 	if(cbuf == ')' || cbuf == '>') isstrongs = TRUE;
	 	++endindex;   /* increment endindex */
	 } 	
	 --endindex; /* our last char was not a number so back up one */
	 startindex = index; /* set startindex to index */
	 cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), startindex); /* get char at index - we know it is a number */
	 while(isdigit(cbuf)){  /* loop backward util cbuf is not a number */	
	        cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), startindex); /* get previous char */
	 	if(cbuf == '(' || cbuf == '<') isstrongs = TRUE;
	 	--startindex; /* decrement startindex  */ 	 		
	 }	
	 ++startindex; /* last char (cbuf) was not a number */
	 ++startindex; /* last char (cbuf) was not a number */
	 buf = gtk_editable_get_chars(GTK_EDITABLE(text), startindex, endindex); //-- get verse number
	 return atoi(buf); /* send it back as an integer */
}

/*****************************************************************************
 *getdictnumber
 *text
 *returns strongs number else 0
******************************************************************************/
gint getdictnumber (GtkWidget *text)
{
        gchar   *buf, /* buffer for storing the verse number */
                cbuf; /* char for checking for numbers (isdigit) */
        gint    startindex, /* first digit in verse number */
                endindex,   /* last digit in verse number */
                index;      /* current position in text widget */

         index = gtk_editable_get_position(GTK_EDITABLE(text)); /* get current position for a starting point */
         cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), index); /* get char at current position (index) */
         if(!isdigit(cbuf)) return 0; /* if cbuf is not a number stop - do no more */
         endindex = index;  /* set endindex to index */
         while(isdigit(cbuf)){ /* loop until cbuf is not a number */
                cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), endindex); /* get next char */
                ++endindex;   /* increment endindex */
         }
         --endindex; /* our last char was not a number so back up one */
         startindex = index; /* set startindex to index */
         cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), startindex); /* get char at index - we know it is a number */
         while(isdigit(cbuf)){  /* loop backward util cbuf is not a number */
                cbuf = GTK_TEXT_INDEX(GTK_TEXT(text), startindex); /* get previous char */
                --startindex; /* decrement startindex */
         }
         ++startindex; /* last char (cbuf) was not a number */
         ++startindex; /* last char (cbuf) was not a number */
         buf = gtk_editable_get_chars(GTK_EDITABLE(text), startindex, endindex); /* get verse number */
	 if(endindex-startindex > 1) isstrongs = TRUE;
         return atoi(buf); /* send it back as an integer */
}

/*****************************************************************************
 * sbchangeModSword
 * group_num
 * item_num
******************************************************************************/
void sbchangeModSword(GtkWidget *app, GtkWidget *shortcut_bar, gint group_num, gint item_num)
{
        GtkWidget       *notebook;
        gchar		*type[1],
				*ref[1];
	
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR(shortcut_bar)->model,
						  group_num,
						  item_num,
						  type,
						  ref);
					  	
	if(!strcmp(type[0],"bible:")) {
		if(havebible){/* let's don't do this if we don't have at least one Bible text */
			//changecurModSWORD(ref[0],TRUE);
			notebook = lookup_widget(app,"nbTextMods"); /* get notebook */
		 	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); /* set notebook page */
		}
	}
	if(!strcmp(type[0],"commentary:")) {
		if(havecomm) { /* let's don't do this if we don't have at least one commentary */	           			            	
			notebook = lookup_widget(app,"notebook1"); /* get notebook */
		 	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); /* set notebook page */
    		}
    	}
	if(!strcmp(type[0],"dict/lex:")) {
		if(havedict) { /* let's don't do this if we don't have at least one dictionary / lexicon */	           			            	
			notebook = lookup_widget(app,"notebook4"); /* get notebook */
		 	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); /* set notebook page */
  		}
	}
	if(!strcmp(type[0],"history:")) {
		changeVerseSWORD(ref[0]);
	}						
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
        nbpages->notebook3page = page_num; /* store the page number so we can open to it the next time we start */
        changemain = FALSE; /* we don't want to cause the Bible text window to scrool */
        if(page_num < 3) changeVerseSWORD(current_verse); /* if we changed to page 0, 1 or 2 */
}

/******************************************************************************
 *openpropertiesbox - someone clicked properties
******************************************************************************/
void openpropertiesbox(void)
{
	GtkWidget   *Propertybox, 	/* pointer to propertybox dialog */
                *cpcurrentverse, 	   /* pointer to current verse color picker */
		*texttabsbutton,	/* show textwindow notebook tabs toggle button */
                *comtabsbutton,  	   /* show commentary notebook tabs toggle button */
                *dicttabsbutton, 	     /* show dict/lex notebook tabs toggle button */
                *shortcutbarbutton, 	   /* show shortcut bar toggle button */
                *textgroupbutton,   	   /* show text group toggle button */
                *comgroupbutton,    	 /* show commentary group toggle button */
                *dictgroupbutton,  	   /* show dict/lex group toggle button */
                *historygroupbutton;

	gushort red,       /* vars for setting color */
		green,
		blue,
		a;
	
	Propertybox = create_dlgSettings(); /* create propertybox dialog */
	shortcutbarbutton = lookup_widget(Propertybox,"cbtnShowSCB");
	texttabsbutton = lookup_widget(Propertybox,"cbtnShowTXtabs");
	comtabsbutton = lookup_widget(Propertybox,"cbtnShowCOMtabs");
	dicttabsbutton  = lookup_widget(Propertybox,"cbtnShowDLtabs");
	cpcurrentverse = lookup_widget(Propertybox,"cpfgCurrentverse"); /* set cpcurrentverse to point to color picker */
        textgroupbutton = lookup_widget(Propertybox,"cbtnShowTextgroup");
	comgroupbutton  = lookup_widget(Propertybox,"cbtnShowComGroup");
	dictgroupbutton = lookup_widget(Propertybox,"cbtnShowDictGroup");
	historygroupbutton = lookup_widget(Propertybox,"cbtnShowHistoryGroup");
        a = 000000;
        /* setup current verse color picker */
	red = settings->currentverse_red;  /* get color from settings structure */
	green = settings->currentverse_green;
	blue =settings->currentverse_blue;
	gnome_color_picker_set_i16 (GNOME_COLOR_PICKER(cpcurrentverse),red ,green , blue, a); /* set color of current verse color picker button */
	/* set toggle buttons to settings structur */
	GTK_TOGGLE_BUTTON(shortcutbarbutton)->active = settings->showshortcutbar;
	GTK_TOGGLE_BUTTON(texttabsbutton)->active = p_tabs->textwindow;
	GTK_TOGGLE_BUTTON(comtabsbutton)->active = p_tabs->commwindow;
	GTK_TOGGLE_BUTTON(dicttabsbutton)->active = p_tabs->dictwindow;
	GTK_TOGGLE_BUTTON(textgroupbutton)->active = settings->showtextgroup;
	GTK_TOGGLE_BUTTON(comgroupbutton)->active = settings->showcomgroup;
	GTK_TOGGLE_BUTTON(dictgroupbutton)->active = settings->showdictgroup;
	GTK_TOGGLE_BUTTON(historygroupbutton)->active = settings->showhistorygroup;	
	GTK_TOGGLE_BUTTON(GTK_BUTTON(lookup_widget(Propertybox,"cbtnPNformat")))->active = settings->formatpercom; /* set Personal note format check button */
	gtk_widget_show(Propertybox); /* show propertybox */
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
 *addBookmark - someone clicked add bookmark to get us here
*****************************************************************************/
void addBookmark(GtkWidget *app)  
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
	removemenuitems(app, "_Bookmarks/<Separator>", ibookmarks); /* remove old bookmarks form menu -- menustuff.cpp */	
        sprintf(buf,"%s%s", "_Bookmarks/",remembersubtree);
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

