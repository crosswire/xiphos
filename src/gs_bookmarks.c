/***************************************************************************
                                    gs_bookmarks.c
                             -------------------
    begin                : Thu July 05 2001
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
#include <math.h>

#include "gs_bookmarks.h"
#include "sw_bookmarks.h"
#include "gs_sword.h"
#include "gs_gnomesword.h"

GdkPixmap *pixmap1;
GdkPixmap *pixmap2;
GdkPixmap *pixmap3;
GdkBitmap *mask1;
GdkBitmap *mask2;
GdkBitmap *mask3;
GtkStyle *style;
extern GtkWidget *MainFrm;
BM_TREE bmtree;
BM_TREE *p_bmtree;
extern GS_APP gs;
extern GtkCTreeNode *personal_node;

static gint books = 0;
static gint pages = 0;
static char * book_open_xpm[] = {
"16 16 4 1",
"       c None s None",
".      c black",
"X      c #808080",
"o      c white",
"                ",
"  ..            ",
" .Xo.    ...    ",
" .Xoo. ..oo.    ",
" .Xooo.Xooo...  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
" .Xooo.Xooo.X.  ",
" .Xooo.oooo.X.  ",
"  .Xoo.Xoo..X.  ",
"   .Xo.o..ooX.  ",
"    .X..XXXXX.  ",
"    ..X.......  ",
"     ..         ",
"                "};

static char * book_closed_xpm[] = {
"16 16 6 1",
"       c None s None",
".      c black",
"X      c red",
"o      c yellow",
"O      c #808080",
"#      c white",
"                ",
"       ..       ",
"     ..XX.      ",
"   ..XXXXX.     ",
" ..XXXXXXXX.    ",
".ooXXXXXXXXX.   ",
"..ooXXXXXXXXX.  ",
".X.ooXXXXXXXXX. ",
".XX.ooXXXXXX..  ",
" .XX.ooXXX..#O  ",
"  .XX.oo..##OO. ",
"   .XX..##OO..  ",
"    .X.#OO..    ",
"     ..O..      ",
"      ..        ",
"                "};

static char * mini_page_xpm[] = {
"16 16 4 1",
"       c None s None",
".      c black",
"X      c white",
"o      c #808080",
"                ",
"   .......      ",
"   .XXXXX..     ",
"   .XoooX.X.    ",
"   .XXXXX....   ",
"   .XooooXoo.o  ",
"   .XXXXXXXX.o  ",
"   .XooooooX.o  ",
"   .XXXXXXXX.o  ",
"   .XooooooX.o  ",
"   .XXXXXXXX.o  ",
"   .XooooooX.o  ",
"   .XXXXXXXX.o  ",
"   ..........o  ",
"    oooooooooo  ",
"                "};


void after_press (GtkCTree *ctree, gpointer data)
{

	
}
void
on_ctree_select_row(GtkCList * clist,
		       gint row,
		       gint column, GdkEvent * event, gpointer user_data)
{
	GtkCTreeNode *node;
	gchar *modName, *key;
	GtkCTree *ctree;
	
	ctree = user_data;
	node = gtk_ctree_node_nth(p_bmtree->ctree, row);
	if (GTK_CTREE_ROW (node)->is_leaf){
		key = GTK_CELL_PIXTEXT (GTK_CTREE_ROW (node)->row.cell[0])->text;
		modName = GTK_CELL_PIXTEXT (GTK_CTREE_ROW (node)->row.cell[1])->text;
		
		/*gtk_ctree_get_node_info(ctree,
						  node,
						  &text,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL,
						  NULL);*/	
		gotBookmarkSWORD(modName,key);
		//changeVerseSWORD(key);
	}
}

void after_move (GtkCTree *ctree, GtkCTreeNode *child, GtkCTreeNode *parent, 
		 GtkCTreeNode *sibling, gpointer data)
{
  char *source;
  char *target1;
  char *target2;

  gtk_ctree_get_node_info (ctree, child, &source, 
			   NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (parent)
    gtk_ctree_get_node_info (ctree, parent, &target1, 
			     NULL, NULL, NULL, NULL, NULL, NULL, NULL);
  if (sibling)
    gtk_ctree_get_node_info (ctree, sibling, &target2, 
			     NULL, NULL, NULL, NULL, NULL, NULL, NULL);

  g_print ("Moving \"%s\" to \"%s\" with sibling \"%s\".\n", source,
	   (parent) ? target1 : "nil", (sibling) ? target2 : "nil");
}

void count_items (GtkCTree *ctree, GtkCTreeNode *list)
{
  if (GTK_CTREE_ROW (list)->is_leaf)
    pages--;
  else
    books--;
}

void expand_all (GtkWidget *widget, GtkCTree *ctree)
{
  gtk_ctree_expand_recursive (ctree, NULL);
  after_press (ctree, NULL);
}

void collapse_all (GtkWidget *widget, GtkCTree *ctree)
{
  gtk_ctree_collapse_recursive (ctree, NULL);
  after_press (ctree, NULL);
}

void select_all (GtkWidget *widget, GtkCTree *ctree)
{
  gtk_ctree_select_recursive (ctree, NULL);
  after_press (ctree, NULL);
}

void change_style (GtkWidget *widget, GtkCTree *ctree)
{
  static GtkStyle *style1 = NULL;
  static GtkStyle *style2 = NULL;

  GtkCTreeNode *node;
  GdkColor col1;
  GdkColor col2;

  if (GTK_CLIST (ctree)->focus_row >= 0)
    node = GTK_CTREE_NODE
      (g_list_nth (GTK_CLIST (ctree)->row_list,GTK_CLIST (ctree)->focus_row));
  else
    node = GTK_CTREE_NODE (GTK_CLIST (ctree)->row_list);

  if (!node)
    return;

  if (!style1)
    {
      col1.red   = 0;
      col1.green = 56000;
      col1.blue  = 0;
      col2.red   = 32000;
      col2.green = 0;
      col2.blue  = 56000;

      style1 = gtk_style_new ();
      style1->base[GTK_STATE_NORMAL] = col1;
      style1->fg[GTK_STATE_SELECTED] = col2;

      style2 = gtk_style_new ();
      style2->base[GTK_STATE_SELECTED] = col2;
      style2->fg[GTK_STATE_NORMAL] = col1;
      style2->base[GTK_STATE_NORMAL] = col2;
      gdk_font_unref (style2->font);
      style2->font =
	gdk_font_load ("-*-courier-medium-*-*-*-*-300-*-*-*-*-*-*");
    }

  gtk_ctree_node_set_cell_style (ctree, node, 1, style1);
  gtk_ctree_node_set_cell_style (ctree, node, 0, style2);

  if (GTK_CTREE_ROW (node)->children)
    gtk_ctree_node_set_row_style (ctree, GTK_CTREE_ROW (node)->children,
				  style2);
}

void unselect_all (GtkWidget *widget, GtkCTree *ctree)
{
  gtk_ctree_unselect_recursive (ctree, NULL);
  after_press (ctree, NULL);
}

void remove_selection (GtkWidget *widget, GtkCTree *ctree)
{
  GtkCList *clist;
  GtkCTreeNode *node;

  clist = GTK_CLIST (ctree);

  gtk_clist_freeze (clist);

  while (clist->selection)
    {
      node = clist->selection->data;

      if (GTK_CTREE_ROW (node)->is_leaf)
	pages--;
      else
	gtk_ctree_post_recursive (ctree, node,
				  (GtkCTreeFunc) count_items, NULL);

      gtk_ctree_remove_node (ctree, node);

      if (clist->selection_mode == GTK_SELECTION_BROWSE)
	break;
    }

  if (clist->selection_mode == GTK_SELECTION_EXTENDED && !clist->selection &&
      clist->focus_row >= 0)
    {
      node = gtk_ctree_node_nth (ctree, clist->focus_row);

      if (node)
	gtk_ctree_select (ctree, node);
    }
    
  gtk_clist_thaw (clist);
  after_press (ctree, NULL);
}

struct _ExportStruct {
  gchar *tree;
  gchar *info;
  gboolean is_leaf;
};

typedef struct _ExportStruct ExportStruct;

gboolean
gnode2ctree (GtkCTree   *ctree,
	     guint       depth,
	     GNode        *gnode,
	     GtkCTreeNode *cnode,
	     gpointer    data)
{
  ExportStruct *es;
  GdkPixmap *pixmap_closed;
  GdkBitmap *mask_closed;
  GdkPixmap *pixmap_opened;
  GdkBitmap *mask_opened;

  if (!cnode || !gnode || (!(es = gnode->data)))
    return FALSE;

  if (es->is_leaf)
    {
      pixmap_closed = pixmap3;
      mask_closed = mask3;
      pixmap_opened = NULL;
      mask_opened = NULL;
    }
  else
    {
      pixmap_closed = pixmap1;
      mask_closed = mask1;
      pixmap_opened = pixmap2;
      mask_opened = mask2;
    }

  gtk_ctree_set_node_info (ctree, cnode, es->tree, 2, pixmap_closed,
			   mask_closed, pixmap_opened, mask_opened,
			   es->is_leaf, (depth < 3));
  gtk_ctree_node_set_text (ctree, cnode, 1, es->info);
  g_free (es);
  gnode->data = NULL;

  return TRUE;
}

gboolean
ctree2gnode (GtkCTree   *ctree,
	     guint       depth,
	     GNode        *gnode,
	     GtkCTreeNode *cnode,
	     gpointer    data)
{
  ExportStruct *es;

  if (!cnode || !gnode)
    return FALSE;
  
  es = g_new (ExportStruct, 1);
  gnode->data = es;
  es->is_leaf = GTK_CTREE_ROW (cnode)->is_leaf;
  es->tree = GTK_CELL_PIXTEXT (GTK_CTREE_ROW (cnode)->row.cell[0])->text;
  es->info = GTK_CELL_PIXTEXT (GTK_CTREE_ROW (cnode)->row.cell[1])->text;
  return TRUE;
}

void export_ctree (GtkWidget *widget, GtkCTree *ctree)
{
  char *title[] = { "Tree" , "Info" };
  static GtkWidget *export_window = NULL;
  static GtkCTree *export_ctree;
  GtkWidget *vbox;
  GtkWidget *scrolled_win;
  GtkWidget *button;
  GtkWidget *sep;
  GNode *gnode;
  GtkCTreeNode *node;

  if (!export_window)
    {
      export_window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  
      gtk_signal_connect (GTK_OBJECT (export_window), "destroy",
			  GTK_SIGNAL_FUNC (gtk_widget_destroyed),
			  &export_window);

      gtk_window_set_title (GTK_WINDOW (export_window), "exported ctree");
      gtk_container_set_border_width (GTK_CONTAINER (export_window), 5);

      vbox = gtk_vbox_new (FALSE, 0);
      gtk_container_add (GTK_CONTAINER (export_window), vbox);
      
      button = gtk_button_new_with_label ("Close");
      gtk_box_pack_end (GTK_BOX (vbox), button, FALSE, TRUE, 0);

      gtk_signal_connect_object (GTK_OBJECT (button), "clicked",
				 (GtkSignalFunc) gtk_widget_destroy,
				 GTK_OBJECT(export_window));

      sep = gtk_hseparator_new ();
      gtk_box_pack_end (GTK_BOX (vbox), sep, FALSE, TRUE, 10);

      export_ctree = GTK_CTREE (gtk_ctree_new_with_titles (2, 0, title));
      gtk_ctree_set_line_style (export_ctree, GTK_CTREE_LINES_DOTTED);

      scrolled_win = gtk_scrolled_window_new (NULL, NULL);
      gtk_container_add (GTK_CONTAINER (scrolled_win),
			 GTK_WIDGET (export_ctree));
      gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win),
				      GTK_POLICY_AUTOMATIC,
				      GTK_POLICY_AUTOMATIC);
      gtk_box_pack_start (GTK_BOX (vbox), scrolled_win, TRUE, TRUE, 0);
      gtk_clist_set_selection_mode (GTK_CLIST (export_ctree),
				    GTK_SELECTION_EXTENDED);
      gtk_clist_set_column_width (GTK_CLIST (export_ctree), 0, 200);
      gtk_clist_set_column_width (GTK_CLIST (export_ctree), 1, 200);
      gtk_widget_set_usize (GTK_WIDGET (export_ctree), 300, 200);
    }

  if (!GTK_WIDGET_VISIBLE (export_window))
    gtk_widget_show_all (export_window);
      
  gtk_clist_clear (GTK_CLIST (export_ctree));

  node = GTK_CTREE_NODE (g_list_nth (GTK_CLIST (ctree)->row_list,
				     GTK_CLIST (ctree)->focus_row));
  if (!node)
    return;

  gnode = gtk_ctree_export_to_gnode (ctree, NULL, NULL, node,
				     ctree2gnode, NULL);
  if (gnode)
    {
      gtk_ctree_insert_gnode (export_ctree, NULL, NULL, gnode,
			      gnode2ctree, NULL);
      g_node_destroy (gnode);
    }
}

void change_indent (GtkWidget *widget, GtkCTree *ctree)
{
  gtk_ctree_set_indent (ctree, GTK_ADJUSTMENT (widget)->value);
}

void change_spacing (GtkWidget *widget, GtkCTree *ctree)
{
  gtk_ctree_set_spacing (ctree, GTK_ADJUSTMENT (widget)->value);
}

void change_row_height (GtkWidget *widget, GtkCList *clist)
{
  gtk_clist_set_row_height (clist, GTK_ADJUSTMENT (widget)->value);
}

void set_background (GtkCTree *ctree, GtkCTreeNode *node, gpointer data)
{
  GtkStyle *style = NULL;
  
  if (!node)
    return;
  
  if (ctree->line_style != GTK_CTREE_LINES_TABBED)
    {
      if (!GTK_CTREE_ROW (node)->is_leaf)
	style = GTK_CTREE_ROW (node)->row.data;
      else if (GTK_CTREE_ROW (node)->parent)
	style = GTK_CTREE_ROW (GTK_CTREE_ROW (node)->parent)->row.data;
    }

  gtk_ctree_node_set_row_style (ctree, node, style);
}

/*
 *
 */
void
addbookmarktotree(gchar *modName, gchar *verse)
{
	gchar *text[2];
	
	text[0] = verse;
	text[1] = modName;
	gtk_ctree_insert_node(p_bmtree->ctree, personal_node, NULL,text, 3, pixmap3,mask3,NULL,NULL, TRUE, FALSE);
}


void
loadtree(GtkWidget *ctree1)
{	
	GdkColor transparent = { 0 };	
	bmtree.ctree = GTK_CTREE(ctree1);
	bmtree.ctree_widget = ctree1;
	p_bmtree = &bmtree;
	
	pixmap1 = gdk_pixmap_create_from_xpm_d(MainFrm->window, &mask1, &transparent, book_closed_xpm);
	pixmap2 = gdk_pixmap_create_from_xpm_d(MainFrm->window, &mask2, &transparent, book_open_xpm);
	pixmap3 = gdk_pixmap_create_from_xpm_d(MainFrm->window, &mask2, &transparent, mini_page_xpm);
	
	gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "button_press_event",
				GTK_SIGNAL_FUNC (after_press), NULL);
      gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "button_release_event",
				GTK_SIGNAL_FUNC (after_press), NULL);
      gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "tree_move",
				GTK_SIGNAL_FUNC (after_move), NULL);
      gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "end_selection",
				GTK_SIGNAL_FUNC (after_press), NULL);
      gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "toggle_focus_row",
				GTK_SIGNAL_FUNC (after_press), NULL);
      gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "select_all",
				GTK_SIGNAL_FUNC (after_press), NULL);
      gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "unselect_all",
				GTK_SIGNAL_FUNC (after_press), NULL);
      gtk_signal_connect_after (GTK_OBJECT (p_bmtree->ctree_widget), "scroll_vertical",
				GTK_SIGNAL_FUNC (after_press), NULL);					
	
	loadbookmarks(gs.ctree_widget);
	
	gtk_signal_connect(GTK_OBJECT(gs.ctree_widget), "select_row",
			   			GTK_SIGNAL_FUNC(on_ctree_select_row), bmtree.ctree);
	gtk_clist_set_row_height (GTK_CLIST(p_bmtree->ctree), 15);
	gtk_ctree_set_spacing(p_bmtree->ctree, 3);
	gtk_ctree_set_indent (p_bmtree->ctree, 8);
	gtk_clist_set_reorderable (GTK_CLIST(p_bmtree->ctree), TRUE);
}
