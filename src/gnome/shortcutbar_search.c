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

#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_search.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/html.h"

#include "main/shortcutbar.h"
#include "main/sword.h"
#include "main/gs_gnomesword.h"
#include "main/settings.h"

#define HTML_START "<html><head><meta http-equiv='content-type' content='text/html; charset=utf8'></head>"

extern SB_VIEWER sb_v, *sv;

SEARCH_OPT so, *p_so;

GList *sblist;	/* for saving search results to bookmarks */

	
static  GtkWidget *rrbUseBounds;
static 	GtkWidget *entrySearch;
static 	GtkWidget *entryLower;
static 	GtkWidget *entryUpper;
static 	GtkWidget *rbRegExp;
static 	GtkWidget *rbLastSearch;
static 	GtkWidget *rbPhraseSearch;
static 	GtkWidget *ckbCaseSensitive;



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

static void fill_search_results_clist(GList *glist, SEARCH_OPT *so) 
{
	GList *tmp = NULL;
	gchar *utf8str, buf[256];
	gint i = 0;
	
	tmp = glist;
	gtk_clist_clear(GTK_CLIST(sv->clist));
	while (tmp != NULL) {	
		gchar *buf1 = (gchar*)tmp->data;
		gchar *token = strtok(buf1, "|");
		buf1 = token;
		token = strtok(NULL, "|");
		buf1 = token;
		gtk_clist_insert(GTK_CLIST(sv->clist), i, &buf1);
		++i;
		tmp = g_list_next(tmp);	
	}
	g_list_free(tmp);
	sprintf(settings.groupName,"%s","Search Results");
	sprintf(buf,"%d matches",i);
	gnome_appbar_set_status (GNOME_APPBAR (settings.appbar), buf);
	gtk_notebook_set_page(GTK_NOTEBOOK(sv->notebook), 1);
	showSBVerseList();
	
	/* report results */
	gui_begin_html(settings.htmlRP, TRUE);
	sprintf(buf,HTML_START 
	    "<body><center>%d Occurrences of <br><font color=\"%s\"><b>\"%s\"</b></font><br>found in <font color=\"%s\"><b>[%s]</b></font></center></body</html>", 
				i, settings.found_color,settings.searchText,
				settings.bible_verse_num_color,so->module_name);	
	utf8str = e_utf8_from_gtk_string(settings.htmlRP, buf);
	gui_display_html(settings.htmlRP, utf8str, strlen(utf8str));
	gui_end_html(settings.htmlRP);	
	
	/* cleanup appbar progress */
	gnome_appbar_set_progress ((GnomeAppBar *)settings.appbar, 0);
	/* display first item in list by selection row 0 */
	gtk_clist_select_row(GTK_CLIST(sv->clist), 0, 0);	
}

/******************************************************************************
 * Name
 *   search_module 
 *
 * Synopsis
 *   #include "shortcutbar_search.h"
 *
 *   void search_module(SEARCH_OPT *so)	
 *
 * Description
 *   begin search and display results
 *
 * Return value
 *   
 */

static void search_module(SEARCH_OPT *so)
{
	if (sblist)
		g_list_free(sblist);
	sblist = NULL;
	sblist = do_search((gpointer*)so);
	fill_search_results_clist(sblist, so);
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
	if (GTK_TOGGLE_BUTTON (p_so->ckbCommentary)->active) {	/* if true search commentary */	  
		p_so->module_name = settings.CommWindowModule;
	} 
	
	else if (GTK_TOGGLE_BUTTON (p_so->ckbPerCom)->active) {	/* if true search personal commentary */	  
		p_so->module_name = settings.personalcommentsmod;
	} 
	
	else if (GTK_TOGGLE_BUTTON (p_so->ckbGBS)->active) {	/* if true search book */	  
		p_so->module_name = settings.BookWindowModule;  
	} 
	
	else {			/* search Bible text */	 
		p_so->module_name = settings.MainWindowModule;  
	}
	
	p_so->use_bonds = GTK_TOGGLE_BUTTON(rrbUseBounds)->active;
	if (p_so->use_bonds) {
		/* read lower bounds entry and set lower bounds for search */
		p_so->lower_bond = gtk_entry_get_text(GTK_ENTRY(entryLower)); 
		/* read upper bounds entry and set upper bounds for search */
		p_so->upper_bond = gtk_entry_get_text(GTK_ENTRY(entryUpper)); 
	} 
	
	p_so->use_lastsearch_for_bonds = GTK_TOGGLE_BUTTON(rbLastSearch)->active;
	
	
	p_so->found_count = 0;	
	p_so->search_string = gtk_entry_get_text(GTK_ENTRY(entrySearch));
	sprintf(settings.searchText, "%s",p_so->search_string);
	
	p_so->search_type = GTK_TOGGLE_BUTTON 
			(rbRegExp)->active ? 0 : 
			GTK_TOGGLE_BUTTON 
			(rbPhraseSearch)->active ? -1 : -2;
		settings.searchType = p_so->search_type;
	p_so->search_params =
			  GTK_TOGGLE_BUTTON(ckbCaseSensitive)->
			  active ? 0 : 1;	/* get search params - case sensitive */
			
	search_module(p_so);
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
	GtkWidget *frame7;
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
	GtkWidget *frame5;
	GtkWidget *table1;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkTooltips *tooltips;

	p_so = &so;

	tooltips = gtk_tooltips_new();

	frame1 = gtk_frame_new(NULL);
	gtk_widget_ref(frame1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame1",
				 frame1, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_container_add(GTK_CONTAINER(vp), frame1);
	//gtk_widget_set_usize(frame1, 162, 360);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox1",
				 vbox1, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(frame1), vbox1);

	frame7 = gtk_frame_new(NULL);
	gtk_widget_ref(frame7);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame7",
				 frame7, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame7);
	gtk_box_pack_start(GTK_BOX(vbox1), frame7, FALSE, TRUE, 0);

	vbox5 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox5);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox5",
				 vbox5, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox5);
	gtk_container_add(GTK_CONTAINER(frame7), vbox5);

	entrySearch = gtk_entry_new();
	gtk_widget_ref(entrySearch);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "entrySearch",
				 entrySearch, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(entrySearch);
	gtk_box_pack_start(GTK_BOX(vbox5), entrySearch, TRUE, TRUE, 0);
	gtk_widget_set_usize(entrySearch, 130, -2);

	btnSearch = gnome_stock_button(GNOME_STOCK_BUTTON_OK);
	gtk_widget_ref(btnSearch);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "btnSearch",
				 btnSearch, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(btnSearch);
	gtk_box_pack_start(GTK_BOX(vbox5), btnSearch, TRUE, FALSE, 0);
	gtk_tooltips_set_tip(tooltips, btnSearch, _("Begin Search"),
			     NULL);
	gtk_button_set_relief(GTK_BUTTON(btnSearch), GTK_RELIEF_HALF);

	frame2 = gtk_frame_new(_("Search Type"));
	gtk_widget_ref(frame2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame2",
				 frame2, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox1), frame2, FALSE, TRUE, 0);

	vbox2 = gtk_vbox_new(TRUE, 0);
	gtk_widget_ref(vbox2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox2",
				 vbox2, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frame2), vbox2);

	rbMultiword =
	    gtk_radio_button_new_with_label(vbox2_group,
					    _("Multi Word"));
	vbox2_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbMultiword));
	gtk_widget_ref(rbMultiword);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "rbMultiword",
				 rbMultiword, (GtkDestroyNotify)
				 gtk_widget_unref);
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
	gtk_widget_ref(rbRegExp);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "rbRegExp",
				 rbRegExp, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(rbRegExp);
	gtk_box_pack_start(GTK_BOX(vbox2), rbRegExp, FALSE, FALSE, 0);
	gtk_widget_set_usize(rbRegExp, -2, 20);

	rbPhraseSearch =
	    gtk_radio_button_new_with_label(vbox2_group,
					    _("Exact Phrase"));
	vbox2_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON
				   (rbPhraseSearch));
	gtk_widget_ref(rbPhraseSearch);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "rbPhraseSearch",
				 rbPhraseSearch,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(rbPhraseSearch);
	gtk_box_pack_start(GTK_BOX(vbox2), rbPhraseSearch,
			   FALSE, FALSE, 0);
	gtk_widget_set_usize(rbPhraseSearch, -2, 20);

	frame3 = gtk_frame_new(_("Search Options"));
	gtk_widget_ref(frame3);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame3",
				 frame3, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame3);
	gtk_box_pack_start(GTK_BOX(vbox1), frame3, FALSE, TRUE, 0);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox3",
				 vbox3, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(frame3), vbox3);

	ckbCaseSensitive =
	    gtk_check_button_new_with_label(_("Case Sensitive"));
	gtk_widget_ref(ckbCaseSensitive);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "ckbCaseSensitive", ckbCaseSensitive,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(ckbCaseSensitive);
	gtk_box_pack_start(GTK_BOX(vbox3), ckbCaseSensitive, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(ckbCaseSensitive, -2, 20);

	p_so->ckbCommentary =
	    gtk_check_button_new_with_label(_("Search Commentary"));
	gtk_widget_ref(p_so->ckbCommentary);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "p_so->ckbCommentary",
				 p_so->ckbCommentary, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(p_so->ckbCommentary);
	gtk_box_pack_start(GTK_BOX(vbox3), p_so->ckbCommentary,
			   FALSE, FALSE, 0);
	gtk_widget_set_usize(p_so->ckbCommentary, -2, 20);

	p_so->ckbPerCom =
	    gtk_check_button_new_with_label(_("Search Personal"));
	gtk_widget_ref(p_so->ckbPerCom);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "p_so->ckbPerCom", p_so->ckbPerCom,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(p_so->ckbPerCom);
	gtk_box_pack_start(GTK_BOX(vbox3), p_so->ckbPerCom, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(p_so->ckbPerCom, -2, 20);

	p_so->ckbGBS =
	    gtk_check_button_new_with_label(_("Search Book"));
	gtk_widget_ref(p_so->ckbGBS);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "p_so->ckbGBS", p_so->ckbGBS,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(p_so->ckbGBS);
	gtk_tooltips_set_tip(tooltips, p_so->ckbGBS,
			     _("Search Current Book"), NULL);
	gtk_box_pack_start(GTK_BOX(vbox3), p_so->ckbGBS, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(p_so->ckbGBS, -2, 20);

	frame4 = gtk_frame_new(_("Search Scope"));
	gtk_widget_ref(frame4);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame4",
				 frame4, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame4);
	gtk_box_pack_start(GTK_BOX(vbox1), frame4, FALSE, TRUE, 0);

	vbox4 = gtk_vbox_new(TRUE, 0);
	gtk_widget_ref(vbox4);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "vbox4",
				 vbox4, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame4), vbox4);

	rbNoScope =
	    gtk_radio_button_new_with_label(vbox4_group, _("No Scope"));
	vbox4_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbNoScope));
	gtk_widget_ref(rbNoScope);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "rbNoScope",
				 rbNoScope, (GtkDestroyNotify)
				 gtk_widget_unref);
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
	gtk_widget_ref(rrbUseBounds);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "rrbUseBounds", rrbUseBounds,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(rrbUseBounds);
	gtk_box_pack_start(GTK_BOX(vbox4), rrbUseBounds, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(rrbUseBounds, -2, 20);

	rbLastSearch =
	    gtk_radio_button_new_with_label(vbox4_group,
					    _("Last Search"));
	vbox4_group =
	    gtk_radio_button_group(GTK_RADIO_BUTTON(rbLastSearch));
	gtk_widget_ref(rbLastSearch);
	gtk_object_set_data_full(GTK_OBJECT(settings.app),
				 "rbLastSearch", rbLastSearch,
				 (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(rbLastSearch);
	gtk_box_pack_start(GTK_BOX(vbox4), rbLastSearch, FALSE,
			   FALSE, 0);
	gtk_widget_set_usize(rbLastSearch, -2, 20);

	frame5 = gtk_frame_new(_("Bounds"));
	gtk_widget_ref(frame5);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "frame5",
				 frame5, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(frame5);
	gtk_box_pack_start(GTK_BOX(vbox1), frame5, TRUE, TRUE, 0);

	table1 = gtk_table_new(2, 2, FALSE);
	gtk_widget_ref(table1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "table1",
				 table1, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(table1);
	gtk_container_add(GTK_CONTAINER(frame5), table1);

	label1 = gtk_label_new(_("Lower"));
	gtk_widget_ref(label1);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label1",
				 label1, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label1);
	gtk_table_attach(GTK_TABLE(table1), label1, 0, 1, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label1), 0, 0.5);

	label2 = gtk_label_new(_("Upper "));
	gtk_widget_ref(label2);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "label2",
				 label2, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_table_attach(GTK_TABLE(table1), label2, 0, 1, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_misc_set_alignment(GTK_MISC(label2), 0, 0.5);

	entryLower = gtk_entry_new();
	gtk_widget_ref(entryLower);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "entryLower",
				 entryLower, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(entryLower);
	gtk_table_attach(GTK_TABLE(table1), entryLower, 1, 2, 0, 1,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(entryLower, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryLower), _("Genesis"));

	entryUpper = gtk_entry_new();
	gtk_widget_ref(entryUpper);
	gtk_object_set_data_full(GTK_OBJECT(settings.app), "entryUpper",
				 entryUpper, (GtkDestroyNotify)
				 gtk_widget_unref);
	gtk_widget_show(entryUpper);
	gtk_table_attach(GTK_TABLE(table1), entryUpper, 1, 2, 1, 2,
			 (GtkAttachOptions) (GTK_FILL),
			 (GtkAttachOptions) (0), 0, 0);
	gtk_widget_set_usize(entryUpper, 114, 22);
	gtk_entry_set_text(GTK_ENTRY(entryUpper), _("Revelation"));

	gtk_signal_connect(GTK_OBJECT(btnSearch), "clicked",
			   GTK_SIGNAL_FUNC(on_btnSearch_clicked), NULL);
	gtk_object_set_data(GTK_OBJECT(settings.app), "tooltips", tooltips);
}

