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
#include <gal/e-paned/e-hpaned.h>
#include <gal/widgets/e-unicode.h>
#include <regex.h>

#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_search.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/html.h"
#include "gui/gnomesword.h"

#include "main/shortcutbar.h"
#include "main/module.h"
#include "main/key.h"
#include "main/sword.h"
#include "main/search.h"
#include "main/settings.h"

#include "gnome/shortcutbar.h"

#define HTML_START "<html><head><meta http-equiv='content-type' content='text/html; charset=utf-8'></head>"

extern SB_VIEWER sb_v, *sv;
extern gint groupnum6;

SEARCH_OPT so, *p_so;

GList *sblist;			/* for saving search results to bookmarks */


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

//static GtkWidget *radiobutton_search_dict;
//static GtkWidget *radiobutton_search_book;


void gui_search_update(char percent, void *userData)
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
 *   gui_set_search_label 
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *    void gui_set_search_label(void) 
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void gui_set_search_label(void)
{
	gchar search_label[80];
	gchar *buf = N_("Search");

	if (GTK_TOGGLE_BUTTON(radiobutton_search_text)->active) {
		sprintf(search_label, "%s %s", buf,
			settings.MainWindowModule);
		strcpy(settings.sb_search_mod,
		       settings.MainWindowModule);
	} 
	
	else if (GTK_TOGGLE_BUTTON(radiobutton_search_comm)->active) {
		sprintf(search_label, "%s %s", buf,
			settings.CommWindowModule);
		strcpy(settings.sb_search_mod,
		       settings.CommWindowModule);
	} 
/*
	else if (GTK_TOGGLE_BUTTON(radiobutton_search_dict)->active) {
		sprintf(search_label, "%s %s", buf,
			settings.DictWindowModule);
		strcpy(settings.sb_search_mod,
		       settings.DictWindowModule);
	} 
	
	else if (GTK_TOGGLE_BUTTON(radiobutton_search_book)->active) {
		sprintf(search_label, "%s %s", buf,
			settings.BookWindowModule);
		strcpy(settings.sb_search_mod,
		       settings.BookWindowModule);
	}
*/
	/*
	 * set search label to current module 
	 */
	gtk_frame_set_label((GtkFrame *) frame_module, search_label);
	//changegroupnameSB(search_label, groupnum6);
}


/******************************************************************************
 * Name
 *    fill_search_results_clist
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void fill_search_results_clist(GList *glist, SEARCH_OPT *so) 
 *
 * Description
 *   display a list of keys found during search
 *
 * Return value
 *   void
 */

static void fill_search_results_clist(int finds)
{
	gchar *utf8str, buf[256];
	gchar *tmpbuf;
	const gchar *key_buf = NULL;
	gint i = 0;
	gchar *buf0 = N_("Search Results");
	gchar *buf1 = N_("matches");
	gchar *buf2 = N_("Occurrences of");
	gchar *buf3 = N_("found in");



	gtk_clist_clear(GTK_CLIST(sv->clist));
	set_results_position((char) 1);	/* TOP */
	while ((key_buf = get_next_result_key()) != NULL) {
		tmpbuf = (gchar *) key_buf;
		gtk_clist_insert(GTK_CLIST(sv->clist), i++, &tmpbuf);

	}

	strcpy(settings.groupName, buf0);
	sprintf(buf, "%d %s", finds, buf1);
	gnome_appbar_set_status(GNOME_APPBAR(widgets.appbar), buf);
	gtk_notebook_set_page(GTK_NOTEBOOK(sv->notebook), 1);
	showSBVerseList();

	/* report results */

	gui_begin_html(widgets.html_search_report, TRUE);
	sprintf(buf, HTML_START
		"<body><center>%d %s <br><font color=\"%s\">"
		"<b>\"%s\"</b></font><br>%s <font color=\"%s\">"
		"<b>[%s]</b></font></center></body</html>",
		finds,
		buf2,
		settings.found_color,
		settings.searchText,
		buf3,
		settings.bible_verse_num_color, settings.sb_search_mod);
	utf8str =
	    e_utf8_from_gtk_string(widgets.html_search_report, buf);
	gui_display_html(widgets.html_search_report, utf8str,
			 strlen(utf8str));
	gui_end_html(widgets.html_search_report);

	/* cleanup appbar progress */
	gtk_progress_bar_update(GTK_PROGRESS_BAR(progressbar_search),
				0.0);
	/* display first item in list by selection row 0 */
	gtk_clist_select_row(GTK_CLIST(sv->clist), 0, 0);
}


/******************************************************************************
 * Name
 *    on_btnSearch_clicked
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void on_btnSearch_clicked(GtkButton * button, gpointer user_data)
 *
 * Description
 *   prepare to begin search
 *
 * Return value
 *   void
 */

static void on_btnSearch_clicked(GtkButton * button, gpointer user_data)
{
	GString *str;
	gint search_params, finds;
	gchar *search_string, *search_module;

	search_module = settings.sb_search_mod;
	p_so->module_name = search_module;

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


	search_string = gtk_entry_get_text(GTK_ENTRY(entrySearch));
	sprintf(settings.searchText, "%s", search_string);

	settings.searchType = GTK_TOGGLE_BUTTON
	    (rbRegExp)->active ? 0 :
	    GTK_TOGGLE_BUTTON(rbPhraseSearch)->active ? -1 : -2;

	search_params =
	    GTK_TOGGLE_BUTTON(ckbCaseSensitive)->active ? 0 : REG_ICASE;

	//clear_search_list();

	finds = do_module_search(search_module, search_string,
				 settings.searchType, search_params,
				 FALSE);

	fill_search_results_clist(finds);

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
 *   radiobutton_search_toggled
 *
 * Synopsis
 *   #include "gui/shortcutbar_search.h"
 *
 *   void radiobutton_search_toggled(GtkToggleButton * togglebutton,
				       gpointer user_data)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void radiobutton_search_toggled(GtkToggleButton * togglebutton,
				       gpointer user_data)
{
	gui_set_search_label();
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

void gui_create_shortcutbar_search(GtkWidget * vp)
{
	GtkWidget *frame1;
	GtkWidget *vbox1;
	GtkWidget *vbox90;
	GSList *vbox90_group = NULL;
	GtkWidget *frame_search;
	GtkWidget *vbox5;
	GtkWidget *btnSearch;
	GtkWidget *frame2;
	GtkWidget *vbox2;
	GSList *vbox2_group = NULL;
	GtkWidget *rbMultiword;
	GtkWidget *frame3;
	GtkWidget *vbox3;
	GtkWidget *frame4;
	GtkWidget *vbox4;
	GSList *vbox4_group = NULL;
	GtkWidget *rbNoScope;
	GtkWidget *table1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *frame6;
	GtkTooltips *tooltips;

	p_so = &so;

	tooltips = gtk_tooltips_new();

	frame1 = gtk_frame_new(NULL);
	gtk_widget_show(frame1);
	gtk_container_add(GTK_CONTAINER(vp), frame1);
	//gtk_widget_set_usize(frame1, 162, 360);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(frame1), vbox1);

	frame_search = gtk_frame_new(NULL);
	gtk_widget_show(frame_search);
	gtk_box_pack_start(GTK_BOX(vbox1), frame_search, FALSE, TRUE,
			   0);

	vbox5 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox5);
	gtk_container_add(GTK_CONTAINER(frame_search), vbox5);

	entrySearch = gtk_entry_new();
	gtk_widget_show(entrySearch);
	gtk_box_pack_start(GTK_BOX(vbox5), entrySearch, TRUE, TRUE, 0);
	gtk_widget_set_usize(entrySearch, 130, -2);;


	btnSearch = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
	gtk_widget_show(btnSearch);
	gtk_box_pack_start(GTK_BOX(vbox5), btnSearch, TRUE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, btnSearch, _("Begin Search"),
			     NULL);
	gtk_button_set_relief(GTK_BUTTON(btnSearch), GTK_RELIEF_HALF);

	frame6 = gtk_frame_new(NULL);
	gtk_widget_show(frame6);
	gtk_box_pack_start(GTK_BOX(vbox5), frame6, FALSE, TRUE, 0);

	progressbar_search = gtk_progress_bar_new();
	gtk_widget_show(progressbar_search);
	gtk_container_add(GTK_CONTAINER(frame6), progressbar_search);

	frame_module = gtk_frame_new(_("Search Module"));
	gtk_widget_show(frame_module);
	gtk_box_pack_start(GTK_BOX(vbox1), frame_module, FALSE, FALSE,
			   0);

	vbox90 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox90);
	gtk_container_add(GTK_CONTAINER(frame_module), vbox90);

	radiobutton_search_text =
	    gtk_radio_button_new_with_label(vbox90_group, _("Bible"));
	vbox90_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (radiobutton_search_text));
	gtk_widget_show(radiobutton_search_text);
	gtk_widget_set_usize(radiobutton_search_text, -2, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), radiobutton_search_text,
			   FALSE, FALSE, 0);

	radiobutton_search_comm =
	    gtk_radio_button_new_with_label(vbox90_group,
					    _("Commentary"));
	vbox90_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (radiobutton_search_comm));
	gtk_widget_show(radiobutton_search_comm);
	gtk_widget_set_usize(radiobutton_search_comm, -2, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), radiobutton_search_comm,
			   FALSE, FALSE, 0);
/*
	radiobutton_search_dict =
	    gtk_radio_button_new_with_label(vbox90_group,
					    _("Dictionary"));
	vbox90_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (radiobutton_search_dict));
	gtk_widget_show(radiobutton_search_dict);
	gtk_widget_set_usize(radiobutton_search_dict, -2, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), radiobutton_search_dict,
			   FALSE, FALSE, 0);

	radiobutton_search_book =
	    gtk_radio_button_new_with_label(vbox90_group,
					    _("General Book"));
	vbox90_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (radiobutton_search_book));
	gtk_widget_show(radiobutton_search_book);
	gtk_widget_set_usize(radiobutton_search_book, -2, 20);
	gtk_box_pack_start(GTK_BOX(vbox90), radiobutton_search_book,
			   FALSE, FALSE, 0);

*/
	frame2 = gtk_frame_new(_("Search Type"));
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, FALSE, TRUE, 0);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);

	rbMultiword =
	    gtk_radio_button_new_with_label(vbox2_group,
					    _("Multi Word"));
	vbox2_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbMultiword));
	gtk_widget_show(rbMultiword);
	gtk_box_pack_start(GTK_BOX(vbox2), rbMultiword, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(rbMultiword, -2, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON
				     (rbMultiword), TRUE);

	rbRegExp =
	    gtk_radio_button_new_with_label(vbox2_group,
					    _("Regular Expression"));
	vbox2_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbRegExp));
	gtk_widget_show(rbRegExp);
	gtk_box_pack_start(GTK_BOX(vbox2), rbRegExp, FALSE, FALSE, 0);
	gtk_widget_set_usize(rbRegExp, -2, 20);

	rbPhraseSearch =
	    gtk_radio_button_new_with_label(vbox2_group,
					    _("Exact Phrase"));
	vbox2_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbPhraseSearch));
	gtk_widget_show(rbPhraseSearch);
	gtk_box_pack_start(GTK_BOX(vbox2), rbPhraseSearch,
			   FALSE, FALSE, 0);
	gtk_widget_set_usize(rbPhraseSearch, -2, 20);

	frame3 = gtk_frame_new(_("Search Options"));
	gtk_widget_show(frame3);
	gtk_box_pack_start(GTK_BOX(vbox1), frame3, FALSE, TRUE, 0);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(frame3), vbox3);

	ckbCaseSensitive =
	    gtk_check_button_new_with_label(_("Case Sensitive"));
	gtk_widget_show(ckbCaseSensitive);
	gtk_box_pack_start(GTK_BOX(vbox3), ckbCaseSensitive, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(ckbCaseSensitive, -2, 20);


	frame4 = gtk_frame_new(_("Search Scope"));
	gtk_widget_show(frame4);
	gtk_box_pack_start(GTK_BOX(vbox1), frame4, FALSE, TRUE, 0);

	vbox4 = gtk_vbox_new(TRUE, 0);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame4), vbox4);

	rbNoScope =
	    gtk_radio_button_new_with_label(vbox4_group, _("No Scope"));
	vbox4_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbNoScope));
	gtk_widget_show(rbNoScope);
	gtk_box_pack_start(GTK_BOX(vbox4), rbNoScope, FALSE, FALSE, 0);
	gtk_widget_set_usize(rbNoScope, -2, 20);
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(rbNoScope),
				     TRUE);

	rrbUseBounds =
	    gtk_radio_button_new_with_label(vbox4_group,
					    _("Use Bounds"));
	vbox4_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rrbUseBounds));
	gtk_widget_show(rrbUseBounds);
	gtk_box_pack_start(GTK_BOX(vbox4), rrbUseBounds, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(rrbUseBounds, -2, 20);

	rbLastSearch =
	    gtk_radio_button_new_with_label(vbox4_group,
					    _("Last Search"));
	vbox4_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbLastSearch));
	gtk_widget_show(rbLastSearch);
	gtk_box_pack_start(GTK_BOX(vbox4), rbLastSearch, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(rbLastSearch, -2, 20);

	frame5 = gtk_frame_new(_("Bounds"));
	gtk_widget_show(frame5);
	gtk_box_pack_start(GTK_BOX(vbox1), frame5, FALSE, TRUE, 0);
	gtk_widget_hide(frame5);

	table1 = gtk_table_new(2, 2, FALSE);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(frame5), table1);

	label1 = gtk_label_new(_("Lower"));
	gtk_widget_show(label1);
	gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

	label2 = gtk_label_new(_("Upper "));
	gtk_widget_show(label2);
	gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

	entryLower = gtk_entry_new();;
	gtk_widget_show(entryLower);
	gtk_table_attach(GTK_TABLE(table1), entryLower, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(entryLower, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryLower), _("Genesis"));

	entryUpper = gtk_entry_new();;
	gtk_widget_show(entryUpper);
	gtk_table_attach(GTK_TABLE(table1), entryUpper, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(entryUpper, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryUpper), _("Revelation"));




	gtk_signal_connect(GTK_OBJECT(rrbUseBounds),
			   "toggled",
			   GTK_SIGNAL_FUNC(on_rrbUseBounds_toggled),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnSearch), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSearch_clicked), NULL);
			   
       gtk_signal_connect(GTK_OBJECT(entrySearch), "activate",
                          GTK_SIGNAL_FUNC(on_btnSearch_clicked), NULL);
			   
			   
	gtk_object_set_data(GTK_OBJECT(widgets.app), "tooltips",
			    tooltips);

	gtk_signal_connect(GTK_OBJECT(radiobutton_search_text),
			   "toggled",
			   GTK_SIGNAL_FUNC(radiobutton_search_toggled),
			   GINT_TO_POINTER(0));
	gtk_signal_connect(GTK_OBJECT(radiobutton_search_comm),
			   "toggled",
			   GTK_SIGNAL_FUNC(radiobutton_search_toggled),
			   GINT_TO_POINTER(1));
/*
	gtk_signal_connect(GTK_OBJECT(radiobutton_search_dict),
			   "toggled",
			   GTK_SIGNAL_FUNC(radiobutton_search_toggled),
			   GINT_TO_POINTER(2));
	gtk_signal_connect(GTK_OBJECT(radiobutton_search_book),
			   "toggled",
			   GTK_SIGNAL_FUNC(radiobutton_search_toggled),
			   GINT_TO_POINTER(3));
*/
}
