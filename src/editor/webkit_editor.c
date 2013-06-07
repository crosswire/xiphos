/*
 * Xiphos Bible Study Tool
 * webkit_editor.c - html editor using webkit
 *
 * Copyright (C) 2005-2013 Xiphos Developer Team
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

#include <config.h>

#ifdef USE_WEBKIT_EDITOR

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include <glib.h>
#include <webkit/webkit.h>


#include <libintl.h>
#include <locale.h>
#include <stdlib.h>
#include <glib/gi18n.h>
#include <gio/gio.h>
//#include <editor/gtkhtml-editor.h>
//#include <gtkhtml/gtkhtml-stream.h>

#include "editor/webkit_editor.h"
#include "editor/editor.h"
#include "editor/link_dialog.h"
//#include "editor/html.h"

#include "main/settings.h"
#include "main/sword.h"
#include "main/sword_treekey.h"
#include "main/url.hh"
#include "main/xml.h"

#include "gui/navbar_versekey_editor.h"
#include "gui/dialog.h"
#include "gui/widgets.h"
#include "gui/xiphos.h"
#include "gui/treekey-editor.h"
#include "gui/utilities.h"

#include "gui/debug_glib_null.h"
//TOOL_ITEMS toolitems;
MENU popup;
FIND_DIALOG find_dialog;

extern gboolean do_display;

static
void change_window_title(GtkWidget * window, const gchar * window_title);

static
gboolean editor_is_dirty(EDITOR * e);

static
void _load_file (EDITOR * e, const gchar * filename);

static
void _save_file (EDITOR * e);

static
void _save_note (EDITOR * e);

static
void _save_book (EDITOR * e);

static 
GList *editors_all = NULL;

static
gchar * editor_get_filename(EDITOR * e)
{
	return NULL;
}

static
void  editor_set_filename(EDITOR * e, const gchar * new_filename)
{
	
}

void action_about_activate_cb (GtkWidget *widget, EDITOR * e)
{

}

void action_increase_indent_activate_cb (GtkWidget *widget, EDITOR * e)  
{   
 		gchar * script = NULL;
	
		script = g_strdup ("document.execCommand('Indent', false, false);");
		editor_execute_script (script, e);
		editor_insert_new_outline_level (e->toolitems.outline_level, e);		
}

void action_decrease_indent_activate_cb (GtkWidget *widget, EDITOR * e)     
{   
 		gchar * script = NULL;

		script = g_strdup ("document.execCommand('Outdent', false, false);");
		editor_execute_script (script, e);
}


void action_insert_rule_activate_cb (GtkWidget *widget, EDITOR * e)
{
		gchar * html = NULL;
		html = g_strdup("<hr>");
		editor_insert_html (html, e);
		if (html) g_free(html);
}

void action_insert_table_activate_cb (GtkWidget *widget,EDITOR * e) 
{

}

void action_insert_emoticon_activate_cb (GtkWidget *widget, EDITOR * e) 
{
	//script = g_strdup ("document.execCommand('', null, \"\");");  
}


void action_insert_image_activate_cb (GtkWidget *widget, EDITOR * e)
{	
	gchar * script = NULL; 
	gchar * filename = NULL;


	GtkWidget * dialog = gtk_file_chooser_dialog_new ("Select an image file", 
                                                         NULL,
                                                         GTK_FILE_CHOOSER_ACTION_OPEN, 
                                                         GTK_STOCK_OK, GTK_RESPONSE_OK,
		                                         		 GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, 
                                                         NULL);  
		// gtk_file_chooser_set_filter  ((GtkFileChooser *)dialog, GtkFileFilter *filter);
		if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
            filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));  
			script = g_strdup_printf ("document.execCommand('insertImage', null, '%s');" ,filename);
			editor_execute_script (script, e);
		}
		
		if (filename) g_free (filename);
		gtk_widget_destroy (dialog);
}


void action_insert_outline_activate_cb (GtkWidget *widget, EDITOR * e)
{
 		gchar * script = NULL;

		script = g_strdup ("document.execCommand('insertHTML', null, \"<OL CLASS=L1><LI> </LI></OL> \");");
		editor_execute_script (script, e);
		
} 


void action_justify_right_activate_cb (GtkWidget *widget, EDITOR * e)
{      	
		gchar * script = NULL;

		script = g_strdup ("document.execCommand('justifyright', false, false);");
		editor_execute_script (script, e);

}

void action_justify_left_activate_cb (GtkWidget *widget, EDITOR * e)
{    
		gchar * script = NULL;

		script = g_strdup ("document.execCommand('justifyleft', false, false);");
		editor_execute_script (script, e);
}

void action_justify_center_activate_cb (GtkWidget *widget, EDITOR * e)
{
 		gchar * script = NULL;

		script = g_strdup ("document.execCommand('justifycenter', false, false);");
		editor_execute_script (script, e);
}

void action_justify_full_activate_cb (GtkWidget *widget, EDITOR * e)
{
 		gchar * script = NULL;

		script = g_strdup ("document.execCommand('justifyfull', false, false);");
		editor_execute_script (script, e);

}

void action_bold_activate_cb (GtkWidget *widget, EDITOR * e)
{
		extern BUTTONS_STATE buttons_state;
		gchar * script = NULL;
		if(buttons_state.nochange)
			return;
		
		script = g_strdup("document.execCommand('bold',false,false);");
		editor_execute_script (script, e);
}
      
void action_italic_activate_cb (GtkWidget *widget, EDITOR * e)
{
		extern BUTTONS_STATE buttons_state;
		gchar * script = NULL;
		if(buttons_state.nochange)
			return;;

		script = g_strdup("document.execCommand('italic',false,false);");
		editor_execute_script (script, e);
}
 
void action_undo_activate_cb (GtkWidget *widget, EDITOR * e)
{     
		gchar * script = NULL;

		script = g_strdup("document.execCommand('undo',false,false);");
		editor_execute_script (script, e);
}

void action_redo_activate_cb (GtkWidget *widget, EDITOR * e)
{  
		gchar * script = NULL;

		script = g_strdup("document.execCommand('redo',false,false);");
		editor_execute_script (script, e);
}

void action_underline_activate_cb (GtkWidget *widget, EDITOR * e)
{  
		extern BUTTONS_STATE buttons_state;
		gchar * script = NULL;
		if(buttons_state.nochange)
			return;

		script = g_strdup("document.execCommand('underline', false, false);");
		editor_execute_script (script, e);
}

void action_strikethrough_activate_cb (GtkWidget *widget, EDITOR * e)
{    
		extern BUTTONS_STATE buttons_state;
		gchar * script = NULL;
		if(buttons_state.nochange)
			return;

		script = g_strdup ("document.execCommand('strikethrough', false, false);");
		editor_execute_script (script, e);

}


void action_cut_activate_cb (GtkWidget *widget, EDITOR * e)
{
		editor_cut(e);
}

    
void action_copy_activate_cb (GtkWidget *widget, EDITOR * e)
{
		if(editor_copy(e))
				GS_message(("%s", "copy sucess"));
		else
				GS_message(("%s", "copy failed"));
}

void action_paste_activate_cb (GtkWidget *widget, EDITOR * e)
{  
		editor_paste(e);
}

void action_delete_activate_cb (GtkWidget *widget, EDITOR * e)
{

}

void action_delete_item_activate_cb (GtkWidget *widget, EDITOR * e)
{
	gchar *buf = NULL;
	gchar *uri = NULL;
	
	if (e->studypad)
		return;

	buf = g_strdup_printf
	    ("<span weight=\"bold\" size=\"larger\">%s %s?</span>",
	    _("Are you sure you want to delete the note for") , e->key);

	if (gui_yes_no_dialog(buf, GTK_STOCK_DIALOG_WARNING)) {
		main_delete_note(e->module, e->key);
						
		/* open with new empty document */	
		uri = g_strdup_printf("file://%s/%s",settings.gSwordDir,"studypad.spt");
		webkit_web_view_load_uri (WEBKIT_WEB_VIEW(e->html_widget), uri); 
		g_free (uri);		
	}
	e->is_changed = FALSE;
	g_free(buf);	
}


void set_button_state(BUTTONS_STATE state, EDITOR * e)
{
	GdkRGBA rgba;
	gtk_toggle_tool_button_set_active (e->toolitems.bold,state.bold);
	gtk_toggle_tool_button_set_active (e->toolitems.italic,state.italic);
	gtk_toggle_tool_button_set_active (e->toolitems.underline,state.underline);
	gtk_toggle_tool_button_set_active (e->toolitems.strike,state.strike);
	gtk_combo_box_set_active ((GtkComboBox *)e->toolitems.cb, state.style);
	if (state.color) {
		GS_message(("state.color: %s",state.color));
		if(gdk_rgba_parse (&rgba, state.color))
			gtk_color_chooser_set_rgba ((GtkColorChooser*) e->toolitems.color, &rgba);
	}
}


void  colorbutton1_color_set_cb (GtkColorButton *widget, EDITOR * e )
{
	GdkRGBA color;
	gchar *color_str;
	gchar *forecolor = NULL;
	
	gtk_color_chooser_get_rgba ((GtkColorChooser *)widget, &color);
	color_str = gdk_rgba_to_string (&color);
	forecolor =  g_strdup_printf("document.execCommand('forecolor', null, '%s');",color_str);
	editor_execute_script (forecolor, e);
}


void  colorbutton_highlight_color_set_cb (GtkColorButton *widget, EDITOR * e )
{
	GdkRGBA color;
	gchar *color_str;
	gchar *highligntcolor = NULL;
	
	gtk_color_chooser_get_rgba ((GtkColorChooser *)widget, &color);
	color_str = gdk_rgba_to_string (&color);
	highligntcolor =  g_strdup_printf("document.execCommand('backColor', null, '%s');",color_str);
	editor_execute_script (highligntcolor, e);
}


static 
gchar *get_font_size_from_name(GString * fontname)
{
	gchar buf[80];
	gint len, i = 0, j = 0;

	len = fontname->len;
	GS_message(("\nlength: %d\n",len));
	for (i = 0; (i < 79 && i < len); i++) {
		if (isdigit(fontname->str[i])) {
			GS_message(("\n\nfontname->str[i]: %c",fontname->str[i]));
			buf[j] = fontname->str[i];
			buf[j + 1] = '\0';
			j++;
		} 
	}
	return g_strdup(buf);
}

void action_font_activate_cb (GtkWidget *widget, EDITOR * e)
{ 
	GtkWidget * dialog;
    GString *  name;
	const gchar * fontname = NULL; 
	gchar * selected_text = NULL;
	gchar * script = NULL;
	gchar *  size = NULL;
	PangoFontDescription * font_description;
	
	dialog = gtk_font_chooser_dialog_new ("Select font",NULL);
	gtk_font_chooser_set_font ((GtkFontChooser *)dialog,
                                                 "Droid Sans 14");
	if(gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_OK)
	{	
		fontname = gtk_font_chooser_get_font ((GtkFontChooser*)dialog);
		name = g_string_new (fontname);
		size = get_font_size_from_name (name);
		g_string_free (name,TRUE);	

		selected_text = editor_get_selected_text (e);

		font_description = gtk_font_chooser_get_font_desc ((GtkFontChooser*)dialog);
		fontname = pango_font_description_get_family   (font_description);

		script = g_strdup_printf("<SPAN STYLE=\"font-family:%s;font-size:%spx;\">%s</SPAN>", fontname, size, selected_text);

		editor_insert_html (script, e);	
	}
	if (size) g_free(size);   
	if (selected_text) g_free(selected_text); 
	if (script) g_free(script); 
	gtk_widget_destroy (dialog);
}


void find_replace_response_cb (GtkDialog *dialog, gint response_id,
                                                  EDITOR * e)
{
	switch (response_id) {
		case GTK_RESPONSE_CANCEL:
			gtk_widget_hide (find_dialog.window);
			break;
		case 1:			
			editor_find_string ((gchar*)gtk_entry_get_text (GTK_ENTRY(find_dialog.find_entry)),e);
			break;
		case 2:
			editor_replace_string((gchar*)gtk_entry_get_text (GTK_ENTRY(find_dialog.find_entry)),
									(gchar*)gtk_entry_get_text (GTK_ENTRY(find_dialog.replace_entry)),e);
			break;
		default:
			gtk_widget_hide (find_dialog.window);
			break;	
	}

}


void action_find_activate_cb (GtkWidget *widget, EDITOR * e)  
{
	gtk_widget_show (find_dialog.window);
	gtk_widget_hide (find_dialog.box_replace);
	gtk_widget_hide (find_dialog.button_replace);
}

void action_replace_activate_cb (GtkWidget *widget, EDITOR * e) 
{
	gtk_widget_show (find_dialog.window);
	gtk_widget_show (find_dialog.box_replace);
	gtk_widget_show (find_dialog.button_replace);
}

static void
handle_error (GError **error)
{
	if (*error != NULL) {
		g_warning ("%s", (*error)->message);
		g_clear_error (error);
	}
}

static
void do_exit(EDITOR * e)
{
	if (e->filename) {
		g_free(e->filename);
	}
	if (e->module) {
		g_free(e->module);
	}
	if (e->key) {
		g_free(e->key);
	}
	if (e->window)
		gtk_widget_destroy(e->window);
	g_free(e);
}


void recent_item_cb (GtkRecentChooser *chooser, EDITOR * e)
{
	gchar * file_uri = gtk_recent_chooser_get_current_uri (chooser);

	GS_message(("file_uri: %s", file_uri));
	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(file_uri);
	
	//editor_open_recent (file_uri, e);	
	_load_file(e, file_uri);
	xml_set_value("Xiphos", "studypad", "lastfile",
		      e->filename);
	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	
	change_window_title(e->window, e->filename);
}

static
void change_window_title(GtkWidget * window, const gchar * window_title)
{
	gtk_window_set_title(GTK_WINDOW(window), window_title);
}

static GtkPrintOperationResult
print (WebKitWebView * html, GtkPrintOperationAction action)
{
	GtkPrintOperation *operation;
	GtkPrintOperationResult result;
	GError *error = NULL;
	WebKitWebFrame *frame;

	frame = webkit_web_view_get_main_frame ( html );
	operation = gtk_print_operation_new ();
	
	result = webkit_web_frame_print_full(frame,
                                     operation,
                                     action,
                                     &error);

	g_object_unref (operation);
	handle_error (&error);

	return result;
}

static gint
open_dialog (EDITOR * e)
{
	GtkWidget *dialog;
	gint response;
	GtkFileFilter *filter = gtk_file_filter_new ();
	
	gtk_file_filter_add_mime_type (filter,"text/html");
	
	dialog = gtk_file_chooser_dialog_new (
		_("Open"), GTK_WINDOW (e->window),
		GTK_FILE_CHOOSER_ACTION_OPEN,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
		NULL);
	
	gtk_file_chooser_set_current_folder (
			GTK_FILE_CHOOSER (dialog), settings.studypaddir); 
	gtk_file_chooser_set_filter ((GtkFileChooser*)dialog, filter);
	
	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_ACCEPT) {
		gchar *new_filename;

		new_filename = gtk_file_chooser_get_filename ( GTK_FILE_CHOOSER (dialog));
		_load_file(e, new_filename);
		g_free (new_filename);
	}

	gtk_widget_destroy (dialog);

	return response;
}


static gint
save_dialog (EDITOR * e)
{
	GtkWidget *dialog;
	const gchar *filename;
	gint response;

	dialog = gtk_file_chooser_dialog_new (
		_("Save As"), GTK_WINDOW (e->window),
		GTK_FILE_CHOOSER_ACTION_SAVE,
		GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
		GTK_STOCK_SAVE, GTK_RESPONSE_ACCEPT,
		NULL);

	gtk_file_chooser_set_do_overwrite_confirmation (
		GTK_FILE_CHOOSER (dialog), TRUE);

	filename = editor_get_filename (e);

	if (filename != NULL)
		gtk_file_chooser_set_filename (
			GTK_FILE_CHOOSER (dialog), filename);
	else {
		gtk_file_chooser_set_current_folder (
			GTK_FILE_CHOOSER (dialog), settings.studypaddir); 
		gtk_file_chooser_set_current_name (
			GTK_FILE_CHOOSER (dialog), _("Untitled document"));
	}

	response = gtk_dialog_run (GTK_DIALOG (dialog));

	if (response == GTK_RESPONSE_ACCEPT) {
		gchar *new_filename;

		new_filename = gtk_file_chooser_get_filename (
			GTK_FILE_CHOOSER (dialog));
		editor_set_filename (e, new_filename);

		if (e->filename)
			g_free(e->filename);
		e->filename = g_strdup(new_filename);

		xml_set_value("Xiphos", "studypad", "lastfile",
			      e->filename);
		settings.studypadfilename =
		    xml_get_value("studypad", "lastfile");

		change_window_title(e->window, e->filename);

		g_free (new_filename);
	}

	gtk_widget_destroy (dialog);

	return response;
}

/* Helper for view_source_dialog() */
/*
static gboolean
view_source_dialog_receiver (HTMLEngine *engine,
                             const gchar *data,
                             guint length,
                             GString *string)
{
	g_string_append_len (string, data, length);

	return TRUE;
}

static void
view_source_dialog (GtkhtmlEditor *editor,
                    const gchar *title,
                    const gchar *content_type,
                    gboolean show_output)
{
	GtkWidget *dialog;
	GtkWidget *content;
	GtkWidget *scrolled_window;
	GString *string;

	dialog = gtk_dialog_new_with_buttons (
		title, GTK_WINDOW (editor),
		GTK_DIALOG_DESTROY_WITH_PARENT,
		GTK_STOCK_CLOSE, GTK_RESPONSE_CLOSE,
		NULL);

	scrolled_window = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (
		GTK_SCROLLED_WINDOW (scrolled_window),
		GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_set_shadow_type (
		GTK_SCROLLED_WINDOW (scrolled_window), GTK_SHADOW_IN);
	gtk_box_pack_start (
		GTK_BOX (gtk_dialog_get_content_area(GTK_DIALOG (dialog))),
		scrolled_window, TRUE, TRUE, 0);

	gtk_container_set_border_width (GTK_CONTAINER (dialog), 6);
	gtk_container_set_border_width (GTK_CONTAINER (scrolled_window), 6);
	gtk_window_set_default_size (GTK_WINDOW (dialog), 400, 300);

	string = g_string_sized_new (4096);

	gtk_html_export (
		gtkhtml_editor_get_html (editor),
		content_type, (GtkHTMLSaveReceiverFn)
		view_source_dialog_receiver, string);

	if (show_output) {
		GtkHTMLStream *stream;

		content = gtk_html_new ();
		stream = gtk_html_begin (GTK_HTML (content));
		gtk_html_stream_write (stream, string->str, string->len);
		gtk_html_stream_close (stream, GTK_HTML_STREAM_OK);
	} else {
		GtkTextBuffer *buffer;

		content = gtk_text_view_new ();
		buffer = gtk_text_view_get_buffer (GTK_TEXT_VIEW (content));
		gtk_text_buffer_set_text (buffer, string->str, string->len);
		gtk_text_view_set_editable (GTK_TEXT_VIEW (content), FALSE);
	}

	g_string_free (string, TRUE);

	gtk_container_add (GTK_CONTAINER (scrolled_window), content);
	gtk_widget_show_all (scrolled_window);

	gtk_dialog_run (GTK_DIALOG (dialog));
	gtk_widget_destroy (dialog);
}
*/
void
action_print_cb (GtkAction *action,
                 EDITOR *e)
{
	print ( WEBKIT_WEB_VIEW(e->html_widget), GTK_PRINT_OPERATION_ACTION_PRINT_DIALOG );
}

void
action_print_preview_cb (GtkAction *action,
                         EDITOR *e)
{
	print (WEBKIT_WEB_VIEW(e->html_widget), GTK_PRINT_OPERATION_ACTION_PREVIEW);
}

void action_quit_activate_cb (GtkWidget *widget, EDITOR * e)
{
	delete_event (NULL, NULL,e);
}

void action_open_activate_cb (GtkWidget * widget, EDITOR * e)
{
		open_dialog(e); 
}


void
action_save_activate_cb (GtkWidget * widget, EDITOR *e)
{
	switch (e->type) {
	case STUDYPAD_EDITOR:
		_save_file (e);
		break;
	case NOTE_EDITOR:
		_save_note (e);
		break;
	case BOOK_EDITOR:
		_save_book (e);
		break;
	default:
		GS_message (("\naction_save_cb oops!\n"));
		break;
	}
}


void
action_new_activate_cb ( GtkWidget * widget,
            			EDITOR *e)	/* for studypad only */
{
	gchar *filename = NULL;
	
	if (e->is_changed)
		ask_about_saving (e);

	filename = g_strdup_printf("%s/%s",settings.gSwordDir,"studypad.spt");

	
	_load_file(e, filename);

	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(_("Untitled document"));

	xml_set_value("Xiphos", "studypad", "lastfile",
		      e->filename);
	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	change_window_title(e->window, e->filename);
	e->is_changed = TRUE;
}


void 
action_insert_sword_link_activate_cb (GtkWidget * widget, gpointer data)
{    
		editor_link_dialog (data); //editor_insert_sword_link();
}

void
action_insert_link_activate_cb (GtkWidget * widget, EDITOR *e)
{
	editor_link_dialog (e);
}


void
action_save_as_activate_cb (GtkWidget * widget, EDITOR *e)
{	
	if (e->filename)
		g_free (e->filename);
	e->filename = NULL;
	_save_file(e);	
}
/*
static void
action_view_html_output (GtkAction *action,
                         GtkhtmlEditor *editor)
{
	view_source_dialog (editor, _("HTML Output"), "text/html", TRUE);
}

static void
action_view_html_source (GtkAction *action,
                         GtkhtmlEditor *editor)
{
	view_source_dialog (editor, _("HTML Source"), "text/html", FALSE);
}

static void
action_view_plain_source (GtkAction *action,
                          GtkhtmlEditor *editor)
{
	view_source_dialog (editor, _("Plain Source"), "text/plain", FALSE);
}
*/


void combo_box_changed_cb (GtkComboBox *widget, EDITOR * e)
{
		gchar * script = NULL;
		gint choice = gtk_combo_box_get_active(widget);
		/* we need the line of text where the cursor is */
		extern BUTTONS_STATE buttons_state;
	
		if(buttons_state.nochange) return;
	
		switch (choice) {
			case 0: /* Normal  */
				script = g_strdup ("document.execCommand('formatBlock', false, \"div\");");
				break;
			case 1: /* H1  */
				script = g_strdup ("document.execCommand('formatBlock', null, \"H1\");");
				break;
			case 2: /* H2 */
				script = g_strdup ("document.execCommand('formatBlock', null, \"H2\");");
				break;
			case 3: /* H3  */
				script = g_strdup ("document.execCommand('formatBlock', null, \"H3\");");

				break;
			case 4: /* H4  */
				script = g_strdup ("document.execCommand('formatBlock', null, \"H4\");");

				break;
			case 5: /* H5 */
				script = g_strdup ("document.execCommand('formatBlock', null, \"H5\");");

				break;
			case 6: /* H6  */
				script = g_strdup ("document.execCommand('formatBlock', null, \"H6\");");

				break;
			case 7: /* Address  */
				script = g_strdup ("document.execCommand('formatBlock', null, \"ADDRESS\");");

				break;
			case 8: /* Preformatted  */
				script = g_strdup ("document.execCommand('formatBlock', null, \"PRE\");");

				break;
			case 9: /* Bulleted List  insertUnorderedList*/
				script = g_strdup ("document.execCommand('insertUnorderedList', null, \"\");");

				break;
			case 10: /* Roman Numeral List  */
				script = g_strdup ("document.execCommand('insertHTML', null, \"<OL type=I><LI> </LI></OL> \");");  
				
				break;
			case 11: /* Numbered List  insertOrderedList*/
				script = g_strdup ("document.execCommand('insertOrderedList', null, \"\");");

				break;
			case 12: /* Alphabetical List  */
				script = g_strdup ("document.execCommand('insertHTML', null, \"<OL type=A><LI> </LI></OL> \");");  

				break;
			default:
				return;
				break;
		}
		if (script) {
				GS_message(("%s",script));
				editor_execute_script (script, e);
		}
}

/* need note toolbars when type is note */
static 
GtkWidget* editor_new (const gchar * title, EDITOR * e)
{ 
	GtkWidget *window; 
	GtkWidget *scrollwindow;
	GtkWidget *statusbar;
	GtkBuilder *builder;
	gchar *gbuilder_file;
	GError* error = NULL; 
	GtkMenuItem *item;
	GtkWidget *recent_item;
	extern BUTTONS_STATE buttons_state;
	GtkRecentFilter *filter;

	buttons_state.nochange = 1;

   	builder = gtk_builder_new ();
	gbuilder_file = gui_general_user_file ("gtk_webedit.ui", FALSE);
	builder = gtk_builder_new ();   
	
	if (!gtk_builder_add_from_file (builder, gbuilder_file, &error))
	{
		g_warning ("Couldn't load builder file: %s", error->message);
		g_error_free (error);
	}
 
	
	window = GTK_WIDGET (gtk_builder_get_object (builder, "window"));
	e->window = window;
	gtk_window_set_title(GTK_WINDOW(window),title);
	
	statusbar = GTK_WIDGET(gtk_builder_get_object (builder,"statusbar1"));
	gtk_widget_hide (statusbar);
	
	e->toolitems.outline_level = 0;
	e->toolitems.bold = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object (builder,"toolbutton_bold"));
	e->toolitems.italic = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object (builder,"toolbutton_italic")); 
	e->toolitems.underline = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object (builder,"toolbuttonunderline"));
	e->toolitems.strike = GTK_TOGGLE_TOOL_BUTTON(gtk_builder_get_object (builder,"toolbutton_strikethrough"));
	e->toolitems.open = GTK_TOOL_BUTTON(gtk_builder_get_object (builder,"toolbutton_open"));
	e->toolitems.newdoc = GTK_TOOL_BUTTON(gtk_builder_get_object (builder,"toolbutton_new"));
	e->toolitems.deletedoc = GTK_TOOL_BUTTON(gtk_builder_get_object (builder,"toolbutton_delete"));
	e->toolitems.color = GTK_COLOR_BUTTON(gtk_builder_get_object (builder,"colorbutton1"));
	
	e->toolitems.cb = GTK_COMBO_BOX_TEXT(gtk_builder_get_object (builder,"comboboxtext1")); 
	gtk_combo_box_set_active ((GtkComboBox *)e->toolitems.cb, 0);

	item = GTK_MENU_ITEM( gtk_builder_get_object( builder, "menuitem_recent" ) );
	
	switch (e->type){
		case STUDYPAD_EDITOR:
			gtk_widget_hide(GTK_WIDGET(e->toolitems.deletedoc));
			
			//filter = gtk_recent_filter_new ();
			//gtk_recent_filter_add_mime_type ( filter, "text/html"); 
			
			recent_item = gtk_recent_chooser_menu_new();
			//gtk_recent_chooser_set_filter ((GtkRecentChooser*)recent_item,
            //                                             filter);
			g_signal_connect (G_OBJECT (recent_item), "item-activated",
					  		 G_CALLBACK (recent_item_cb),
					  		 e);
			gtk_menu_item_set_submenu( item,  recent_item); 
			break;
		case NOTE_EDITOR:
			gtk_widget_hide(GTK_WIDGET(e->toolitems.open));
			gtk_widget_hide(GTK_WIDGET(e->toolitems.newdoc));
			gtk_widget_hide(GTK_WIDGET(item));
			break;
		case BOOK_EDITOR:
			gtk_widget_hide(GTK_WIDGET(e->toolitems.open));
			gtk_widget_hide(GTK_WIDGET(e->toolitems.newdoc));
			gtk_widget_hide(GTK_WIDGET(item));
			break;

	}
	
	e->navbar_box = GTK_WIDGET (gtk_builder_get_object (builder, "box_navbar"));
	e->box = GTK_WIDGET (gtk_builder_get_object (builder, "vbox1"));

	
    scrollwindow = GTK_WIDGET(gtk_builder_get_object (builder,"scrolledwindow1")); 
	create_editor_window(scrollwindow, e);
	e->is_changed = FALSE;
	

	/* This is important */
	gtk_builder_connect_signals (builder, (EDITOR *) e);
	
	find_dialog.window = GTK_WIDGET (gtk_builder_get_object (builder, "dialog_find_replace"));
	find_dialog.find_entry = GTK_WIDGET (gtk_builder_get_object (builder, "entry1"));
	find_dialog.replace_entry = GTK_WIDGET (gtk_builder_get_object (builder, "entry2"));
	find_dialog.box_replace = GTK_WIDGET (gtk_builder_get_object (builder, "box4"));
	find_dialog.button_replace = GTK_WIDGET (gtk_builder_get_object (builder, "button_replace"));

	
	g_object_unref (builder);
	
	return window;
}

static void
_save_note(EDITOR * e)
{		
	GString * data = g_string_new ("");

	editor_get_document_content (data, e);
	main_save_note (e->module, e->key, data->str);
	e->is_changed = FALSE;
	g_string_free (data, TRUE);
}


static void
_save_book(EDITOR * e)
{	
	GString * data = g_string_new ("");

	editor_get_document_content (data, e);
	main_treekey_save_book_text(e->module, e->key, data->str);
	e->is_changed = FALSE;
	g_string_free (data, TRUE);
}


static void
_save_file (EDITOR * e)
{
	gchar * filename = NULL;
	GtkRecentManager * rm = NULL;
	GString * data = g_string_new ("");


	editor_get_document_content (data, e);
	GS_message(("%s",data->str));
 
	if (!e->filename || ( 0 == g_strcmp0 ("Untitled document",e->filename)) ||
						g_strrstr (e->filename, ".spt")) {	
		GtkWidget * dialog = gtk_file_chooser_dialog_new ("Save as", //const gchar *title,
                                         NULL, //GtkWindow *parent,
                                         GTK_FILE_CHOOSER_ACTION_SAVE, //GtkFileChooserAction action,
                                         GTK_STOCK_OK, GTK_RESPONSE_OK,
                                         GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL, //const gchar *first_button_text,
                                         NULL);  
		gtk_file_chooser_set_do_overwrite_confirmation (GTK_FILE_CHOOSER (dialog), TRUE);
		
		gtk_file_chooser_set_current_folder ( GTK_FILE_CHOOSER (dialog), settings.studypaddir); 
		if (gtk_dialog_run (GTK_DIALOG(dialog)) == GTK_RESPONSE_OK) {
            filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
			e->filename = g_strdup (filename);
			GFile * gfile = g_file_parse_name (filename);
			g_file_replace_contents (gfile,
                                         data->str,
                                         data->len,
                                         NULL,
                                         TRUE,
                                         G_FILE_CREATE_NONE,
                                         NULL,
                                         NULL,
                                         NULL);  //GError **error 
			
		}	
		change_window_title(e->window, e->filename);
		gtk_widget_destroy (dialog);
		
	} else {
	
		GFile * gfile = g_file_parse_name (e->filename);  
		 g_file_replace_contents (gfile,
                                         data->str,
                                         data->len,
                                         NULL,
                                         TRUE,
                                         G_FILE_CREATE_NONE,
                                         NULL,
                                         NULL,
                                         NULL);  //GError **error 
	}
	
	rm = gtk_recent_manager_get_default ();
	gtk_recent_manager_add_item (rm,e->filename);
	
	e->is_changed = FALSE;
	g_string_free (data,TRUE);
}


static void
_load_file (EDITOR * e, const gchar * filename)
{
	char *uri;
	gsize length;
	GtkRecentManager * rm = NULL;
	
	rm = gtk_recent_manager_get_default ();
	gtk_recent_manager_add_item (rm, filename);
	
	if (e->filename)
		g_free(e->filename);
	e->filename = g_strdup(filename);

	GS_message(("_load_file filename: %s",filename));

	xml_set_value("Xiphos", "studypad", "lastfile",
		      e->filename);
	settings.studypadfilename =
	    xml_get_value("studypad", "lastfile");
	
	change_window_title(e->window, e->filename);
	if (g_strstr_len (filename,6,"file:")) 
		uri = g_strdup_printf("%s",filename);
	else
		uri = g_strdup_printf("file://%s",filename);
		
	webkit_web_view_load_uri (WEBKIT_WEB_VIEW(e->html_widget),uri); 
	
	g_free (uri);
	e->is_changed =FALSE;
}

gboolean editor_is_dirty(EDITOR * e)
{
	return e->is_changed;
}


void editor_save_book(EDITOR * e)
{
	if (editor_is_dirty(e)) {
		_save_book(e);
	}
}

/* save if needed is done in treeky-editor.c before calling editor_load_book() */
void editor_load_book(EDITOR * e)
{
	gchar *title = NULL;
	gchar *text = NULL;
	gchar *uri = NULL;

	if (!g_ascii_isdigit(e->key[0])) return; /* make sure is a number (offset) */


	title = g_strdup_printf("%s", e->module);
	GS_message (("book: %s\noffset :%s", e->module, e->key));

	if (atol(e->key) != 0)
		text = main_get_book_raw_text (e->module, e->key);
	else
		text = g_strdup(e->module);

	if (strlen(text)) {
		webkit_web_view_load_string ((WebKitWebView*)e->html_widget,
                                         text, //const gchar          *content,
                                         "text/html", //const gchar          *mime_type,
                                         "utf_8", //const gchar          *encoding,
                                         "file://" ); //const gchar          *base_uri);
	} else {
		/* open with new empty document */	
		uri = g_strdup_printf("file://%s/%s",settings.gSwordDir,"studypad.spt");
		webkit_web_view_load_uri (WEBKIT_WEB_VIEW(e->html_widget), uri); 
		g_free (uri);			
	}


	change_window_title(e->window, title);
	e->is_changed = FALSE;

	if (text)
		g_free(text);
	if (title)
		g_free(title);
}

/******************************************************************************
 * Name
 *   sync_toggled
 *
 * Synopsis
 *   #include "/.h"
 *
 *   void sync_toggled(GtkToggleButton * button, DIALOG_DATA * vc)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void editor_sync_with_main(void)
{
	GList *tmp = NULL;
	EDITOR * e;

	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		//if (!e->studypad) {
		switch (e->type) {
			case STUDYPAD_EDITOR:
			case BOOK_EDITOR:
				break;
			case NOTE_EDITOR:
			break;
		}
		tmp = g_list_next(tmp);
	}
}

void
editor_load_note(EDITOR * e, const gchar * module_name,
		 const gchar * key)
{
	gchar *title = NULL;
	gchar *text = NULL;
	gchar *uri = NULL;


	if (e->is_changed)
		_save_note (e);

	if (module_name) {
		if (e->module)
			g_free(e->module);
		e->module = g_strdup(module_name);
	}
	if (key) {
		if (e->key)
			g_free(e->key);
		e->key = g_strdup(key);
	}


	title = g_strdup_printf("%s - %s", e->module, e->key);
	text = main_get_raw_text((gchar *) e->module, (gchar *) e->key);
	if (strlen(text)) {
		webkit_web_view_load_string  ((WebKitWebView*)e->html_widget,
                                                 text, //const gchar          *content,
                                                 "text/html", //const gchar          *mime_type,
                                                 "utf_8", //const gchar          *encoding,
                                                 "file://" ); //const gchar          *base_uri);
	} else {	
		/* open with new empty document */	
		uri = g_strdup_printf("file://%s/%s",settings.gSwordDir,"studypad.spt");
		webkit_web_view_load_uri (WEBKIT_WEB_VIEW(e->html_widget), uri); 
		g_free (uri);	
	}
	e->is_changed = FALSE;
	change_window_title(e->window, title);
    if (e->type == NOTE_EDITOR)
		main_navbar_versekey_set(e->navbar, e->key);

	if (text)
		g_free(text);
	if (title)
		g_free(title);
}


int
delete_event (GtkWidget *widget, GdkEvent  *event, EDITOR * e)
{
	if (e->is_changed) {
		switch (ask_about_saving(e)) {
		case GS_YES:	/* exit saving */

			break;
		case GS_NO:	/* exit without saving */

			break;
		case GS_CANCEL:
			return TRUE;
			break;
		}
	}
	editors_all = g_list_remove(editors_all, e);
	do_exit(e);
	return FALSE;
}


gint ask_about_saving(EDITOR * e)
{
	gint test;
	GS_DIALOG *info;
	gchar *buf = NULL;
	gchar *buf1 = NULL;
	gchar *buf2 = NULL;
	gchar *buf3 = NULL;
	gint retval = FALSE;

	switch (e->type) {
	case BOOK_EDITOR:
	case NOTE_EDITOR:
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;

		buf = g_strdup_printf("%s: %s",e->module, e->key);
		buf1 = _("Save the changes to document");
		buf2 = _("before closing?");
		buf3 =
		    g_strdup_printf
		    ("<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
		     buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 =
		    _("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;

		test = gui_alert_dialog(info);
		retval = test;

		if (test == GS_YES) {
			if (e->type == NOTE_EDITOR) {
				/* save notes and prayer lists */
				_save_note(e);

			} else {
				/* save notes and prayer lists */
				_save_book(e);
			}
		}
		g_free(info);
                g_free(buf);
		g_free(buf3);
		break;

	case STUDYPAD_EDITOR:
		info = gui_new_dialog();
		info->stock_icon = GTK_STOCK_DIALOG_WARNING;
		if (settings.studypadfilename)
			buf = settings.studypadfilename;
		else
			buf = N_("File");
		buf1 = _("Save the changes to document");
		buf2 = _("before closing?");
		buf3 =
		    g_strdup_printf
		    ("<span weight=\"bold\" size=\"larger\">%s %s %s</span>",
		     buf1, buf, buf2);
		info->label_top = buf3;
		info->label2 =
		    _("If you don't save, changes will be permanently lost.");
		info->save = TRUE;
		info->cancel = TRUE;
		info->no_save = TRUE;

		test = gui_alert_dialog(info);
		retval = test;
		if (test == GS_YES) {
			_save_file (e);
		}
		g_free(info);
		g_free(buf3);
		break;
	}
	sync_windows();
	return retval;
}


static
gint _create_new(const gchar * filename, const gchar * key, gint editor_type)
{
	EDITOR *editor;
	GtkWidget *vbox = NULL;
	GtkWidget *toolbar_nav = NULL;


	editor = g_new(EDITOR, 1);
	editor->html_widget = NULL;
	editor->sync = FALSE;
	editor->type = editor_type;

	switch (editor_type) {
	case STUDYPAD_EDITOR:
		editor->studypad = TRUE;
		editor->bookeditor = FALSE;
		editor->noteeditor = FALSE;
		editor->module = NULL;
		editor->key = NULL;
		editor->filename = NULL;
		widgets.studypad_dialog =
			editor_new(_("StudyPad"), editor);

		if (filename) {
			editor->filename = g_strdup(filename);
			_load_file(editor, g_strdup(filename));
		}
		break;
	case NOTE_EDITOR:
		editor->noteeditor = TRUE;
		editor->bookeditor = FALSE;
		editor->studypad = FALSE;
		editor->filename = NULL;
		editor->module = g_strdup(filename);
		editor->key = g_strdup(key);
		editor->navbar.key = NULL;
		editor_new(_("Note Editor"), editor);
			
		toolbar_nav = gui_navbar_versekey_editor_new(editor);			
		gtk_widget_show(toolbar_nav);			
		gtk_box_pack_start (GTK_BOX (editor->navbar_box), GTK_WIDGET(toolbar_nav), FALSE, TRUE, 0);
			
		editor_load_note(editor, NULL, NULL);
		break;
	case BOOK_EDITOR:
		editor->bookeditor = TRUE;
		editor->noteeditor = FALSE;
		editor->studypad = FALSE;
		editor->filename = NULL;
		editor->module = g_strdup(filename);
		editor->key = g_strdup(key);
		editor_new(_("Prayer List/Journal Editor"), editor);

		GtkWidget *box;
		UI_VBOX(box, FALSE, 0);
		gtk_widget_show (box);
		GtkWidget *hpaned1 = UI_HPANE();
		gtk_widget_show (hpaned1);
		gtk_paned_pack2 (GTK_PANED (hpaned1), box, TRUE, TRUE);

		GtkWidget *scrollbar = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_show(scrollbar);
		gtk_paned_pack1(GTK_PANED(hpaned1), GTK_WIDGET(scrollbar),
					TRUE, TRUE);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrollbar),
					       GTK_POLICY_AUTOMATIC,
					       GTK_POLICY_AUTOMATIC);
		gtk_scrolled_window_set_shadow_type((GtkScrolledWindow *)
						    scrollbar,
				    settings.shadow_type);

		editor->treeview = gui_create_editor_tree(editor);
		gtk_widget_show(editor->treeview);
		gtk_container_add(GTK_CONTAINER(scrollbar),
				 editor->treeview);
		gtk_paned_set_position(GTK_PANED(hpaned1),
			       125);
		gtk_tree_view_expand_all((GtkTreeView *)editor->treeview); 
		//gtk_tree_view_collapse_all((GtkTreeView *)editor->treeview);
		// then we should expand on the item to which we've opened for edit.		

		gtk_widget_reparent (editor->box,box);

		gtk_container_add (GTK_CONTAINER (editor->window), hpaned1);

		editor_load_book(editor);

		break;
	}
	editor->is_changed = FALSE;
	editors_all = g_list_append(editors_all,(EDITOR*) editor);
	return 1;

}


/******************************************************************************
 * Name
 *   editor_create_new
 *
 * Synopsis
 *   #include "editor/html-editor.h"
 *
 *   gint editor_create_new(const gchar * filename, const gchar * key, gint note)
 *
 * Description
 *   limits editors to one note editor and one studypad open at a time
 *
 * Return value
 *   gint
 */

gint editor_create_new(const gchar * filename, const gchar * key, gint editor_type)
{
	GList *tmp = NULL;
	EDITOR *e;

	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		e = (EDITOR*)tmp->data;
		switch (editor_type) {
		case STUDYPAD_EDITOR:
			if (e->studypad) {
				if (editor_is_dirty(e))
					_save_file (e);
				if (e->filename)
					g_free(e->filename);
				e->filename = g_strdup(filename);
				gtk_widget_show(e->window);
				gdk_window_raise(gtk_widget_get_parent_window(GTK_WIDGET(e->window)));

				_load_file(e, g_strdup(filename));
				return 1;
			}
			break;
		case NOTE_EDITOR:
			if (!e->noteeditor) break;
			if (editor_is_dirty(e))
				_save_note (e);
			if (e->module)
				g_free(e->module);
			e->module = g_strdup(filename);
			if (e->key)
				g_free(e->key);
			e->key = g_strdup(key);
			gtk_widget_show(e->window);
			gdk_window_raise(gtk_widget_get_parent_window(GTK_WIDGET(e->window)));

			editor_load_note(e, NULL, NULL);

			return 1;
			break;
		case BOOK_EDITOR:
			if (!e->bookeditor) break;
			if (editor_is_dirty(e))
				_save_book (e);
			if (e->module)
				g_free(e->module);
			e->module = g_strdup(filename);
			if (e->key)
				g_free(e->key);
			e->key = g_strdup(key);
			gtk_widget_show(e->window);
			gdk_window_raise(gtk_widget_get_parent_window(GTK_WIDGET(e->window)));
			main_load_book_tree_in_editor (GTK_TREE_VIEW (e->treeview), e->module);
			editor_load_book(e);

			return 1;
			break;
		}
		tmp = g_list_next(tmp);
	}
    	GS_message (("filename %s, key %s", (filename ? filename : "-null-"), (key ? key : "-null-")));
	return _create_new(filename, key, editor_type);
}

void editor_maybe_save_all(void)
{
	GList *tmp, *tmp2;

	tmp = g_list_first(editors_all);
	while (tmp != NULL) {
		/* 2ndary list chaser:
		   elements will be removed by the saver. */
		tmp2 = g_list_next(tmp);

		delete_event(NULL, NULL, (EDITOR*)tmp->data);
		tmp = tmp2;
	}
}

#endif /* USE_WEBKIT_EDITOR */
