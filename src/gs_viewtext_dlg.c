/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                            gs_viewtext_dlg.c
                             -------------------
    begin                : Sat Mar 24 2001
    copyright            : (C) 2001 by Terry Biggs
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
#include <gtkhtml/gtkhtml.h>

#include "sw_viewtext_dlg.h"
#include "gs_viewtext_dlg.h"
#include "sw_gnomesword.h"
#include "gs_gnomesword.h"
#include "gs_html.h"
#include "gs_information_dlg.h"
#include "support.h"

/****************************************************************************************
 *globals
 ****************************************************************************************/
gboolean isrunningVT = FALSE;	/* is the view text dialog runing */
GtkWidget *text;
GList *textList;
GtkWidget *dlgViewText;
gchar vt_current_verse[80];
GtkWidget *cbeBook;
GtkWidget *spbVTChapter;
GtkWidget *spbVTVerse;


/****************************************************************************************
 * externs
 ****************************************************************************************/
extern gchar current_verse[];
//extern GList *cbBook_items;
extern SETTINGS *settings;
extern gboolean gsI_isrunning;

/****************************************************************************************
 * update the book, chapter and verse contorls
 ****************************************************************************************/
static void updatecontrols(void)
{
	gchar *buf;
	
	buf =  VTgetbookSWORD();
	gtk_entry_set_text(GTK_ENTRY(cbeBook), buf);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spbVTChapter),
				  VTgetchapterSWORD());
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spbVTVerse),
				  VTgetverseSWORD());
	/*** buf was allocated with g_strdup() in gs_viewtext_sw.cpp
	       so we free it here ***/
	g_free(buf);
}


/******************************************************************************
 * viewtext callbacks
 ******************************************************************************/
 
 
/*****************************************************************************
 *link in text window clicked
 *****************************************************************************/
static
void on_linkVT_clicked(GtkHTML * html, const gchar * url, gpointer data)
{
	gchar *buf,*modName;
	static GtkWidget *dlg;
	if (*url == '#') {
		
		if(!gsI_isrunning){
		 	dlg = create_dlgInformation();
		 }
		++url;		/* remove # */
		if(*url == 'T') ++url;
		if(*url == 'G' ) {
			++url;
			modName = g_strdup(settings->lex_greek);
			buf = g_strdup(url);
			loadmodandkey(modName, buf);
			g_warning(modName);
			g_free(buf);
			g_free(modName);
		}   		
		if(*url == 'H') {
			++url;
			modName =  g_strdup(settings->lex_hebrew);
			buf = g_strdup(url);
			loadmodandkey(modName, buf);
			g_warning(modName);
			g_free(buf);
			g_free(modName);
		} 
		 gtk_widget_show(dlg);
	} else if (*url == 'M') {
		if(!gsI_isrunning){
		 	dlg = create_dlgInformation();
		 }
		++url;		/* remove M */
		buf = g_strdup(url); 
		loadmodandkey("Packard", buf);
		g_free(buf);
		 gtk_widget_show(dlg); 
	}else  if(*url == '*'){
		++url;
		while(*url != ']') {
			++url;
		} 
		++url;
		buf = g_strdup(url);
		VTgotoverseSWORD(buf);
		updatecontrols();
		g_free(buf);
	}  else {		
		buf = g_strdup(url);
		VTgotoverseSWORD(buf);
		updatecontrols();
		g_free(buf);
	}
}

/*****************************************************************************
 * toggle  Strong's Numbers
 *****************************************************************************/
static void
on_tbtVTStrongs_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	if(togglebutton->active){ /* we want strongs numbers  */
		/* turn strongs on  */
		VTsetGlobalOptionsSWORD("Strong's Numbers","On");
	}else{   /* we don't want strongs numbers */
	              /* turn strongs off   */
		VTsetGlobalOptionsSWORD("Strong's Numbers","Off");	
	}
}

/*****************************************************************************
 *  toggle Footnotes
 *****************************************************************************/
static void
on_tbtnVTFootnotes_toggled(GtkToggleButton * togglebutton,
			   gpointer user_data)
{
	if(togglebutton->active){  /* we want footnotes */
		/* turn footnotes on  */
		VTsetGlobalOptionsSWORD("Footnotes","On");
	}else{  /* we don't want footnotes  */
	              /* turn footnotes off */
		VTsetGlobalOptionsSWORD("Footnotes","Off");	
	}
}

/*****************************************************************************
 *  toggle Morphological Tags
 *****************************************************************************/
static void
on_tbtnVTMorf_toggled                  (GtkToggleButton *togglebutton,
                                        gpointer         user_data)
{
	if(togglebutton->active){ //-- if choice is TRUE - we want morphs
		//-- turn morphs on
		VTsetGlobalOptionsSWORD("Morphological Tags","On");
	}else{   //-- we don't want morphs	
	              //-- turn morphs off
		VTsetGlobalOptionsSWORD("Morphological Tags","Off");
	}
}

static void on_cbeBook_changed(GtkEditable * editable, gpointer user_data)
{

}


static void on_btnGotoVerse_clicked(GtkButton * button, gpointer user_data)
{
	gchar *bookname, buf[120];
	gint iChap, iVerse;
	
	bookname = gtk_entry_get_text(GTK_ENTRY(cbeBook));
	iChap = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spbVTChapter)); 
	iVerse = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(spbVTVerse)); 
	sprintf(buf,"%s %d:%d", bookname, iChap, iVerse);
	//g_warning(buf);
	VTgotoverseSWORD(buf);
	updatecontrols();
}



static void
on_cbeModule_changed(GtkEditable * editable, gpointer user_data)
{
	gchar *buf, title[256];
	static gboolean firsttime = TRUE;
	buf = gtk_entry_get_text(GTK_ENTRY(editable));
	VTloadmodSWORD(buf);
	VTgotoverseSWORD(current_verse);
	sprintf(title, "GnomeSword - %s", VTgetmodDescriptionSWORD());
	gtk_window_set_title(GTK_WINDOW(dlgViewText), title);
	firsttime = FALSE;
}


static void on_btnSync_clicked(GtkButton * button, gpointer user_data)
{
	VTgotoverseSWORD(current_verse);
	updatecontrols();
}

/*
 * shut down the View Text Dialog
 */
static void on_dlgViewText_destroy(GtkObject * object, gpointer user_data)
{
	isrunningVT = FALSE;
	VTshutdownSWORD();
}


static void on_btnVTAdd_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnVTClose_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/******************************************************************************
 * viewtext ui
 ******************************************************************************/
GtkWidget *create_dlgViewText(void)
{

	GtkWidget *dialog_vbox14;
	GtkWidget *vbox33;
	GtkWidget *toolbar29;
	GtkWidget *combo10;
	GtkWidget *cbeModule;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnVTAdd;
	GtkWidget *btnSync;
	GtkWidget *vseparator16;
	GtkWidget *tbtVTStrongs;
	GtkWidget *tbtnVTFootnotes;
	GtkWidget *tbtnVTMorf;
	GtkWidget *toolbar30;
	GtkWidget *combo11;
	GtkObject *spbVTChapter_adj;
	GtkObject *spbVTVerse_adj;
	GtkWidget *btnGotoVerse;
	GtkWidget *frame21;
	GtkWidget *swVText;
	GtkWidget *dialog_action_area14;
	GtkWidget *btnVTClose;

	dlgViewText = gnome_dialog_new(_("GnomeSword"), NULL);
	gtk_object_set_data(GTK_OBJECT(dlgViewText), "dlgViewText",
			    dlgViewText);
	gtk_window_set_default_size(GTK_WINDOW(dlgViewText), 300, 412);
	gtk_window_set_policy(GTK_WINDOW(dlgViewText), TRUE, TRUE, FALSE);

	dialog_vbox14 = GNOME_DIALOG(dlgViewText)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgViewText), "dialog_vbox14",
			    dialog_vbox14);
	gtk_widget_show(dialog_vbox14);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox33);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "vbox33", vbox33,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox33);
	gtk_box_pack_start(GTK_BOX(dialog_vbox14), vbox33, TRUE, TRUE, 0);

	toolbar29 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar29);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "toolbar29",
				 toolbar29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar29);
	gtk_box_pack_start(GTK_BOX(vbox33), toolbar29, FALSE, FALSE, 0);
	gtk_toolbar_set_space_size(GTK_TOOLBAR(toolbar29), 3);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar29),
				      GTK_RELIEF_NONE);

	combo10 = gtk_combo_new();
	gtk_widget_ref(combo10);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "combo10",
				 combo10,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo10);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), combo10, NULL,
				  NULL);

	cbeModule = GTK_COMBO(combo10)->entry;
	gtk_widget_ref(cbeModule);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "cbeModule",
				 cbeModule,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeModule);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlgViewText, GNOME_STOCK_PIXMAP_ADD);
	btnVTAdd =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar29),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Add"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnVTAdd);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "btnVTAdd",
				 btnVTAdd,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnVTAdd);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlgViewText,
				      GNOME_STOCK_PIXMAP_REFRESH);
	btnSync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar29),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"),
				       _("Sync with main window"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnSync);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "btnSync",
				 btnSync,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnSync);

	vseparator16 = gtk_vseparator_new();
	gtk_widget_ref(vseparator16);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "vseparator16",
				 vseparator16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator16);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), vseparator16,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator16, 7, 13);

	tmp_toolbar_icon =
	    create_pixmap(dlgViewText, "gnomesword/strongs2.xpm", TRUE);
	tbtVTStrongs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar29),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Strongs"),
				       _("Toggle Strongs Numbers"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(tbtVTStrongs);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "tbtVTStrongs",
				 tbtVTStrongs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tbtVTStrongs);

	tmp_toolbar_icon =
	    create_pixmap(dlgViewText, "gnomesword/footnote3.xpm", TRUE);
	tbtnVTFootnotes =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar29),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Footnotes"),
				       _("Toggle Footnotes"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(tbtnVTFootnotes);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText),
				 "tbtnVTFootnotes", tbtnVTFootnotes,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tbtnVTFootnotes);
	gtk_widget_set_usize(tbtnVTFootnotes, 30, 30);

     tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgViewText, GNOME_STOCK_PIXMAP_ALIGN_JUSTIFY);
	tbtnVTMorf = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton1"),
                                _("toggle Morf "), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (tbtnVTMorf);
  gtk_object_set_data_full (GTK_OBJECT (dlgViewText), "tbtnVTMorf", tbtnVTMorf,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tbtnVTMorf);
	
	toolbar30 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar30);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "toolbar30",
				 toolbar30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar30);
	gtk_box_pack_start(GTK_BOX(vbox33), toolbar30, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar30),
				      GTK_RELIEF_NONE);

	combo11 = gtk_combo_new();
	gtk_widget_ref(combo11);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "combo11",
				 combo11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo11);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30), combo11, NULL,
				  NULL);

	cbeBook = GTK_COMBO(combo11)->entry;
	gtk_widget_ref(cbeBook);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "cbeBook",
				 cbeBook,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(cbeBook);


	spbVTChapter_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
	spbVTChapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVTChapter_adj), 1, 0);
	gtk_widget_ref(spbVTChapter);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "spbVTChapter",
				 spbVTChapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbVTChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30), spbVTChapter,
				  NULL, NULL);
	gtk_widget_set_usize(spbVTChapter, 53, -2);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbVTChapter), TRUE);

	spbVTVerse_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
	spbVTVerse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVTVerse_adj), 1, 0);
	gtk_widget_ref(spbVTVerse);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "spbVTVerse",
				 spbVTVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbVTVerse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30),
				  GTK_WIDGET(spbVTVerse), NULL, NULL);
	gtk_widget_set_usize(spbVTVerse, 51, -2);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(dlgViewText,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnGotoVerse =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar30),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"), _("Go to verse"),
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnGotoVerse);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "btnGotoVerse",
				 btnGotoVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnGotoVerse);

	frame21 = gtk_frame_new(NULL);
	gtk_widget_ref(frame21);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "frame21",
				 frame21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame21);
	gtk_box_pack_start(GTK_BOX(vbox33), frame21, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame21, -2, 400);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swVText);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "swVText",
				 swVText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame21), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	text = gtk_html_new();
	gtk_widget_ref(text);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "text", text,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(text);
	gtk_container_add(GTK_CONTAINER(swVText), text);

	dialog_action_area14 = GNOME_DIALOG(dlgViewText)->action_area;
	gtk_object_set_data(GTK_OBJECT(dlgViewText),
			    "dialog_action_area14", dialog_action_area14);
	gtk_widget_show(dialog_action_area14);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area14),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area14),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(dlgViewText),
				   GNOME_STOCK_BUTTON_CLOSE);
	btnVTClose =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(dlgViewText)->buttons)->
		       data);
	gtk_widget_ref(btnVTClose);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "btnVTClose",
				 btnVTClose,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnVTClose);
	GTK_WIDGET_SET_FLAGS(btnVTClose, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(dlgViewText), "destroy",
			   GTK_SIGNAL_FUNC(on_dlgViewText_destroy), NULL);

	gtk_signal_connect(GTK_OBJECT(text), "link_clicked",
			   GTK_SIGNAL_FUNC(on_linkVT_clicked), NULL);
/*	gtk_signal_connect (GTK_OBJECT (htmlTexts), "on_url",
			    GTK_SIGNAL_FUNC (on_url), (gpointer)mainwindow);			
*/
	gtk_signal_connect(GTK_OBJECT(cbeModule), "changed",
			   GTK_SIGNAL_FUNC(on_cbeModule_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(btnVTAdd), "clicked",
			   GTK_SIGNAL_FUNC(on_btnVTAdd_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnSync), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSync_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(tbtVTStrongs), "toggled",
			   GTK_SIGNAL_FUNC(on_tbtVTStrongs_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(tbtnVTFootnotes), "toggled",
			   GTK_SIGNAL_FUNC(on_tbtnVTFootnotes_toggled),
			   NULL);
  	gtk_signal_connect (GTK_OBJECT (tbtnVTMorf), "toggled",
                      GTK_SIGNAL_FUNC (on_tbtnVTMorf_toggled),
                      NULL);
	gtk_signal_connect(GTK_OBJECT(cbeBook), "changed",
			   GTK_SIGNAL_FUNC(on_cbeBook_changed), NULL);
			   
	gtk_signal_connect(GTK_OBJECT(btnGotoVerse), "clicked",
			   GTK_SIGNAL_FUNC(on_btnGotoVerse_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnVTClose), "clicked",
			   GTK_SIGNAL_FUNC(on_btnVTClose_clicked), NULL);
			   
	textList = NULL;
	textList = VTsetupSWORD(text, combo11);
	gtk_combo_set_popdown_strings(GTK_COMBO(combo10), textList);
	gtk_entry_set_text(GTK_ENTRY(cbeModule), gettextmodSWORD());
	VTgotoverseSWORD(current_verse);
	sprintf(vt_current_verse, "%s", current_verse);
	updatecontrols();
	g_list_free(textList);
	isrunningVT = TRUE;

	return dlgViewText;
}
