/*  
 * gtksword.cpp  - Fri Oct 26 11:15:54 2001
 * copyright (C) 2001 CrossWire Bible Society
 *			P. O. Box 2528
 *			Tempe, AZ  85280-2528
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
*/

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <swmgr.h>
#include <markupfiltmgr.h>
#include <versekey.h>
#include <regex.h>
#include <string.h>

#include "gtksword_cbs.h"
#include "gtksword.h"

GdkColor myGreen;
GdkColor BGcolor;
GdkFont *roman_font, *italic_font, *versenum_font, *bold_font;
gchar *mycolor;

gchar *font_mainwindow =
    "-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_italic_mainwindow =
    "-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_interlinear =
    "-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_italic_interlinear =
    "-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_currentverse =
    "-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
    *font_italic_currentverse =
    "-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1";

MainWindow *gtkswordWindow = NULL;

MainWindow::MainWindow()
{
	mainMgr = new SWMgr(new MarkupFilterMgr(FMT_THML)); //new MarkupFilterMgr(FMT_GBF )
	curMod = NULL;
	chapDisplay = 0;	// set in create
	entryDisplay = 0;	// set in create
}


MainWindow::~MainWindow()
{
	delete mainMgr;
	if (chapDisplay)
		delete chapDisplay;
	if (entryDisplay)
		delete entryDisplay;
}

GtkWidget *create_mainWindow()
{
	GtkWidget *retWidget;
	gtkswordWindow = new MainWindow();
	retWidget = gtkswordWindow->create();
	gtkswordWindow->initSWORD();
	return retWidget;
}

							
void destroy_mainWindow()
{
	delete gtkswordWindow;
}

GtkWidget *MainWindow::create()
{

	tooltips = gtk_tooltips_new();
	gchar *titles[2] = { "Ingredients", "Amount" };
	
	
	mainWindow = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_object_set_data(GTK_OBJECT(mainWindow), "mainWindow",
			    mainWindow);
	gtk_container_set_border_width(GTK_CONTAINER(mainWindow), 4);
	gtk_window_set_title(GTK_WINDOW(mainWindow),
			     "SWORD Quick Lookup / Search");
	gtk_window_set_position(GTK_WINDOW(mainWindow),
				GTK_WIN_POS_CENTER);
	gtk_window_set_default_size(GTK_WINDOW(mainWindow), 600, 400);

	gtk_window_set_policy(GTK_WINDOW(mainWindow), TRUE, TRUE, FALSE);
	
	mainPanel = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(mainPanel);
	gtk_widget_show(mainPanel);
	gtk_container_add(GTK_CONTAINER(mainWindow), mainPanel);

	headerPanel = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(headerPanel);
	gtk_widget_show(headerPanel);
	gtk_box_pack_start(GTK_BOX(mainPanel), headerPanel, FALSE, TRUE,
			   0);

	handlebox3 = gtk_handle_box_new();
	gtk_widget_show(handlebox3);
	gtk_box_pack_start(GTK_BOX(headerPanel), handlebox3, TRUE, TRUE,
			   0);

	menubar4 = gtk_menu_bar_new();
	gtk_widget_ref(menubar4);
	gtk_widget_show(menubar4);
	gtk_container_add(GTK_CONTAINER(handlebox3), menubar4);
	gtk_widget_set_usize(menubar4, 217, -2);

	File3 = gtk_menu_item_new_with_label("File");
	gtk_widget_show(File3);
	gtk_container_add(GTK_CONTAINER(menubar4), File3);

	File3_menu = gtk_menu_new();
	gtk_widget_ref(File3_menu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(File3), File3_menu);

	Exit3 = gtk_menu_item_new_with_label("Exit");
	gtk_widget_ref(Exit3);
	gtk_widget_show(Exit3);
	gtk_container_add(GTK_CONTAINER(File3_menu), Exit3);

	options1 = gtk_menu_item_new_with_label("Options");
	gtk_widget_ref(options1);
				 
	gtk_widget_show(options1);
	gtk_container_add(GTK_CONTAINER(menubar4), options1);

	options1_menu = gtk_menu_new();
	gtk_widget_ref(options1_menu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(options1), options1_menu);

	verse_style1 = gtk_check_menu_item_new_with_label("Verse Style");
	
	gtk_widget_ref(verse_style1);
	gtk_widget_show(verse_style1);
	gtk_container_add(GTK_CONTAINER(options1_menu), verse_style1);
	gtk_tooltips_set_tip(tooltips, verse_style1,
			     "Display Bible text in verses", NULL);
	gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM(verse_style1),
				       TRUE);
	/*gtk_check_menu_item_set_show_toggle(GTK_CHECK_MENU_ITEM
					    (verse_style1), TRUE);
*/
	Help = gtk_menu_item_new_with_label("Help");
	gtk_widget_ref(Help);
	gtk_widget_show(Help);
	gtk_container_add(GTK_CONTAINER(menubar4), Help);
	//gtk_menu_item_right_justify(GTK_MENU_ITEM(Help));

	Help_menu = gtk_menu_new();
	gtk_widget_ref(Help_menu);
	gtk_menu_item_set_submenu(GTK_MENU_ITEM(Help), Help_menu);

	About = gtk_menu_item_new_with_label("About");
	gtk_widget_ref(About);
	gtk_widget_show(About);
	gtk_container_add(GTK_CONTAINER(Help_menu), About);

	handlebox4 = gtk_handle_box_new();
	gtk_widget_ref(handlebox4);
	gtk_widget_show(handlebox4);
	gtk_box_pack_start(GTK_BOX(headerPanel), handlebox4, TRUE, TRUE,
			   0);

	toolbar1 =
	    gtk_toolbar_new();
	gtk_widget_ref(toolbar1);
	gtk_widget_show(toolbar1);
	gtk_container_add(GTK_CONTAINER(handlebox4), toolbar1);

	hbox6 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox6);
	gtk_widget_show(hbox6);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar1), hbox6, NULL,
				  NULL);

	label2 = gtk_label_new("FreeForm Lookup:");
	gtk_widget_ref(label2);
	gtk_widget_show(label2);
	gtk_box_pack_start(GTK_BOX(hbox6), label2, FALSE, TRUE, 0);
	gtk_label_set_justify(GTK_LABEL(label2), GTK_JUSTIFY_RIGHT);
	gtk_misc_set_padding(GTK_MISC(label2), 10, 0);

	lookupText = gtk_entry_new();
	gtk_widget_ref(lookupText);
	gtk_widget_show(lookupText);
	gtk_box_pack_start(GTK_BOX(hbox6), lookupText, FALSE, TRUE, 0);
	gtk_widget_set_usize(lookupText, 179, -2);

/**********************************************************************************************************/


	hpaned3 = gtk_hpaned_new();
	gtk_widget_ref(hpaned3);
	gtk_widget_show(hpaned3);
	gtk_box_pack_start(GTK_BOX(mainPanel), hpaned3, TRUE, TRUE, 0);
	gtk_paned_set_position(GTK_PANED(hpaned3), 186);

	frameSidebar = gtk_frame_new(NULL);
	gtk_widget_ref(frameSidebar);
	gtk_widget_show(frameSidebar);
	gtk_paned_pack1(GTK_PANED(hpaned3), frameSidebar, FALSE, TRUE);

	vbox2 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox2);
	gtk_widget_show(vbox2);
	gtk_container_add(GTK_CONTAINER(frameSidebar), vbox2);
	
	buttonBM1 = gtk_button_new_with_label("Bookmarks");
	gtk_widget_ref(buttonBM1);
	gtk_widget_show(buttonBM1);
	gtk_box_pack_start(GTK_BOX(vbox2), buttonBM1, FALSE, FALSE, 0);

	buttonSearch1 = gtk_button_new_with_label("Search");
	gtk_widget_ref(buttonSearch1);
	gtk_box_pack_start(GTK_BOX(vbox2), buttonSearch1, FALSE, FALSE, 0);

	buttonSR1 = gtk_button_new_with_label("Search Results");
	gtk_widget_ref(buttonSR1);
	gtk_box_pack_start(GTK_BOX(vbox2), buttonSR1, FALSE, FALSE, 0);

	notebookSidebar = gtk_notebook_new();
	gtk_widget_ref(notebookSidebar);
	gtk_widget_show(notebookSidebar);
	gtk_box_pack_start(GTK_BOX(vbox2), notebookSidebar, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(notebookSidebar, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebookSidebar), FALSE);

	scrolledwindow6 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow6);
	gtk_widget_show(scrolledwindow6);
	gtk_container_add(GTK_CONTAINER(notebookSidebar), scrolledwindow6);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow6), GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	viewport4 = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(viewport4);
	gtk_widget_show(viewport4);
	gtk_container_add(GTK_CONTAINER(scrolledwindow6), viewport4);


	GtkListStore *model;
	GtkWidget *view;
	model = gtk_list_store_new (1, G_TYPE_STRING);
	view = gtk_tree_view_new_with_model (GTK_TREE_MODEL (model));
	gtk_container_add(GTK_CONTAINER(viewport4), view);
	gtk_widget_show(view);
/*


	ctreeBookmarks = gtk_ctree_new(3, 0);
	gtk_widget_ref(ctreeBookmarks);
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(ctreeBookmarks);
	gtk_container_add(GTK_CONTAINER(viewport4), ctreeBookmarks);
	gtk_clist_set_column_width(GTK_CLIST(ctreeBookmarks), 0, 80);
	gtk_clist_set_column_width(GTK_CLIST(ctreeBookmarks), 1, 80);
	gtk_clist_set_column_width(GTK_CLIST(ctreeBookmarks), 2, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(ctreeBookmarks));

	label16 = gtk_label_new("label16");
	gtk_widget_ref(label16);
	gtk_widget_show(label16);
	gtk_clist_set_column_widget(GTK_CLIST(ctreeBookmarks), 0, label16);

	label17 = gtk_label_new("label17");
	gtk_widget_ref(label17);
	gtk_widget_show(label17);
	gtk_clist_set_column_widget(GTK_CLIST(ctreeBookmarks), 1, label17);

	label18 = gtk_label_new("label18");
	gtk_widget_ref(label18);
	gtk_widget_show(label18);
	gtk_clist_set_column_widget(GTK_CLIST(ctreeBookmarks), 2, label18);
*/
	
	scrolledwindow7 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow7);
	gtk_widget_show(scrolledwindow7);
	gtk_container_add(GTK_CONTAINER(notebookSidebar), scrolledwindow7);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow7), GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	viewport5 = gtk_viewport_new(NULL, NULL);
	gtk_widget_ref(viewport5);
	gtk_widget_show(viewport5);
	gtk_container_add(GTK_CONTAINER(scrolledwindow7), viewport5);

	searchPanel = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(searchPanel);
	gtk_widget_show(searchPanel);
	gtk_container_add(GTK_CONTAINER(viewport5), searchPanel);

	searchText = gtk_entry_new();
	gtk_widget_ref(searchText);
	gtk_widget_show(searchText);
	gtk_box_pack_start(GTK_BOX(searchPanel), searchText, FALSE, FALSE,
			   0);

	hbox4 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox4);
	gtk_widget_show(hbox4);
	gtk_box_pack_start(GTK_BOX(searchPanel), hbox4, FALSE, FALSE, 0);

	searchButton = gtk_button_new_with_label("Search");
	gtk_widget_ref(searchButton);
	gtk_widget_show(searchButton);
	gtk_box_pack_start(GTK_BOX(hbox4), searchButton, TRUE, TRUE, 0);

	searchSettingsPanel = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(searchSettingsPanel);
	gtk_widget_show(searchSettingsPanel);
	gtk_box_pack_start(GTK_BOX(searchPanel), searchSettingsPanel,
			   FALSE, FALSE, 0);

	searchTypePanel = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(searchTypePanel);
	gtk_widget_show(searchTypePanel);
	gtk_box_pack_start(GTK_BOX(searchSettingsPanel), searchTypePanel,
			   FALSE, FALSE, 0);

	searchType_group = NULL;
	regexSearch =
	    gtk_radio_button_new_with_label(searchType_group,
					    "Regular Expression");
	searchType_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (regexSearch));
	    
	gtk_widget_ref(regexSearch);
	gtk_widget_show(regexSearch);
	gtk_box_pack_start(GTK_BOX(searchTypePanel), regexSearch, FALSE,
			   FALSE, 0);

	phraseSearch =
	    gtk_radio_button_new_with_label(searchType_group,
					    "Exact Phrase");
	
	gtk_widget_ref(phraseSearch);
	gtk_widget_show(phraseSearch);
	gtk_box_pack_start(GTK_BOX(searchTypePanel), phraseSearch, FALSE,
			   FALSE, 0);

	multiWordSearch =
	    gtk_radio_button_new_with_label(searchType_group,
					    "Multi Word");
	gtk_widget_ref(multiWordSearch);
	gtk_widget_show(multiWordSearch);
	gtk_box_pack_start(GTK_BOX(searchTypePanel), multiWordSearch,
			   FALSE, FALSE, 0);

	caseSensitive = gtk_check_button_new_with_label("Case Sensitive");
	gtk_widget_ref(caseSensitive);
	gtk_widget_show(caseSensitive);
	gtk_box_pack_start(GTK_BOX(searchTypePanel), caseSensitive, FALSE,
			   FALSE, 0);

	searchOptionsPanel = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(searchOptionsPanel);
	gtk_widget_show(searchOptionsPanel);
	gtk_box_pack_start(GTK_BOX(searchSettingsPanel),
			   searchOptionsPanel, FALSE, FALSE, 0);


	
	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_widget_show(vbox3);
	gtk_container_add(GTK_CONTAINER(notebookSidebar), vbox3);

	labelNumFound = gtk_label_new("0 Occurence(s) of");
	gtk_widget_ref(labelNumFound);
	gtk_widget_show(labelNumFound);
	gtk_box_pack_start(GTK_BOX(vbox3), labelNumFound, FALSE, FALSE, 0);

	labelSearchText = gtk_label_new("???????");
	gtk_widget_ref(labelSearchText);
	gtk_widget_show(labelSearchText);
	gtk_box_pack_start(GTK_BOX(vbox3), labelSearchText, FALSE, FALSE,
			   0);

	label22 = gtk_label_new("found");
	gtk_widget_ref(label22);
	gtk_widget_show(label22);
	gtk_box_pack_start(GTK_BOX(vbox3), label22, FALSE, FALSE, 0);

	scrolledwindow9 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow9);
	gtk_widget_show(scrolledwindow9);
	gtk_box_pack_start(GTK_BOX(vbox3), scrolledwindow9, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow9),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	resultList = gtk_clist_new_with_titles (2, titles);
	gtk_widget_ref(resultList);
	gtk_widget_show(resultList);
	gtk_container_add(GTK_CONTAINER(scrolledwindow9), resultList);
	gtk_clist_set_column_width (GTK_CLIST(resultList), 0, 150);
	
	buttonSearch2 = gtk_button_new_with_label("Search");
	gtk_widget_ref(buttonSearch2);
	gtk_widget_show(buttonSearch2);
	gtk_box_pack_start(GTK_BOX(vbox2), buttonSearch2, FALSE, FALSE, 0);

	buttonSR2 = gtk_button_new_with_label("Search Results");
	gtk_widget_ref(buttonSR2);
	gtk_widget_show(buttonSR2);
	gtk_box_pack_start(GTK_BOX(vbox2), buttonSR2, FALSE, FALSE, 0);

	vpaned1 = gtk_vpaned_new();
	gtk_widget_ref(vpaned1);
	gtk_widget_show(vpaned1);
	gtk_paned_pack2(GTK_PANED(hpaned3), vpaned1, TRUE, TRUE);
	gtk_paned_set_gutter_size(GTK_PANED(vpaned1), 10);
	gtk_paned_set_position(GTK_PANED(vpaned1), 250);
	gtk_widget_set_usize(vpaned1, -2, 250);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_widget_show(hbox2);
	gtk_paned_pack1(GTK_PANED(vpaned1), hbox2, FALSE, TRUE);

	hpaned1 = gtk_hpaned_new();
	gtk_widget_ref(hpaned1);
	gtk_widget_show(hpaned1);
	gtk_box_pack_start(GTK_BOX(hbox2), hpaned1, TRUE, TRUE, 0);
	gtk_paned_set_position(GTK_PANED(hpaned1), 300);

	notebookText = gtk_notebook_new();
	gtk_widget_ref(notebookText);
	gtk_widget_show(notebookText);
	gtk_paned_pack1(GTK_PANED(hpaned1), notebookText, FALSE, TRUE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebookText), TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebookText));
	
	notebookCommentary = gtk_notebook_new();
	gtk_widget_ref(notebookCommentary);
	gtk_widget_show(notebookCommentary);
	gtk_paned_pack2(GTK_PANED(hpaned1), notebookCommentary, TRUE,
			TRUE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebookCommentary),
				    TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebookCommentary));

	hpaned2 = gtk_hpaned_new();
	gtk_widget_ref(hpaned2);
	gtk_widget_show(hpaned2);
	gtk_paned_pack2(GTK_PANED(vpaned1), hpaned2, TRUE, TRUE);
	gtk_paned_set_gutter_size(GTK_PANED(hpaned2), 10);
	gtk_paned_set_position(GTK_PANED(hpaned2), 291);

	notebookDictionary = gtk_notebook_new();
	gtk_widget_ref(notebookDictionary);
	gtk_widget_show(notebookDictionary);
	gtk_paned_pack1(GTK_PANED(hpaned2), notebookDictionary, FALSE,
			TRUE);
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebookDictionary),
				    TRUE);
	gtk_notebook_popup_enable(GTK_NOTEBOOK(notebookDictionary));

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_widget_show(vbox1);
	gtk_paned_pack2(GTK_PANED(hpaned2), vbox1, TRUE, TRUE);

	entryDictionaryKey = gtk_entry_new();
	gtk_widget_ref(entryDictionaryKey);
	gtk_widget_show(entryDictionaryKey);
	gtk_box_pack_start(GTK_BOX(vbox1), entryDictionaryKey, FALSE,
			   FALSE, 0);
	gtk_entry_set_text(GTK_ENTRY(entryDictionaryKey), "GRACE");

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow2);
	gtk_widget_show(scrolledwindow2);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow2, TRUE, TRUE, 0);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow2),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_NEVER);

	clistKeys = gtk_clist_new(1);
	gtk_widget_ref(clistKeys);
	gtk_widget_show(clistKeys);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), clistKeys);
	gtk_clist_set_column_width(GTK_CLIST(clistKeys), 0, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(clistKeys));

	label9 = gtk_label_new("label9");
	gtk_widget_ref(label9);
	gtk_widget_show(label9);
	gtk_clist_set_column_widget(GTK_CLIST(clistKeys), 0, label9);


 /************************************************************************************************************************/

	arrow1 = gtk_button_new_with_label("[<]");
	gtk_widget_show(arrow1);
	gtk_box_pack_start(GTK_BOX(headerPanel), arrow1, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(arrow1), "clicked",
			   GTK_SIGNAL_FUNC(on_arrow1_button_press_event),
			   NULL);

	arrow2 = gtk_button_new_with_label("[>]");
	gtk_widget_show(arrow2);
	gtk_box_pack_start(GTK_BOX(headerPanel), arrow2, FALSE, FALSE, 0);
	gtk_signal_connect(GTK_OBJECT(arrow2), "clicked",
			   GTK_SIGNAL_FUNC(on_arrow2_button_press_event),
			   NULL);
	statusbar1 = gtk_label_new("");
	gtk_widget_show(statusbar1);
	gtk_box_pack_start(GTK_BOX(mainPanel), statusbar1, FALSE, FALSE,
			   0);
	gtk_label_set_justify(GTK_LABEL(statusbar1), GTK_JUSTIFY_LEFT);
	gtk_misc_set_padding(GTK_MISC(statusbar1), 2, 0);

	gtk_signal_connect(GTK_OBJECT(mainWindow), "destroy",
			   GTK_SIGNAL_FUNC(on_mainWindow_destroy), NULL);
	gtk_signal_connect(GTK_OBJECT(Exit3), "activate",
			   GTK_SIGNAL_FUNC(on_Exit3_activate), NULL);
	gtk_signal_connect(GTK_OBJECT(verse_style1), "activate",
			   GTK_SIGNAL_FUNC(on_verse_style1_activate),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(Help), "activate",
			   GTK_SIGNAL_FUNC(on_Help_activate), NULL);
	gtk_signal_connect(GTK_OBJECT(About), "activate",
			   GTK_SIGNAL_FUNC(on_About_activate), NULL);
	gtk_signal_connect(GTK_OBJECT(lookupText), "changed",
			   GTK_SIGNAL_FUNC(on_lookupText_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(buttonBM1), "clicked",
			   GTK_SIGNAL_FUNC(on_buttonBM1_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(buttonSearch1), "clicked",
			   GTK_SIGNAL_FUNC(on_buttonSearch1_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(buttonSR1), "clicked",
			   GTK_SIGNAL_FUNC(on_buttonSR1_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(searchButton), "clicked",
			   GTK_SIGNAL_FUNC(on_searchButton_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(resultList), "select_row",
			   GTK_SIGNAL_FUNC
			   (on_resultList_selection_changed), NULL);
	gtk_signal_connect(GTK_OBJECT(buttonSearch2), "clicked",
			   GTK_SIGNAL_FUNC(on_buttonSearch2_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(buttonSR2), "clicked",
			   GTK_SIGNAL_FUNC(on_buttonSR2_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(entryDictionaryKey), "changed",
			   GTK_SIGNAL_FUNC(on_entryDictionaryKey_changed),
			   NULL);
	gtk_object_set_data(GTK_OBJECT(mainWindow), "tooltips", tooltips);
	
	GTKEntryDisp::__initialize();

	return mainWindow;
}

void MainWindow::initSWORD()
{

	GtkWidget *menuChoice;
	int viewNumber = 1;
	char menuName[64];
	ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	char *font;
	SWModule *curMod;
	textcount = 0;
	commcount = 0;
	dictcount = 0;

/*	First, check and see whether SWMgr found any installed books.  If not, tell the user what the problem is.	*/
/*	Print a short notice to stderr, for use when the program is invoked from the command line,		*/
/*	and also put  a bit longer message into the spot where bible text would otherwise show up. 		*/
/*	Then, return out of this function, because there's nothing left to do.					*/
	if (mainMgr->Modules.empty() == TRUE) {
		GtkWidget *scrolledwindow1;
		GtkWidget *moduleText;
		GtkWidget *label3;

		fprintf(stderr,
			"SWMgr: Can't find 'mods.conf' or 'mods.d'.  Try setting:\n\tSWORD_PATH=<directory containing mods.conf>\n\tOr see the README file for a full description of setup options.\n\t");

		scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
		gtk_widget_ref(scrolledwindow1);
		gtk_object_set_data_full(GTK_OBJECT(mainWindow),
					 "scrolledwindow1",
					 scrolledwindow1,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(scrolledwindow1);
		gtk_container_add(GTK_CONTAINER(notebookText),
				  scrolledwindow1);
		gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
					       (scrolledwindow1),
					       GTK_POLICY_NEVER,
					       GTK_POLICY_AUTOMATIC);

		moduleText = gtk_text_view_new();
		gtk_object_set_data(GTK_OBJECT(mainWindow), "moduleText",
				    moduleText);
		gtk_widget_show(moduleText);
		gtk_container_add(GTK_CONTAINER(scrolledwindow1),
				  moduleText);
		//gtk_text_set_editable(GTK_TEXT(moduleText), FALSE);
		//gtk_text_set_word_wrap(GTK_TEXT(moduleText), TRUE);
		gtk_widget_realize(moduleText);

		//gtk_text_set_point(GTK_TEXT(moduleText), 0);
		int curPos = 0;
/*		gtk_text_freeze(GTK_TEXT(moduleText));
		gtk_text_insert(GTK_TEXT(moduleText), NULL,
				&moduleText->style->black, NULL,
				"\nCheatah was unable to find any books installed!\n\n",
				-1);
		gtk_text_insert(GTK_TEXT(moduleText), NULL,
				&moduleText->style->black, NULL,
				"They should be listed in a configuration file named either: ",
				-1);
		gtk_text_insert(GTK_TEXT(moduleText), NULL,
				&moduleText->style->black, NULL,
				"'mods.conf' or 'mods.d'. \n\n", -1);
		gtk_text_insert(GTK_TEXT(moduleText), NULL,
				&moduleText->style->black, NULL,
				"Try setting:\n\tSWORD_PATH=<directory containing mods.conf>\n\t",
				-1);
		gtk_text_insert(GTK_TEXT(moduleText), NULL,
				&moduleText->style->black, NULL,
				"Or see the README file for a full description of setup options.\n",
				-1);
		gtk_text_set_point(GTK_TEXT(moduleText), curPos);
		gtk_text_thaw(GTK_TEXT(moduleText));
*/
		return;
	}

/*
* Create filters to use, depending on the needs of the installed modules.
* Right now, basically just handling GBF files and converting them to plain
* text for display.  So far, I haven't found any modules that specify
* anything other than GBF, but I've dropped in code to 
* install the thtml to plain filter just in case, since it's available.
* Question: I'm installing this as a display filter - would it make more
* sense to install it is a raw filter, so that I could use "plain" as the
* base for any strip filters we may need?  Or is that used for something else,
* like compression or decyphering locked books?
*/

	
/*
*	Next, if SWMgr did find  some book modules installed , hook them up to any filters needed
*	and set up the notebooks to reflect them.
*	Pick a default module to show on startup.
*/
	gint text_page = 0, comm_page = 0, dict_page = 0;
	for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
		/* modules we do not wish to show 1) locked without cipherkey and 2) utf8 encoded*/
		if ((sit = mainMgr->config->Sections.find((*it).second->Name())) != mainMgr->config->Sections.end()) {
			    
			if ((eit = (*sit).second.find("CipherKey")) != (*sit).second.end()) {
				const char *cipherKey = (char *) (*eit).second.c_str();
				if (strlen(cipherKey) < 1)
					continue;
			}			
		}
		
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			curMod = (*it).second;			
			CreateTextPane((*it).second, notebookText);
			if (!this->curMod)	// set currently selected module for app to first module from SWMgr (Bible Texts get first preference
				this->curMod = curMod;
		}
		
		if (!strcmp((*it).second->Type(), "Commentaries")) {
			curcommMod = (*it).second;
			CreateCommentaryPane(curcommMod, notebookCommentary);			
		}

		if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) {
			curdictMod = (*it).second;
			CreateDictionaryPane(curdictMod, notebookDictionary);
		}

	}
// Add options to Options Main Menu choice
	OptionsList options = mainMgr->getGlobalOptions();
	for (OptionsList::iterator it = options.begin();
	     it != options.end(); it++) {
		GtkWidget *optItem =
		    gtk_check_menu_item_new_with_label((*it).c_str());
		gtk_widget_ref(optItem);
		gtk_object_set_data_full(GTK_OBJECT(mainWindow), "optItem",
					 optItem,
					 (GtkDestroyNotify)
					 gtk_widget_unref);
		gtk_widget_show(optItem);
		gtk_container_add(GTK_CONTAINER(options1_menu), optItem);
		gtk_tooltips_set_tip(tooltips, optItem,
				     mainMgr->getGlobalOptionTip((*it).
								 c_str()),
				     NULL);
		gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
					       (optItem), FALSE);
		gtk_signal_connect(GTK_OBJECT(optItem), "activate",
				   GTK_SIGNAL_FUNC
				   (on_option_item_activate),
				   (gchar *) (*it).c_str());
	}
	if (!this->curMod)	// We didn't find a bible text, so set currently selected module for app to first module from SWMgr
		this->curMod = curMod;	// Should be o.k. to do this, because we've already tested to see if SWMgr thinks it found something
	assert(this->curMod != NULL);
	if (this->curMod != NULL)	// Go ahead and display default module
		lookupTextChanged();

	gtk_signal_connect(GTK_OBJECT(notebookText), "switch_page",
			   GTK_SIGNAL_FUNC(on_notebookText_switch_page),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(notebookCommentary), "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebookCommentary_switch_page), NULL);
	gtk_signal_connect(GTK_OBJECT(notebookDictionary), "switch_page",
			   GTK_SIGNAL_FUNC
			   (on_notebookDictionary_switch_page), NULL);
}


/*******************************************************************************
 * toggle options on and off
 * choice - true = on, false = off
 ******************************************************************************/
void MainWindow::setGlobalOption(gchar * option, gboolean choice)
{
	if (choice) {		//-- if choice is TRUE - we want option on
		mainMgr->setGlobalOption(option, "On");
	} else {		//-- we want option off    
		mainMgr->setGlobalOption(option, "Off");
	}
	curMod->Display();	//-- we need to show change

}

static
gchar *gettaggedText(GtkTextTag *tag, 
                  const GtkTextIter *iter)
{
	GtkTextIter *enditer, *startiter;
	gchar *buf;
	
	startiter = gtk_text_iter_copy(iter);
	enditer = gtk_text_iter_copy(iter);
	
	if(gtk_text_iter_begins_tag(startiter, tag)){
		while(gtk_text_iter_forward_char(enditer))
			if(gtk_text_iter_ends_tag(enditer, tag)){
				break;
			}
	} else if(gtk_text_iter_ends_tag(enditer, tag)){
		while(gtk_text_iter_backward_char(startiter))
			if(gtk_text_iter_begins_tag(startiter, tag)){
				break;
			}
	} else {
		while(gtk_text_iter_backward_char(startiter))
			if(gtk_text_iter_begins_tag(startiter, tag)){
				break;
			}
		while(gtk_text_iter_forward_char(enditer))
			if(gtk_text_iter_ends_tag(enditer, tag)){
				break;
			}
	}
	buf = gtk_text_iter_get_visible_text(startiter,
                                          enditer);
	printf("\ntag text = %s\n",buf);
	return buf;
	
	
}


static
gint tag_event_handler (GtkTextTag *tag, GtkWidget *widget, GdkEvent *event,
                  const GtkTextIter *iter, gpointer user_data)
{
	gint char_index;
	//GtkTextIter *enditer, *startiter;
	gchar *buf;
	bool gotoVerse = false;
	
	char_index = gtk_text_iter_get_offset (iter);
	
	//startiter = gtk_text_iter_copy(iter);
	//enditer = gtk_text_iter_copy(iter);	
  switch (event->type)
    {
    case GDK_MOTION_NOTIFY:
      printf ("Motion event at char %d tag `%s'\n",
             char_index, tag->name);
      break;
        
    case GDK_BUTTON_PRESS:
      printf ("Button press at char %d tag `%s'\n",
             char_index, tag->name);
	    switch(event->button.button){
		    case 1:
			if(!strcmp(tag->name,"verseNumber") || !strcmp(tag->name,"p_verseNumber")){
			buf = gettaggedText(tag, iter);
			gtkswordWindow->changeVerse(atoi(buf));
			return true;
		    case 2:
		    case 3:
		        break;
	    }
	}
	
      break;
        
    case GDK_2BUTTON_PRESS:
      printf ("Double click at char %d tag `%s'\n",
             char_index, tag->name);
      break;
        
    case GDK_3BUTTON_PRESS:
      printf ("Triple click at char %d tag `%s'\n",
             char_index, tag->name);
      break;
        
    case GDK_BUTTON_RELEASE:
      printf ("Button release at char %d tag `%s'\n",
             char_index, tag->name);
      break;
        
    case GDK_KEY_PRESS:
    case GDK_KEY_RELEASE:
      printf ("Key event at char %d tag `%s'\n",
              char_index, tag->name);
      break;
      
    case GDK_ENTER_NOTIFY:
    case GDK_LEAVE_NOTIFY:
    case GDK_PROPERTY_NOTIFY:
    case GDK_SELECTION_CLEAR:
    case GDK_SELECTION_REQUEST:
    case GDK_SELECTION_NOTIFY:
    case GDK_PROXIMITY_IN:
    case GDK_PROXIMITY_OUT:
    case GDK_DRAG_ENTER:
    case GDK_DRAG_LEAVE:
    case GDK_DRAG_MOTION:
    case GDK_DRAG_STATUS:
    case GDK_DROP_START:
    case GDK_DROP_FINISHED:
    default:
      break;
    }
    
    return false;
}

void
MainWindow::setup_tag (GtkTextTag *tag, gpointer user_data)
{
	g_signal_connect (G_OBJECT (tag),
		    "event",
		    G_CALLBACK (tag_event_handler),
		    user_data);
}


void MainWindow::CreateTextTags(GtkTextBuffer *buffer)
{
	GtkTextTag *tag;
	GdkColor color;
	GdkColor color2;
	GdkColor colorLink;
	PangoFontDescription *font_desc;
	
	/* link color */
	colorLink.red = 0xbbbb;
	colorLink.green = 0xbbbb;
	colorLink.blue = 0xffff;
	
	/* invisible tag */
	/*
	tag = gtk_text_buffer_create_tag (buffer, "invisible", NULL);
	setup_tag (tag, NULL); 
	g_object_set (G_OBJECT (tag),
                "invisible", TRUE,
                NULL);
	*/
	
	/* verse number tag verse style*/
	tag = gtk_text_buffer_create_tag (buffer, "verseNumber", NULL);
	setup_tag (tag, NULL);  
	color.red = color.green = 0;
	color.blue = 0xffff;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                "underline", PANGO_UNDERLINE_SINGLE,
                NULL);
	
	/* verse number tag paragraph style*/
	tag = gtk_text_buffer_create_tag (buffer, "p_verseNumber", NULL);
	setup_tag (tag, NULL);  
	color.red = color.green = 0;
	color.blue = 0xffff;
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &color,
                "underline", PANGO_UNDERLINE_SINGLE,
                NULL);	
		
	/* reference tag */
	tag = gtk_text_buffer_create_tag (buffer, "reference", NULL);
	setup_tag (tag, NULL);  	
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &colorLink,
                NULL);
		
		
	/* strongs hebrew tag */
	tag = gtk_text_buffer_create_tag (buffer, "strongshebrew", NULL);
	setup_tag (tag, NULL);  
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &colorLink,
                NULL);
		
	/* strongs hebrew tag current verse*/
	tag = gtk_text_buffer_create_tag (buffer, "cv_strongshebrew", NULL);
	setup_tag (tag, NULL);  
	color.red = 0xbbbb;
	color.green = 0xbbbb;
	color.blue = 0xffff;
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &color,
                NULL);	
			
	/* strongs greek tag */
	tag = gtk_text_buffer_create_tag (buffer, "strongsgreek", NULL);
	setup_tag (tag, NULL);  
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &colorLink,
                NULL);
		
	/* strongs greek tag current verse*/
	tag = gtk_text_buffer_create_tag (buffer, "cv_strongsgreek", NULL);
	setup_tag (tag, NULL);  
	color.red = 0xbbbb;
	color.green = 0xbbbb;
	color.blue = 0xffff;
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &color,
                NULL);	
			
			
	/* morph  tag greek */
	tag = gtk_text_buffer_create_tag (buffer, "morphgreek", NULL);
	setup_tag (tag, NULL);  
	color.red = 0;
	color.green = 0xbbbb;
	color.blue = 0xffff;
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &color,
                NULL);
		
	/*morph tag greek current verse */
	tag = gtk_text_buffer_create_tag (buffer, "cv_morphgreek", NULL);
	setup_tag (tag, NULL);  
	color.red = 0;
	color.green = 0xbbbb;
	color.blue = 0xffff;
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &color,
                NULL);			
		
		
	/* footnote tag */
	tag = gtk_text_buffer_create_tag (buffer, "footnote", NULL);
	setup_tag (tag, NULL);  
	color.red = 0x9999;
	color.green = 0x4444;
	color.blue = 0x5555;
	g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_SMALL,
                "foreground_gdk", &color,
                NULL);		
		
	
	/* current verse color tag */	
	tag = gtk_text_buffer_create_tag (buffer, "fg_currentverse", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);
		
	/*  verse color tag */	
	tag = gtk_text_buffer_create_tag (buffer, "fg_verse", NULL);
	color.blue = 0;
	color.green = 0;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                NULL);
		
	/* underline tag */
	tag = gtk_text_buffer_create_tag (buffer, "underline", NULL);
	g_object_set (G_OBJECT (tag),
                "underline", PANGO_UNDERLINE_SINGLE,
                NULL);
		
	/* italics tag */
	tag = gtk_text_buffer_create_tag (buffer, "italic", NULL);
	g_object_set (G_OBJECT (tag),
                "style", PANGO_STYLE_ITALIC,
                NULL);
		
		
	/* underline tag current verse */
	tag = gtk_text_buffer_create_tag (buffer, "cv_underline", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                "underline", PANGO_UNDERLINE_SINGLE,
                NULL);
		
	/* italics tag current verse */
	tag = gtk_text_buffer_create_tag (buffer, "cv_italic", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                "style", PANGO_STYLE_ITALIC,
                NULL);
		
	
	/* cite tag */
	tag = gtk_text_buffer_create_tag (buffer, "cite", NULL);
	g_object_set (G_OBJECT (tag),
                "style", PANGO_STYLE_ITALIC,
		"weight", PANGO_WEIGHT_BOLD,
                NULL);
		
	/* cite tag current verse */
	tag = gtk_text_buffer_create_tag (buffer, "cv_cite", NULL);
	color.blue = 0;
	color.green = 0xbbbb;
	color.red = 0;
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                "style", PANGO_STYLE_ITALIC,
		"weight", PANGO_WEIGHT_BOLD,
                NULL);
				
		
	/* centered tag */
	tag = gtk_text_buffer_create_tag (buffer, "centered", NULL);      
	g_object_set (G_OBJECT (tag),
                "justification", GTK_JUSTIFY_CENTER,
                NULL);	
				
		
	/* centered tag current verse*/
	tag = gtk_text_buffer_create_tag (buffer, "cv_centered", NULL);      
	g_object_set (G_OBJECT (tag),
                "foreground_gdk", &color,
                "justification", GTK_JUSTIFY_CENTER,
                NULL);	
		
	/* right to left tag */
	tag = gtk_text_buffer_create_tag (buffer, "rtl_quote", NULL);
        g_object_set (G_OBJECT (tag),
		"scale", PANGO_SCALE_LARGE,
                "wrap_mode", GTK_WRAP_WORD,
                "direction", GTK_TEXT_DIR_RTL,
                "indent", 0,
                "left_margin", 0,
                "right_margin", 0,
                NULL);
		
	/*  tag */
	tag = gtk_text_buffer_create_tag (buffer, "ltr_quote", NULL);
        g_object_set (G_OBJECT (tag),
                "wrap_mode", GTK_WRAP_WORD,
                "direction", GTK_TEXT_DIR_LTR,
                "indent", 0,
                "left_margin", 0,
                "right_margin", 0,
                NULL);
	
	
	
}

void MainWindow::CreateTextPane(SWModule * mod, GtkWidget * notebook)
{
	GtkWidget *scrolledwindow1;
	GtkWidget *moduleText;
	GtkWidget *label3;	
	GtkTextBuffer *buffer;
	PangoFontDescription *font_desc;
  
 
	buffer = gtk_text_buffer_new (NULL);
	
	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(mainWindow), "scrolledwindow1",
				 scrolledwindow1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	gtk_container_add(GTK_CONTAINER(notebook), scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	moduleText = gtk_text_view_new_with_buffer(buffer);
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (moduleText),
                               GTK_WRAP_WORD);
	gtk_object_set_data(GTK_OBJECT(mainWindow), "moduleText",
			    moduleText);
	gtk_widget_show(moduleText);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), moduleText);
	gtk_widget_realize(moduleText);
	
	font_desc = pango_font_description_from_string ("Sans 14");
	gtk_widget_modify_font (moduleText, font_desc);
	pango_font_description_free (font_desc);

	
	CreateTextTags(GTK_TEXT_VIEW(moduleText)->buffer);

	label3 = gtk_label_new(mod->Name());
	gtk_widget_ref(label3);
	gtk_object_set_data_full(GTK_OBJECT(mainWindow), "label3", label3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook),
							     textcount++),
				   label3);
	if(!strcmp(mod->Name(), "BHS"))
		chapDisplay = new GTKChapDisp(moduleText);
	else
		chapDisplay = new HTMLChapDisp(moduleText);
	mod->Disp(chapDisplay);
}

void MainWindow::CreateCommentaryPane(SWModule * mod, GtkWidget * notebook)
{
	GtkWidget *scrolledwindow2;
	GtkWidget *moduleText;
	GtkWidget *label3;
	GtkTextTag *tag;

	scrolledwindow2 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow2);
	gtk_object_set_data_full(GTK_OBJECT(mainWindow), "scrolledwindow2",
				 scrolledwindow2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow2);
	gtk_container_add(GTK_CONTAINER(notebook), scrolledwindow2);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow2), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	moduleText = gtk_text_view_new();
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (moduleText),
                               GTK_WRAP_WORD);
	gtk_object_set_data(GTK_OBJECT(mainWindow), "moduleText",
			    moduleText);
	gtk_widget_show(moduleText);
	gtk_container_add(GTK_CONTAINER(scrolledwindow2), moduleText);
	gtk_widget_realize(moduleText);
	/*  tag */
	CreateTextTags(GTK_TEXT_VIEW(moduleText)->buffer);
	
	label3 = gtk_label_new(mod->Name());
	gtk_widget_ref(label3);
	gtk_object_set_data_full(GTK_OBJECT(mainWindow), "label3", label3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook),
							     commcount++),
				   label3);


	entryDisplay = new GTKEntryDisp(moduleText);
	mod->Disp(entryDisplay);

}


void MainWindow::CreateDictionaryPane(SWModule * mod, GtkWidget * notebook)
{
	GtkWidget *scrolledwindow1;
	GtkWidget *moduleText;
	GtkWidget *label3;
	GtkTextTag *tag;

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(mainWindow), "scrolledwindow1",
				 scrolledwindow1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	gtk_container_add(GTK_CONTAINER(notebook), scrolledwindow1);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);
	gtk_paned_pack1(GTK_PANED(hpaned2), scrolledwindow1, FALSE, TRUE);

	moduleText = gtk_text_view_new();
	gtk_text_view_set_wrap_mode (GTK_TEXT_VIEW (moduleText),
                               GTK_WRAP_WORD);
	gtk_object_set_data(GTK_OBJECT(mainWindow), "moduleText",
			    moduleText);
	gtk_widget_show(moduleText);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), moduleText);
	gtk_widget_realize(moduleText);
	/*  tag */
	CreateTextTags(GTK_TEXT_VIEW(moduleText)->buffer);

	label3 = gtk_label_new(mod->Name());
	gtk_widget_ref(label3);
	gtk_object_set_data_full(GTK_OBJECT(mainWindow), "label3", label3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(notebook),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (notebook),
							     dictcount++),
				   label3);


	entryDisplay = new GTKEntryDisp(moduleText);
	mod->Disp(entryDisplay);
	mod->SetKey("GRACE");
	mod->Display();

}

/*
 *dictSearchTextChangedSWORD - dict lookup text changed
 *  mytext from the dict dict lookup entry
 */
void MainWindow::dictSearchTextChangedSWORD(char *mytext)
{				//-- mytext from the dict dict lookup entry
	if (curdictMod) {	//-- if we have a dict module  
		if (strcmp(mytext, "")) {	//-- if text is not null                
			curdictMod->SetKey(mytext);	//-- set key to our text
			curdictMod->Display();	//-- show what we found
			FillDictKeysSWORD(curdictMod->Name());	//-- fill the list widget with keys
			/*g_warning("key = %s\nmodName = %s", mytext,
				  curdictMod->Name());*/
		}
	}
}


/*** fill clist with dictionary keys - number of keys depends on hight of list widget and size of font ***/
void MainWindow::FillDictKeysSWORD(gchar * modName)
{
	ModMap::iterator it;	//-- iterator to go through modules and find modName
	int index = 0;		//-- for index into list widget
	SWKey saveKey;		//-- for starting point
	int i, j, count;	//-- counters
	gchar *listitem;	//-- hold item until added to list

	j = 0;
	it = mainMgr->Modules.find(modName);	//-- find module to use for search
	if (it != mainMgr->Modules.end()) {	//-- if we dont reach the end of our modules 
		SWModule *mod = (*it).second;	//-- temp module to work with
		mod->KeyText();	//-- snap to entry
		saveKey = mod->KeyText();	//-- save our place
		count = GTK_CLIST(clistKeys)->clist_window_height / GTK_CLIST(clistKeys)->row_height;	//-- how many items do we need to fill our clistKeys    
		gtk_clist_clear(GTK_CLIST(clistKeys));	//-- start with empty list
		if (mod) {	//-- make sure we have module to work with             
			for (i = 0; i < (count / 2); i++)
				(*mod)++;	//-- get equal number of keys before and after our starting key(saveKey)
			for (i = 0; i < count - 1; i++)
				(*mod)--;
			mod->Error();	//-- clear errors
			for (; !mod->Error() && count; count--, (*mod)++) {
				++j;
				listitem = g_strdup((const char *) mod->KeyText());	//-- key to listitem
				gtk_clist_append(GTK_CLIST(clistKeys), &listitem);	//-- listitem to list
				g_free(listitem);
				if (saveKey == mod->Key())	//-- if we are back to starting place set index
					index = j - 1;
			}
		}
		gtk_clist_moveto(GTK_CLIST(clistKeys), index, 0, 0.5, 0.0);	//-- move in list to index
		mod->SetKey(saveKey);
	}

}

void MainWindow::changeSidebarPage(int page)
{
	switch (page) {
	case 0:		//   bookmarks               
		gtk_widget_show(buttonSearch2);
		gtk_widget_show(buttonSR2);

		gtk_widget_hide(buttonSearch1);
		gtk_widget_hide(buttonSR1);
		break;
	case 1:		//   search  
		gtk_widget_show(buttonSearch1);
		gtk_widget_show(buttonSR2);
	
		gtk_widget_hide(buttonSearch2);
		gtk_widget_hide(buttonSR1);
		break;
	case 2:		//   search resluts 
		gtk_widget_show(buttonSearch1);
		gtk_widget_show(buttonSR1);

		gtk_widget_hide(buttonSearch2);
		gtk_widget_hide(buttonSR2);
		break;
	}
	gtk_notebook_set_page(GTK_NOTEBOOK(notebookSidebar), page);
}

void MainWindow::changeVerse(gint verse)
{
	VerseKey key = curMod->KeyText();	
	printf("verse = %d\n",verse);
	key.Verse(verse);
	//curMod->SetKey(key);
	gchar *text = g_strdup(key);
	gtk_entry_set_text(GTK_ENTRY(lookupText), text);
	g_free(text);
	lookupTextChanged();	
}


void MainWindow::lookupTextChanged()
{
	string keyText;
	gchar *entryText;
	char tmpBuf[255];

	entryText = (gchar*)gtk_entry_get_text(GTK_ENTRY(lookupText));
	keyText = entryText;

	if (curMod) {
		curMod->SetKey(keyText.c_str());
		curMod->Display();
		sprintf(tmpBuf, "%s (%s)", curMod->KeyText(),
			curMod->Name());
		gtk_label_set(GTK_LABEL(statusbar1), tmpBuf);

	}

	if (curcommMod) {
		curcommMod->SetKey(keyText.c_str());
		curcommMod->Display();
	}
}

void MainWindow::changeTextMod(gint modNum)
{
	ModMap::iterator it;
	gint i=0;
	
    for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
	if (it != mainMgr->Modules.end()) {
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			
			if(i == modNum){
				curMod = (*it).second;
				lookupTextChanged();
			}
			++i;
			
		}
	}
    }
}

void MainWindow::changeCommMod(gint modNum)
{
	ModMap::iterator it;
	gint i=0;
	
    for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
	if (it != mainMgr->Modules.end()) {
		if (!strcmp((*it).second->Type(), "Commentaries")) {
			
			if(i == modNum){
				curcommMod = (*it).second;
				lookupTextChanged();
			}
			++i;
			
		}
	}
    }
}

void MainWindow::changeDictMod(gint modNum)
{
	ModMap::iterator it;
	gint i=0;
	
    for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
	if (it != mainMgr->Modules.end()) {
		if (!strcmp((*it).second->Type(), "Lexicons / Dictionaries")) {
			
			if(i == modNum){
				curdictMod = (*it).second;
				lookupTextChanged();
			}
			++i;
			
		}
	}
    }
}

void MainWindow::viewModActivate(gchar *modName)
{
	ModMap::iterator it;

	it = mainMgr->Modules.find(modName);
	if (it != mainMgr->Modules.end()) {
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			curMod = (*it).second;
			lookupTextChanged();
		}

		if (!strcmp((*it).second->Type(), "Commentaries")) {
			curcommMod = (*it).second;
			lookupTextChanged();
		}

		if (!strcmp
		    ((*it).second->Type(), "Lexicons / Dictionaries")) {
			curdictMod = (*it).second;
			/*g_warning("gtksword 1417 curdictMod->Name = %s",
				  curdictMod->Name());*/
			FillDictKeysSWORD(curdictMod->Name());
		}

	}
}


void MainWindow::navigateButtonClicked(int direction)
{
	if (curMod) {
		if (direction)
			(*curMod)++;
		else
			(*curMod)--;
		(const char *) *curMod;	// snap to closest locations
		gchar *text = g_strdup(curMod->KeyText());
		gtk_entry_set_text(GTK_ENTRY(lookupText), text);
		g_free(text);
		lookupTextChanged();
              //curMod->Display();
	}
}


void MainWindow::searchButtonClicked()
{
	string srchText;
	gchar *entryText, buf[80];
	const gchar *resultText;
	gchar **clistText = (gchar **) & resultText;
	gint count = 0;

	gtk_clist_clear(GTK_CLIST(resultList));

	entryText = (gchar*)gtk_entry_get_text(GTK_ENTRY(searchText));
	srchText = entryText;

	if (curMod) {
		int searchType =
		    GTK_TOGGLE_BUTTON(regexSearch)->
		    active ? 0 : GTK_TOGGLE_BUTTON(phraseSearch)->
		    active ? -1 : -2;
		int searchParams =
		    GTK_TOGGLE_BUTTON(caseSensitive)->
		    active ? 0 : REG_ICASE;
		gtk_clist_freeze(GTK_CLIST(resultList));
		for (ListKey searchResults =
		     curMod->Search(srchText.c_str(), searchType,
				    searchParams); !searchResults.Error();
		     searchResults++) {
			++count;
			resultText = (const char *) searchResults;
			//g_warning((const char *) searchResults);
			gtk_clist_append(GTK_CLIST(resultList), clistText);
		}
		gtk_clist_thaw(GTK_CLIST(resultList));

		if (count) {
			sprintf(buf, "%d Occurence(s) of", count);
			gtk_label_set_text(GTK_LABEL(labelNumFound), buf);
			gtk_label_set_text(GTK_LABEL(labelSearchText),
					   srchText.c_str());
			changeSidebarPage(5);
		}
	}
}


void MainWindow::resultListSelectionChanged(GtkWidget * clist, gint row,
					    gint column,
					    GdkEventButton * event,
					    gpointer data)
{
	gchar *text;

	gtk_clist_get_text(GTK_CLIST(resultList), row, column, &text);
	//g_warning(text);
	if (text) {
		gtk_entry_set_text(GTK_ENTRY(lookupText), text);
	}
}

/* 
 * SideBarChangeMod - bad hack but it works
 * 
 */
void MainWindow::SideBarChangeMod(gchar * modName)
{
	ModMap::iterator it;
	gint 
		gbibleindex = -1, 
		commindex = 0, 
		dictindex = 0;

	for (it = mainMgr->Modules.begin(); it != mainMgr->Modules.end(); it++) {
		
		if (!strcmp((*it).second->Type(), "Biblical Texts")) {
			if (!strcmp((*it).second->Name(), modName)) {
				gtk_notebook_set_page(GTK_NOTEBOOK
						      (notebookText),
						      gbibleindex);
				return;
			}
			++gbibleindex;
			
		} else if (!strcmp((*it).second->Type(), "Commentaries")) {
			if (!strcmp((*it).second->Name(), modName)) {
				gtk_notebook_set_page(GTK_NOTEBOOK
						      (notebookCommentary),
						      commindex);
				return;
			}
			++commindex;
			
		} else if (!strcmp
			((*it).second->Type(),
			 "Lexicons / Dictionaries")) {
			if (!strcmp((*it).second->Name(), modName)) {
				gtk_notebook_set_page(GTK_NOTEBOOK
						      (notebookDictionary),
						      dictindex);
				return;
			}
			++dictindex;
		}
	}
}

GdkColor GTKEntryDisp::colourBlue;
GdkColor GTKEntryDisp::colourGreen;

/*										*/
/*	Note: we have a warning that should be fixed here.  GTKEntryDisp::Display() is defined	*/
/*	as returning char, but doesn't have a return statement				*/
/*	What was intended - a return code?  That doesn't seem right, because those are	*/
/*	usually int's.  But it isn't defined as returning a char*, so it doesn't seem		*/
/*	likely that it's intended to return the text involved.					*/
/*										*/
char GTKEntryDisp::Display(SWModule & imodule)
{
	char tmpBuf[255];
	GtkTextIter iter,startiter, enditer;
	GString *str;	
	
	gtk_text_buffer_set_text(GTK_TEXT_VIEW(gtkText)->buffer,
                                "",
                                -1);
	
	str = g_string_new("");	
	g_string_printf(str,"[%s]",  imodule.KeyText());
	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_VIEW(gtkText)->buffer, &iter, 0);
	gtk_text_buffer_insert_at_cursor (GTK_TEXT_VIEW(gtkText)->buffer, str->str, str->len);
	
	g_string_printf(str,"%s",  (const gchar*)imodule);
//	gtk_text_buffer_get_iter_at_offset (GTK_TEXT_VIEW(gtkText)->buffer, &iter, 0);
//	gtk_text_buffer_insert_at_cursor (GTK_TEXT_VIEW(gtkText)->buffer, str->str, str->len);
	applyTags(GTK_TEXT_VIEW(gtkText)->buffer, iter, str, false);
	/*	gtk_text_buffer_get_start_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                              &startiter);
		gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                              &enditer);
		gtk_text_buffer_apply_tag_by_name(GTK_TEXT_VIEW(gtkText)->buffer, 
				"ltr_quote", 
				&startiter, 
				&enditer);
	*/


	g_string_free(str,0);
	return 0;		// makes complier happy
}

/*

*/
char GTKChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[255];
	GtkTextIter iter,startiter, enditer;
	GString *str;
	gchar *utf8str;
	GtkTextMark   *mark = NULL;
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;	
	gfloat adjVal;
	GtkTextIter *curiter;
	const gchar *mark_name = "CurrentVerse";
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	bool rtol = false;
	
	if ((sit = gtkswordWindow->mainMgr->config->Sections.find(imodule.Name())) !=
	    gtkswordWindow->mainMgr->config->Sections.end()) {
		if ((eit = (*sit).second.find("Direction")) !=
		    (*sit).second.end()) {
			if(!strcmp((char *) (*eit).second.c_str(),"RtoL"))
				rtol = true;
			else
				rtol = false;
		}
	}
	
	gtk_text_buffer_set_text(GTK_TEXT_VIEW(gtkText)->buffer,
                                "",
                                -1);
	
	str = g_string_new("");
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter
	      && !imodule.Error()); imodule++) {
		g_string_printf(str, "%d", key->Verse());
	        gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                       &iter);
		gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW(gtkText)->buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (GTK_TEXT_VIEW(gtkText)->buffer),
                                                                   "p_verseNumber"),
                                        NULL);
		      
		      
		      //g_string_free(str,0);
	        if(rtol)
			g_string_printf(str, " %s ", (const gchar*)imodule);
		else
			g_string_printf(str, ". %s\n", (const gchar*)imodule);
			
	        if(key->Verse() == curVerse) {
		        gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                       &iter);
			g_warning("curverse = %d\n keyverse = %d",curVerse ,key->Verse());
			mark = gtk_text_buffer_create_mark (GTK_TEXT_VIEW(gtkText)->buffer,
						mark_name,
						&iter,
						TRUE);
			gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                       &iter);
			gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW(gtkText)->buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (GTK_TEXT_VIEW(gtkText)->buffer),
                                                                   "fg_currentverse"),
                                        NULL);
			
		} else {	
			gtk_text_buffer_get_iter_at_offset (GTK_TEXT_VIEW(gtkText)->buffer, &iter, 0);
			gtk_text_buffer_insert_at_cursor (GTK_TEXT_VIEW(gtkText)->buffer, str->str, str->len);
		}
		
	}
	g_string_free(str,0);
	

	if(rtol) {
		gtk_text_buffer_get_start_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                              &startiter);
		gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                              &enditer);
		gtk_text_buffer_apply_tag_by_name(GTK_TEXT_VIEW(gtkText)->buffer, 
				"rtl_quote", 
				&startiter, 
				&enditer);
	} 
		
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(gtkText),
                                mark,
                                false,
                                true,
                                0.0,
                                0.0);
	gtk_text_buffer_delete_mark(GTK_TEXT_VIEW(gtkText)->buffer,
                                mark);
				
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);				
	return 0;		// makes complier happy
}

/***************************************************************************** 
* this will handle some html formating a chapter at a time for displaying 
* Bible text
* imodule - text to format for gtktext widget
******************************************************************************/
char HTMLChapDisp::Display(SWModule & imodule)
{
	char tmpBuf[255];
	GtkTextIter iter,startiter, enditer;
	GString *str;
	gchar *utf8str;
	GtkTextMark   *mark = NULL;
	VerseKey *key = (VerseKey *) (SWKey *) imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	const gchar *mark_name = "CurrentVerse";
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;
	bool rtol = false;
	
	if ((sit = gtkswordWindow->mainMgr->config->Sections.find(imodule.Name())) !=
	    gtkswordWindow->mainMgr->config->Sections.end()) {
		if ((eit = (*sit).second.find("Direction")) !=
		    (*sit).second.end()) {
			if(!strcmp((char *) (*eit).second.c_str(),"RtoL"))
				rtol = true;
			else
				rtol = false;
		}
	}
	
	gtk_text_buffer_set_text(GTK_TEXT_VIEW(gtkText)->buffer,
                                "",
                                -1);
	
	str = g_string_new("");
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error()); imodule++) {
		g_string_printf(str, "%d", key->Verse());
	        gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                       &iter);
		gtk_text_buffer_insert_with_tags (GTK_TEXT_VIEW(gtkText)->buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (GTK_TEXT_VIEW(gtkText)->buffer),
                                                                   "verseNumber"),
                                        NULL);
	
	        if(rtol)
			g_string_printf(str, " %s ", (const gchar*)imodule);
		else
			g_string_printf(str, ". %s\n", (const gchar*)imodule);
	        if(key->Verse() == curVerse) {
		        gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                       &iter);
			g_warning("curverse = %d\n keyverse = %d",curVerse ,key->Verse());
			mark = gtk_text_buffer_create_mark (GTK_TEXT_VIEW(gtkText)->buffer,
						mark_name,
						&iter,
						TRUE);
			applyTags(GTK_TEXT_VIEW(gtkText)->buffer, iter, str, true);
		} else {
			applyTags(GTK_TEXT_VIEW(gtkText)->buffer, iter, str, false);
		}		
	}
	g_string_free(str,0);
	

	if(rtol) {
		gtk_text_buffer_get_start_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                              &startiter);
		gtk_text_buffer_get_end_iter(GTK_TEXT_VIEW(gtkText)->buffer,
                                              &enditer);
		gtk_text_buffer_apply_tag_by_name(GTK_TEXT_VIEW(gtkText)->buffer, 
				"rtl_quote", 
				&startiter, 
				&enditer);
	} 
		
	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(gtkText),
                                mark,
                                false,
                                true,
                                0.0,
                                0.0);
	gtk_text_buffer_delete_mark(GTK_TEXT_VIEW(gtkText)->buffer,
                                mark);
				
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);				
	return 0;		// makes complier happy
}

/***  ***/
void GTKEntryDisp::applyTags(GtkTextBuffer *buffer, GtkTextIter iter, GString *str, bool bcurrentVerse)
{
	int j = 0;
	int i = 0;
	int taglen = 0;
	char 
		*tverse = "fg_verse",
		*tsheb = "strongshebrew",
		*tsgreek = "strongsgreek",
		*tgmorph = "morphgreek",
		*titalic = "italic",
		*tbold = "bold",
		*tcite = "cite",
		*tfootnote = "footnote";
	
	if(bcurrentVerse) {
		tverse = "fg_currentverse",
		tsheb = "cv_strongshebrew",
		tsgreek = "cv_strongsgreek",
		tgmorph = "cv_morphgreek",
		titalic = "cv_italic",
		tbold = "cv_bold",
		tcite = "cv_cite";
	}
	//g_print("\n%s\n", str->str);
	char *verseBuf = new char[str->len +1];
	char *tag = new char[str->len];
	while(i < str->len ){
		if(str->str[i] == '<'){ 
			tag[0] = '\0';
			taglen = gettags(str->str, tag, i);	/* get html tags */
			if (tag) {
				i = i + taglen-1;	/* remove tags (we do not want to see them) */
				/* strongs numbers */
				if(!strncmp(tag, "<sync type=\"Strongs\" value=\"",  28)) {
					gtk_text_buffer_get_end_iter(buffer, &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);									
					j = 0;
					verseBuf[j] = '\0';
					for (unsigned int x = 28; tag[x] != '\"'; x++){
						verseBuf[j] = tag[x];
						++j;						
						verseBuf[j] = '\0';
					}
					verseBuf[j] = ' ';
						++j;				
					verseBuf[j] = '\0';
					gtk_text_buffer_get_end_iter(buffer, &iter);
					
					//g_print("\ntag = %s\ntagbuf = %s\n",tag,verseBuf);
					if(verseBuf[0] == 'H' || (verseBuf[0] == 'T' && verseBuf[1] == 'H')){
						++verseBuf;
						gtk_text_buffer_insert_with_tags (buffer, &iter,
							verseBuf,
							strlen(verseBuf),
							gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
							tsheb),
							NULL);
						
					} else if(verseBuf[0] == 'G' || (verseBuf[0] == 'T' && verseBuf[1] == 'G')){
						++verseBuf;
						gtk_text_buffer_insert_with_tags (buffer, &iter,
							verseBuf,
							strlen(verseBuf),
							gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
							tsgreek),
							NULL);
					}					
					j = 0;
					verseBuf[j] = '\0';
				} 
				
				else if(!strncmp(tag, "<sync type=\"morph\" class=\"Packard\" value=\"",  41)) {
					gtk_text_buffer_get_end_iter(buffer, &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);									
					j = 0;
					verseBuf[j] = ' ';	
					++j;			
					verseBuf[j] = '\0';					
					for (unsigned int x = 42; tag[x] != '\"'; x++){
						verseBuf[j] = tag[x];
						++j;						
						verseBuf[j] = '\0';
					}
					verseBuf[j] = ' ';
					++j;				
					verseBuf[j] = '\0';
					gtk_text_buffer_get_end_iter(buffer, &iter);
					
					//g_print("\ntag = %s\ntagbuf = %s\n",tag,verseBuf);
					
					gtk_text_buffer_insert_with_tags (buffer, &iter,
							verseBuf,
							strlen(verseBuf),
							gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
							tgmorph),
							NULL);
								
					j = 0;
					verseBuf[j] = '\0';
				}
				/* new line */
				else if (!strcmp(tag, "<br />")) {
					verseBuf[j] = '\n';
					verseBuf[j+1] =  '\0';
					gtk_text_buffer_get_end_iter(buffer, &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);
					j = 0;
					verseBuf[j] = '\0';
				}
				/* footnotes open */
				else if (!strncmp(tag, "<note>", 6)) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);
					j = 0;
					verseBuf[j] = '\0';
				}
				/* footnotes close */
				else if (!strcmp(tag, "</note>")) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tfootnote),
						NULL);
					j = 0;			
					verseBuf[j] = '\0';
				}
				
				else if (!strcmp(tag, "<i>")) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);
					j = 0;
					verseBuf[j] = '\0';
				}
				
				else if (!strcmp(tag, "</i>")) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						titalic),
						NULL);
					j = 0;			
					verseBuf[j] = '\0';
				}
				
				else if (!strcmp(tag, "<b>")) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);
					j = 0;
					verseBuf[j] = '\0';
				}
				
				else if (!strcmp(tag, "</b>")) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tbold),
						NULL);
					j = 0;			
					verseBuf[j] = '\0';
				}
				
				else if (!strcmp(tag, "<cite>")) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);
					j = 0;
					verseBuf[j] = '\0';
				}
				
				else if (!strcmp(tag, "</cite>")) {
					gtk_text_buffer_get_end_iter(buffer,
						       &iter);
					gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tcite),
						NULL);
					j = 0;			
					verseBuf[j] = '\0';
				}
			}
						
		} else {
			verseBuf[j] = str->str[i];					
			verseBuf[j + 1] = '\0';	
			++j;				
		}
		++i;
	}
	gtk_text_buffer_get_end_iter(buffer, &iter);
	gtk_text_buffer_insert_with_tags (buffer, &iter,
						verseBuf,
						strlen(verseBuf),
						gtk_text_tag_table_lookup (gtk_text_buffer_get_tag_table (buffer),
						tverse),
						NULL);
//	delete[]verseBuf;
	verseBuf = NULL;
	delete[]tag;
	tag = NULL;
}

/***************************************************************************** 
* read html tags for formatting text in gtktext widget
* text - text from (const char*)imodule filtered for html
* tag - html tag
* pos - position in text to start looking for tag
******************************************************************************/
gint GTKEntryDisp::gettags(gchar * text, gchar * tag, gint pos)
{
	gint i, j, len;

	j = 0;
	len = strlen(text);
	tag[0] = '\0';
	for (i = pos; i < len; i++) {		
		tag[j] = text[i];		
		++j;
		//g_warning("tag length = %d",j);
		tag[j] = '\0';
		if (text[i] == '>') {
			return j;
		}
	}
	return 0;
}

