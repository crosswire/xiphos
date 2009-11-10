/* 
 * Xiphos Bible Study Tool
 * export_bookmarks.c -
 *
 * Copyright (C) 2003-2009 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <libxml/parser.h>
#include <glade/glade-xml.h>

#include "gui/export_bookmarks.h"
#include "gui/xiphos.h"
#include "gui/bookmarks_menu.h"
#include "gui/bookmarks_treeview.h"
#include "gui/utilities.h"


#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"
//#include "main/module_dialogs.h"
#include "main/url.hh"


enum {
	BOOKMARKS,
	HTML,
	PLAIN
};

void dialog_export_bookmarks_response_cb (  GtkDialog * dialog,
								gint response_id,
								BK_EXPORT * data);
void checkbutton_include_text_toggled_cb(GtkToggleButton * togglebutton,
								BK_EXPORT * data);
 void radiobutton_plain_text_toggled_cb(GtkToggleButton * togglebutton,
								BK_EXPORT * data);
 void radiobutton_xiphos_bookmarks_toggled_cb(GtkToggleButton * togglebutton,
								BK_EXPORT * data);
void radiobutton_html_toggled_cb(GtkToggleButton * togglebutton,
								BK_EXPORT * data);


/******************************************************************************
 * Name
 *  _parse_treeview_html
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void _parse_treeview_html (xmlNodePtr parent, GtkTreeIter * tree_parent)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */
static
void _parse_treeview (GString *str, GtkTreeIter * tree_parent, 
                           GtkTreeStore *model, BK_EXPORT *data)
{
	GtkTreeIter child;
	gchar *caption = NULL;
	gchar *key = NULL;
	gchar *module = NULL;
	gchar *mod_desc = NULL;
	gchar *description = NULL;
	gchar *buf = NULL;	
	static gchar *tab = "";
	
	gtk_tree_model_iter_children(GTK_TREE_MODEL(model), &child,
                                             tree_parent);
	
	do {
		gtk_tree_model_get(GTK_TREE_MODEL(model), &child,
			   		2, &caption, 
					3, &key, 
					4, &module, 
					5, &mod_desc, 
					6, &description,
					-1);
		if(gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model), &child)) {
			tab = "\t";
			switch (data->type) {
				case HTML:
					buf = g_strdup_printf("<ul><b>%s</b>",caption);
				break;
				case PLAIN:
					buf = g_strdup_printf("%s%s\n",tab,caption);
				break;
			}
			g_string_append(str,buf);
			g_free(buf);
			_parse_treeview(str, &child, model, data);
						     
		} 
		if ( key ) {
			switch (data->type) {
				case HTML:
					buf = g_strdup_printf("<li>%s &nbsp;&nbsp;%s &nbsp;&nbsp;%s",description,module,key);
				break;
				case PLAIN:
					buf = g_strdup_printf("%s%s   %s  %s\n",tab,description,module,key);
				break;
			}
		} else { 
			switch (data->type) {
				case HTML:
					buf = g_strdup_printf("</ul>");
				break;
				case PLAIN:
					tab = "";
					buf = g_strdup_printf(" ");
				break;
			}
		}
				
		g_string_append(str,buf);
		g_free(buf);
		
		g_free(caption);
		g_free(key);
		g_free(module);	
		g_free(mod_desc);
		g_free(description);
	} while(gtk_tree_model_iter_next(GTK_TREE_MODEL(model), &child));
}


/******************************************************************************
 * Name
 *   save_iter_to_xml
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_iter_to_xml(GtkTreeIter * iter)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void save_iter_to_xml(GtkTreeIter * iter, BK_EXPORT *data)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	xmlAttrPtr root_attr;
	gchar *caption = NULL;
	gchar filename[256];
	
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
	sprintf(filename, "%s.xml", data->filename);
	
	root_doc = xmlNewDoc((const xmlChar *) "1.0");
	
	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)
				       "SwordBookmarks");
		root_attr =
		    xmlNewProp(root_node, (const xmlChar *)"syntaxVersion",
			       (const xmlChar *) "1");
		xmlDocSetRootElement(root_doc, root_node);
	}
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
		if( gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),iter)) {
			cur_node = xml_add_folder_to_parent(root_node, caption);
			utilities_parse_treeview(cur_node, iter, model);
		}
	g_free(caption);
	xmlSaveFormatFile(filename, root_doc,1);
	xmlFreeDoc(root_doc);
	
	bookmarks_changed = FALSE;
}

/******************************************************************************
 * Name
 *   _save_iter
 *
 * Synopsis
 *   #include "gui/bookmarks_menu.h"
 *
 *   void save_iter_to_html(GtkTreeIter * iter, BK_EXPORT *data)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void _save_iter(GtkTreeIter * iter, BK_EXPORT *data)
{
	gchar *caption = NULL;
	gchar filename[256];
	GString *str = g_string_new(NULL);
	
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
	switch (data->type) {
		case HTML:
			sprintf(filename, "%s.html", data->filename);
		break;
		case PLAIN:
			sprintf(filename, "%s.txt", data->filename);
		break;
	}
	
	gtk_tree_model_get(GTK_TREE_MODEL(model), iter,
			   2, &caption, -1);
		if( gtk_tree_model_iter_has_child(GTK_TREE_MODEL(model),iter)) {
			switch (data->type) {
				case HTML:
					g_string_printf(str, "<html><body><ul><b>%s</b>", caption);
				break;
				case PLAIN:
					g_string_printf(str, "%s\n", caption);
				break;
			}
			
			_parse_treeview (str, iter, model, data);
		}
	
	g_free(caption);
	switch (data->type) {
		case HTML:
			g_string_append(str,"</ul></body></html>");
		break;
		case PLAIN:
			g_string_append(str,"\n");
		break;
	}
	
	/* not sure how well this will work with windows */
	if (!g_file_set_contents (filename, str->str, str->len, NULL))
		GS_warning(("Unable to save %s", filename));
	
	g_string_free(str,1);
}


static void export_2_bookmarks(BK_EXPORT *data)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	gchar buf[256];
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	sprintf(buf, "%s.xml", data->filename);

	save_iter_to_xml(&selected, data);
}

static void export_2_html(BK_EXPORT *data)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	gchar buf[256];
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	sprintf(buf, "%s.html", data->filename);
	_save_iter(&selected, data);

}

static void export_2_text(BK_EXPORT *data)
{
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	gchar buf[256];
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	sprintf(buf, "%s.txt", data->filename);
	_save_iter(&selected, data);

}


static void _export (BK_EXPORT *data)
{ 
	switch (data->type) {
	case BOOKMARKS:
		export_2_bookmarks(data);
		break;
	case HTML:
		export_2_html(data);
		break;
	case PLAIN:
		export_2_text(data);
		break;

	}
}


static void setup_filechooserwidget (GtkFileChooser * chooser)
{ 
	GtkTreeSelection* selection;
	GtkTreeIter selected;
	gchar *caption = NULL;
	
	selection = gtk_tree_view_get_selection(bookmark_tree);
	if(!gtk_tree_selection_get_selected(selection, NULL, &selected)) 
		return;
	gtk_tree_model_get(GTK_TREE_MODEL(model), &selected,
				   2, &caption, -1);
	gtk_file_chooser_set_current_name (chooser, caption);
	
	g_free(caption);
}


G_MODULE_EXPORT void
checkbutton_include_text_toggled_cb(GtkToggleButton * togglebutton,
							     BK_EXPORT * data)
{
	data->with_scripture = togglebutton->active;
}

G_MODULE_EXPORT void
radiobutton_xiphos_bookmarks_toggled_cb(GtkToggleButton * togglebutton,
							     BK_EXPORT * data)
{
	if (togglebutton->active) 
		data->type = BOOKMARKS;
}

G_MODULE_EXPORT void
radiobutton_html_toggled_cb(GtkToggleButton * togglebutton,
							     BK_EXPORT * data)
{
	if (togglebutton->active) 
		data->type = HTML;
}


G_MODULE_EXPORT void
radiobutton_plain_text_toggled_cb(GtkToggleButton * togglebutton,
							     BK_EXPORT * data)
{
	if (togglebutton->active) 
		data->type = PLAIN;
}


G_MODULE_EXPORT
void dialog_export_bookmarks_response_cb (  GtkDialog * dialog,
									   gint response_id,
									   BK_EXPORT * data)
{
	switch (response_id) {
	case GTK_RESPONSE_CANCEL:
		break;
	case GTK_RESPONSE_OK:
		data->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (data->filechooserwidget));
		_export(data);
		break;
	}
	if (data->filename)
		g_free(data->filename);
	g_free(data);
	gtk_widget_destroy(GTK_WIDGET(dialog));
}

void gui_export_bookmarks_dialog(void)
{
        GtkBuilder *builder;
	gchar *gbuilder_file;
	GtkWidget *dialog;
	BK_EXPORT *ex_data = g_new0(BK_EXPORT,1);
	
	ex_data->filename = NULL;
	ex_data->type = BOOKMARKS;
	builder = gtk_builder_new ();
	gbuilder_file = gui_general_user_file ("xi-export-bookmarks.glade", FALSE);
        gtk_builder_add_from_file (builder, gbuilder_file, NULL);

        dialog = GTK_WIDGET (gtk_builder_get_object (builder, "dialog_export_bookmarks"));
        ex_data->rb_bookmarks = GTK_WIDGET (gtk_builder_get_object (builder, "radiobutton_xiphos_bookmarks"));
        ex_data->rb_html = GTK_WIDGET (gtk_builder_get_object (builder, "radiobutton_html"));
        ex_data->rb_plain = GTK_WIDGET (gtk_builder_get_object (builder, "radiobutton_plain_text"));
        ex_data->cb_scripture = GTK_WIDGET (gtk_builder_get_object (builder, "checkbutton_include_text"));
	ex_data->filechooserwidget = GTK_WIDGET (gtk_builder_get_object (builder, "filechooserwidget"));
	setup_filechooserwidget ((GtkFileChooser *) ex_data->filechooserwidget);
        gtk_builder_connect_signals (builder,ex_data); 
	gtk_widget_hide(ex_data->cb_scripture);
        g_object_unref (G_OBJECT (builder));        
        gtk_widget_show (dialog); 
}


