/*  
 * gtksword_cbs.h -  - Fri Oct 26 11:15:54 2001
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
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 


#include <gtk/gtk.h>


/* Use this function to set the directory containing installed pixmaps. */
void add_pixmap_directory(const gchar * directory);

/* This is used to create the pixmaps in the interface. */
GtkWidget *create_pixmap(GtkWidget * widget, const gchar * filename);

void on_entryDictionaryKey_changed(GtkEditable * editable, gpointer user_data);

void on_Exit3_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_Help_activate(GtkMenuItem * menuitem, gpointer user_data);

char *str_fixup_format(const char *string);

void on_About_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_lookupText_changed(GtkEditable * editable, gpointer user_data);

void on_searchButton_clicked(GtkButton * button, gpointer user_data);

void on_resultList_selection_changed(GtkWidget * clist, gint row, gint column,
				GdkEventButton * event, gpointer data);

void on_arrow1_button_press_event(GtkButton * button, gpointer user_data);

void on_arrow2_button_press_event(GtkButton * button, gpointer user_data);

void on_verse_style1_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_option_item_activate(GtkMenuItem * menuitem, gpointer user_data);

void on_notebookText_switch_page(GtkNotebook * notebook,
			    GtkNotebookPage * page,
			    gint page_num, gpointer user_data);

void on_notebookCommentary_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);

void on_notebookDictionary_switch_page(GtkNotebook * notebook,
				  GtkNotebookPage * page,
				  gint page_num, gpointer user_data);

void on_buttonSearchClose_clicked(GtkButton * button, gpointer user_data);

void on_mainWindow_destroy(GtkObject * object, gpointer user_data);

void on_buttonBT1_clicked(GtkButton * button, gpointer user_data);

void on_buttonCom1_clicked(GtkButton * button, gpointer user_data);

void on_buttonDict1_clicked(GtkButton * button, gpointer user_data);

void on_buttonBM1_clicked(GtkButton * button, gpointer user_data);

void on_buttonSearch1_clicked(GtkButton * button, gpointer user_data);

void on_buttonSR1_clicked(GtkButton * button, gpointer user_data);

void on_buttonCom2_clicked(GtkButton * button, gpointer user_data);

void on_buttonDict2_clicked(GtkButton * button, gpointer user_data);

void on_buttonBM2_clicked(GtkButton * button, gpointer user_data);

void on_buttonSearch2_clicked(GtkButton * button, gpointer user_data);

void on_buttonSR2_clicked(GtkButton * button, gpointer user_data);

/*void on_ctreeBookmarks_tree_select_row(GtkCTree * ctree,
				  GList * node,
				  gint column, gpointer user_data);*/

void on_buttonSBitem_clicked(GtkButton * button, gpointer user_data);

/*void on_clistKeys_select_row(GtkCList * clist,
			gint row,
			gint column, GdkEvent * event, gpointer user_data);*/
