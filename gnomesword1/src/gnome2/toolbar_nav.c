/*
 * GnomeSword Bible Study Tool
 * toolbar_nav.c - Bible text navigation toolbar
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <gal/widgets/e-unicode.h>

#include "gui/toolbar_nav.h"
#include "gui/shortcutbar_main.h"
#include "gui/history.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"
#include "gui/tabbed_browser.h"

#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/key.h"
#include "main/xml.h"

NAV_BAR nav_bar;
static GtkWidget *cbe_book;
static GtkWidget *spb_chapter;
static GtkWidget *spb_verse;
static GtkWidget *cbe_freeform_lookup;


/******************************************************************************
 * Name
 *   gui_update_nav_controls
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gchar *gui_update_nav_controls(const gchar * key)	
 *
 * Description
 *   updates the nav toolbar controls 
 *
 * Return value
 *   gchar *
 */

gchar *gui_update_nav_controls(const gchar * key)
{
	char *val_key;
	gint cur_chapter = 8, cur_verse = 28;

	settings.apply_change = FALSE;
	val_key = get_valid_key(key);
	//g_warning(val_key);
	//g_warning("key = %s val_key = %s",key,val_key);
	cur_chapter = get_chapter_from_key(val_key);
	cur_verse = get_verse_from_key(val_key);
	/* 
	 *  remember last verse 
	 */
	xml_set_value("GnomeSword", "keys", "verse", val_key);
	settings.currentverse = xml_get_value("keys", "verse");
	//g_warning("cv = %s\nval_key = %s",settings.currentverse,val_key);
	/* 
	 *  set book, chapter,verse and freeform lookup entries
	 *  to new verse - settings.apply_change is set to false so we don't
	 *  start a loop
	 */
	gtk_entry_set_text(GTK_ENTRY(cbe_book),
			   get_book_from_key(val_key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (spb_chapter), cur_chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (spb_verse), cur_verse);
	gtk_entry_set_text(GTK_ENTRY(cbe_freeform_lookup), val_key);
	if(settings.browsing)
		gui_set_tab_label(val_key);
	settings.apply_change = TRUE;
	return val_key;
}


/******************************************************************************
 * Name
 *   on_cbeBook_changed
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_cbeBook_changed(GtkEditable * editable, 
 *						gpointer user_data)	
 *
 * Description
 *   change book 
 *
 * Return value
 *   void
 */

static void on_cbeBook_changed(GtkEditable * editable,
			       gpointer user_data)
{
	gchar buf[256];
	gchar *bookname = NULL;
	if (settings.apply_change) {
		bookname = e_utf8_gtk_editable_get_text(editable);//gtk_editable_get_chars(editable, 0, -1);
		//g_warning(bookname);
		if (*bookname) {
			sprintf(buf, "%s 1:1", bookname);
			//g_warning(bookname);
			//g_warning(buf);
			gui_change_verse(buf);
		}
	}
}

/******************************************************************************
 * Name
 *   on_spbChapter_button_release_event
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gboolean on_spbChapter_button_release_event(GtkWidget * widget,
 *			GdkEventButton * event,   gpointer user_data)	
 *
 * Description
 *    change chapter 
 *
 * Return value
 *   gboolean
 */

static gboolean on_spbChapter_button_release_event(GtkWidget * widget,
						   GdkEventButton *
						   event,
						   gpointer user_data)
{
	if (settings.apply_change) {
		const gchar *bookname;
		gchar buf[256];
		gint chapter;

		bookname = gtk_entry_get_text(GTK_ENTRY(cbe_book));
		chapter =
		    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
						     (spb_chapter));
		sprintf(buf, "%s %d:1", bookname, chapter);
		gui_change_verse(buf);
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_spbVerse_button_release_event
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gboolean on_spbVerse_button_release_event(GtkWidget * widget, 
 *			GdkEventButton * event,  gpointer user_data)	
 *
 * Description
 *    change verse
 *
 * Return value
 *   gboolean
 */

static gboolean on_spbVerse_button_release_event(GtkWidget * widget,
						 GdkEventButton * event,
						 gpointer user_data)
{
	if (settings.apply_change) {
		const gchar *bookname;
		gchar buf[256];
		int chapter, verse;

		bookname = gtk_entry_get_text(GTK_ENTRY(cbe_book));
		chapter =
		    gtk_spin_button_get_value(GTK_SPIN_BUTTON
					      (spb_chapter));
		verse =
		    gtk_spin_button_get_value(GTK_SPIN_BUTTON
					      (spb_verse));
		sprintf(buf, "%s %d:%d", bookname, chapter, verse);
		gui_change_verse(buf);
	}
	return FALSE;
}


/******************************************************************************
 * Name
 *   on_button_dict_book_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_button_dict_book_clicked(GtkButton * button,
 *				      gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_button_dict_book_clicked(GtkButton * button,
					gpointer user_data)
{
	if (gtk_notebook_get_current_page
	    (GTK_NOTEBOOK(widgets.workbook_lower)))
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.workbook_lower),
					      0);
	else
		gtk_notebook_set_current_page(GTK_NOTEBOOK
					      (widgets.workbook_lower),
					      1);
}


/******************************************************************************
 * Name
 *   on_togglebutton_parallel_view_toggled
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_togglebutton_parallel_view_toggled(GtkToggleButton *
 *						  togglebutton,
 *						  gpointer user_data)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void on_togglebutton_parallel_view_toggled(GtkToggleButton *
						  togglebutton,
						  gpointer user_data)
{
	if(togglebutton->active) 
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_parallel_text),
						      1);
	else
		gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_parallel_text),
						      0);
}


/******************************************************************************
 * Name
 *   on_btnLookup_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_btnLookup_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *    go to verse in free form entry
 *
 * Return value
 *   void
 */

static void on_btnLookup_clicked(GtkButton * button, gpointer user_data)
{
	const gchar *buf;

	buf = gtk_entry_get_text(GTK_ENTRY(cbe_freeform_lookup));
	gui_change_verse(buf);	//-- change verse to entry text 
}

/******************************************************************************
 * Name
 *   on_cbeFreeformLookup_key_press_event
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   gboolean on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
 *				GdkEventKey * event, gpointer user_data)	
 *
 * Description
 *   go to verse in free form entry if user hit <enter>
 *
 * Return value
 *   gboolean
 */

static gboolean on_cbeFreeformLookup_key_press_event(GtkWidget * widget,
						     GdkEventKey *
						     event,
						     gpointer user_data)
{
	const gchar *buf;

	buf = gtk_entry_get_text(GTK_ENTRY(widget));
	/* if <enter> key */
	if (event->keyval == 65293 || event->keyval == 65421) {
		gui_change_verse(buf);
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_button_back_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_nav_bar.button_back_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *    move backward through history list
 *
 * Return value
 *   void
 */

static void on_button_back_clicked(GtkButton * button,
				   gpointer user_data)
{
	gui_navigate_history(widgets.app, 0);
}

/******************************************************************************
 * Name
 *   on_button_forward_clicked
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   void on_button_forward_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   go forward through history list 
 *
 * Return value
 *   void
 */

static void on_button_forward_clicked(GtkButton * button,
				      gpointer user_data)
{
	gui_navigate_history(widgets.app, 1);
}


/******************************************************************************
 * Name
 *   gui_create_nav_toolbar
 *
 * Synopsis
 *   #include "toolbar_nav.h"
 *
 *   GtkWidget *gui_create_nav_toolbar(void)	
 *
 * Description
 *    create the Bible text navigation toolbar and return it
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_nav_toolbar(GtkWidget * app)
{
	GtkWidget *toolbarNav;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *cbBook;
	GtkObject *spbChapter_adj;
	GtkObject *spbVerse_adj;
	GtkWidget *btnLookup;
	GtkWidget *button_dict_book;
	GtkWidget *togglebutton_parallel_view;
	//GtkWidget *vseparator1;
	GtkWidget *label;
	//GtkWidget *vseparator2;
	GtkObject *spb_chapter_adj;
	GtkObject *spb_verse_adj;
	GtkTooltips *tooltips;
	
	tooltips = gtk_tooltips_new ();


	toolbarNav = gtk_toolbar_new();
	gtk_widget_show(toolbarNav);

	gnome_app_add_toolbar(GNOME_APP(app), GTK_TOOLBAR(toolbarNav),
			      "toolbarNav",
			      BONOBO_DOCK_ITEM_BEH_EXCLUSIVE,
			      BONOBO_DOCK_TOP, 1, 0, 0);

	gtk_toolbar_set_style(GTK_TOOLBAR(toolbarNav),
			      GTK_TOOLBAR_ICONS);

	tmp_toolbar_icon =
	    gtk_image_new_from_stock(GNOME_STOCK_BOOK_BLUE,
				     gtk_toolbar_get_icon_size
				     (GTK_TOOLBAR(toolbarNav)));
	widgets.button_dict_book =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Dictionary/Book"), 
				       _("Toggle Dictionary View"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_label_set_use_underline(GTK_LABEL
				    (((GtkToolbarChild
				       *) (g_list_last(GTK_TOOLBAR
						       (toolbarNav)->
						       children)->
					   data))->label), TRUE);
	gtk_widget_show(widgets.button_dict_book);

	tmp_toolbar_icon =
	    gtk_image_new_from_stock(GNOME_STOCK_TEXT_BULLETED_LIST,
				     gtk_toolbar_get_icon_size
				     (GTK_TOOLBAR(toolbarNav)));
	widgets.button_parallel_view =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Parallel View"),
				       _("Toggle Parallel View"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_label_set_use_underline(GTK_LABEL
				    (((GtkToolbarChild
				       *) (g_list_last(GTK_TOOLBAR
						       (toolbarNav)->
						       children)->
					   data))->label), TRUE);
	gtk_widget_show(widgets.button_parallel_view);
	
	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
  /*
	vseparator1 = gtk_vseparator_new();
	gtk_widget_show(vseparator1);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), vseparator1,
				  NULL, NULL);
	gtk_widget_set_size_request(vseparator1, 5, 0);
*/
	cbBook = gtk_combo_new();
	g_object_set_data(G_OBJECT(GTK_COMBO(cbBook)->popwin),
			  "GladeParentKey", cbBook);
	gtk_widget_show(cbBook);
	//gtk_widget_set_size_request(cbBook, 130, -1);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), cbBook, 
				NULL,
				NULL);
	/*
	 * get and load books of the Bible 
	 */
	gtk_combo_set_popdown_strings(GTK_COMBO(cbBook),
				      get_list(BOOKS_LIST));

	cbe_book = GTK_COMBO(cbBook)->entry;
	gtk_widget_show(cbe_book);
	gtk_tooltips_set_tip(tooltips, cbe_book, _("Select a Book of the Bible"), 
				NULL);
	gtk_entry_set_text(GTK_ENTRY(cbe_book), _("Romans"));

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	spb_chapter_adj = gtk_adjustment_new(8, 0, 151, 1, 10, 10);
	spb_chapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spb_chapter_adj), 1, 0);
	gtk_widget_show(spb_chapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), spb_chapter,
				  _("Change Chapter"), NULL);

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	spb_verse_adj = gtk_adjustment_new(28, 0, 180, 1, 10, 10);
	spb_verse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spb_verse_adj), 1, 0);
	gtk_widget_show(spb_verse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), spb_verse,
				  _("Change Verse"), NULL);

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	
	cbe_freeform_lookup = gtk_entry_new();
	gtk_widget_show(cbe_freeform_lookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav),
				  cbe_freeform_lookup, 
		_("Enter a Verse reference in Book 1:1 format and press Return or click the Go to Verse button")
		, NULL);
	//gtk_widget_set_size_request(cbe_freeform_lookup, 150, -1);
	nav_bar.lookup_entry = cbe_freeform_lookup;

	label = gtk_label_new ("");
	gtk_widget_show (label);
	gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarNav), label, NULL, NULL);
	gtk_widget_set_size_request (label, 6, -1);
	gtk_label_set_justify (GTK_LABEL (label), GTK_JUSTIFY_LEFT);
	/*
	vseparator2 = gtk_vseparator_new();
	gtk_widget_show(vseparator2);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarNav), vseparator2,
				  NULL, NULL);
	gtk_widget_set_size_request(vseparator2, 5, 0);
*/
	btnLookup = gtk_toolbar_insert_stock(GTK_TOOLBAR(toolbarNav),
					     GTK_STOCK_JUMP_TO,
		_("Go to verse"),
					     NULL, NULL, NULL, -1);
	gtk_widget_show(btnLookup);

	/*tmp_toolbar_icon =
	    gtk_image_new_from_stock(GTK_STOCK_GO_BACK,
				     gtk_toolbar_get_icon_size
				     (GTK_TOOLBAR(toolbarNav)));*/
	nav_bar.button_back = gtk_toolbar_insert_stock (GTK_TOOLBAR (toolbarNav),
                                "gtk-go-back",
                                _("Move backwards through history"),
                                NULL, NULL, NULL, -1);/*
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button5"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_label_set_use_underline(GTK_LABEL
				    (((GtkToolbarChild
				       *) (g_list_last(GTK_TOOLBAR
						       (toolbarNav)->
						       children)->
					   data))->label), TRUE);*/
	gtk_widget_show(nav_bar.button_back);

	/*tmp_toolbar_icon =
	    gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,
				     gtk_toolbar_get_icon_size
				     (GTK_TOOLBAR(toolbarNav)));*/
	nav_bar.button_forward = gtk_toolbar_insert_stock (GTK_TOOLBAR (toolbarNav),
                                "gtk-go-forward",
                                _("Move foward through history"),
                                NULL, NULL, NULL, -1);
	/*    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarNav),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button6"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_label_set_use_underline(GTK_LABEL
				    (((GtkToolbarChild
				       *) (g_list_last(GTK_TOOLBAR
						       (toolbarNav)->
						       children)->
					   data))->label), TRUE);*/
					   
	gtk_widget_show(nav_bar.button_forward);
	gtk_widget_set_sensitive(nav_bar.button_forward, FALSE);
	gtk_widget_set_sensitive(nav_bar.button_back, FALSE);

	g_signal_connect((gpointer) widgets.button_dict_book, "clicked",
			 G_CALLBACK(on_button_dict_book_clicked), NULL);
	g_signal_connect((gpointer) widgets.button_parallel_view,
			 "toggled",
			 G_CALLBACK
			 (on_togglebutton_parallel_view_toggled), NULL);

	g_signal_connect(GTK_OBJECT(cbe_book), "changed",
			   G_CALLBACK(on_cbeBook_changed), NULL);
	g_signal_connect(GTK_OBJECT(spb_chapter),
			   "button_release_event",
			   G_CALLBACK
			   (on_spbChapter_button_release_event), NULL);
	g_signal_connect(GTK_OBJECT(spb_verse),
			   "button_release_event",
			   G_CALLBACK(on_spbVerse_button_release_event),
			   NULL);
	g_signal_connect(GTK_OBJECT(cbe_freeform_lookup),
			   "key_press_event",
			   G_CALLBACK
			   (on_cbeFreeformLookup_key_press_event),
			   NULL);
	g_signal_connect(GTK_OBJECT(btnLookup), "clicked",
			   G_CALLBACK(on_btnLookup_clicked), NULL);
	g_signal_connect(GTK_OBJECT(nav_bar.button_back), "clicked",
			   G_CALLBACK(on_button_back_clicked), NULL);
	g_signal_connect(GTK_OBJECT(nav_bar.button_forward),
			   "clicked",
			   G_CALLBACK(on_button_forward_clicked), NULL);

	return toolbarNav;
}
