/*
 * GnomeSword Bible Study Tool
 * shortcutbar_search.c - create search group gui
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
#include <gal/widgets/e-unicode.h>
#include <regex.h>

#include "gui/sidebar.h"
#include "gui/search_sidebar.h"
#include "gui/html.h"
#include "gui/gnomesword.h"
#include "gui/widgets.h"

#include "main/module.h"
#include "main/key.h"
#include "main/sword.h"
#include "main/search.h"
#include "main/settings.h"


static GtkWidget *rrbUseBounds;
static GtkWidget *entrySearch;
static GtkWidget *entryLower;
static GtkWidget *entryUpper;
static GtkWidget *rbRegExp;
static GtkWidget *rbLastSearch;
static GtkWidget *rbPhraseSearch;
static GtkWidget *ckbCaseSensitive;
static GtkWidget *progressbar_search;
static GtkWidget *frame5;
static GtkWidget *frame_module;
static GtkWidget *radiobutton_search_text;
static GtkWidget *radiobutton_search_comm;



/******************************************************************************
 * Name
 *    gui_search_update_sidebar
 *
 * Synopsis
 *   #include "sidebar_search.h"
 *
 *    void gui_search_update_sidebar(char percent, void *userData) 
 *
 * Description
 *   updates the progress bar during search
 *
 * Return value
 *   void
 */

void gui_search_update_sidebar(char percent, void *userData)
{
	char maxHashes = *((char *) userData);
	float num;
	char buf[80];
	static char printed = 0;

	while ((((float) percent) / 100) * maxHashes > printed) {
		sprintf(buf, "%f", (((float) percent) / 100));
		num = (float) percent / 100;
		gtk_progress_bar_update((GtkProgressBar *)
					progressbar_search, num);
		printed++;
	}
	while (gtk_events_pending())
		gtk_main_iteration();
	printed = 0;
}


/******************************************************************************
 * Name
 *    fill_search_results_list
 *
 * Synopsis
 *   #include "sidebar_search.h"
 *
 *   void fill_search_results_list(int finds) 
 *
 * Description
 *   display a list of keys found during search
 *
 * Return value
 *   void
 */

static void fill_search_results_list(int finds)
{
	gchar *utf8str, buf[256];
	gchar *tmpbuf;
	const gchar *key_buf = NULL;
	gint i = 0;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	GtkTreeSelection *selection;
	GtkTreePath *path;	
	gchar *buf1 = N_("matches");
	RESULTS *list_item;
	
	if(list_of_verses) {
		while(list_of_verses) {
			g_free(list_of_verses->data);
			list_of_verses = g_list_next(list_of_verses);
		}
		g_list_free(list_of_verses);
	}
	list_of_verses = NULL;
	
	selection = gtk_tree_view_get_selection
                                          (GTK_TREE_VIEW(sidebar.results_list));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(sidebar.results_list));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);
	set_results_position((char) 1);	/* TOP */
	while ((key_buf = get_next_result_key()) != NULL) {
		tmpbuf = (gchar *) key_buf;
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0,
					   tmpbuf, -1);
		list_item = g_new(RESULTS,1);
		list_item->module = settings.sb_search_mod;
		list_item->key = tmpbuf;
		list_of_verses = g_list_append(list_of_verses, 
						(RESULTS *) list_item);
	}
	
	sprintf(buf, "%d %s", finds, buf1);
	gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), buf);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_sidebar),3);
	/* cleanup progress bar */
	gtk_progress_bar_update(GTK_PROGRESS_BAR(progressbar_search),
				0.0);
	/* display first item in list by selection row*/	
	if(!gtk_tree_model_get_iter_first(model,&iter))
		return;
	path = gtk_tree_model_get_path(model,&iter);				
	gtk_tree_selection_select_path(selection,
                                             path);
	gtk_tree_path_free(path);	 
	gui_verselist_button_release_event(NULL,NULL,NULL);
}


/******************************************************************************
 * Name
 *    on_search_botton_clicked
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void on_search_botton_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   prepare to begin search
 *
 * Return value
 *   void
 */

static void on_search_botton_clicked(GtkButton * button, gpointer user_data)
{
	GString *str;
	gint search_params, finds;
	const gchar *search_string = NULL;
	gchar *search_module;	
	gchar search_label[80];
	gchar *buf = N_("Search");
	
	search_string = gtk_entry_get_text(GTK_ENTRY(entrySearch));
	
	if(strlen(search_string) < 1) 
		return;
	
	if (GTK_TOGGLE_BUTTON(radiobutton_search_text)->active) {
		sprintf(search_label, "%s %s", buf,
			settings.MainWindowModule);
		strcpy(settings.sb_search_mod,
		       settings.MainWindowModule);
	}
	else if (GTK_TOGGLE_BUTTON(radiobutton_search_comm)->active) {
		sprintf(search_label, "%s %s", buf,
			settings.CommWindowModule);
		strcpy(settings.sb_search_mod,settings.CommWindowModule);
	} 	
	
	search_module = settings.sb_search_mod;

	clear_scope();

	if (GTK_TOGGLE_BUTTON(rrbUseBounds)->active) {
		clear_search_list();
		str = g_string_new(" ");
		g_string_sprintf(str, "%s - %s",
				 gtk_entry_get_text(GTK_ENTRY
						    (entryLower)),
				 gtk_entry_get_text(GTK_ENTRY
						    (entryUpper)));
		set_range(str->str);
		set_scope2range();
		g_string_free(str, TRUE);
	}

	if (GTK_TOGGLE_BUTTON(rbLastSearch)->active)
		set_scope2last_search();

	sprintf(settings.searchText, "%s", search_string);

	settings.searchType = GTK_TOGGLE_BUTTON
	    (rbRegExp)->active ? 0 :
	    GTK_TOGGLE_BUTTON(rbPhraseSearch)->active ? -1 : -2;

	search_params =
	    GTK_TOGGLE_BUTTON(ckbCaseSensitive)->active ? 0 : REG_ICASE;

	finds = do_module_search(search_module, search_string,
				 settings.searchType, search_params,
				 FALSE);

	fill_search_results_list(finds);
	gtk_widget_set_sensitive(sidebar.menu_item_save_search,finds);
}


/******************************************************************************
 * Name
 *   on_rrbUseBounds_toggled
 *
 * Synopsis
 *   #include "gui/shortcutbar_search.h"
 *
 *   void on_rrbUseBounds_toggled(GtkToggleButton * togglebutton,
 *						gpointer user_data)	
 *
 * Description
 *   a toggle button has changed
 *   set ok - apply button sensitive
 *
 * Return value
 *   void
 */

static void on_rrbUseBounds_toggled(GtkToggleButton * togglebutton,
				    gpointer user_data)
{
	if (togglebutton->active) {
		gtk_widget_show(frame5);
	} else {
		gtk_widget_hide(frame5);
	}
}


/******************************************************************************
 * Name
 *   gui_create_shortcutbar_search 
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void gui_create_shortcutbar_search(GtkWidget * vp)	
 *
 * Description
 *   create search group of shortcut bar
 *
 * Return value
 *   void
 */

void gui_create_search_sidebar(void)
{
	GtkWidget *vbox1;
	GtkWidget *vbox90;
	GtkWidget *frame_search;
	GtkWidget *vbox5;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnSearch;
	GtkWidget *frame2;
	GtkWidget *vbox2;
	GtkWidget *rbMultiword;
	GtkWidget *frame3;
	GtkWidget *vbox3;
	GtkWidget *frame4;
	GtkWidget *vbox4;
	GtkWidget *rbNoScope;
	GtkWidget *table1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *frame6;
	GtkWidget *scrolledwindow_search;
	GtkWidget *viewport_search;
	GtkTooltips *tooltips;
	gchar *header;

	tooltips = gtk_tooltips_new();

	
	scrolledwindow_search = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow_search);
	gtk_container_add(GTK_CONTAINER(widgets.notebook_sidebar),
			  scrolledwindow_search);
	gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (
					scrolledwindow_search), GTK_SHADOW_IN);
	gtk_container_set_border_width (GTK_CONTAINER (scrolledwindow_search), 2);

	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow_search),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	viewport_search = gtk_viewport_new(NULL, NULL);
	gtk_widget_show(viewport_search);
	gtk_container_add(GTK_CONTAINER(scrolledwindow_search),
			  viewport_search);
			  
	vbox1 = gtk_vbox_new(FALSE, 4);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(viewport_search), vbox1);
	
	vbox5 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox5);
	gtk_box_pack_start(GTK_BOX(vbox1), vbox5, FALSE, TRUE,
			   0);

	entrySearch = gtk_entry_new();
	gtk_widget_show(entrySearch);
	gtk_box_pack_start(GTK_BOX(vbox5), entrySearch, TRUE, TRUE, 0);
	gtk_widget_set_size_request(entrySearch, 130, -1);;	
	
	btnSearch = gtk_button_new_from_stock(GTK_STOCK_FIND);
	gtk_widget_show (btnSearch);
	gtk_box_pack_start(GTK_BOX(vbox5), btnSearch, TRUE, FALSE, 0);
	gtk_tooltips_set_tip (tooltips, btnSearch, _("Start searching"), NULL);
	gtk_button_set_relief (GTK_BUTTON (btnSearch), GTK_RELIEF_NONE);

	progressbar_search = gtk_progress_bar_new();
	gtk_widget_show(progressbar_search);
	gtk_box_pack_start(GTK_BOX(vbox5), progressbar_search, FALSE, TRUE, 0);

	frame_module = gtk_frame_new(NULL);
	gtk_widget_show(frame_module);
	gtk_box_pack_start(GTK_BOX(vbox1), frame_module, FALSE, FALSE,
			   0);
	gtk_container_set_border_width (GTK_CONTAINER (frame_module), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(frame_module),GTK_SHADOW_NONE);
	
	label1 = gtk_label_new(NULL); 

	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Search Module"));
  	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget      (GTK_FRAME(frame_module),
                                             label1);
					     
	vbox90 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox90);
	gtk_container_add(GTK_CONTAINER(frame_module), vbox90);
	gtk_container_set_border_width (GTK_CONTAINER (vbox90), 4);

	radiobutton_search_text =
	    gtk_radio_button_new_with_label(NULL, _("Bible"));
	gtk_widget_show(radiobutton_search_text);
	gtk_widget_set_size_request(radiobutton_search_text, -1, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), radiobutton_search_text,
			   FALSE, FALSE, 0);

	radiobutton_search_comm =
	    gtk_radio_button_new_with_label_from_widget(
	    			GTK_RADIO_BUTTON(radiobutton_search_text),
				_("Commentary"));
	gtk_widget_show(radiobutton_search_comm);
	gtk_widget_set_size_request(radiobutton_search_comm, -1, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), radiobutton_search_comm,
			   FALSE, FALSE, 0);
			   
	frame2 = gtk_frame_new(NULL);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame2), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(frame2),GTK_SHADOW_NONE);
	
	label1 = gtk_label_new(NULL);

	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Search Type"));
  	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);
	
	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(frame2), label1);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);
	gtk_container_set_border_width (GTK_CONTAINER (vbox2), 4);

	rbMultiword =
	    gtk_radio_button_new_with_label(NULL,
					    _("Multi word"));
	gtk_widget_show(rbMultiword);
	gtk_box_pack_start(GTK_BOX(vbox2), rbMultiword, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(rbMultiword, -1, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (rbMultiword), TRUE);

	rbRegExp =
	    gtk_radio_button_new_with_label_from_widget(
	    				GTK_RADIO_BUTTON(rbMultiword),
					_("Regular expression"));
	gtk_widget_show(rbRegExp);
	gtk_box_pack_start(GTK_BOX(vbox2), rbRegExp, FALSE, FALSE, 0);
	gtk_widget_set_size_request(rbRegExp, -1, 20);

	rbPhraseSearch =
	    gtk_radio_button_new_with_label_from_widget(
	    				GTK_RADIO_BUTTON(rbMultiword),
					_("Exact phrase"));
	gtk_widget_show(rbPhraseSearch);
	gtk_box_pack_start(GTK_BOX(vbox2), rbPhraseSearch,
			   FALSE, FALSE, 0);
	gtk_widget_set_size_request(rbPhraseSearch, -1, 20);

	frame3 = gtk_frame_new(NULL);
	gtk_widget_show(frame3);
	gtk_box_pack_start(GTK_BOX(vbox1), frame3, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame3), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(frame3),GTK_SHADOW_NONE);
	
	label1 = gtk_label_new(NULL); 
	
	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Search Options"));
  	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);
	
	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(frame3), label1);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(frame3), vbox3);
	gtk_container_set_border_width (GTK_CONTAINER (vbox3), 4);

	ckbCaseSensitive =
	    gtk_check_button_new_with_label(_("Match case"));
	gtk_widget_show(ckbCaseSensitive);
	gtk_box_pack_start(GTK_BOX(vbox3), ckbCaseSensitive, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(ckbCaseSensitive, -1, 20);


	frame4 = gtk_frame_new(NULL);
	gtk_widget_show(frame4);
	gtk_box_pack_start(GTK_BOX(vbox1), frame4, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame4), 2);
	gtk_frame_set_shadow_type(GTK_FRAME(frame4),GTK_SHADOW_NONE);
	
	label1 = gtk_label_new(NULL); 

	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Search Scope"));
  	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(frame4),label1);

	vbox4 = gtk_vbox_new(TRUE, 0);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame4), vbox4);
	gtk_container_set_border_width (GTK_CONTAINER (vbox4), 4);

	rbNoScope =
	    gtk_radio_button_new_with_label(NULL, _("No scope"));
	gtk_widget_show(rbNoScope);
	gtk_box_pack_start(GTK_BOX(vbox4), rbNoScope, FALSE, FALSE, 0);
	gtk_widget_set_size_request(rbNoScope, -1, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbNoScope),
				     TRUE);

	rrbUseBounds =
	    gtk_radio_button_new_with_label_from_widget(
	    				GTK_RADIO_BUTTON(rbNoScope),
					_("Use bounds"));
	gtk_widget_show(rrbUseBounds);
	gtk_box_pack_start(GTK_BOX(vbox4), rrbUseBounds, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(rrbUseBounds, -1, 20);

	rbLastSearch =
	    gtk_radio_button_new_with_label_from_widget(
	    				GTK_RADIO_BUTTON(rbNoScope),
					_("Last search"));
	gtk_widget_show(rbLastSearch);
	gtk_box_pack_start(GTK_BOX(vbox4), rbLastSearch, FALSE,
			   FALSE, 0);
	gtk_widget_set_size_request(rbLastSearch, -1, 20);

	frame5 = gtk_frame_new(NULL);
	gtk_widget_show(frame5);
	gtk_box_pack_start(GTK_BOX(vbox1), frame5, FALSE, TRUE, 0);
	gtk_container_set_border_width (GTK_CONTAINER (frame5), 2);
	gtk_widget_hide(frame5);
	gtk_frame_set_shadow_type(GTK_FRAME(frame5),GTK_SHADOW_NONE);
	
	label1 = gtk_label_new(NULL); 

	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Bounds"));
  	gtk_label_set_markup(GTK_LABEL(label1), header);
	g_free(header);

	gtk_widget_show(label1);
	gtk_frame_set_label_widget(GTK_FRAME(frame5), label1);

	table1 = gtk_table_new(2, 2, FALSE);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(frame5), table1);
	gtk_table_set_row_spacings(GTK_TABLE(table1), 3);
	gtk_table_set_col_spacings(GTK_TABLE(table1), 3);
	gtk_container_set_border_width (GTK_CONTAINER (table1), 8);

	label1 = gtk_label_new(_("Lower"));
	gtk_widget_show(label1);
	gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label1), 1.0, 0.5);

	label2 = gtk_label_new(_("Upper"));
	gtk_widget_show(label2);
	gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label2), 1.0, 0.5);

	entryLower = gtk_entry_new();;
	gtk_widget_show(entryLower);
	gtk_table_attach(GTK_TABLE(table1), entryLower, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_size_request(entryLower, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryLower), _("Genesis"));

	entryUpper = gtk_entry_new();;
	gtk_widget_show(entryUpper);
	gtk_table_attach(GTK_TABLE(table1), entryUpper, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_size_request(entryUpper, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryUpper), _("Revelation"));
	

	g_signal_connect(GTK_OBJECT(rrbUseBounds),
			   "toggled",
			   G_CALLBACK(on_rrbUseBounds_toggled),
			   NULL);
	g_signal_connect(GTK_OBJECT(btnSearch), "clicked",
			   G_CALLBACK(on_search_botton_clicked), NULL);
			   
       g_signal_connect(GTK_OBJECT(entrySearch), "activate",
                          G_CALLBACK(on_search_botton_clicked), NULL);
			   
			   
	gtk_object_set_data(GTK_OBJECT(widgets.app), "tooltips",
			    tooltips);
}
