/*
 * GnomeSword Bible Study Tool
 * editor_spell.c - spell checker for editors
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
#  include "config.h"
#endif

#ifdef USE_PSPELL

#include <glib.h>
#include <gtk/gtk.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlengine-edit-movement.h>
#include <gtkhtml/htmlengine-edit-cursor.h>
#include <gtkhtml/htmlengine-search.h>
#include <gtkhtml/htmlselection.h>
#include <gal/widgets/e-unicode.h>



#include <pspell/pspell.h>

#include "gui/_editor.h"
#include "gui/editor_spell.h"

/* messages to the spell checker */
enum {
        SPC_NONE,
        SPC_CLOSE,
        SPC_ACCEPT,
        SPC_IGNORE,
        SPC_INSERT,
        SPC_REPLACE
};

typedef struct {
	GtkWidget *window;
	GtkWidget *vbox;
	GtkWidget *scrollwindow;
	GtkWidget *hbuttonbox_top;
	GtkWidget *start_button;
	GtkWidget *close_button;
	GtkWidget *options_button;
	GtkWidget *word_frame;
	GtkWidget *word_entry;
	GtkWidget *replace_frame;
	GtkWidget *replace_entry;
	GtkWidget *progress_frame;
	GtkWidget *progress_bar;
	GtkWidget *button_table;
	GtkWidget *accept_button;
	GtkWidget *insert_button;
	GtkWidget *ignore_button;
	GtkWidget *replace_button;
	GtkWidget *near_misses_clist;
	GtkWidget *near_misses_scrolled_window;
	gint status_bar_count;
} Tspc_gui;

static Tspc_gui spc_gui;
static gboolean spc_is_running;
static gint spc_message; /* message from the spell checker GUI */


/******************************************************************************
 * Name
 *   check_for_error
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void check_for_error(PspellManager * spell_checker)
 *
 * Description
 *   check for pspell error and print it if found
 *
 * Return value
 *   void
 */

static void check_for_error(PspellManager * spell_checker)
{
	if (pspell_manager_error_number(spell_checker) != 0) {
		g_warning("Pspell Error: %s\n", 
			pspell_manager_error_message(spell_checker));
	}
}


/******************************************************************************
 * Name
 *   select_word
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void select_word(GSHTMLEditorControlData * ecd)
 *
 * Description
 *   select (mark) misspelled word
 *
 * Return value
 *   void
 */

static inline void select_word(GSHTMLEditorControlData * ecd)
{	
	gtk_html_select_word(ecd->html);	
}


/******************************************************************************
 * Name
 *   change_word_color
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void change_word_color(GSHTMLEditorControlData * ecd)
 *
 * Description
 *   change slected word color back to original color
 *
 * Return value
 *   void
 */

static void change_word_color(GSHTMLEditorControlData * ecd)
{
	
	
}


/******************************************************************************
 * Name
 *   correct_word
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void correct_word(PspellManager * spell_checker, gchar * word,
 *					GSHTMLEditorControlData * ecd)
 *
 * Description
 *   ask the user how to correct a word, and do it
 *
 * Return value
 *   void
 */

static void correct_word(PspellManager * spell_checker, const gchar * word,
					GSHTMLEditorControlData * ecd)
{
	gboolean word_corrected = 0;
	gint done = 0;
	select_word(ecd);
	do {
		spc_message = SPC_NONE;
		
		while (gtk_events_pending())
			gtk_main_iteration();
		switch (spc_message) {
		case SPC_INSERT:			
			done = 
			   pspell_manager_add_to_personal(spell_checker,
						word, -1);
			if(!done) g_warning("word not added to personal");
			check_for_error(spell_checker);
			word_corrected = 1;
			break;
		case SPC_ACCEPT:
			pspell_manager_add_to_session(spell_checker, 
							word, -1);
			check_for_error(spell_checker);
			change_word_color(ecd);;
			word_corrected = 1;
			break;
		case SPC_IGNORE:
		        change_word_color(ecd);
			word_corrected = 1;
			break;
		case SPC_REPLACE:{			
			gchar *buf = gtk_entry_get_text(GTK_ENTRY
					     (spc_gui.replace_entry));
			pspell_manager_store_replacement(spell_checker,  
                                word, -1, 
                                buf, -1); 
			html_engine_replace_spell_word_with(
					ecd->html->engine,
					buf);			
			word_corrected = 1;
			ecd->changed = TRUE;
			update_statusbar(ecd);
			break;
		}
		case SPC_CLOSE:
			pspell_manager_save_all_word_lists(spell_checker); 
			pspell_manager_clear_session(spell_checker);
			change_word_color(ecd);
			word_corrected = 1;	/* to exit the while loop */
			break;
		}
	} while (!word_corrected);
};


/******************************************************************************
 * Name
 *   fill_near_misses_clist
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void fill_near_misses_clist(PspellManager * spell_checker, 
 *						const gchar * word)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void fill_near_misses_clist(PspellManager * spell_checker, 
						const gchar * word)
{
	const char * new_word; 
	gchar * buf;
	PspellWordList * suggestions;
	PspellStringEmulation * elements;
	
	suggestions = pspell_manager_suggest(spell_checker, word, -1);
        elements = pspell_word_list_elements(suggestions); 
        	
        while ( (new_word = pspell_string_emulation_next(elements)) 
							!= NULL ) {
		buf = g_strdup(new_word);
		gtk_clist_append(GTK_CLIST
			(spc_gui.near_misses_clist),
			 &buf);
		g_free(buf);
        } 
        delete_pspell_string_emulation(elements); 
	gtk_entry_set_text(GTK_ENTRY(spc_gui.word_entry),
				   word);
	gtk_entry_set_text(GTK_ENTRY(spc_gui.replace_entry),
				   word);	
	
}


/******************************************************************************
 * Name
 *   update_progress_bar
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void update_progress_bar(void)
 *
 * Description
 *   
 *
 * Return value
 *  void 
 */

inline static void update_progress_bar(void)
{
	gfloat percentage;
	gfloat current_position;
	gfloat text_length;
	
	/*
	current_position =
	    (gfloat)
	    gtk_text_get_point(GTK_TEXT(text_widget));
	text_length =
	    (gfloat)
	    gtk_text_get_length(GTK_TEXT(text_widget));
	*/
	
	/* percentage is betweeen 0 and 1 */
	percentage = current_position / text_length;
	gtk_progress_bar_update(GTK_PROGRESS_BAR(spc_gui.progress_bar),
				percentage);
};


/******************************************************************************
 * Name
 *   run_spell_checker
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   gboolean run_spell_checker(GSHTMLEditorControlData * ecd)
 *
 * Description
 *   Perform a pspell session
 *
 * Return value
 *   gboolean
 */

static gboolean run_spell_checker(GSHTMLEditorControlData * ecd)
{
	unsigned int word_count = 0;
	GdkColormap *cmap;
	PspellCanHaveError * ret;
	PspellManager * manager;
	PspellConfig * config;
	gint have;
	const gchar *utf8str = NULL;
	gchar *buf = NULL;
	
	
	config = new_pspell_config();
	pspell_config_replace(config, "language-tag", "en_US");  
	pspell_config_replace(config, "spelling", "american");
	pspell_config_replace(config, "encoding","utf-8");

	ret = new_pspell_manager(config);
	delete_pspell_config(config);
	
	if (pspell_error_number(ret) != 0) {
		g_warning("pspell error %s",pspell_error_message(ret));
		return -1;
	}
	
	manager = to_pspell_manager(ret);
	config = pspell_manager_config(manager);
	
	g_print("Using: \n");
	g_print("language tag \"%s\"\n",pspell_config_retrieve(config, "language-tag"));
	g_print("encoding \"%s\"\n",pspell_config_retrieve(config, "encoding"));
	g_print("spelling \"%s\"\n",pspell_config_retrieve(config, "spelling"));
	g_print("module \"%s\"\n",pspell_config_retrieve(config, "module"));
	g_print("path \"%s\"\n",pspell_config_retrieve(config, "master"));
	
	
	html_engine_edit_cursor_position_save(ecd->html->engine);
	cmap = gdk_colormap_get_system();
	
	html_engine_beginning_of_document (ecd->html->engine);
	while(html_engine_forward_word (ecd->html->engine)){
		if(html_engine_spell_word_is_valid (ecd->html->engine)) {
			buf = html_engine_get_spell_word(ecd->html->engine);
			utf8str = e_utf8_from_gtk_string(ecd->htmlwidget, buf);
			if(utf8str)	{
				if (strlen(utf8str) > 3) {
					word_count++;
					spc_message = SPC_NONE;
					have = pspell_manager_check(manager,  utf8str, strlen(utf8str));
					if (have == 0){
						fill_near_misses_clist(manager, utf8str);
						correct_word(manager, utf8str, ecd);
						gtk_clist_clear(GTK_CLIST(spc_gui.near_misses_clist));							
						gtk_entry_set_text(GTK_ENTRY(spc_gui.word_entry), "");
						gtk_entry_set_text(GTK_ENTRY(spc_gui.replace_entry), "");
					}
					else if (have == -1)
						g_warning("Error: %s", pspell_manager_error_message(manager));
					if (spc_message == SPC_CLOSE) {
						break;
					};
					//update_progress_bar();
				}
				utf8str = NULL;
			}
			
		}
	}
	gtk_widget_set_sensitive(spc_gui.accept_button, 0);
	gtk_widget_set_sensitive(spc_gui.replace_button, 0);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 0);
	gtk_widget_set_sensitive(spc_gui.insert_button, 0);
	html_engine_unselect_all(ecd->html->engine);
	html_engine_deactivate_selection (ecd->html->engine);
	html_engine_edit_cursor_position_restore(ecd->html->engine);
	delete_pspell_manager(manager);
	return (spc_message != SPC_CLOSE);
}	


/******************************************************************************
 * Name
 *   spc_start_button_clicked_lcb
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void spc_start_button_clicked_lcb(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
 *
 * Description
 *   start the spell check session
 *
 * Return value
 *   void
 */

static void spc_start_button_clicked_lcb(GtkButton * button, 
					GSHTMLEditorControlData * ecd)
{
	gtk_widget_set_sensitive(spc_gui.start_button, 0);
	gtk_widget_set_sensitive(spc_gui.options_button, 0);
	gtk_widget_set_sensitive(spc_gui.close_button, 0);
	
	gtk_widget_set_sensitive(spc_gui.accept_button, 1);
	gtk_widget_set_sensitive(spc_gui.replace_button, 1);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 1);
	gtk_widget_set_sensitive(spc_gui.insert_button, 1);

	spc_is_running = TRUE;
	if(run_spell_checker(ecd)) {
		spc_is_running = FALSE;
		gtk_widget_set_sensitive(spc_gui.close_button, 1);
	}
}


/******************************************************************************
 * Name
 *   spc_close_button_clicked_lcb
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void spc_close_button_clicked_lcb(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void spc_close_button_clicked_lcb(GtkButton * button, 
						gpointer user_data)
{
	spc_message = SPC_CLOSE;
	if (!spc_is_running)
		gtk_widget_destroy(spc_gui.window);
}


/******************************************************************************
 * Name
 *   on_spc_options_button_clicked
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void on_spc_options_button_clicked(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_spc_options_button_clicked(GtkButton * button, 
						gpointer user_data)
{
	
}


/******************************************************************************
 * Name
 *  on_spc_accept_button_clicked 
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void on_spc_accept_button_clicked(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_spc_accept_button_clicked(GtkButton * button, 
						gpointer user_data)
{
	spc_message = SPC_ACCEPT;	/* accept word */
}


/******************************************************************************
 * Name
 *   on_spc_insert_button_clicked
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void on_spc_insert_button_clicked(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_spc_insert_button_clicked(GtkButton * button, 
						gpointer user_data)
{
	spc_message = SPC_INSERT;
}


/******************************************************************************
 * Name
 *   on_spc_ignore_button_clicked
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void on_spc_ignore_button_clicked(GtkButton * button, 
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_spc_ignore_button_clicked(GtkButton * button, 
						gpointer user_data)
{
	spc_message = SPC_IGNORE;	/* ignore word */
}


/******************************************************************************
 * Name
 *   on_spc_replace_button_clicked
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void on_spc_replace_button_clicked(GtkButton * button,
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_spc_replace_button_clicked(GtkButton * button,
						gpointer user_data)
{
	spc_message = SPC_REPLACE;	/* replace word */
}


/******************************************************************************
 * Name
 *   delete_event_lcb
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   gboolean delete_event_lcb(GtkButton * button, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

static gboolean delete_event_lcb(GtkButton * button, gpointer user_data)
{
	spc_message = SPC_CLOSE;
	return (spc_is_running);	/* do not call the destroy event */
}


/******************************************************************************
 * Name
 *   on_near_misses_select_row_lcb
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void on_near_misses_select_row_lcb(GtkWidget * clist, gint row,
 *		gint column, GdkEventButton * event, gpointer data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_near_misses_select_row_lcb(GtkWidget * clist, gint row,
		gint column, GdkEventButton * event, gpointer data)
{
	gchar *text;
	gtk_clist_get_text(GTK_CLIST(clist), row, column, &text);
	gtk_entry_set_text(GTK_ENTRY(spc_gui.replace_entry), text);
}


/******************************************************************************
 * Name
 *   create_spc_window
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   GtkWidget *create_spc_window(GSHTMLEditorControlData *ecd)
 *
 * Description
 *   create the spell check dialog
 *
 * Return value
 *   GtkWidget *
 */

static GtkWidget *create_spc_window(GSHTMLEditorControlData *ecd)
{
	spc_is_running = FALSE;
	spc_gui.window = gtk_window_new(GTK_WINDOW_DIALOG);
	gtk_object_set_data(GTK_OBJECT(spc_gui.window), "spc_gui.window",
			    spc_gui.window);
	gtk_window_set_title(GTK_WINDOW(spc_gui.window),
			     _("GnomeSWORD Spell Checker"));
        gtk_window_set_default_size (GTK_WINDOW (spc_gui.window), 300, 200);

	spc_gui.vbox = gtk_vbox_new(FALSE, 0);
	/*  gtk_widget_ref (spc_gui.vbox);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_vbox", spc_gui.vbox,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.vbox);
	gtk_container_add(GTK_CONTAINER(spc_gui.window), spc_gui.vbox);
	
	spc_gui.hbuttonbox_top = gtk_hbutton_box_new();
	/* gtk_widget_ref (spc_gui.hbuttonbox_top);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "sp_hbuttonbox_top", spc_gui.hbuttonbox_top,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.hbuttonbox_top);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.hbuttonbox_top,
			   TRUE, TRUE, 0);

	spc_gui.start_button = gtk_button_new_with_label(_("Start"));
	/* gtk_widget_ref (spc_gui . start_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_start_button", spc_gui.start_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.start_button);
	gtk_container_add(GTK_CONTAINER(spc_gui.hbuttonbox_top),
			  spc_gui.start_button);
	GTK_WIDGET_SET_FLAGS(spc_gui.start_button, GTK_CAN_DEFAULT);

	spc_gui.close_button = gtk_button_new_with_label(_("Close"));
	/* gtk_widget_ref (spc_gui.close_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_close_button", spc_gui.close_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.close_button);
	gtk_container_add(GTK_CONTAINER(spc_gui.hbuttonbox_top),
			  spc_gui.close_button);
	GTK_WIDGET_SET_FLAGS(spc_gui.close_button, GTK_CAN_DEFAULT);

	spc_gui.options_button = gtk_button_new_with_label(_("Options"));
	/* gtk_widget_ref (spc_gui.options_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui. window), "spc_options_button", spc_gui.options_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.options_button);
	gtk_container_add(GTK_CONTAINER(spc_gui.hbuttonbox_top),
			  spc_gui.options_button);
	GTK_WIDGET_SET_FLAGS(spc_gui.options_button, GTK_CAN_DEFAULT);

	spc_gui.word_frame = gtk_frame_new(_("Word"));
	/* gtk_widget_ref (spc_gui.word_frame);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "sp_word_frame", spc_gui.word_frame,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.word_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.word_frame, TRUE,
			   TRUE, 0);

	spc_gui.word_entry = gtk_entry_new();
	/* gtk_widget_ref (spc_gui.word_entry);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_word_entry", spc_gui.word_entry,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_entry_set_editable(GTK_ENTRY(spc_gui.word_entry), 0);
	gtk_widget_show(spc_gui.word_entry);
	gtk_container_add(GTK_CONTAINER(spc_gui.word_frame),
			  spc_gui.word_entry);

	spc_gui.replace_frame = gtk_frame_new(_("Replace"));
	/* gtk_widget_ref (spc_gui.replace_frame);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_replace_frame", spc_gui.replace_frame,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.replace_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.replace_frame,
			   FALSE, FALSE, 0);

	spc_gui.replace_entry = gtk_entry_new();
	/* gtk_widget_ref (spc_gui.replace_entry);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "replace_combo", spc_gui.replace_entry,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.replace_entry);
	gtk_container_add(GTK_CONTAINER(spc_gui.replace_frame),
			  spc_gui.replace_entry);

	spc_gui.near_misses_scrolled_window =
	    gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (spc_gui.
					near_misses_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(spc_gui.near_misses_scrolled_window, 20, 100);
	/* gtk_widget_ref (spc_gui.near_misses_scrolled_window);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.near_misses_scrolled_window),
	   "spc_near_misses_scrolled_window", spc_gui.near_misses_scrolled_window,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.near_misses_scrolled_window);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox),
			   spc_gui.near_misses_scrolled_window, TRUE, TRUE,
			   0);

	spc_gui.near_misses_clist = gtk_clist_new(1);
	gtk_clist_set_selection_mode(GTK_CLIST(spc_gui.near_misses_clist),
				     GTK_SELECTION_SINGLE);
	/* gtk_widget_ref (spc_gui.near_misses_clist);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.near_misses_clist), "near_misses_clist", spc_gui.near_misses_clist,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.near_misses_clist);
	gtk_container_add(GTK_CONTAINER
			  (spc_gui.near_misses_scrolled_window),
			  spc_gui.near_misses_clist);

	spc_gui.progress_frame = gtk_frame_new(_("Progress"));
	/* gtk_widget_ref (spc_gui.progress_frame);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_progress_frame", spc_gui.progress_frame,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.progress_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.progress_frame,
			   TRUE, TRUE, 0);

	spc_gui.progress_bar = gtk_progress_bar_new();
	/* gtk_widget_ref (spc_gui.progress_bar);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_progress_bar", spc_gui.progress_bar,
	   (GtkDestroyNotify) gtk_widget_unref); */
	gtk_widget_show(spc_gui.progress_bar);
	gtk_container_add(GTK_CONTAINER(spc_gui.progress_frame),
			  spc_gui.progress_bar);

	spc_gui.accept_button =
	    gtk_button_new_with_label(_("Accept word\nin this section"));
	/* gtk_widget_ref (spc_gui.accept_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_accept_button", spc_gui.accept_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.accept_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.accept_button, 0);

	spc_gui.insert_button =
	    gtk_button_new_with_label(_("Insert in Personal\nDictionary"));
	/* gtk_widget_ref (spc_gui.insert_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_insert_button", spc_gui.insert_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.insert_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.insert_button, 0);

	spc_gui.ignore_button =
	    gtk_button_new_with_label(_("Ignore\nWord"));
	/* gtk_widget_ref (spc_gui.ignore_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_ignore_button", spc_gui.ignore_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.ignore_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 0);

	spc_gui.replace_button =
	    gtk_button_new_with_label(_("Replace\nWord"));
	/* gtk_widget_ref (spc_gui.replace_button);
	   gtk_object_set_data_full (GTK_OBJECT (spc_gui.window), "spc_replace_button", spc_gui.replace_button,
	   (GtkDestroyNotify) gtk_widget_unref); */
	GTK_WIDGET_SET_FLAGS(spc_gui.replace_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.replace_button, 0);

	spc_gui.button_table = gtk_table_new(2, 2, TRUE);
	gtk_widget_show(spc_gui.button_table);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.accept_button, 0, 1, 0, 1);
	gtk_widget_show(spc_gui.accept_button);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.replace_button, 1, 2, 0, 1);
	gtk_widget_show(spc_gui.replace_button);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.insert_button, 0, 1, 1, 2);
	gtk_widget_show(spc_gui.insert_button);
	gtk_table_attach_defaults(GTK_TABLE(spc_gui.button_table),
				  spc_gui.ignore_button, 1, 2, 1, 2);
	gtk_widget_show(spc_gui.ignore_button);
	gtk_widget_show(spc_gui.button_table);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.button_table,
			   TRUE, TRUE, 0);

	gtk_signal_connect(GTK_OBJECT(spc_gui.window), "delete_event",
			   GTK_SIGNAL_FUNC(delete_event_lcb), NULL);

	gtk_signal_connect(GTK_OBJECT(spc_gui.start_button), "clicked",
			   GTK_SIGNAL_FUNC(spc_start_button_clicked_lcb),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(spc_gui.close_button), "clicked",
			   GTK_SIGNAL_FUNC(spc_close_button_clicked_lcb),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(spc_gui.options_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_options_button_clicked),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(spc_gui.accept_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_accept_button_clicked),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(spc_gui.insert_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_insert_button_clicked),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(spc_gui.ignore_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_ignore_button_clicked),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(spc_gui.replace_button), "clicked",
			   GTK_SIGNAL_FUNC(on_spc_replace_button_clicked),
			   ecd);
	gtk_signal_connect(GTK_OBJECT(spc_gui.near_misses_clist),
			   "select_row",
			   GTK_SIGNAL_FUNC(on_near_misses_select_row_lcb),
			   ecd);
	return spc_gui.window;
}


/******************************************************************************
 * Name
 *   spell_check_cb
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void spell_check_cb(GtkWidget * w, GSHTMLEditorControlData *ecd)
 *
 * Description
 *   create the spell check dialog
 *
 * Return value
 *   void
 */

void spell_check_cb(GtkWidget * w, GSHTMLEditorControlData *ecd)
{
	spc_gui.window = create_spc_window(ecd);
	gtk_widget_set_sensitive(spc_gui.options_button, 0);	
	gtk_widget_show(spc_gui.window);	
}


#endif
