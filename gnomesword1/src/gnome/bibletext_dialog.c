/*
 * GnomeSword Bible Study Tool
 * bibletext_dialog.c - view Bible text module in a dialog
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
#include <gal/widgets/e-unicode.h>

#include "gui/bibletext_dialog.h"
#include "gui/_display_info.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"

#include "main/display_info.h"
#include "main/bibletext.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/support.h"
#include "main/settings.h"
#include "main/lists.h"

/******************************************************************************
 * globals
 */
gboolean isrunningVT = FALSE; /* is the view text dialog runing */
gchar vt_current_verse[80];

/******************************************************************************
 * static - global to this file only
 */
static GList * dialog_list;
static VIEW_TEXT * cur_vt;

/******************************************************************************
 * externs
 *****************************************************************************/
extern gboolean gsI_isrunning; /* information dialog */


/******************************************************************************
 * Name
 *   chapter_display
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void chapter_display(VIEW_TEXT * vt, gchar *key)	
 *
 * Description
 *   display bibletext a chapter at a time
 *
 * Return value
 *   void
 */

static void chapter_display(VIEW_TEXT * vt, gchar *key)
{
	gchar 
		*utf8str,
		*bgColor,
		*textColor,
		buf[500], 
		*tmpkey,
		tmpbuf[256],
		*use_font_size;
	gchar 	*paragraphMark;	
	gint 
		utf8len,
		cur_verse,
		cur_chapter,
		i = 1;
	gboolean newparagraph = FALSE;
	
	const char *cur_book;
	
	GtkHTMLStreamStatus status1 = 0;
	GtkHTMLStream *htmlstream;
	
	GtkHTML *html = GTK_HTML(vt->html);
	gboolean was_editable = gtk_html_get_editable(html);
	
	paragraphMark = "&para;"; 
	
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	htmlstream =
	    gtk_html_begin_content(html, "text/html; charset=utf-8");

	
	sprintf(buf,
		"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
		settings.bible_bg_color, settings.bible_text_color, settings.link_color);
	utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
	utf8len = strlen(utf8str);     
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}	
	
	tmpkey = get_valid_key(key);
	strcpy(vt->key, tmpkey);
	
	bgColor = "#f1f1f1";
	cur_verse = get_verse_from_key(tmpkey);
	cur_chapter = get_chapter_from_key(tmpkey);
	cur_book = get_book_from_key(tmpkey);
	
	for (i = 1; ; i++) {	
		sprintf(tmpbuf,"%s %d:%d",cur_book,cur_chapter,i);
		g_free(tmpkey);
		tmpkey = get_valid_key(tmpbuf);
		if(cur_chapter != get_chapter_from_key(tmpkey))
			break;
		sprintf(buf,"%s","<tr valign=\"top\">");		
		utf8str = e_utf8_from_gtk_string(settings.htmlInterlinear, buf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}		
				
		if(i == cur_verse)
			textColor = settings.currentverse_color;
		else 
			textColor = settings.bible_text_color;
			
					
		use_font_size = settings.bible_font_size; 
						
		sprintf(buf,			
			"&nbsp; <A HREF=\"I%s\" NAME=\"%d\">"
			"<font color=\"%s\">%d</font></A>"
			"<font size=\"%s\" color=\"%s\"> ", 
			tmpkey,
			i,				 
			settings.bible_verse_num_color, 
			i,
			use_font_size,
			textColor);	
		
		utf8str = e_utf8_from_gtk_string(vt->html, buf);
		utf8len = strlen(utf8str);
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
						
		if(newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			sprintf(tmpbuf,  "%s ", paragraphMark);
			utf8str = e_utf8_from_gtk_string(vt->html, tmpbuf);
			utf8len = strlen(utf8str);		
			if (utf8len) {
				gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
			}	
		} 
		
		utf8str = get_viewtext_text(vt->mod_name, tmpkey);
		if (strlen(utf8str)) {
			
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, strlen(utf8str));
			
			if (settings.versestyle) {
				if ((strstr(utf8str , "<BR>") == NULL ) && (strstr(utf8str, "<!P>") == NULL))  {
					sprintf(tmpbuf, " %s", "</font><br>");
				} else {
					sprintf(tmpbuf, " %s", "</font>");
				}
				if (strstr(utf8str, "<!P>") == NULL) {
					newparagraph = FALSE;
				} else {
					newparagraph = TRUE;
				}
			} 
			
			else
				if (strstr(utf8str, "<!P>") == NULL)
					sprintf(tmpbuf, " %s", "</font>");
				else 
					sprintf(tmpbuf, " %s", "</font><p>");
					
			free(utf8str);
				
			utf8str = e_utf8_from_gtk_string(vt->html, tmpbuf);
			utf8len = strlen(utf8str);		
			if (utf8len) {
				gtk_html_write(html, htmlstream, utf8str, utf8len);
			}	
		}
		
		sprintf(buf, "%s", "</font>");	
		utf8str = e_utf8_from_gtk_string(vt->html, buf);
		utf8len = strlen(utf8str);		
		if (utf8len) {
			gtk_html_write(GTK_HTML(html), htmlstream, utf8str, utf8len);
		}
		
	}
	sprintf(buf, "%s", "</body></html>");
	utf8str = e_utf8_from_gtk_string(vt->html, buf);
	utf8len = strlen(utf8str);       
	if (utf8len) {
		gtk_html_write(GTK_HTML(html), htmlstream, utf8str,
			       utf8len);
	}

	gtk_html_end(GTK_HTML(html), htmlstream, status1);
	gtk_html_set_editable(html, was_editable);
	sprintf(buf, "%d", cur_verse);
	gtk_html_jump_to_anchor(html, buf);		
	g_free(tmpkey);
}

/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void update_controls(VIEW_TEXT * vt)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

static void update_controls(VIEW_TEXT * vt)
{
	const gchar *buf;

	buf = get_book_from_key(vt->key); //get_book_viewtext(vt->mod_name);
	gtk_entry_set_text(GTK_ENTRY(vt->cbe_book), buf);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(vt->spb_chapter),
				  get_chapter_from_key(vt->key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON(vt->spb_verse),
				  get_verse_from_key(vt->key));
}

/******************************************************************************
 * Name
 *   link_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void link_clicked(GtkHTML * html, const gchar * url, 
 *							gpointer data)	
 *
 * Description
 *   html link clicked
 *
 * Return value
 *   void
 */

static void link_clicked(GtkHTML * html, const gchar * url, 
							VIEW_TEXT * vt)
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
		chapter_display(vt, buf);
		update_controls(vt);
		g_free(buf);
	}

	else if (!strncmp(url, "passage=", 7)) {/*** remove passage= verse list ***/
		gchar *mybuf = NULL;
		gchar *modbuf = NULL;
		mybuf = strchr(url, '=');
		++mybuf;
		buf = g_strdup(mybuf);
		settings.whichwindow = 0;
		modbuf = get_module_name();
		gui_display_verse_list(modbuf, buf);
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
 *   cbe_book_changed
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void cbe_book_changed(GtkEditable * editable,
 *					       gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void cbe_book_changed(GtkEditable * editable,
					       VIEW_TEXT * vt)
{

}

/******************************************************************************
 * Name
 *   btn_goto_verse_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_goto_verse_clicked(GtkButton * button,
 *						gpointer user_data)
 *
 * Description
 *   change verse
 *
 * Return value
 *   void
 */

static void btn_goto_verse_clicked(GtkButton * button,
						VIEW_TEXT * vt)
{
	gchar *bookname, buf[120], *val_key;
	gint iChap, iVerse;

	bookname = gtk_entry_get_text(GTK_ENTRY(vt->cbe_book));
	iChap =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (vt->spb_chapter));
	iVerse =
	    gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON
					     (vt->spb_verse));
	sprintf(buf, "%s %d:%d", bookname, iChap, iVerse);
	val_key = get_valid_key(buf);
	//g_warning(buf);
	chapter_display(vt, val_key);
	update_controls(vt);
	g_free(val_key);
}

/******************************************************************************
 * Name
 *   cbe_module_changed
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void (cbe_module_changedGtkEditable * editable, 
 *						gpointer user_data)	
 *
 * Description
 *   change to new module
 *
 * Return value
 *   void
 */

static void module_new_activate(GtkMenuItem * menuitem, 
						gpointer user_data)
{	
	gui_open_bibletext_dialog(GPOINTER_TO_INT(user_data));
}

/******************************************************************************
 * Name
 *   btn_sync_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_sync_clicked(GtkButton * button, gpointer user_data)	
 *
 * Description
 *   set text to main window current verse
 *
 * Return value
 *   void
 */

static void btn_sync_clicked(GtkButton * button, VIEW_TEXT * vt)
{
	chapter_display(vt,settings.currentverse);
	update_controls(vt);
}

/******************************************************************************
 * Name
 *   btn_close_clicked
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void btn_close_clicked(GtkButton * button,VIEW_TEXT * vt)	
 *
 * Description
 *   call gtk_widget_destroy to destroy the bibletext dialog
 *   set vt->dialog to NULL
 *
 * Return value
 *   void
 */

static void btn_close_clicked(GtkButton * button, VIEW_TEXT * vt)
{
	gtk_widget_destroy(vt->dialog);
	vt->dialog = NULL;
}

/******************************************************************************
 * Name
 *   mod_ops_activate
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   	void mod_ops_activate(GtkMenuItem * menuitem,
 *					       gpointer user_data)
 *
 * Description
 *   set SWORD module global options
 *
 * Return value
 *   void
 */

static void mod_ops_activate(GtkMenuItem * menuitem,
					       gpointer user_data)
{
	set_global_options_viewtext((gchar *) user_data, 
		GTK_CHECK_MENU_ITEM(menuitem)->active);
	chapter_display(cur_vt, cur_vt->key);
}

/******************************************************************************
 * Name
 *   add_items_to_options_menu
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void add_items_to_options_menu(GtkWidget * shellmenu)	
 *
 * Description
 *   add sword global options to menu
 *
 * Return value
 *   void
 */

static void add_items_to_options_menu(GtkWidget * shellmenu)
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
				   GTK_SIGNAL_FUNC(mod_ops_activate),
				   (gchar *) (gchar *) tmp->data);
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
				      GTK_WIDGET(menuChoice), 1);

		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

/******************************************************************************
 * Name
 *   add_items_to_module_menu
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void add_items_to_module_menu(GtkWidget * shellmenu)	
 *
 * Description
 *   add bibletext module names to menu
 *
 * Return value
 *   void
 */

static void add_items_to_module_menu(GtkWidget * shellmenu)
{
	GtkWidget *menuChoice;
	gchar menuName[64];
	int view_number = 0;
	GList *tmp;

	tmp = NULL;

	tmp = get_list(TEXT_LIST);
	while (tmp != NULL) {

		/* add module name items to menu */
		menuChoice =
		    gtk_check_menu_item_new_with_label((gchar *) (gchar
								  *)
						       tmp->data);
		sprintf(menuName, "ModuleNum%d", view_number);
		gtk_object_set_data(GTK_OBJECT(settings.app), menuName,
				    menuChoice);
		gtk_widget_show(menuChoice);
		gtk_signal_connect(GTK_OBJECT(menuChoice), "activate",
				   GTK_SIGNAL_FUNC(module_new_activate),
				   GINT_TO_POINTER(view_number));
		gtk_menu_shell_insert(GTK_MENU_SHELL(shellmenu),
				      GTK_WIDGET(menuChoice), view_number);
		++view_number;
		tmp = g_list_next(tmp);
	}
	g_list_free(tmp);
}

static gboolean on_dialog_motion_notify_event(GtkWidget *widget,
                      GdkEventMotion  *event, VIEW_TEXT * vt)
{
	cur_vt = vt;
	return TRUE;
}

/******************************************************************************
 * Name
 *   create_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void create_bibletext_dialog(VIEW_TEXT * vt)	
 *
 * Description
 *   viewtext gui
 *
 * Return value
 *   void 
 */

static void create_bibletext_dialog(VIEW_TEXT * vt)
{

	GtkWidget *dialog_vbox14;
	GtkWidget *vbox33;
	GtkWidget *toolbar29;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator16;
	GtkWidget *menubar;
	GtkWidget *module_options;
	GtkWidget *module_options_menu;
	GtkAccelGroup *module_options_menu_accels;
	GtkWidget *module_new;
	GtkWidget *module_new_menu;
	GtkAccelGroup *module_new_menu_accels;
	GtkWidget *toolbar30;
	GtkWidget *combo11;
	GtkObject *spbVTChapter_adj;
	GtkObject *spbVTVerse_adj;
	GtkWidget *btnGotoVerse;
	GtkWidget *frame21;
	GtkWidget *swVText;
	GtkWidget *dialog_action_area14;
	gchar title[256];
	
	sprintf(title,"%s - %s",vt->mod_name, _("GnomeSword"));

	vt->dialog = gnome_dialog_new(title, NULL);
	gtk_object_set_data(GTK_OBJECT(vt->dialog), "vt->dialog",
			    vt->dialog);
	gtk_window_set_default_size(GTK_WINDOW(vt->dialog), 370, 412);
	gtk_window_set_policy(GTK_WINDOW(vt->dialog), TRUE, TRUE,
			      FALSE);
	
	dialog_vbox14 = GNOME_DIALOG(vt->dialog)->vbox;
	gtk_object_set_data(GTK_OBJECT(vt->dialog), "dialog_vbox14",
			    dialog_vbox14);
	gtk_widget_show(dialog_vbox14);

	vbox33 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox33);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vbox33",
				 vbox33,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox33);
	gtk_box_pack_start(GTK_BOX(dialog_vbox14), vbox33, TRUE, TRUE,
			   0);

	toolbar29 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar29);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "toolbar29",
				 toolbar29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar29);
	gtk_box_pack_start(GTK_BOX(vbox33), toolbar29, FALSE, FALSE, 0);
	gtk_toolbar_set_space_size(GTK_TOOLBAR(toolbar29), 3);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar29),
				      GTK_RELIEF_NONE);
	
	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vt->dialog,
				      GNOME_STOCK_PIXMAP_REFRESH);
	vt->btn_sync =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar29),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Sync"),
				       _("Sync with main window"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(vt->btn_sync);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->btn_sync",
				 vt->btn_sync,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->btn_sync);

	vseparator16 = gtk_vseparator_new();
	gtk_widget_ref(vseparator16);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vseparator16", vseparator16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator16);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), vseparator16,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator16, 7, 13);

	menubar = gtk_menu_bar_new();
	gtk_widget_ref(menubar);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "menubar",
				 menubar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), menubar, NULL,
				  NULL);

	module_options =
	    gtk_menu_item_new_with_label(_("Module Options"));
	gtk_widget_ref(module_options);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "module_options", module_options,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(module_options);
	gtk_container_add(GTK_CONTAINER(menubar), module_options);

	module_options_menu = gtk_menu_new();
	gtk_widget_ref(module_options_menu);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "module_options_menu",
				 module_options_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_options),
				  module_options_menu);
	module_options_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (module_options_menu));

	add_items_to_options_menu(module_options_menu);



	vseparator16 = gtk_vseparator_new();
	gtk_widget_ref(vseparator16);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vseparator16", vseparator16,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator16);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), vseparator16,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator16, 7, 13);

	menubar = gtk_menu_bar_new();
	gtk_widget_ref(menubar);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "menubar",
				 menubar,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(menubar);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar29), menubar, NULL,
				  NULL);

	module_new =
	    gtk_menu_item_new_with_label(_("Open New Module "));
	gtk_widget_ref(module_new);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "module_new", module_new,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(module_new);
	gtk_container_add(GTK_CONTAINER(menubar), module_new);

	module_new_menu = gtk_menu_new();
	gtk_widget_ref(module_new_menu);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "module_new_menu",
				 module_new_menu,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(module_new),
				  module_new_menu);
	module_new_menu_accels =
	    gtk_menu_ensure_uline_accel_group(GTK_MENU
					      (module_new_menu));

	add_items_to_module_menu(module_new_menu);



	toolbar30 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar30);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "toolbar30",
				 toolbar30,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar30);
	gtk_box_pack_start(GTK_BOX(vbox33), toolbar30, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar30),
				      GTK_RELIEF_NONE);

	combo11 = gtk_combo_new();
	gtk_widget_ref(combo11);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "combo11",
				 combo11,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(combo11);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30), combo11, NULL,
				  NULL);

	vt->cbe_book = GTK_COMBO(combo11)->entry;
	gtk_widget_ref(vt->cbe_book);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->cbe_book",
				 vt->cbe_book,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->cbe_book);


	spbVTChapter_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
	vt->spb_chapter =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVTChapter_adj), 1, 0);
	gtk_widget_ref(vt->spb_chapter);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "vt->spb_chapter", vt->spb_chapter,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->spb_chapter);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30), vt->spb_chapter,
				  NULL, NULL);
	gtk_widget_set_usize(vt->spb_chapter, 53, -2);
	gtk_spin_button_set_numeric(GTK_SPIN_BUTTON(vt->spb_chapter),
				    TRUE);

	spbVTVerse_adj = gtk_adjustment_new(1, 0, 100, 1, 10, 10);
	vt->spb_verse =
	    gtk_spin_button_new(GTK_ADJUSTMENT(spbVTVerse_adj), 1, 0);
	gtk_widget_ref(vt->spb_verse);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->spb_verse",
				 vt->spb_verse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->spb_verse);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar30),
				  GTK_WIDGET(vt->spb_verse), NULL, NULL);
	gtk_widget_set_usize(vt->spb_verse, 51, -2);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(vt->dialog,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnGotoVerse =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar30),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"), _("Go to verse"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnGotoVerse);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog),
				 "btnGotoVerse", btnGotoVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnGotoVerse);

	frame21 = gtk_frame_new(NULL);
	gtk_widget_ref(frame21);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "frame21",
				 frame21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame21);
	gtk_box_pack_start(GTK_BOX(vbox33), frame21, TRUE, TRUE, 0);
	gtk_widget_set_usize(frame21, -2, 400);

	swVText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swVText);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "swVText",
				 swVText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swVText);
	gtk_container_add(GTK_CONTAINER(frame21), swVText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swVText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	vt->html = gtk_html_new();
	gtk_widget_ref(vt->html);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->html", vt->html,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->html);
	gtk_container_add(GTK_CONTAINER(swVText), vt->html);

	dialog_action_area14 = GNOME_DIALOG(vt->dialog)->action_area;
	gtk_object_set_data(GTK_OBJECT(vt->dialog),
			    "dialog_action_area14",
			    dialog_action_area14);
	gtk_widget_show(dialog_action_area14);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area14),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area14),
				   8);

	gnome_dialog_append_button(GNOME_DIALOG(vt->dialog),
				   GNOME_STOCK_BUTTON_CLOSE);
	vt->btn_close =
	    GTK_WIDGET(g_list_last(GNOME_DIALOG(vt->dialog)->buttons)->
		       data);
	gtk_widget_ref(vt->btn_close);
	gtk_object_set_data_full(GTK_OBJECT(vt->dialog), "vt->btn_close",
				 vt->btn_close,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vt->btn_close);
	GTK_WIDGET_SET_FLAGS(vt->btn_close, GTK_CAN_DEFAULT);

	gtk_signal_connect (GTK_OBJECT (vt->dialog), "motion_notify_event",
			   GTK_SIGNAL_FUNC (on_dialog_motion_notify_event),
			   vt);
	gtk_signal_connect(GTK_OBJECT(vt->html), "link_clicked",
			   GTK_SIGNAL_FUNC(link_clicked), 
			   vt);
	gtk_signal_connect(GTK_OBJECT(vt->btn_sync), "clicked",
			   GTK_SIGNAL_FUNC(btn_sync_clicked), 
			   vt);
	gtk_signal_connect(GTK_OBJECT(vt->cbe_book), "changed",
			   GTK_SIGNAL_FUNC(cbe_book_changed), 
			   vt);
	gtk_signal_connect(GTK_OBJECT(btnGotoVerse), "clicked",
			   GTK_SIGNAL_FUNC(btn_goto_verse_clicked),
			   vt);
	gtk_signal_connect(GTK_OBJECT(vt->btn_close), "clicked",
			   GTK_SIGNAL_FUNC(btn_close_clicked),
			   vt);	
	
	//gtk_combo_set_popdown_strings(GTK_COMBO(vt->combo_module), get_list(TEXT_LIST));
	
	//gtk_entry_set_text(GTK_ENTRY(vt->cbe_module), settings.MainWindowModule);
	
	gtk_combo_set_popdown_strings(GTK_COMBO(combo11), get_list(BOOKS_LIST));
	chapter_display(vt,settings.currentverse);
	sprintf(vt_current_verse, "%s", settings.currentverse);
	update_controls(vt);
	isrunningVT = TRUE;
}

/******************************************************************************
 * Name
 *   gui_open_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_open_bibletext_dialog(gint mod_num)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_open_bibletext_dialog(gint mod_num)
{		
	VIEW_TEXT * vt;
	
	vt = (VIEW_TEXT *) g_list_nth_data(dialog_list, mod_num);	
	
	g_warning(vt->mod_name);
	if(vt->dialog == NULL){
		create_bibletext_dialog(vt);
		gtk_widget_show(vt->dialog);
	}
	else {
		gtk_widget_show(vt->dialog);
		gdk_window_raise(GTK_WIDGET(vt->dialog)->window);
	}
}

/******************************************************************************
 * Name
 *   gui_setup_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_setup_bibletext_dialog(GList *mods)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_setup_bibletext_dialog(GList *mods)
{
	VIEW_TEXT * vt;
	GList *tmp = NULL;
	gint count = 0;
	
	dialog_list = NULL;
	
	tmp = mods;
	tmp = g_list_first(tmp);
	while (tmp != NULL) {
		vt = g_new(VIEW_TEXT, 1);
		vt->dialog = NULL;
		vt->mod_name = (gchar *) tmp->data;
		vt->dialog_number = count;
		++count;
		dialog_list = g_list_append(dialog_list, (VIEW_TEXT *) vt);
		tmp = g_list_next(tmp);
	}
}

/******************************************************************************
 * Name
 *  gui_shutdown_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void gui_shutdown_bibletext_dialog(void)	
 *
 * Description
 *   shut down bibletext dialog support clean mem
 *
 * Return value
 *   void
 */

void gui_shutdown_bibletext_dialog(void) 
{
	dialog_list = g_list_first(dialog_list);
	while (dialog_list != NULL) {
		VIEW_TEXT *vt = (VIEW_TEXT *) dialog_list->data;
		/* 
		 *  destroy any dialogs created 
		 */
		if (vt->dialog)
			 gtk_widget_destroy(vt->dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		g_free((VIEW_TEXT *) dialog_list->data);
		dialog_list = g_list_next(dialog_list);
	} g_list_free(dialog_list);
}

/******   end of file   ******/
