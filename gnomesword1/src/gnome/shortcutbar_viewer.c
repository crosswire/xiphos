/*
 * GnomeSword Bible Study Tool
 * shortcutbar_viewer.c - create viewer group gui
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
#include <gal/e-paned/e-hpaned.h>
#include <gal/widgets/e-unicode.h>

#include "gui/gtkhtml_display.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"

#include "main/gs_gnomesword.h"
#include "main/gs_bookmarks.h"
#include "main/gs_html.h"
#include "main/support.h"
#include "main/settings.h"
#include "main/dictlex.h"

extern SEARCH_OPT so, *p_so;
extern GList *sblist;			/* for saving search results to bookmarks  */
extern gint groupnum7;

SB_VIEWER sb_v, *sv ;


/******************************************************************************
 * Name
 *   set_sb_for_daily_devotion 
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void set_sb_for_daily_devotion(void)	
 *
 * Description
 *   set shortcut bar for displaying Daily Devotion
 *
 * Return value
 *   void
 */

void set_sb_for_daily_devotion(void)
{
	showSBGroup(groupnum7);
	changegroupnameSB(_("Daily Devotion"), groupnum7);
}


/******************************************************************************
 * Name
 *   showSBVerseList 
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void showSBVerseList(void)
 *
 * Description
 *   set shortcut bar to display verse list group
 *
 * Return value
 *   void
 */

void showSBVerseList(void)
{
	showSBGroup(groupnum7);
	changegroupnameSB(settings.groupName, groupnum7);
}

/******************************************************************************
 * Name
 *    gui_display_dictlex_in_viewer
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   gboolean gui_display_dictlex_in_viewer(char *modName, char *key) 
 *
 * Description
 *   
 *
 * Return value
 *   gboolean
 */

gboolean gui_display_dictlex_in_viewer(char *mod_name, char *key) 
{
	if (settings.showshortcutbar) {	
		gchar *text;
		gtk_notebook_set_page(GTK_NOTEBOOK(
			settings.verse_list_notebook), 2);
		sprintf(settings.groupName, "%s", "Viewer");
		showSBVerseList();
		text = get_dictlex_text(mod_name, key);
		if(text) {
			entry_display(sv->html_viewer_widget, mod_name,
				   text, key, TRUE);
			free(text);
			return TRUE;
		}
	}
	return FALSE;
}

/******************************************************************************
 * Name
 *   on_vllink_clicked 
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void on_vllink_clicked(GtkHTML *html, const gchar *url,
 *						gpointer user_data)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void verse_list_link_clicked(GtkHTML *html, const gchar *url,
		SB_VIEWER *sv)
{
	int type;
	gchar *text = NULL;
	
	type = get_mod_type(sv->mod_name);
	switch(type){
		case TEXT_TYPE:			
			chapter_display(sv->htmlshow, sv->mod_name,
				NULL, (gchar *) url, FALSE);
			break;
		case COMMENTARY_TYPE:		
		case DICTIONARY_TYPE:	
			text = get_module_text(sv->mod_name, (gchar *) url);
			if(text) {
				entry_display(sv->htmlshow, sv->mod_name,
				   text, (gchar *) url, TRUE);
				free(text);
			}
			break;
		default:
	}
	
	if(settings.showinmain)
		change_verse((gchar *) url);	
}

/******************************************************************************
 * Name
 *   gui_display_verse_list 
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void gui_display_verse_list(gchar *module_name, gchar *verse_list)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_display_verse_list(gchar * module_name, gchar * verse_list)
{
	GList * tmp = NULL;
	gboolean oddkey = TRUE;
	gchar 
		buf[256], 
		*utf8str,
		*colorkey;
	gchar *first_key = NULL;
	gint    i = 0;
	
	strcpy(sv->mod_name, module_name);
	tmp = get_verse_list(module_name, verse_list);
	
	beginHTML(settings.vlsbhtml, TRUE);
	sprintf(buf,"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><font color=\"%s\"><b>[%s]</b><br></font>",
			settings.bible_bg_color, 
			settings.bible_text_color,
			settings.link_color,
			settings.bible_verse_num_color,
			module_name);
	utf8str = e_utf8_from_gtk_string(settings.vlsbhtml, buf);
	displayHTML(settings.vlsbhtml, utf8str, strlen(utf8str));
	
	while(tmp != NULL) {		
		if(oddkey){
			colorkey = settings.link_color;
			oddkey = FALSE;
		}else{
			colorkey = settings.bible_text_color;
			oddkey = TRUE;
		}
		sprintf(buf,"<a href=\"%s\"><font color=\"%s\"size=\"%s\">%s</font></a><br>",
					(const char *)tmp->data,
					colorkey,
					settings.verselist_font_size,
					(const char *)tmp->data);
		if(i == 0)
			first_key = g_strdup((const char *)tmp->data);
		++i;
		utf8str = e_utf8_from_gtk_string(settings.vlsbhtml, buf);
		displayHTML(settings.vlsbhtml, utf8str, strlen(utf8str));
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	sprintf(buf,"</table></body</html>");	
	utf8str = e_utf8_from_gtk_string(settings.vlsbhtml, buf);
	displayHTML(settings.vlsbhtml, utf8str, strlen(utf8str));
	endHTML(settings.vlsbhtml);
	
	showSBVerseList();
	gtk_notebook_set_page(GTK_NOTEBOOK
				(settings.verse_list_notebook), 0);
	if(first_key) {
		verse_list_link_clicked(NULL, 
			(const gchar *)first_key, sv);
		g_free(first_key);
	}
}

/******************************************************************************
 * Name
 *    change_viewer_page
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void change_viewer_page(GtkWidget *notebook, gint page)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void change_viewer_page(GtkWidget *notebook, gint page)
{	
	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), page);
	switch(page) {
		case 0:
			changegroupnameSB(_("Verse List"), groupnum7);
			break;
		case 1:
			changegroupnameSB(_("Search Results"), groupnum7);
			break;
		case 2:
			changegroupnameSB(_("Viewer"), groupnum7);
			break;
			
	}	
}

/******************************************************************************
 * Name
 *    show_search_results_in_main
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void show_search_results_in_main(gboolean show, gchar *key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void show_search_results_in_main(gboolean show, gchar *key)
{

	if (show) {
		if (GTK_TOGGLE_BUTTON(p_so->ckbCommentary)->active) {
			change_module_and_key(p_so->module_name, key);
		}

		else if (GTK_TOGGLE_BUTTON(p_so->ckbPerCom)->active) {
			change_module_and_key(p_so->module_name, key);
		}

		else if (GTK_TOGGLE_BUTTON(p_so->ckbGBS)->active) {
			change_module_and_key(p_so->module_name, key);
		}

		else
			change_verse(key);
	}
}

/******************************************************************************
 * Name
 *    on_clistSearchResults_select_row
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void on_clistSearchResults_select_row(GtkCList *clist, gint row,
 *			gint column, GdkEvent *event)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void on_clistSearchResults_select_row(GtkCList *clist, gint row,
		gint column, GdkEvent *event)
{
	gchar *key, *text = NULL;

	gtk_clist_get_text(GTK_CLIST(clist), row, 0, &key);
	settings.displaySearchResults = TRUE;
	
	//display_search_results_item(key);
	text = get_search_results_text(p_so->module_name, key);
	if(text) {
		entry_display(sv->html_widget, p_so->module_name,
		   text, key, TRUE);
		free(text);
	}
	if (settings.showinmain)
		show_search_results_in_main(settings.showinmain,key);
	settings.displaySearchResults = FALSE;
}

/******************************************************************************
 * Name
 *   on_btnSBSaveVL_clicked 
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   	void on_btnSBSaveVL_clicked(GtkButton * button,
 *				   gpointer user_data)
 *
 * Description
 *   save verse list as bookmarks
 *
 * Return value
 *   void
 */

static void on_btnSBSaveVL_clicked(GtkButton * button,
				   gpointer user_data)
{
	addverselistBM(sblist);
}

/******************************************************************************
 * Name
 *   on_nbVL_switch_page 
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void on_nbVL_switch_page(GtkNotebook * notebook,
 *		GtkNotebookPage * page,	gint page_num, SB_VIEWER *sv)	
 *
 * Description
 *   viewer notebook page has changed - change button sensitivity
 *
 * Return value
 *   void
 */

static void on_nbVL_switch_page(GtkNotebook * notebook,
		GtkNotebookPage * page,	gint page_num, SB_VIEWER *sv)
{
	switch (page_num) {
	case 0:
		gtk_widget_set_sensitive(sv->btn_save,FALSE);
		gtk_widget_set_sensitive(sv->tbtn_view_main,TRUE);
		break;
	case 1:
		gtk_widget_set_sensitive(sv->btn_save, TRUE);
		gtk_widget_set_sensitive(sv->tbtn_view_main, TRUE);
		break;
	case 2:
		gtk_widget_set_sensitive(sv->btn_save, FALSE);
		gtk_widget_set_sensitive(sv->tbtn_view_main, FALSE);
		break;
	}
}

/******************************************************************************
 * Name
 *   on_tbtnSBViewMain_toggled 
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   	void on_tbtnSBViewMain_toggled(GtkToggleButton * togglebutton,
 *				      gpointer user_data)
 *
 * Description
 *   toggle settings.showinmain
 *
 * Return value
 *   void
 */

static void on_tbtnSBViewMain_toggled(GtkToggleButton *togglebutton,
				      gpointer user_data)
{
	settings.showinmain = togglebutton->active;
}

/******************************************************************************
 * Name
 *    on_btnViewVL_clicked
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void on_btnViewVL_clicked(GtkButton * button, 
 *					gpointer user_data)	
 *
 * Description
 *   change page to verse list
 *
 * Return value
 *   void
 */

static void on_btnViewVL_clicked(GtkButton * button, 
					gpointer user_data)
{
	change_viewer_page(GTK_WIDGET(user_data), 0);
}

/******************************************************************************
 * Name
 *    on_btnViewSR_clicked
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void on_btnViewSR_clicked(GtkButton * button, 
 *					gpointer user_data)	
 *
 * Description
 *   change page to search results
 *
 * Return value
 *   void
 */

static void on_btnViewSR_clicked(GtkButton * button, 
					gpointer user_data)
{
	change_viewer_page(GTK_WIDGET(user_data), 1);
}

/******************************************************************************
 * Name
 *    on_btnViewer_clicked
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   void on_btnViewer_clicked(GtkButton * button, 
 *					gpointer user_data)
 *
 * Description
 *   change page to viewer
 *
 * Return value
 *   void
 */

static void on_btnViewer_clicked(GtkButton * button, 
					gpointer user_data)
{
	change_viewer_page(GTK_WIDGET(user_data), 2);
}

/******************************************************************************
 * Name
 *    gui_create_shortcutbar_viewer
 *
 * Synopsis
 *   #include "shortcutbar_viewer.h"
 *
 *   GtkWidget * gui_create_shortcutbar_viewer(GtkWidget *vboxVL) 
 *
 * Description
 *   create shortcut bar viewer group (viewer, search results and verse list)
 *
 * Return value
 *   GtkWidget *
 */

GtkWidget * gui_create_shortcutbar_viewer(GtkWidget *vboxVL) 
{
	GtkWidget *toolbarVL;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *vseparator1;
	GtkWidget *btnViewVL;
	GtkWidget *btnViewSR;
	GtkWidget *btnViewer;
	GtkWidget *vbox2;
	GtkWidget *frame1;
	GtkWidget *scrolledwindow1;
	GtkWidget *frame2;
	GtkWidget *scrolledwindow2;
	GtkWidget *label1;
	GtkWidget *vbox3;
	GtkWidget *vpaned_srch_rslt;
	GtkWidget *frame3;
	GtkWidget *scrolledwindow3;
	GtkWidget *frame4;
	GtkWidget *scrolledwindow4;
	GtkWidget *label2;
	GtkWidget *frame5;
	GtkWidget *label3;
	GtkWidget *scrolledwindow5;
	GtkWidget *frameTB;
	GtkWidget *frameRP;
	GtkWidget *scrolledwindowRP;
	
	
	sv = &sb_v;
	
	frameTB = gtk_frame_new(NULL);
	gtk_widget_ref(frameTB);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frameTB",
				 frameTB, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frameTB);
	gtk_box_pack_start(GTK_BOX(vboxVL), frameTB, FALSE, TRUE, 0);

	toolbarVL =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarVL);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "toolbarVL",
				 toolbarVL, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(toolbarVL);
	gtk_container_add(GTK_CONTAINER(frameTB), toolbarVL);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarVL),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app, GNOME_STOCK_PIXMAP_SAVE);
	sv->btn_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save List"),
				       _
				       ("Save the current verse list as a bookmark file"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(sv->btn_save);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "sv->btn_save",
				 sv->btn_save, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(sv->btn_save);
	gtk_widget_set_sensitive(sv->btn_save, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	sv->tbtn_view_main =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Main Form"),
				       _
				       ("Toggle to show results in Main Form"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(sv->tbtn_view_main);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "sv->tbtn_view_main", sv->tbtn_view_main,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(sv->tbtn_view_main);
	
	vseparator1 = gtk_vseparator_new();
	gtk_widget_ref(vseparator1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vseparator1",
				 vseparator1, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vseparator1);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarVL),
				  vseparator1, NULL, NULL);
	gtk_widget_set_usize(vseparator1, 12, 12);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_BOOK_RED);
	btnViewVL =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("VerseList"),
				       _("View Verse List"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnViewVL);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnViewVL",
				 btnViewVL, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(btnViewVL);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_BOOK_GREEN);
	btnViewSR =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("SearchResults"),
				       _("View Search Results"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnViewSR);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnViewSR",
				 btnViewSR, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(btnViewSR);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(settings.app,
				      GNOME_STOCK_PIXMAP_BOOK_BLUE);
	btnViewer =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Viewer"), _("Viewer"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_ref(btnViewer);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnViewer",
				 btnViewer, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(btnViewer);

	sv->notebook = gtk_notebook_new();
	gtk_widget_ref(sv->notebook);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "sv->notebook", sv->notebook,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(sv->notebook);
	gtk_box_pack_start(GTK_BOX(vboxVL), sv->notebook, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(sv->notebook, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(sv->notebook), FALSE);

	settings.verse_list_notebook = sv->notebook;
	
	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox2",
				 vbox2, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(sv->notebook), vbox2);

	frame1 = gtk_frame_new(NULL);
	gtk_widget_ref(frame1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame1",
				 frame1, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_box_pack_start(GTK_BOX(vbox2), frame1, FALSE, TRUE, 0);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow1", scrolledwindow1,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	gtk_container_add(GTK_CONTAINER(frame1), scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(scrolledwindow1, -2, 75);

	settings.vlsbhtml = gtk_html_new();
	gtk_widget_ref(settings.vlsbhtml);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "settings.vlsbhtml", settings.vlsbhtml,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(settings.vlsbhtml);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), settings.vlsbhtml);
	gtk_html_load_empty(GTK_HTML(settings.vlsbhtml));

	frame2 = gtk_frame_new(NULL);
	gtk_widget_ref(frame2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame2",
				 frame2, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox2), frame2, TRUE, TRUE, 0);

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow2", scrolledwindow2,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow2);
	gtk_container_add(GTK_CONTAINER(frame2), scrolledwindow2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow2),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	sv->htmlshow = gtk_html_new();
	gtk_widget_ref(sv->htmlshow);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "sv->htmlshow", sv->htmlshow,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(sv->htmlshow);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), sv->htmlshow);
	gtk_html_load_empty(GTK_HTML(sv->htmlshow));

	label1 = gtk_label_new(_("label1"));
	gtk_widget_ref(label1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label1",
				 label1, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label1);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(sv->notebook),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(sv->notebook), 0), label1);

//-------------------------------------------------------------------------

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox3",
				 vbox3, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(sv->notebook), vbox3);

	frameRP = gtk_frame_new(NULL);
	gtk_widget_ref(frameRP);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frameRP",
				 frameRP, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frameRP);
	gtk_box_pack_start(GTK_BOX(vbox3), frameRP, FALSE, TRUE, 0);

	scrolledwindowRP = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindowRP);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindowRP", scrolledwindowRP,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindowRP);
	gtk_container_add(GTK_CONTAINER(frameRP), scrolledwindowRP);
	gtk_widget_set_usize(scrolledwindowRP, -2, 65);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowRP),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_NEVER);

	settings.htmlRP = gtk_html_new();
	gtk_widget_ref(settings.htmlRP);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "settings.htmlRP", settings.htmlRP,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(settings.htmlRP);
	gtk_container_add(GTK_CONTAINER(scrolledwindowRP), settings.htmlRP);
	gtk_html_load_empty(GTK_HTML(settings.htmlRP));

	vpaned_srch_rslt = gtk_vpaned_new();
	gtk_widget_ref(vpaned_srch_rslt);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "vpaned_srch_rslt", vpaned_srch_rslt,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vpaned_srch_rslt);
	gtk_box_pack_start(GTK_BOX(vbox3), vpaned_srch_rslt, TRUE,
			   TRUE, 0);
	gtk_paned_set_position(GTK_PANED(vpaned_srch_rslt), 100);

	frame3 = gtk_frame_new(NULL);
	gtk_widget_ref(frame3);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame3",
				 frame3, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame3);
	gtk_paned_pack1(GTK_PANED(vpaned_srch_rslt), frame3, TRUE,
			TRUE);

	scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow3);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow3", scrolledwindow3,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow3);
	gtk_container_add(GTK_CONTAINER(frame3), scrolledwindow3);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow3),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	sv->clist = gtk_clist_new(1);
	gtk_widget_ref(sv->clist);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "sv->clist",
				 sv->clist, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(sv->clist);
	gtk_container_add(GTK_CONTAINER(scrolledwindow3),
			  sv->clist);
	gtk_clist_set_column_width(GTK_CLIST(sv->clist),
				   0, 100);
	gtk_clist_column_titles_hide(GTK_CLIST(sv->clist));

	frame4 = gtk_frame_new(NULL);
	gtk_widget_ref(frame4);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame4",
				 frame4, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame4);
	gtk_paned_pack2(GTK_PANED(vpaned_srch_rslt), frame4, TRUE,
			TRUE);

	scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow4);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow4", scrolledwindow4,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow4);
	gtk_container_add(GTK_CONTAINER(frame4), scrolledwindow4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow4),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);


	sv->html_widget = gtk_html_new();
	gtk_widget_ref(sv->html_widget);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "sv->html_widget", sv->html_widget,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(sv->html_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4), sv->html_widget);
	gtk_html_load_empty(GTK_HTML(sv->html_widget));
	
	

	label2 = gtk_label_new(_("label2"));
	gtk_widget_ref(label2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label2",
				 label2, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(sv->notebook),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(sv->notebook), 1), label2);

//-------------------------------------------------------------------------

	frame5 = gtk_frame_new(NULL);
	gtk_widget_ref(frame5);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame5",
				 frame5, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame5);
	gtk_container_add(GTK_CONTAINER(sv->notebook), frame5);

	scrolledwindow5 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow5);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "scrolledwindow5", scrolledwindow5,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(scrolledwindow5);
	gtk_container_add(GTK_CONTAINER(frame5), scrolledwindow5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow5),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	sv->html_viewer_widget = gtk_html_new();
	gtk_widget_ref(sv->html_viewer_widget);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "sv->html_viewer_widget", sv->html_viewer_widget,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(sv->html_viewer_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow5), sv->html_viewer_widget);
	gtk_html_load_empty(GTK_HTML(sv->html_viewer_widget));
		
	label3 = gtk_label_new(_("label3"));
	gtk_widget_ref(label3);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label3",
				 label3, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(sv->notebook),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(sv->notebook), 2), label3);

	gtk_signal_connect(GTK_OBJECT(settings.vlsbhtml), "link_clicked",
			   GTK_SIGNAL_FUNC(verse_list_link_clicked), sv);
			   
	gtk_signal_connect(GTK_OBJECT(sv->btn_save), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSBSaveVL_clicked), NULL);
			   
	gtk_signal_connect(GTK_OBJECT(sv->tbtn_view_main), "toggled",
			   GTK_SIGNAL_FUNC
			   (on_tbtnSBViewMain_toggled), NULL);
			   
	gtk_signal_connect(GTK_OBJECT(btnViewVL), "clicked",
			   GTK_SIGNAL_FUNC(on_btnViewVL_clicked),
			   GTK_WIDGET(sv->notebook));
			   
	gtk_signal_connect(GTK_OBJECT(btnViewSR), "clicked",
			   GTK_SIGNAL_FUNC(on_btnViewSR_clicked),
			   GTK_WIDGET(sv->notebook));
			   
	gtk_signal_connect(GTK_OBJECT(btnViewer), "clicked",
			   GTK_SIGNAL_FUNC(on_btnViewer_clicked),
			   GTK_WIDGET(sv->notebook));
			   
	gtk_signal_connect(GTK_OBJECT(sv->notebook), "switch_page",
			   GTK_SIGNAL_FUNC(on_nbVL_switch_page), sv);
			   
	gtk_signal_connect(GTK_OBJECT(sv->clist),
			   "select_row",
			   GTK_SIGNAL_FUNC
			   (on_clistSearchResults_select_row), NULL);
			   
	gtk_signal_connect(GTK_OBJECT(sv->html_widget), "on_url",
			   GTK_SIGNAL_FUNC(on_url), settings.app);
	return sv->htmlshow;
}

