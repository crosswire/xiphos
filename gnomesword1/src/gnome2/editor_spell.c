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

#ifdef USE_SPELL

#include <glib.h>
#include <gnome.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlengine-edit-movement.h>
#include <gtkhtml/htmlengine-edit-cursor.h>
#include <gtkhtml/htmlengine-search.h>
#include <gtkhtml/htmlselection.h>
#ifdef USE_GTKHTML30
#include <gal/widgets/e-unicode.h>
#endif
 
#include "gui/editor.h"
#include "gui/editor_spell.h"

#include "main/spell.h"
#include "main/settings.h"
#include "main/xml.h"


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
	GtkWidget *language_frame;
	GtkWidget *language_combo;
	GtkWidget *language_entry;
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
	GtkWidget *near_misses_list;
	GtkWidget *near_misses_scrolled_window;
	gint status_bar_count;
} Tspc_gui;



static Tspc_gui spc_gui;
static gboolean spc_is_running;
static gint spc_message; /* message from the spell checker GUI */


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
 *   void correct_word()
 *
 * Description
 *   ask the user how to correct a word, and do it
 *
 * Return value
 *   void
 */

static void correct_word(const gchar * word, GSHTMLEditorControlData * ecd)
{
	gboolean word_corrected = 0;
	gchar *language = NULL;
	gint i = 0;
	
#ifdef USE_GNOME_SPELL	
	spell_language = gtk_editable_get_chars(
					GTK_EDITABLE(spc_gui.language_entry),
                                        0,
                                        -1);	

	for (i = 0; i < ecd->languages->_length; i ++) {
		if(!strcmp(ecd->languages->_buffer [i].name, spell_language))
			language = ecd->languages->_buffer [i].abbreviation;
	}
#endif	
	select_word(ecd);
	do {
		spc_message = SPC_NONE;
		
		while (gtk_events_pending())
			gtk_main_iteration();
		
		switch (spc_message) {
		case SPC_INSERT:			
			add_to_personal(word,language);
			word_corrected = 1;
			break;
		case SPC_ACCEPT:
			add_to_session(word);
			change_word_color(ecd);;
			word_corrected = 1;
			break;
		case SPC_IGNORE:
		        change_word_color(ecd);
			word_corrected = 1;
			break;
		case SPC_REPLACE:{			
			const gchar *buf = gtk_entry_get_text(GTK_ENTRY
					     (spc_gui.replace_entry));
			store_replacement(word,buf,language);
			html_engine_replace_spell_word_with(
					ecd->html->engine,
					buf);			
			word_corrected = 1;
			ecd->changed = TRUE;
			gui_update_statusbar(ecd);
			break;
		}
		case SPC_CLOSE:
			change_word_color(ecd);
			word_corrected = 1;	/* to exit the while loop */
			break;
		}
	} while (!word_corrected);
};


/******************************************************************************
 * Name
 *   fill_near_misses_list
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void fill_near_misses_list(const gchar * word)
 *
 * Description
 *   get a list of suggestions for a mispelling
 *   and put them in the clist
 *
 * Return value
 *   void
 */

static void fill_near_misses_list(const gchar * word)
{
	gchar * buf;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	GList * tmp = get_suggest_list(word);
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(spc_gui.near_misses_list));
	list_store = GTK_LIST_STORE(model);
	gtk_list_store_clear(list_store);
	
	tmp = g_list_first(tmp);
        while ( tmp != NULL ) {
		buf = (gchar*)tmp->data;
		gtk_list_store_append(list_store, &iter);
		gtk_list_store_set(list_store, &iter, 0,buf, -1);		
		g_free(buf);
		tmp = g_list_next(tmp);
        } 
	g_list_free(tmp);
	
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

inline static void update_progress_bar(gint end, gint current)
{
	gfloat percentage;
	gfloat current_position;
	gfloat text_length;	
	
	current_position = (gfloat)current;
	text_length = (gfloat)end;	
	
	/* percentage is betweeen 0 and 1 */
	percentage = current_position / text_length;
	if(percentage > 1) percentage = 1.0;
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
	gint have, end, current;
	const gchar *utf8str = NULL;
	gchar *buf = NULL;
	GtkTreeModel *model;
	GtkListStore *list_store;
	GtkTreeIter iter;
	
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(spc_gui.near_misses_list));
	list_store = GTK_LIST_STORE(model);	

	html_engine_edit_cursor_position_save(ecd->html->engine);
	html_engine_end_of_document(ecd->html->engine); 
	end = html_cursor_get_position(ecd->html->engine->cursor); 
	html_engine_beginning_of_document (ecd->html->engine);
	
	while(html_engine_forward_word (ecd->html->engine)){
		if(html_engine_spell_word_is_valid (ecd->html->engine)) {
			buf = html_engine_get_spell_word(ecd->html->engine);
			utf8str = buf;
			if(utf8str)	{
				if (strlen(utf8str) > 2) {
					word_count++;
					spc_message = SPC_NONE;
					have = check_word_spell(utf8str);
					if (have == 0){
						fill_near_misses_list(utf8str);
						correct_word(utf8str, ecd);
						
						gtk_list_store_clear(list_store);
						
						gtk_entry_set_text(GTK_ENTRY(
						       spc_gui.word_entry), "");
						
						gtk_entry_set_text(GTK_ENTRY(
						    spc_gui.replace_entry), "");
					}
					if (spc_message == SPC_CLOSE) {
						spc_is_running = FALSE;
						gtk_widget_set_sensitive(
						       spc_gui.close_button, 1);
						
						break;
					}; 
				}
				utf8str = NULL;
			}
			
		}
		current = html_cursor_get_position(ecd->html->engine->cursor); 
		update_progress_bar(end, current);
	}
	gtk_widget_set_sensitive(spc_gui.accept_button, 0);
	gtk_widget_set_sensitive(spc_gui.replace_button, 0);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 0);
	gtk_widget_set_sensitive(spc_gui.insert_button, 0);
	html_engine_unselect_all(ecd->html->engine);
	html_engine_deactivate_selection (ecd->html->engine);
	html_engine_edit_cursor_position_restore(ecd->html->engine);

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
	gint i = 0;
	gboolean enabled;
	
	//gtk_widget_set_sensitive(spc_gui.start_button, 0);
	gtk_widget_set_sensitive(spc_gui.close_button, 0);
	
	gtk_widget_set_sensitive(spc_gui.accept_button, 1);
	gtk_widget_set_sensitive(spc_gui.replace_button, 1);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 1);
	gtk_widget_set_sensitive(spc_gui.insert_button, 1);
	
#ifdef USE_GNOME_SPELL	
	spell_language = gtk_editable_get_chars(
					GTK_EDITABLE(spc_gui.language_entry),
                                        0,
                                        -1);
	
	for (i = 0; i < ecd->languages->_length; i ++) {
		if(!strcmp(ecd->languages->_buffer [i].name, spell_language))
			set_dictionary_language(
				ecd->languages->_buffer [i].abbreviation);
	}
#endif	
	
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
	if (!spc_is_running){
		gtk_widget_destroy(spc_gui.window);		
		kill_spell();
	}
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
 *   on_cbe_freeform_lookup_changed
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   void on_cbe_freeform_lookup_changed(GtkEditable * editable, gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static
void on_language_entry_changed(GtkEditable * editable, gpointer user_data)
{
	gchar *buf = NULL;
	
	buf = gtk_editable_get_chars(editable,0,-1);
	xml_set_value("GnomeSword", "editor", "spell_language", buf);
	settings.spell_language = xml_get_value("editor", "spell_language");
}


/******************************************************************************
 * Name
 *  list_button_released
 *
 * Synopsis
 *   #include "gui/editor_spell.h"
 *
 *   gint list_button_released(GtkWidget * html, GdkEventButton * event, NULL)	
 *
 * Description
 *    mouse button released in key list
 *
 * Return value
 *   gint
 */
static gint list_button_released(GtkWidget * widget, GdkEventButton * event, 
							gpointer data)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	gchar *buf = NULL;
	GtkTreeModel *model ;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(
						spc_gui.near_misses_list));
                        
	if (!gtk_tree_selection_get_selected(selection, &model, &selected))
		return;
	
	switch (event->button) {
	case 1:
		gtk_tree_model_get(model, &selected, 0, &buf, -1);
		if (buf) {
			gtk_entry_set_text(GTK_ENTRY(
					spc_gui.replace_entry), buf);
			g_free(buf);
		}	 	
		break;
	case 2:
	case 3:
	default:
		break;
	}
	
	return FALSE;
}

static void add_columns(GtkTreeView * treeview)
{
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkTreeModel *model = gtk_tree_view_get_model(treeview);

	renderer = gtk_cell_renderer_text_new();

	column = gtk_tree_view_column_new_with_attributes("Keys",
							  renderer,
							  "text", 0,
							  NULL);
	gtk_tree_view_column_set_sort_column_id(column, 0);

	gtk_tree_view_append_column(treeview, column);
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
	GList *tmp = NULL;
	GtkWidget *dialog_vbox;
	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *vbox;
	GtkWidget *button_alignment;
	GtkWidget *button_hbox;
	GtkWidget *button_image;
	GtkWidget *button_label;
	gchar *header;
	GtkListStore *model;
	GtkTooltips *tooltips;

	tooltips = gtk_tooltips_new ();
	
	spc_is_running = FALSE;
	spc_gui.window = gtk_dialog_new(); 
	gtk_window_set_title(GTK_WINDOW(spc_gui.window),
			     _("Spell Checker"));
        gtk_window_set_default_size (GTK_WINDOW (spc_gui.window), 300, 200);
	gtk_container_set_border_width(GTK_CONTAINER(spc_gui.window), 12);
 
	dialog_vbox = GTK_DIALOG(spc_gui.window)->vbox;
	gtk_object_set_data(GTK_OBJECT(dialog_vbox), "dialog_vbox",
			    dialog_vbox);
	gtk_widget_show(dialog_vbox);
	

	spc_gui.close_button = gtk_dialog_add_button(GTK_DIALOG(spc_gui.window),
				      GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE);
	/*spc_gui.start_button = gtk_dialog_add_button(GTK_DIALOG(spc_gui.window),
				      GTK_STOCK_SPELL_CHECK, 101);*/
				      
	spc_gui.vbox = gtk_vbox_new(FALSE, 6);
	gtk_widget_show(spc_gui.vbox);
	gtk_box_pack_start(GTK_BOX(dialog_vbox), spc_gui.vbox, TRUE, TRUE, 0);


#ifdef USE_GNOME_SPELL	
	spc_gui.language_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(spc_gui.language_frame),
					GTK_SHADOW_NONE);	
	
	label = gtk_label_new(NULL);
	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Dictionary"));
  	gtk_label_set_markup(GTK_LABEL(label), header);
	g_free(header);
	gtk_widget_show(label);
	gtk_frame_set_label_widget(GTK_FRAME(spc_gui.language_frame),label);
	
	gtk_widget_show(spc_gui.language_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.language_frame, TRUE,
			   TRUE, 0);
			   
	spc_gui.language_combo = gtk_combo_new();
	gtk_widget_show(spc_gui.language_combo);
	gtk_container_add(GTK_CONTAINER(spc_gui.language_frame),
			  spc_gui.language_combo);
	tmp = get_dictionary_languages(ecd);
	
	gtk_combo_set_popdown_strings(GTK_COMBO(spc_gui.language_combo),
				      tmp);
	g_list_free(tmp);
	
	spc_gui.language_entry = GTK_COMBO(spc_gui.language_combo)->entry;
	gtk_widget_show(spc_gui.language_entry);
	if(strcmp(settings.spell_language, "unknown"))
		gtk_entry_set_text(GTK_ENTRY(spc_gui.language_entry),
                                            settings.spell_language);
  	g_signal_connect ((gpointer) spc_gui.language_entry, "changed",
                    G_CALLBACK (on_language_entry_changed),
                    NULL);
#endif	
	
	spc_gui.word_frame = gtk_frame_new(NULL);	
	gtk_frame_set_shadow_type(GTK_FRAME(spc_gui.word_frame),
					GTK_SHADOW_NONE);	
	
	label = gtk_label_new(NULL);
	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Word found"));
  	gtk_label_set_markup(GTK_LABEL(label), header);
	g_free(header);
	gtk_widget_show(label);
	gtk_frame_set_label_widget(GTK_FRAME(spc_gui.word_frame),label);
	
	gtk_widget_show(spc_gui.word_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.word_frame, TRUE,
			   TRUE, 0);

	spc_gui.word_entry = gtk_entry_new();
	gtk_entry_set_editable(GTK_ENTRY(spc_gui.word_entry), 0);
	gtk_widget_show(spc_gui.word_entry);
	gtk_container_add(GTK_CONTAINER(spc_gui.word_frame),
			  spc_gui.word_entry);

	spc_gui.replace_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(spc_gui.replace_frame),
					GTK_SHADOW_NONE);	
	
	label = gtk_label_new(NULL);
	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Replace with"));
  	gtk_label_set_markup(GTK_LABEL(label), header);
	g_free(header);
	gtk_widget_show(label);
	gtk_frame_set_label_widget(GTK_FRAME(spc_gui.replace_frame),label);
	gtk_widget_show(spc_gui.replace_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.replace_frame,
			   FALSE, FALSE, 0);

	spc_gui.replace_entry = gtk_entry_new();
	gtk_widget_show(spc_gui.replace_entry);
	gtk_container_add(GTK_CONTAINER(spc_gui.replace_frame),
			  spc_gui.replace_entry);

	hbox = gtk_hbox_new(FALSE, 2);
	gtk_widget_show(hbox);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), hbox, TRUE, TRUE, 0);
	
	spc_gui.near_misses_scrolled_window =
	    gtk_scrolled_window_new(NULL, NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (spc_gui.
					near_misses_scrolled_window),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)spc_gui.near_misses_scrolled_window,
                                             settings.shadow_type);
	gtk_widget_show(spc_gui.near_misses_scrolled_window);
	gtk_box_pack_start(GTK_BOX(hbox),
			   spc_gui.near_misses_scrolled_window, TRUE, TRUE,
			   0);
		
	/* create tree model */
	model = gtk_list_store_new(1, G_TYPE_STRING);
	
	spc_gui.near_misses_list = 
			gtk_tree_view_new_with_model(GTK_TREE_MODEL(model));
	gtk_tree_view_set_rules_hint(GTK_TREE_VIEW(spc_gui.near_misses_list), 
					TRUE);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(
					spc_gui.near_misses_list),
					FALSE);
	gtk_widget_set_size_request(spc_gui.near_misses_list, -1, 40);
	gtk_widget_show(spc_gui.near_misses_list);
	gtk_container_add(GTK_CONTAINER
			  (spc_gui.near_misses_scrolled_window),
			  spc_gui.near_misses_list);
	add_columns(GTK_TREE_VIEW(spc_gui.near_misses_list));

	
	vbox = gtk_vbox_new(TRUE, 2);
	gtk_widget_show(vbox);
	gtk_box_pack_start(GTK_BOX(hbox), vbox, FALSE, FALSE, 0);
	
	/* replace_button */
	spc_gui.replace_button =
	    gtk_button_new_with_label(NULL);
	GTK_WIDGET_SET_FLAGS(spc_gui.replace_button, GTK_CAN_DEFAULT);
	gtk_widget_set_sensitive(spc_gui.replace_button, 0);
	gtk_widget_show (spc_gui.replace_button);
	gtk_box_pack_start(GTK_BOX(vbox), spc_gui.replace_button, 
						FALSE, FALSE, 0);
	gtk_tooltips_set_tip (tooltips, spc_gui.replace_button, 
		_("Replace misspelled word with suggested word"), NULL);
	
	button_alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (button_alignment);
	gtk_container_add (GTK_CONTAINER (spc_gui.replace_button), button_alignment);
	
	button_hbox = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (button_hbox);
	gtk_container_add (GTK_CONTAINER (button_alignment), button_hbox);
	
	button_image = gtk_image_new_from_stock ("gtk-convert", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (button_image);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_image, FALSE, FALSE, 0);
	
	button_label = gtk_label_new_with_mnemonic (_("Replace word"));
	gtk_widget_show (button_label);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (button_label), GTK_JUSTIFY_LEFT);

	/* ignore_button */
	spc_gui.ignore_button =
	    gtk_button_new_with_label(NULL);
	GTK_WIDGET_SET_FLAGS(spc_gui.ignore_button, GTK_CAN_DEFAULT);
	gtk_widget_show(spc_gui.ignore_button);
	gtk_widget_set_sensitive(spc_gui.ignore_button, 0);
	gtk_box_pack_start(GTK_BOX(vbox), spc_gui.ignore_button, 
						FALSE, FALSE, 0);
	gtk_tooltips_set_tip (tooltips, spc_gui.ignore_button, 
					_("Leave word as is"), NULL);
	
	button_alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (button_alignment);
	gtk_container_add (GTK_CONTAINER (spc_gui.ignore_button), button_alignment);
	
	button_hbox = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (button_hbox);
	gtk_container_add (GTK_CONTAINER (button_alignment), button_hbox);
	
	button_image = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (button_image);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_image, FALSE, FALSE, 0);
	
	button_label = gtk_label_new_with_mnemonic (_("Skip Word"));
	gtk_widget_show (button_label);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (button_label), GTK_JUSTIFY_LEFT);

	/* accept_button */
	spc_gui.accept_button = gtk_button_new_with_label(NULL);
	GTK_WIDGET_SET_FLAGS(spc_gui.accept_button, GTK_CAN_DEFAULT);
	gtk_widget_show(spc_gui.accept_button);
	gtk_widget_set_sensitive(spc_gui.accept_button, 0);
	gtk_box_pack_start(GTK_BOX(vbox), spc_gui.accept_button, 
						FALSE, FALSE, 0);
	gtk_tooltips_set_tip (tooltips, spc_gui.accept_button, 
	  _("Accept word for this session but do not add to dictionary"), NULL);

		
	button_alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (button_alignment);
	gtk_container_add (GTK_CONTAINER (spc_gui.accept_button), button_alignment);
	
	button_hbox = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (button_hbox);
	gtk_container_add (GTK_CONTAINER (button_alignment), button_hbox);
	
	button_image = gtk_image_new_from_stock ("gtk-clear", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (button_image);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_image, FALSE, FALSE, 0);
	
	button_label = gtk_label_new_with_mnemonic (_("Accept word"));
	gtk_widget_show (button_label);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (button_label), GTK_JUSTIFY_LEFT);

	/* insert_button */
	spc_gui.insert_button =
	    gtk_button_new_with_label(NULL);
	GTK_WIDGET_SET_FLAGS(spc_gui.insert_button, GTK_CAN_DEFAULT);
	gtk_widget_show(spc_gui.insert_button);
	gtk_widget_set_sensitive(spc_gui.insert_button, 0);
	gtk_box_pack_start(GTK_BOX(vbox), spc_gui.insert_button, 
						FALSE, FALSE, 0);
	gtk_tooltips_set_tip (tooltips, spc_gui.insert_button, 
			_("Insert word into Personal Dictionary"), NULL);
	
	button_alignment = gtk_alignment_new (0.5, 0.5, 0, 0);
	gtk_widget_show (button_alignment);
	gtk_container_add (GTK_CONTAINER (spc_gui.insert_button), button_alignment);
	
	button_hbox = gtk_hbox_new (FALSE, 2);
	gtk_widget_show (button_hbox);
	gtk_container_add (GTK_CONTAINER (button_alignment), button_hbox);
	
	button_image = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_BUTTON);
	gtk_widget_show (button_image);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_image, FALSE, FALSE, 0);
	
	button_label = gtk_label_new_with_mnemonic (_("Insert word"));
	gtk_widget_show (button_label);
	gtk_box_pack_start (GTK_BOX (button_hbox), button_label, FALSE, FALSE, 0);
	gtk_label_set_justify (GTK_LABEL (button_label), GTK_JUSTIFY_LEFT);
	
	
	spc_gui.progress_frame = gtk_frame_new(NULL);
	gtk_frame_set_shadow_type(GTK_FRAME(spc_gui.progress_frame),
					GTK_SHADOW_NONE);	
	
	label = gtk_label_new(NULL);
	header = g_strdup_printf("<span weight=\"bold\">%s</span>",_("Progress"));
  	gtk_label_set_markup(GTK_LABEL(label), header);
	g_free(header);
	gtk_widget_show(label);
	gtk_frame_set_label_widget(GTK_FRAME(spc_gui.progress_frame),label);
	gtk_widget_show(spc_gui.progress_frame);
	gtk_box_pack_start(GTK_BOX(spc_gui.vbox), spc_gui.progress_frame,
			   TRUE, TRUE, 0);

	spc_gui.progress_bar = gtk_progress_bar_new();
	gtk_widget_show(spc_gui.progress_bar);
	gtk_container_add(GTK_CONTAINER(spc_gui.progress_frame),
			  spc_gui.progress_bar);

	

	g_signal_connect(GTK_OBJECT(spc_gui.window), "delete_event",
			   G_CALLBACK(delete_event_lcb), NULL);
	g_signal_connect(GTK_OBJECT(spc_gui.close_button), "clicked",
			   G_CALLBACK(spc_close_button_clicked_lcb),
			   ecd);
	g_signal_connect(GTK_OBJECT(spc_gui.accept_button), "clicked",
			   G_CALLBACK(on_spc_accept_button_clicked),
			   ecd);
	g_signal_connect(GTK_OBJECT(spc_gui.insert_button), "clicked",
			   G_CALLBACK(on_spc_insert_button_clicked),
			   ecd);
	g_signal_connect(GTK_OBJECT(spc_gui.ignore_button), "clicked",
			   G_CALLBACK(on_spc_ignore_button_clicked),
			   ecd);
	g_signal_connect(GTK_OBJECT(spc_gui.replace_button), "clicked",
			   G_CALLBACK(on_spc_replace_button_clicked),
			   ecd);
	g_signal_connect(G_OBJECT(spc_gui.near_misses_list),
			   "button_release_event",
			   G_CALLBACK(list_button_released), NULL);
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
	if(init_spell() == -1) {
		return;
	}	
	spc_gui.window = create_spc_window(ecd);	
	gtk_widget_show(spc_gui.window);
	spc_start_button_clicked_lcb(NULL, ecd);
}


#endif
