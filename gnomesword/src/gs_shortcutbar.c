/***************************************************************************
                                    gs_shortcutbar.c
                             -------------------
    begin                : Thu Jun 14 2001
    copyright            : (C) 2001 by tbiggs
    email                : tbiggs@users.sf.net
 ***************************************************************************/
 /*
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gnome.h>
#include  <gal/shortcut-bar/e-shortcut-bar.h>
#include <gal/e-paned/e-hpaned.h>
#include  "gs_shortcutbar.h"
#include  "gs_gnomesword.h"
#include  "support.h"
#include "gs_bookmarks.h"
#include "sw_search.h"

extern gchar *shortcut_types[];
extern GtkWidget *shortcut_bar;
extern SETTINGS *settings;
extern GtkWidget *MainFrm;
extern gboolean havedict; /* let us know if we have at least one lex-dict module */
extern gboolean havecomm; /* let us know if we have at least one commentary module */
extern gboolean havebible; /* let us know if we have at least one Bible text module */
extern EShortcutModel *shortcut_model;

gint groupnum1 = -1,
        groupnum2 = -1,
        groupnum3 = -1,
        groupnum4 = -1,
	groupnum5 = -1,
	groupnum6 = -1;
/*GtkWidget 
	*vpSearch;*/
	
/******************************************************************************
 * add_sb_group - add group to shourtcut bar
 * shortcut_bar - shortcut bar to add group to
 * group_name - name of the group to be added
 *****************************************************************************/
static gint
add_sb_group(EShortcutBar *shortcut_bar,gchar *group_name)
{
	gint group_num;
	
	group_num = e_shortcut_model_add_group (shortcut_bar->model, -1, group_name); 	
	e_shortcut_bar_set_view_type (shortcut_bar, group_num, E_ICON_BAR_SMALL_ICONS);
        	return group_num;
}


static void
on_button_clicked                      (GtkButton       *button,
                                        gpointer         user_data)
{
 	//g_warning("ok!");
}



//----------------------------------------------------------------------------------------------
void on_btnSB_clicked(GtkButton * button, gpointer user_data)
{
	if (settings->showshortcutbar) {
		settings->showshortcutbar = FALSE;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), 0);
	} else {
		settings->showshortcutbar = TRUE;
		e_paned_set_position (E_PANED(lookup_widget(MainFrm,"epaned")), settings->shortcutbar_width);
	}
}

/*****************************************************************************
 *      for any shortcut bar item clicked
 *****************************************************************************/
static void
on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
			      GdkEvent * event,
			      gint group_num, gint item_num)
{
	GtkWidget       
		*notebook,
		*app;
        gchar		
		*type,
		*ref;
	
	app = gtk_widget_get_toplevel(GTK_WIDGET(shortcut_bar));	
	e_shortcut_model_get_item_info(E_SHORTCUT_BAR(shortcut_bar)->model,
						  group_num,
						  item_num,
						  &type,
						  &ref);					  	
	if(!strcmp(type,"bible:")) {
		if(havebible){/* let's don't do this if we don't have at least one Bible text */
			notebook = lookup_widget(app,"nbTextMods"); /* get notebook */
		 	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); /* set notebook page */
		}
	}
	if(!strcmp(type,"commentary:")) {
		if(havecomm) { /* let's don't do this if we don't have at least one commentary */	           			            	
			notebook = lookup_widget(app,"notebook1"); /* get notebook */
		 	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); /* set notebook page */
    		}
    	}
	if(!strcmp(type,"dict/lex:")) {
		if(havedict) { /* let's don't do this if we don't have at least one dictionary / lexicon */	           			            	
			notebook = lookup_widget(app,"notebook4"); /* get notebook */
		 	gtk_notebook_set_page(GTK_NOTEBOOK(notebook), item_num); /* set notebook page */
  		}
	}
	if(!strcmp(type,"history:")) {
		changeVerseSWORD(ref);
	}
	g_free(type);
	g_free(ref);
}


static void
on_btnSaveList_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{

}


static void
on_btnSearch_clicked                   (GtkButton       *button,
                                        SETTINGS *s)
{
	searchSWORD(s->app, s);	
}




static void 
setupSearchBar(GtkWidget *vp, SETTINGS *s)
{
  GtkWidget *frame1;
  GtkWidget *vbox1;
  GtkWidget *frame7;
  GtkWidget *vbox5;
  GtkWidget *entrySearch;
  GtkWidget *toolbar1;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnSaveList;
  GtkWidget *vseparator1;
  GtkWidget *btnSearch;
  GtkWidget *frame2;
  GtkWidget *vbox2;
  GSList *vbox2_group = NULL;
  GtkWidget *rbMultiword;
  GtkWidget *rbRegExp;
  GtkWidget *rbPhraseSearch;
  GtkWidget *frame3;
  GtkWidget *vbox3;
  GtkWidget *ckbCaseSensitive;
  GtkWidget *ckbCommentary;
  GtkWidget *ckbPerCom;
  GtkWidget *frame4;
  GtkWidget *vbox4;
  GSList *vbox4_group = NULL;
  GtkWidget *rbNoScope;
  GtkWidget *rrbUseBounds;
  GtkWidget *rbLastSearch;
  GtkWidget *frame5;
  GtkWidget *table1;
  GtkWidget *label1;
  GtkWidget *label2;
  GtkWidget *entryLower;
  GtkWidget *entryUpper;

  frame1 = gtk_frame_new (NULL);
  gtk_widget_ref (frame1);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "frame1", frame1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame1);
  gtk_container_add (GTK_CONTAINER (vp), frame1);
  gtk_widget_set_usize (frame1, 162, 360);

  vbox1 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox1);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "vbox1", vbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox1);
  gtk_container_add (GTK_CONTAINER (frame1), vbox1);

  frame7 = gtk_frame_new (NULL);
  gtk_widget_ref (frame7);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "frame7", frame7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame7);
  gtk_box_pack_start (GTK_BOX (vbox1), frame7, FALSE, TRUE, 0);

  vbox5 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox5);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "vbox5", vbox5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox5);
  gtk_container_add (GTK_CONTAINER (frame7), vbox5);

  entrySearch = gtk_entry_new ();
  gtk_widget_ref (entrySearch);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "entrySearch", entrySearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entrySearch);
  gtk_box_pack_start (GTK_BOX (vbox5), entrySearch, TRUE, TRUE, 0);
  gtk_widget_set_usize (entrySearch, 130, -2);

  toolbar1 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar1);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "toolbar1", toolbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar1);
  gtk_box_pack_start (GTK_BOX (vbox5), toolbar1, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar1), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (s->app, GNOME_STOCK_PIXMAP_SAVE);
  btnSaveList = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Save List"),
                                _("Save search results as bookmark file"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnSaveList);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "btnSaveList", btnSaveList,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSaveList);

  vseparator1 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator1);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "vseparator1", vseparator1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator1);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar1), vseparator1, NULL, NULL);
  gtk_widget_set_usize (vseparator1, 90, 13);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (s->app, GNOME_STOCK_PIXMAP_SEARCH);
  btnSearch = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar1),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("Search"),
                                _("Click to begin search"), NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnSearch);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "btnSearch", btnSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSearch);

  frame2 = gtk_frame_new (_("Search Type"));
  gtk_widget_ref (frame2);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "frame2", frame2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame2);
  gtk_box_pack_start (GTK_BOX (vbox1), frame2, FALSE, TRUE, 0);

  vbox2 = gtk_vbox_new (TRUE, 0);
  gtk_widget_ref (vbox2);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "vbox2", vbox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox2);
  gtk_container_add (GTK_CONTAINER (frame2), vbox2);

  rbMultiword = gtk_radio_button_new_with_label (vbox2_group, _("Multi Word"));
  vbox2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbMultiword));
  gtk_widget_ref (rbMultiword);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "rbMultiword", rbMultiword,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbMultiword);
  gtk_box_pack_start (GTK_BOX (vbox2), rbMultiword, FALSE, FALSE, 0);
  gtk_widget_set_usize (rbMultiword, -2, 20);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rbMultiword), TRUE);

  rbRegExp = gtk_radio_button_new_with_label (vbox2_group, _("Regular Expression"));
  vbox2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbRegExp));
  gtk_widget_ref (rbRegExp);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "rbRegExp", rbRegExp,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbRegExp);
  gtk_box_pack_start (GTK_BOX (vbox2), rbRegExp, FALSE, FALSE, 0);
  gtk_widget_set_usize (rbRegExp, -2, 20);

  rbPhraseSearch = gtk_radio_button_new_with_label (vbox2_group, _("Exact Phrase"));
  vbox2_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbPhraseSearch));
  gtk_widget_ref (rbPhraseSearch);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "rbPhraseSearch", rbPhraseSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbPhraseSearch);
  gtk_box_pack_start (GTK_BOX (vbox2), rbPhraseSearch, FALSE, FALSE, 0);
  gtk_widget_set_usize (rbPhraseSearch, -2, 20);

  frame3 = gtk_frame_new (_("Search Options"));
  gtk_widget_ref (frame3);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "frame3", frame3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame3);
  gtk_box_pack_start (GTK_BOX (vbox1), frame3, FALSE, TRUE, 0);

  vbox3 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox3);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "vbox3", vbox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox3);
  gtk_container_add (GTK_CONTAINER (frame3), vbox3);

  ckbCaseSensitive = gtk_check_button_new_with_label (_("Case Sensitive"));
  gtk_widget_ref (ckbCaseSensitive);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "ckbCaseSensitive", ckbCaseSensitive,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ckbCaseSensitive);
  gtk_box_pack_start (GTK_BOX (vbox3), ckbCaseSensitive, FALSE, FALSE, 0);
  gtk_widget_set_usize (ckbCaseSensitive, -2, 20);

  ckbCommentary = gtk_check_button_new_with_label (_("Search Commentary"));
  gtk_widget_ref (ckbCommentary);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "ckbCommentary", ckbCommentary,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ckbCommentary);
  gtk_box_pack_start (GTK_BOX (vbox3), ckbCommentary, FALSE, FALSE, 0);
  gtk_widget_set_usize (ckbCommentary, -2, 20);

  ckbPerCom = gtk_check_button_new_with_label (_("Search Personal"));
  gtk_widget_ref (ckbPerCom);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "ckbPerCom", ckbPerCom,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ckbPerCom);
  gtk_box_pack_start (GTK_BOX (vbox3), ckbPerCom, FALSE, FALSE, 0);
  gtk_widget_set_usize (ckbPerCom, -2, 20);

  frame4 = gtk_frame_new (_("Search Scope"));
  gtk_widget_ref (frame4);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "frame4", frame4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame4);
  gtk_box_pack_start (GTK_BOX (vbox1), frame4, FALSE, TRUE, 0);

  vbox4 = gtk_vbox_new (TRUE, 0);
  gtk_widget_ref (vbox4);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "vbox4", vbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox4);
  gtk_container_add (GTK_CONTAINER (frame4), vbox4);

  rbNoScope = gtk_radio_button_new_with_label (vbox4_group, _("No Scope"));
  vbox4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbNoScope));
  gtk_widget_ref (rbNoScope);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "rbNoScope", rbNoScope,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbNoScope);
  gtk_box_pack_start (GTK_BOX (vbox4), rbNoScope, FALSE, FALSE, 0);
  gtk_widget_set_usize (rbNoScope, -2, 20);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rbNoScope), TRUE);

  rrbUseBounds = gtk_radio_button_new_with_label (vbox4_group, _("Use Bounds"));
  vbox4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rrbUseBounds));
  gtk_widget_ref (rrbUseBounds);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "rrbUseBounds", rrbUseBounds,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rrbUseBounds);
  gtk_box_pack_start (GTK_BOX (vbox4), rrbUseBounds, FALSE, FALSE, 0);
  gtk_widget_set_usize (rrbUseBounds, -2, 20);

  rbLastSearch = gtk_radio_button_new_with_label (vbox4_group, _("Last Search"));
  vbox4_group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbLastSearch));
  gtk_widget_ref (rbLastSearch);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "rbLastSearch", rbLastSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbLastSearch);
  gtk_box_pack_start (GTK_BOX (vbox4), rbLastSearch, FALSE, FALSE, 0);
  gtk_widget_set_usize (rbLastSearch, -2, 20);

  frame5 = gtk_frame_new (_("Bounds"));
  gtk_widget_ref (frame5);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "frame5", frame5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame5);
  gtk_box_pack_start (GTK_BOX (vbox1), frame5, TRUE, TRUE, 0);

  table1 = gtk_table_new (2, 2, FALSE);
  gtk_widget_ref (table1);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "table1", table1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table1);
  gtk_container_add (GTK_CONTAINER (frame5), table1);

  label1 = gtk_label_new (_("Lower"));
  gtk_widget_ref (label1);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "label1", label1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label1);
  gtk_table_attach (GTK_TABLE (table1), label1, 0, 1, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label1), 0, 0.5);

  label2 = gtk_label_new (_("Upper "));
  gtk_widget_ref (label2);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "label2", label2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label2);
  gtk_table_attach (GTK_TABLE (table1), label2, 0, 1, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (label2), 0, 0.5);

  entryLower = gtk_entry_new ();
  gtk_widget_ref (entryLower);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "entryLower", entryLower,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryLower);
  gtk_table_attach (GTK_TABLE (table1), entryLower, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (entryLower, 114, 22);
  gtk_entry_set_text (GTK_ENTRY (entryLower), _("Genesis"));

  entryUpper = gtk_entry_new ();
  gtk_widget_ref (entryUpper);
  gtk_object_set_data_full (GTK_OBJECT (s->app), "entryUpper", entryUpper,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryUpper);
  gtk_table_attach (GTK_TABLE (table1), entryUpper, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (entryUpper, 114, 22);
  gtk_entry_set_text (GTK_ENTRY (entryUpper), _("Revelation"));

  gtk_signal_connect (GTK_OBJECT (btnSaveList), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSaveList_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnSearch), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSearch_clicked),
                      s);
 
}

void setupSB(GList *textlist, 
			GList *commentarylist, 
			GList *dictionarylist)
{
	GList 
		*tmp;
	GtkWidget	
		*treebar,
		*button,
		*searchbar,
		*searchbutton,
		*ctree,
		*scrolledwindow1,
		*scrolledwindow2,
		*vpSearch;
	tmp = NULL;
	if(settings->showtextgroup){
	    	groupnum1 = add_sb_group((EShortcutBar *)shortcut_bar, "Bible Text");
		tmp = textlist;
		while (tmp != NULL) {
		 	e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum1, -1,
					shortcut_types[0],
					(gchar *) tmp->data);
			tmp = g_list_next(tmp);
		}	
	}
	if(settings->showcomgroup){
	    	groupnum2 = add_sb_group((EShortcutBar *)shortcut_bar, "Commentaries");
		tmp = commentarylist;
	        while (tmp != NULL) {	
			e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum2, -1,
					shortcut_types[1],
					(gchar *) tmp->data);
	                tmp = g_list_next(tmp);
		}	
	}
	if(settings->showdictgroup){
		groupnum3 = add_sb_group((EShortcutBar *)shortcut_bar, "Dict/Lex");
		tmp = dictionarylist;
	        while (tmp != NULL) { 	
			e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum3, -1,
					shortcut_types[2],
					(gchar *) tmp->data);
			tmp = g_list_next(tmp);
		}
	}
	if(settings->showhistorygroup){
		groupnum4 = add_sb_group((EShortcutBar *)shortcut_bar, "History");	
	}
	g_list_free(tmp);
	/*** add bookmark group to shortcut bar ***/
	treebar = e_group_bar_new();
	
	scrolledwindow1 = gtk_scrolled_window_new (NULL, NULL);
  	gtk_widget_ref (scrolledwindow1);
  	gtk_object_set_data_full (GTK_OBJECT (MainFrm), "scrolledwindow1", scrolledwindow1,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);
	ctree = gtk_ctree_new (3, 0);
  	gtk_widget_ref (ctree);
  	gtk_object_set_data_full (GTK_OBJECT (MainFrm), "ctree",ctree ,
                            (GtkDestroyNotify) gtk_widget_unref);
  	gtk_widget_show (ctree);
	gtk_container_add (GTK_CONTAINER (scrolledwindow1), ctree);
  	gtk_clist_set_column_width (GTK_CLIST (ctree), 0, 280);
  	gtk_clist_set_column_width (GTK_CLIST (ctree), 1, 80);
	gtk_clist_set_column_width (GTK_CLIST (ctree), 2, 80);
  	//gtk_clist_column_titles_show (GTK_CLIST (ctree1));
	//gs.ctree_widget = ctree;
	settings->ctree_widget = lookup_widget(settings->app,"ctree");
	button = gtk_button_new_with_label("Bookmarks");
	gtk_widget_ref (button);
	gtk_object_set_data_full (GTK_OBJECT (settings->app), "button", button,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (button);
	groupnum5  = e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
						 scrolledwindow1,
						 button,
						 -1); 
						 
	loadtree(settings);
	
	searchbar = e_group_bar_new();   
	
	
  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (settings->app), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  //gtk_box_pack_start (GTK_BOX (hbox25), scrolledwindow42, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), 
  		GTK_POLICY_AUTOMATIC, 
		GTK_POLICY_AUTOMATIC);

  vpSearch = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (vpSearch);
  gtk_object_set_data_full (GTK_OBJECT (settings->app), "vpSearch", vpSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vpSearch);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), vpSearch);
  gtk_widget_set_usize (vpSearch, 234, -2);

	searchbutton = gtk_button_new_with_label("Search");
	gtk_widget_ref (searchbutton);
	gtk_object_set_data_full (GTK_OBJECT (MainFrm), "searchbutton", searchbutton,
                            (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show (searchbutton);	

	setupSearchBar(vpSearch, settings);
	
	groupnum6  = e_group_bar_add_group(E_GROUP_BAR(shortcut_bar),
						 scrolledwindow2,
						 searchbutton,
						 -1); 
	settings->searchbargroup = groupnum6;
	gtk_signal_connect (GTK_OBJECT (button), "clicked",
                      	GTK_SIGNAL_FUNC (on_button_clicked),
                      	NULL);  
	gtk_signal_connect(GTK_OBJECT(shortcut_bar), "item_selected",
			GTK_SIGNAL_FUNC(on_shortcut_bar_item_selected),
			NULL);
}

void
update_shortcut_bar(SETTINGS *s,
				GList *text, 
				GList *commentary, 
				GList *dictionary)
{
	gint	count, i,current_group;
	GList *tmp;
	EShortcutBar *bar;
	
	bar = E_SHORTCUT_BAR(shortcut_bar);	
	tmp = NULL;
	current_group = e_group_bar_get_current_group_num(E_GROUP_BAR(bar));
	count = e_shortcut_model_get_num_groups(E_SHORTCUT_MODEL(shortcut_model));
	//*** remove shortcutbar groups so we can load the one the user has sellected
	for(i=count-1; i>-1; i--){
		e_shortcut_model_remove_group(E_SHORTCUT_MODEL(shortcut_model), i);
	}	
	if(s->showtextgroup){
	    	groupnum1 = add_sb_group((EShortcutBar *)shortcut_bar, "Bible Text");
		tmp = text;
		while (tmp != NULL) {
		 	e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum1, -1,
					shortcut_types[0],
					(gchar *) tmp->data);
			tmp = g_list_next(tmp);
		}	
	}
	if(s->showcomgroup){
	    	groupnum2 = add_sb_group((EShortcutBar *)shortcut_bar, "Commentaries");
		tmp = commentary;
	        while (tmp != NULL) {	
			e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum2, -1,
					shortcut_types[1],
					(gchar *) tmp->data);
	                tmp = g_list_next(tmp);
		}	
	}
	if(s->showdictgroup){
		groupnum3 = add_sb_group((EShortcutBar *)shortcut_bar, "Dict/Lex");
		 tmp = dictionary;
	        while (tmp != NULL) { 	
			e_shortcut_model_add_item (E_SHORTCUT_BAR(shortcut_bar)->model,
					groupnum3, -1,
					shortcut_types[2],
					(gchar *) tmp->data);
			tmp = g_list_next(tmp);
		}
	}
	if(s->showhistorygroup){
		groupnum4 = add_sb_group((EShortcutBar *)shortcut_bar, "History");	
	}	
	e_group_bar_set_current_group_num(E_GROUP_BAR(bar),
						 current_group,
						 TRUE);		
        g_list_free(tmp);
}


