/*
 * GnomeSword Bible Study Tool
 * text_view.c - view Bible text module in a dialog
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
#include <gtkhtml/gtkhtml.h>


/* frontend */
#include "_bibletext_view.h"
#include "_display_info.h"

/* main */ 
#include "display_info.h"
#include "bibletext_view.h"
#include "gs_gnomesword.h"
#include "gs_shortcutbar.h"
#include "gs_html.h"
#include "support.h"
#include "settings.h"
#include "lists.h"

/******************************************************************************
 * globals
 *****************************************************************************/

/* is the view text dialog runing */
gboolean isrunningVT = FALSE;

GtkWidget *text;
GtkWidget *dlgViewText;
gchar vt_current_verse[80];
GtkWidget *cbeBook;
GtkWidget *spbVTChapter;
GtkWidget *spbVTVerse;

/******************************************************************************
 * externs
 *****************************************************************************/
extern gboolean gsI_isrunning;

/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void update_controls(void)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

static void update_controls(void)
{
	const gchar *buf;

	buf = get_book_viewtext();
	gtk_entry_set_text(GTK_ENTRY(cbeBook), buf);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spbVTChapter),
				  get_chapter_viewtext());
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(spbVTVerse),
				  get_verse_viewtext());
}

/******************************************************************************
 * Name
 *   on_linkVT_clicked
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_linkVT_clicked(GtkHTML * html, const gchar * url, 
 *							gpointer data)	
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

static void on_linkVT_clicked(GtkHTML * html, const gchar * url, 
							gpointer data)
{
	gchar *buf, *modName;
	static GtkWidget *dlg;

	if (*url == '#') {

		if (!gsI_isrunning) {
			dlg = gui_create_display_informtion_dialog();
		}
		++url;		/* remove # */

		if (*url == 'T')
			++url;

		if (*url == 'G') {
			++url;
			modName = g_strdup(settings.lex_greek);
			buf = g_strdup(url);
			loadmodandkey(modName, buf);
			g_free(buf);
			g_free(modName);
		}

		if (*url == 'H') {
			++url;
			modName = g_strdup(settings.lex_hebrew);
			buf = g_strdup(url);
			loadmodandkey(modName, buf);
			g_free(buf);
			g_free(modName);
		}
		gtk_widget_show(dlg);
	}

	else if (*url == 'M') {
		if (!gsI_isrunning) {
			dlg = gui_create_display_informtion_dialog();
		}
		++url;		/* remove M */
		buf = g_strdup(url);
		loadmodandkey("Packard", buf);
		g_free(buf);
		gtk_widget_show(dlg);
	}

	else if (*url == '*') {
		++url;
		while (*url != ']') {
			++url;
		}
		++url;
		buf = g_strdup(url);
		goto_verse_viewtext(buf);
		update_controls();
		g_free(buf);
	}

	else if (!strncmp(url, "passage=", 7)) {/*** remove passage= verse list ***/
		gchar *mybuf = NULL;
		gchar *modbuf = NULL;
		mybuf = strchr(url, '=');
		++mybuf;
		buf = g_strdup(mybuf);
		settings.whichwindow = 0;
		modbuf = get_module_name(&settings);
		display_verse_list(modbuf, buf, &settings);
		g_free(buf);

	}

	else if (!strncmp(url, "type=morph", 10)) {
		gchar *modbuf = NULL;
		gchar *mybuf = NULL;
		buf = g_strdup(url);
		g_warning("mybuf = %s", url);
		mybuf = strstr(url, "class=");
		if (mybuf) {
			gint i;
			modbuf = strchr(mybuf, '=');
			++modbuf;
			for (i = 0; i < strlen(modbuf); i++) {
				if (modbuf[i] == ' ') {
					modbuf[i] = '\0';
					break;
				}
			}
		}


		mybuf = NULL;
		mybuf = strstr(buf, "value=");
		if (mybuf) {
			mybuf = strchr(mybuf, '=');
			++mybuf;
			buf = g_strdup(mybuf);
			if (!gsI_isrunning) {
				dlg = gui_create_display_informtion_dialog();
			}
			loadmodandkey(modbuf, buf);
		}
		g_free(buf);
		gtk_widget_show(dlg);
	}
}

/******************************************************************************
 * Name
 *   on_cbeBook_changed
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_cbeBook_changed(GtkEditable * editable,
 *					       gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_cbeBook_changed(GtkEditable * editable,
					       gpointer user_data)
{

}

/******************************************************************************
 * Name
 *   on_btnGotoVerse_clicked
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_btnGotoVerse_clicked(GtkButton * button,
 *						gpointer user_data)
 *
 * Description
 *   change verse
 *
 * Return value
 *   void
 */

static void on_btnGotoVerse_clicked(GtkButton * button,
						gpointer user_data)
{
	gchar *bookname, buf[120];
	gint iChap, iVerse;

	bookname = gtk_entry_get_text(GTK_ENTRY(cbeBook));
	iChap =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (spbVTChapter));
	iVerse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (spbVTVerse));
	sprintf(buf, "%s %d:%d", bookname, iChap, iVerse);
	//g_warning(buf);
	goto_verse_viewtext(buf);
	update_controls();
}

/******************************************************************************
 * Name
 *   on_cbeModule_changed
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_cbeModule_changed(GtkEditable * editable, 
 *						gpointer user_data)	
 *
 * Description
 *   change to new module
 *
 * Return value
 *   void
 */

static void on_cbeModule_changed(GtkEditable * editable, 
						gpointer user_data)
{
	gchar *buf, title[256];
	static gboolean firsttime = TRUE;
	buf = gtk_entry_get_text(GTK_ENTRY(editable));
	load_module_viewtext(buf);
	goto_verse_viewtext(settings.currentverse);
	sprintf(title, "GnomeSword - %s",
		get_module_description(buf));
	gtk_window_set_title(GTK_WINDOW(dlgViewText), title);
	firsttime = FALSE;
}


/******************************************************************************
 * Name
 *   on_btnSync_clicked
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_btnSync_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   set text to main window current verse
 *
 * Return value
 *   void
 */

static void on_btnSync_clicked(GtkButton * button, gpointer user_data)
{
	goto_verse_viewtext(settings.currentverse);
	update_controls();
}

/******************************************************************************
 * Name
 *   on_dlgViewText_destroy
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_dlgViewText_destroy(GtkObject * object,
 *						gpointer user_data)
 *
 * Description
 *   shut down the View Text Dialog
 *
 * Return value
 *   void
 */

static void on_dlgViewText_destroy(GtkObject * object,
						gpointer user_data)
{
	isrunningVT = FALSE;
	shutdown_viewtext();
}


/******************************************************************************
 * Name
 *   on_btnVTAdd_clicked
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_btnVTAdd_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_btnVTAdd_clicked(GtkButton * button, gpointer user_data)
{

}


/******************************************************************************
 * Name
 *   on_btnVTClose_clicked
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void on_btnVTClose_clicked(GtkButton * button,
 *						gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_btnVTClose_clicked(GtkButton * button,
						gpointer user_data)
{
	gtk_widget_hide(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/******************************************************************************
 * Name
 *   on_modops_activate
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   	void on_modops_activate(GtkMenuItem * menuitem,
 *					       gpointer user_data)
 *
 * Description
 *   set SWORD module global options
 *
 * Return value
 *   void
 */

static void on_modops_activate(GtkMenuItem * menuitem,
					       gpointer user_data)
{
	set_global_options_viewtext((gchar *) user_data, 
		GTK_CHECK_MENU_ITEM(menuitem)->active);
}

/******************************************************************************
 * Name
 *   additemstooptionsmenu
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   void additemstooptionsmenu(GtkWidget * shellmenu)	
 *
 * Description
 *   add sword global options to menu
 *
 * Return value
 *   void
 */

static void additemstooptionsmenu(GtkWidget * shellmenu)
{
	GtkWidget *menuChoice;
	gchar menuName[64];
	int viewNumber = 0;
	GList *tmp;

	tmp = NULL;

	tmp = get_list(OPTIONS_LIST);
	while (tmp != NULL) {

		/* add global option items to menu */
		menuChoice =
		    gtk_check_menu_item_new_with_label((gchar *) (gchar
								  *)
						       tmp->data);
		sprintf(menuName, "optionNum%d", viewNumber++);
		gtk_object_set_data(GTK_OBJECT(settings.app), menuName,
				    menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
				   GTK_SIGNAL_FUNC(on_modops_activate),
				   (gchar *) (gchar *) tmp->data);
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
				      GTK_WIDGET(menuChoice), 1);

		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}


/******************************************************************************
 * Name
 *   gui_create_viewtext_dialog
 *
 * Synopsis
 *   #include "text_view.h"
 *
 *   GtkWidget *gui_create_viewtext_dialog(void)	
 *
 * Description
 *   viewtext gui
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget *gui_create_viewtext_dialog(void)
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
	GtkWidget *menubar;
	GtkWidget *module_options;
	GtkWidget *module_options_menu;
	GtkAccelGroup *module_options_menu_accels;
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
	gtk_window_set_default_size(GTK_WINDOW(dlgViewText), 370, 412);
	gtk_window_set_policy(GTK_WINDOW(dlgViewText), TRUE, TRUE,
			      FALSE);

	dialog_vbox14 = GNOME_DIALOG(dlgViewText)->vbox;
	gtk_object_set_data(GTK_OBJECT(dlgViewText), "dialog_vbox14",
			    dialog_vbox14);
	gtk_widget_show(dialog_vbox14);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox33);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "vbox33",
				 vbox33,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox33);
	gtk_box_pack_start(GTK_BOX(dialog_vbox14), vbox33, TRUE, TRUE,
			   0);

	toolbar29 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
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
	    gnome_stock_pixmap_widget(dlgViewText,
				      GNOME_STOCK_PIXMAP_ADD);
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
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText),
				 "vseparator16", vseparator16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator16);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), vseparator16,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator16, 7, 13);

	menubar = gtk_menu_bar_new();
	gtk_widget_ref(menubar);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText), "menubar",
				 menubar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), menubar, NULL,
				  NULL);

	module_options =
	    gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_ref(module_options);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText),
				 "module_options", module_options,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(menubar), module_options);

	module_options_menu = gtk_menu_new();
	gtk_widget_ref(module_options_menu);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText),
				 "module_options_menu",
				 module_options_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  module_options_menu);
	module_options_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (module_options_menu));

	additemstooptionsmenu(module_options_menu);

	toolbar30 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
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
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText),
				 "spbVTChapter", spbVTChapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(spbVTChapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30), spbVTChapter,
				  NULL, NULL);
	gtk_widget_set_usize(spbVTChapter, 53, -2);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(spbVTChapter),
				    TRUE);

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
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnGotoVerse);
	gtk_object_set_data_full(GTK_OBJECT(dlgViewText),
				 "btnGotoVerse", btnGotoVerse,
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
			    "dialog_action_area14",
			    dialog_action_area14);
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
			   GTK_SIGNAL_FUNC(on_dlgViewText_destroy),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(text), "link_clicked",
			   GTK_SIGNAL_FUNC(on_linkVT_clicked), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbeModule), "changed",
			   GTK_SIGNAL_FUNC(on_cbeModule_changed), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnVTAdd), "clicked",
			   GTK_SIGNAL_FUNC(on_btnVTAdd_clicked), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnSync), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSync_clicked), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(cbeBook), "changed",
			   GTK_SIGNAL_FUNC(on_cbeBook_changed), 
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnGotoVerse), "clicked",
			   GTK_SIGNAL_FUNC(on_btnGotoVerse_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnVTClose), "clicked",
			   GTK_SIGNAL_FUNC(on_btnVTClose_clicked),
			   NULL);

		
	setup_viewtext(text);
	
	
	gtk_combo_set_popdown_strings(GTK_COMBO(combo10), get_list(TEXT_LIST));
	
	
	
	gtk_entry_set_text(GTK_ENTRY(cbeModule), settings.MainWindowModule);
	
	gtk_combo_set_popdown_strings(GTK_COMBO(combo11), get_list(BOOKS_LIST));
	goto_verse_viewtext(settings.currentverse);
	sprintf(vt_current_verse, "%s", settings.currentverse);
	update_controls();
	isrunningVT = TRUE;
	return dlgViewText;
}
