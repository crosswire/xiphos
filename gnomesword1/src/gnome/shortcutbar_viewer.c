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
#include <gal/e-paned/e-vpaned.h>
#include <gal/widgets/e-unicode.h>
#include <ctype.h>
#include <time.h>

#include "gui/gtkhtml_display.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/bookmarks.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"

#include "main/shortcutbar.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/dictlex.h"

#include "gnome/shortcutbar.h"

extern SEARCH_OPT so, *p_so;
extern GList *sblist;			/* for saving search results to bookmarks  */
extern gint groupnum7;
SB_VIEWER sb_v, *sv ;


static GtkWidget * verse_list_notebook;
static GtkWidget * vl_html;

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

static void set_sb_for_daily_devotion(void)
{
	showSBGroup(groupnum7);
	changegroupnameSB(_("Daily Devotion"), groupnum7);
}



/******************************************************************************
 * Name
 *  display_devotional
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void display_devotional(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void gui_display_devotional(void)
{
	gchar buf[80];
	time_t curtime;
	struct tm *loctime;

	/* 
	 * Get the current time. 
	 */
	curtime = time(NULL);

	/* 
	 * Convert it to local time representation. 
	 */
	loctime = localtime(&curtime);

	/* 
	 * Print it out in a nice format. 
	 */
	strftime(buf, 80, "%m.%d", loctime);

	gui_display_dictlex_in_viewer(settings.devotionalmod, buf);
	set_sb_for_daily_devotion();
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
			verse_list_notebook), 2);
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
			break;
	}
	
	if(settings.showinmain)
		gui_change_verse((gchar *) url);	
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
	
	gui_begin_html(vl_html, TRUE);
	sprintf(buf,"<html><body bgcolor=\"%s\" text=\"%s\" link=\"%s\"><font color=\"%s\"><b>[%s]</b><br></font>",
			settings.bible_bg_color, 
			settings.bible_text_color,
			settings.link_color,
			settings.bible_verse_num_color,
			module_name);
	utf8str = e_utf8_from_gtk_string(vl_html, buf);
	gui_display_html(vl_html, utf8str, strlen(utf8str));
	
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
		utf8str = e_utf8_from_gtk_string(vl_html, buf);
		gui_display_html(vl_html, utf8str, strlen(utf8str));
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	sprintf(buf,"</table></body</html>");	
	utf8str = e_utf8_from_gtk_string(vl_html, buf);
	gui_display_html(vl_html, utf8str, strlen(utf8str));
	gui_end_html(vl_html);
	
	showSBVerseList();
	gtk_notebook_set_page(GTK_NOTEBOOK
				(verse_list_notebook), 0);
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
		gui_change_module_and_key(p_so->module_name, key);
		/*if (GTK_TOGGLE_BUTTON(p_so->ckbCommentary)->active) {
			gui_change_module_and_key(p_so->module_name, key);
		}

		else if (GTK_TOGGLE_BUTTON(p_so->ckbPerCom)->active) {
			gui_change_module_and_key(p_so->module_name, key);
		}

		else if (GTK_TOGGLE_BUTTON(p_so->ckbGBS)->active) {
			gui_change_module_and_key(p_so->module_name, key);
		}

		else
			gui_change_verse(key);*/
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
	gui_verselist_to_bookmarks(sblist);
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
	gtk_widget_show(frameTB);
	gtk_box_pack_start(GTK_BOX(vboxVL), frameTB, FALSE, TRUE, 0);

	toolbarVL =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL,
			    GTK_TOOLBAR_ICONS);
	gtk_widget_show(toolbarVL);
	gtk_container_add(GTK_CONTAINER(frameTB), toolbarVL);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarVL),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app, GNOME_STOCK_PIXMAP_SAVE);
	sv->btn_save =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Save List"),
				       _
				       ("Save the current verse list as a bookmark file"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(sv->btn_save);
	gtk_widget_set_sensitive(sv->btn_save, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	sv->tbtn_view_main =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("Main Form"),
				       _
				       ("Toggle to show results in Main Form"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(sv->tbtn_view_main);
	
	vseparator1 = gtk_vseparator_new();
	gtk_widget_show(vseparator1);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarVL),
				  vseparator1, NULL, NULL);
	gtk_widget_set_usize(vseparator1, 12, 12);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_BOOK_RED);
	btnViewVL =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("VerseList"),
				       _("View Verse List"), NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_show(btnViewVL);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_BOOK_GREEN);
	btnViewSR =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("SearchResults"),
				       _("View Search Results"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(btnViewSR);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(widgets.app,
				      GNOME_STOCK_PIXMAP_BOOK_BLUE);
	btnViewer =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarVL),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("Viewer"), _("Viewer"),
				       NULL, tmp_toolbar_icon, NULL,
				       NULL);
	gtk_widget_show(btnViewer);

	sv->notebook = gtk_notebook_new();
	gtk_widget_show(sv->notebook);
	gtk_box_pack_start(GTK_BOX(vboxVL), sv->notebook, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(sv->notebook, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(sv->notebook), FALSE);

	verse_list_notebook = sv->notebook;
	
	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(sv->notebook), vbox2);

	frame1 = gtk_frame_new(NULL);
	gtk_widget_show(frame1);
	gtk_box_pack_start(GTK_BOX(vbox2), frame1, FALSE, TRUE, 0);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow1);
	gtk_container_add(GTK_CONTAINER(frame1), scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_widget_set_usize(scrolledwindow1, -2, 75);

	vl_html = gtk_html_new();
	gtk_widget_show(vl_html);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), vl_html);
	gtk_html_load_empty(GTK_HTML(vl_html));

	frame2 = gtk_frame_new(NULL);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox2), frame2, TRUE, TRUE, 0);

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow2);
	gtk_container_add(GTK_CONTAINER(frame2), scrolledwindow2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow2),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	sv->htmlshow = gtk_html_new();
	gtk_widget_show(sv->htmlshow);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), sv->htmlshow);
	gtk_html_load_empty(GTK_HTML(sv->htmlshow));

	label1 = gtk_label_new("");
	gtk_widget_show(label1);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(sv->notebook),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(sv->notebook), 0), label1);

//-------------------------------------------------------------------------

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(sv->notebook), vbox3);

	frameRP = gtk_frame_new(NULL);
	gtk_widget_show(frameRP);
	gtk_box_pack_start(GTK_BOX(vbox3), frameRP, FALSE, TRUE, 0);

	scrolledwindowRP = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindowRP);
	gtk_container_add(GTK_CONTAINER(frameRP), scrolledwindowRP);
	gtk_widget_set_usize(scrolledwindowRP, -2, 65);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindowRP),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_NEVER);

	widgets.html_search_report = gtk_html_new();
	gtk_widget_show(widgets.html_search_report);
	gtk_container_add(GTK_CONTAINER(scrolledwindowRP), widgets.html_search_report);
	gtk_html_load_empty(GTK_HTML(widgets.html_search_report));

	vpaned_srch_rslt = e_vpaned_new();
	gtk_widget_show(vpaned_srch_rslt);
	gtk_box_pack_start(GTK_BOX(vbox3), vpaned_srch_rslt, TRUE,
			   TRUE, 0);
	e_paned_set_position(E_PANED(vpaned_srch_rslt),
				     100);

	frame3 = gtk_frame_new(NULL);
	gtk_widget_show(frame3);
	e_paned_pack1(E_PANED(vpaned_srch_rslt), frame3, TRUE, TRUE);

	scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow3);
	gtk_container_add(GTK_CONTAINER(frame3), scrolledwindow3);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow3),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	sv->clist = gtk_clist_new(1);
	gtk_widget_show(sv->clist);
	gtk_container_add(GTK_CONTAINER(scrolledwindow3),
			  sv->clist);
	gtk_clist_set_column_width(GTK_CLIST(sv->clist),
				   0, 100);
	gtk_clist_column_titles_hide(GTK_CLIST(sv->clist));

	frame4 = gtk_frame_new(NULL);
	gtk_widget_show(frame4);
	e_paned_pack2(E_PANED(vpaned_srch_rslt), frame4, TRUE, TRUE);

	scrolledwindow4 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow4);
	gtk_container_add(GTK_CONTAINER(frame4), scrolledwindow4);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow4),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);


	sv->html_widget = gtk_html_new();
	gtk_widget_show(sv->html_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow4), sv->html_widget);
	gtk_html_load_empty(GTK_HTML(sv->html_widget));
	
	

	label2 = gtk_label_new("");
	gtk_widget_show(label2);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(sv->notebook),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(sv->notebook), 1), label2);

//-------------------------------------------------------------------------

	frame5 = gtk_frame_new(NULL);
	gtk_widget_show(frame5);
	gtk_container_add(GTK_CONTAINER(sv->notebook), frame5);

	scrolledwindow5 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_show(scrolledwindow5);
	gtk_container_add(GTK_CONTAINER(frame5), scrolledwindow5);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow5),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	sv->html_viewer_widget = gtk_html_new();
	gtk_widget_show(sv->html_viewer_widget);
	gtk_container_add(GTK_CONTAINER(scrolledwindow5), sv->html_viewer_widget);
	gtk_html_load_empty(GTK_HTML(sv->html_viewer_widget));
		
	label3 = gtk_label_new("");
	gtk_widget_show(label3);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(sv->notebook),
				   gtk_notebook_get_nth_page
				   (GTK_NOTEBOOK(sv->notebook), 2), label3);

	gtk_signal_connect(GTK_OBJECT(vl_html), "link_clicked",
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
			   GTK_SIGNAL_FUNC(gui_url), widgets.app);
	return sv->htmlshow;
}

