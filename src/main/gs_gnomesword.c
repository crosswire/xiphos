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
#include "sword.h"
#include "gs_gui_cb.h"
#include "gs_commentary.h"
#include "gs_gbs.h"
#include "gs_dictlex.h"
#include "gs_mainmenu_cb.h"
#include "support.h"
#include "gs_file.h"
#include "gs_html.h"
#include "gs_menu.h"
#include "gs_shortcutbar.h"
#include "about_modules.h"

#include <gal/e-paned/e-hpaned.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>

/*****************************************************************************
* globals
*****************************************************************************/
GtkWidget *notepage,            /* widget to access toggle menu - for interlinear notebook page */
*autosaveitem,                  /* widget to access toggle menu - for personal comments auto save */
*notes;                         /* notes text widget */

gboolean file_changed = FALSE,  /* set to true if text is study pad has changed - and file is not saved  */
 changemain = TRUE;             /* change verse of Bible text window */

gchar *current_filename = NULL; /* filename for open file in study pad window */

gchar current_verse[80] = N_("Romans 8:28");    /* current verse showing in main window - 1st - 2nd - 3rd interlinear window - commentary window */

gint dictpages, compages, textpages;

SETTINGS *settings;


/*****************************************************************************
* externs
*****************************************************************************/
extern GList
    * biblemods,
    *commentarymods,
    *dictionarymods,
    *bookmods,
    *percommods,
    *sbfavoritesmods,
    *sbbiblemods, *sbdictmods, *sbcommods, *sbbookmods, *options;

extern gchar *mydictmod, *shortcut_types[], rememberlastitem[];

extern gboolean havedict,       /* let us know if we have at least one lex-dict module */
 havecomm,                      /* let us know if we have at least one commentary module */
 havebible,                     /* let us know if we have at least one Bible text module */
 usepersonalcomments,           /* do we setup for personal comments - default is FALSE */
 autoSave, addhistoryitem;      /* do we need to add item to history */

extern gint groupnum4, iquickmarks;     /* number of items in bookmark menu  -- declared in gs_file.c */

/******************************************************************************
 * initGnomeSword - sets up the interface
 *****************************************************************************/
void
initGnomeSword(SETTINGS * s,
               GList * biblemods,
               GList * commentarymods,
               GList * dictionarymods,
               GList * percommods)
{
        GtkWidget *notebook;

        gint biblepage, commpage;


        g_print("%s\n", "Initiating GnomeSword\n");

        /*
           setup shortcut bar 
         */
        setupSB(s);
        /*
           setup commentary gui support 
         */
        gui_setupCOMM(s);
        /*
           setup general book gui support 
         */
        gui_setupGBS(s);
        /*
           setup Dict/Lex gui support 
         */
        gui_setupDL(s);
        s->settingslist = NULL;
        s->displaySearchResults = FALSE;
        /*
           add modules to menus -- gs_menu.c 
         */
        addmodstomenus(s,
                       biblemods,
                       commentarymods, dictionarymods, bookmods,
                       percommods);
        /*
           create popup menus -- gs_menu.c 
         */
        createpopupmenus(s, sbbiblemods, sbcommods, sbdictmods,
                         options);
        additemstooptionsmenu(options, s);
        /*
           add pages to module notebooks 
         */
        biblepage =
            addnotebookpages(lookup_widget(s->app, "nbTextMods"),
                             biblemods, s->MainWindowModule);
        /*
           commpage =
           addnotebookpages(lookup_widget(s->app, "notebook1"),
           commentarymods, s->CommWindowModule);
         */

        gtk_notebook_set_page(GTK_NOTEBOOK
                              (lookup_widget(s->app, "nbPerCom")), 0);
        /*
           set text windows to word warp 
         */
        gtk_text_set_word_wrap(GTK_TEXT
                               (lookup_widget(s->app, "textComments")),
                               TRUE);
        /*
           store text widgets for spell checker 
         */
        notes = lookup_widget(s->app, "textComments");

        s->versestyle_item =
            additemtooptionmenu(s->app, _("_Settings/"),
                                _("Verse Style"),
                                (GtkMenuCallback)
                                on_verse_style1_activate);

        //loadquickmarks_programstart(s->app);      /* add quickmarks to menubar */

        /*
           set Bible module to open notebook page 
         */
        /*
           let's don't do this if we don't have at least one text module 
         */
        if (havebible) {
                if (biblepage == 0)
                        changecurModSWORD(s->MainWindowModule, TRUE);
                /*
                   get notebook 
                 */
                notebook = lookup_widget(s->app, "nbTextMods");
                gtk_signal_connect(GTK_OBJECT(notebook), "switch_page",
                                   GTK_SIGNAL_FUNC
                                   (on_nbTextMods_switch_page), NULL);
                /*
                   set notebook page 
                 */
                gtk_notebook_set_page(GTK_NOTEBOOK(notebook),
                                      biblepage);
                if (settings->text_tabs)
                        gtk_widget_show(notebook);
                else
                        gtk_widget_hide(notebook);
        }

        /*
           set com module to open notebook page 
         */
        /*
           if (havecomm) {          
           if (commpage == 0)
           changcurcomModSWORD(s->CommWindowModule, TRUE);
           notebook = lookup_widget(s->app, "notebook1");
           gtk_notebook_set_page(GTK_NOTEBOOK(notebook), commpage);
           if (settings->comm_tabs) {
           gtk_widget_show(notebook);
           } else
           gtk_widget_hide(notebook);
           gtk_signal_connect(GTK_OBJECT(notebook), "switch_page",
           GTK_SIGNAL_FUNC(on_notebook1_switch_page),
           NULL);
           }
         */
        /*
           set personal commets notebook label and display module 
         */
        if (usepersonalcomments) {
                /*
                   change personal comments module 
                 */
                changepercomModSWORD(s->personalcommentsmod);
        }

        /*
           free module lists 
         */
        g_list_free(biblemods);
        g_list_free(commentarymods);
        g_list_free(dictionarymods);
        g_list_free(percommods);
        g_list_free(sbbiblemods);
        g_list_free(sbcommods);
        g_list_free(sbdictmods);
        g_list_free(bookmods);
        g_list_free(sbbookmods);
        // options list freed on exit

        g_print("done\n");
}

/**********************************************************************************************
 * addnotebookpages - add pages to commentary and dictionary notebooks
 * notebook - notebook to add the pages to
 * list - list of modules - add one page per module
 *********************************************************************************************/
gint
addnotebookpages(GtkWidget * notebook, GList * modlist, gchar * modName)
{
        GList *tmp;
        gint pg = 0, retVal = 0;
        GtkWidget *empty_notebook_page, /* used to create new pages */
        *label;
        GtkLabel *mylabel;

        tmp = modlist;
        while (tmp != NULL) {
                empty_notebook_page = gtk_vbox_new(FALSE, 0);
                gtk_widget_show(empty_notebook_page);
                gtk_container_add(GTK_CONTAINER(notebook),
                                  empty_notebook_page);
                label = gtk_label_new((gchar *) tmp->data);
                mylabel = GTK_LABEL(label);
                if (!strcmp((gchar *) mylabel->label, modName)) /* set retVal to saved mod's page number */
                        retVal = pg;
                gtk_widget_show(label);
                gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook),
                                           gtk_notebook_get_nth_page
                                           (GTK_NOTEBOOK(notebook), pg),
                                           label);
                gtk_notebook_set_menu_label_text(GTK_NOTEBOOK(notebook),
                                                 gtk_notebook_get_nth_page
                                                 (GTK_NOTEBOOK
                                                  (notebook), pg),
                                                 (gchar *) tmp->data);
                ++pg;
                tmp = g_list_next(tmp);
        }
        g_list_free(tmp);
        return retVal;
}

/*****************************************************************************
 * UpdateChecks(GtkWidget *app) update chech menu items
 * and toggle buttons - called on start up
 *****************************************************************************/
void UpdateChecks(SETTINGS * s)
{
        /*
           does user want verses or paragraphs 
         */
        GTK_CHECK_MENU_ITEM(s->versestyle_item)->active = s->versestyle;

        if (s->footnotesint)
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Footnotes", "On");       /* keep footnotes in sync with menu */
        else
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Footnotes", "Off");      /* keep footnotes in sync with menu */

        /*
           set interlinear Strong's Numbers to last setting used 
         */
        if (s->strongsint)
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Strong's Numbers", "On");        /* keep Strongs in sync with menu */
        else
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Strong's Numbers", "Off");       /* keep Strongs in sync with menu */

        /*
           set interlinear morph tags to last setting used 
         */
        if (s->morphsint)
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Morphological Tags", "On");      /* keep Morph Tags in sync with menu */
        else
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Morphological Tags", "Off");     /* keep Morph Tag in sync with menu */

        /*
           set interlinear Hebrew Vowel Points to last setting used 
         */
        if (s->hebrewpointsint)
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Hebrew Vowel Points", "On");     /* keep Hebrew Vowel Points in sync with menu */
        else
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Hebrew Vowel Points", "Off");    /* keep Hebrew Vowel Points in sync with menu */

        /*
           set interlinear Hebrew Cantillation to last setting used 
         */
        if (s->cantillationmarksint)
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Hebrew Cantillation", "On");     /* keep Hebrew Cantillation in sync with menu */
        else
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Hebrew Cantillation", "Off");    /* keep Hebrew Cantillation in sync with menu */

        /*
           set interlinear Greek Accents to last setting used 
         */
        if (s->greekaccentsint)
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Greek Accents", "On");   /* keep Greek Accents in sync with menu */
        else
                setglobalopsSWORD(INTERLINEAR_WINDOW, "Greek Accents", "Off");  /* keep Greek Accents in sync with menu */

        /*
           set auto save personal comments to last setting 
         */
        autoSave = s->autosavepersonalcomments;
        /*
           set auto save menu check item 
         */
        //GTK_CHECK_MENU_ITEM (autosaveitem)->active = settings->autosavepersonalcomments;  

        /*
           show hide shortcut bar - set to options setting 
         */
        if (s->showshortcutbar) {
                gtk_widget_show(s->shortcut_bar);
                e_paned_set_position(E_PANED
                                     (lookup_widget(s->app, "epaned")),
                                     s->shortcutbar_width);
        }

        else if (!s->showshortcutbar && s->showdevotional) {
                gtk_widget_show(s->shortcut_bar);
                on_btnSB_clicked(NULL, settings);
        }

        else {
                gtk_widget_hide(s->shortcut_bar);
                e_paned_set_position(E_PANED
                                     (lookup_widget(s->app, "epaned")),
                                     1);
        }

        /*
           set hight of bible and commentary pane 
         */
        e_paned_set_position(E_PANED(lookup_widget(s->app, "vpaned1")),
                             s->upperpane_hight);
        /*
           set width of bible pane 
         */
        e_paned_set_position(E_PANED(lookup_widget(s->app, "hpaned1")),
                             s->biblepane_width);

        if (!s->docked) {
                s->docked = TRUE;
                on_btnSBDock_clicked(NULL, s);
        }
        gtk_widget_show(s->app); /** display the whole thing **/

        /*
           fill the dict key clist 
         */
        /*
           if (havedict)
           FillDictKeysSWORD();
         */

        addhistoryitem = FALSE;
        changeVerseSWORD(s->currentverse);
}

/*****************************************************************************
 *setformatoption
 *button
*****************************************************************************/
void setformatoption(GtkWidget * button)
{
        settings->formatpercom =
            GTK_TOGGLE_BUTTON(GTK_BUTTON(button))->active;
}

/*****************************************************************************
 *changepagenotebook - someone changed the page in the main notebook
 *notebook - notebook widget - main notebook
 *page_num - notebook page number
*****************************************************************************/
void changepagenotebook(GtkNotebook * notebook, gint page_num)
{
        settings->notebook3page = page_num;     /* store the page number so we can open to it the next time we start */
        changemain = FALSE;     /* we don't want to cause the Bible text window to scrool */
        //if(page_num == 4) changeVerseSWORD(current_verse); /* if we changed to page 0, 1 or 2 */
}


/*****************************************************************************
 * showIntPage - do we want to see interlinear page?
 * choice
*****************************************************************************/
void showIntPage(GtkWidget * app, gboolean choice)
{
        GtkWidget *intpage, *frame;     /* pointer to interlinear notebook page */

        intpage = lookup_widget(app, "vboxInt");        /* set pointer to page */
        frame = lookup_widget(app, "frame2");   /* set pointer to page */
        if (choice) {
                gtk_widget_show(intpage);       /* show page */
                gtk_widget_show(frame); /* show page */
        } else {
                gtk_widget_hide(intpage);       /* hide page */
                gtk_widget_hide(frame);
        }
        settings->interlinearpage = choice;     /* remember choice for next program startup */
}


/*****************************************************************************
 * setautosave - someone clicked auto save personal  comments
 * choice
*****************************************************************************/
void setautosave(gboolean choice)
{
        if (choice) {           /* if choice was to autosave */
                autoSave = TRUE;
        } else {                /* if choice was not to autosave    */
                autoSave = FALSE;
        }
        settings->autosavepersonalcomments = choice;    /* remember our choice for next startup */
}


/*****************************************************************************
 *   -string_is_color- this code is from bluefish-0.6
 *
*****************************************************************************/
gint string_is_color(gchar * color)
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
        for (i = 1; i < 7; i++) {
                if ((color[i] > 102)
                    || (color[i] < 48)
                    || ((color[i] > 57) && (color[i] < 65))
                    || ((color[i] > 70) && (color[i] < 97))) {
                        g_warning
                            ("string_is_color, %d in %s is not from a color, it is %d\n",
                             i, color, color[i]);
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
gchar *gdouble_arr_to_hex(gdouble * color, gint websafe)
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
                red_int = 0x33 * ((unsigned int) (red * 255 / 0x33));
                green_int =
                    0x33 * ((unsigned int) (green * 255 / 0x33));
                blue_int = 0x33 * ((unsigned int) (blue * 255 / 0x33));
        } else {
                red_int = (unsigned int) (red * 255);
                green_int = (unsigned int) (green * 255);
                blue_int = (unsigned int) (blue * 255);
        }
        tmpstr = g_malloc(8 * sizeof(char));
        g_snprintf(tmpstr, 8, "#%.2X%.2X%.2X", red_int, green_int,
                   blue_int);
        return tmpstr;
}

/*****************************************************************************
 *  hex_to_gdouble_arr -- this code is from bluefish-0.6
 *
*****************************************************************************/
gdouble *hex_to_gdouble_arr(gchar * color)
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


/*
 * to display Sword module about information
 */
static void about_module_display(gchar *to, gchar *text)
{
        gint len, maxlen, i;
        gboolean center = FALSE;

        len = strlen(text);

        maxlen = len * 80;

       
        // -------------------------------
        for (i = 0; i < strlen(text) - 1; i++) {
                if (text[i] == '\\')    // a RTF command
                {
                        if ((text[i + 1] == 'p') && 
                                (text[i + 2] == 'a') && 
                                (text[i + 3] == 'r') && 
                                (text[i + 4] == 'd')) {     

                                if (center) {
                                        *to++ = '<';
                                        *to++ = '/';
                                        *to++ = 'C';
                                        *to++ = 'E';
                                        *to++ = 'N';
                                        *to++ = 'T';
                                        *to++ = 'E';
                                        *to++ = 'R';
                                        *to++ = '>';
                                        center = FALSE;
                                }
                                i += 4;
                                continue;
                        }
                        if ((text[i + 1] == 'p') && (text[i + 2] == 'a')
                            && (text[i + 3] == 'r')) {
                                *to++ = '<';
                                *to++ = 'b';
                                *to++ = 'r';
                                *to++ = '>';
                                *to++ = '\n';
                                i += 3;
                                continue;
                        }
                        if (text[i + 1] == ' ') {
                                i += 1;
                                continue;
                        }
                        if (text[i + 1] == '\n') {
                                i += 1;
                                continue;
                        }
                        if ((text[i + 1] == 'q') && (text[i + 2] == 'c'))  
                        {
                                if (!center) {
                                        *to++ = '<';
                                        *to++ = 'C';
                                        *to++ = 'E';
                                        *to++ = 'N';
                                        *to++ = 'T';
                                        *to++ = 'E';
                                        *to++ = 'R';
                                        *to++ = '>';
                                        center = TRUE;
                                }
                                i += 2;
                                continue;
                        }
                }
                *to++ = text[i];
        }
        *to++ = 0;
}

void display_about_module_dialog(gchar * modname, gboolean isGBS)
{
        GtkWidget *aboutbox = NULL;     //-- pointer to about dialog        
        GtkWidget *text;        //-- pointer to text widget of dialog
        gchar *buf, *to = NULL,             //-- pointer to text buffer for label (mod name)
        *bufabout,              //-- pointer to text buffer for text widget (mod about)
        discription[500];
        gint len, maxlen;

        bufabout = NULL;

        buf = backend_get_module_description(modname);
        bufabout = backend_get_mod_aboutSWORD(modname);

        sprintf(discription,
                "<FONT COLOR=\"#000FCF\"><center><b>%s</b></center></font><HR>",
                buf);
        if (!isGBS) {
                aboutbox = gui_create_about_modules();
                gtk_widget_show(aboutbox);
        }

        if (bufabout) { 
                                
                len = strlen(bufabout);
                maxlen = len * 8;
                
                if ((to = (gchar *) malloc(maxlen)) == NULL) {
                        return ;
                }
                
                if (!isGBS) {
                        text = lookup_widget(aboutbox, "text"); /* get text widget */
                } else {
                        text = settings->htmlBook;
                }

                about_module_display(to, bufabout); /* send about info to display function */
                beginHTML(text, FALSE);
                displayHTML(text, "<html><body>",
                            strlen("<html><body>"));
                displayHTML(text, discription, strlen(discription));
                if (to)
                        displayHTML(text, to, strlen(to));
                displayHTML(text, "</body></html>",
                            strlen("</body></html>"));
                endHTML(text);
        }

        else
                g_warning("oops");

        if (bufabout)
                g_free(bufabout);
        if (to)
                free(to);
}

/*****   end of file   ******/
