/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                                      gs_editor.c
                             -------------------
    begin                : Mon June 18 2001
    copyright            : (C) 2001 by Terry Biggs
    email                : tbiggs@infinet.com
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
#include <gtk/gtk.h>
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmlengine.h>
#include <gtkhtml/htmlobject.h>
#include <gtkhtml/htmlengine-edit-clueflowstyle.h>
#include <gtkhtml/htmlengine-edit-cut-and-paste.h>
#include <gtkhtml/htmlengine-edit-fontstyle.h>
#include <gtkhtml/htmlengine-edit-rule.h>
#include <gtkhtml/htmlengine-edit-movement.h>
#include <gtkhtml/htmlengine-edit-cursor.h>
#include <gtkhtml/htmlengine-edit-text.h>
#include <gtkhtml/htmlengine-edit.h>
#include <gtkhtml/htmlengine-print.h>
#include <gtkhtml/htmlengine-save.h>
#include <gtkhtml/htmlsettings.h>
#include <gtkhtml/htmlcolorset.h>
#include <gtkhtml/htmlselection.h>
#include <gtkhtml/htmlcursor.h>

#include "gs_editor.h"
#include "gs_control_data.h"

GtkWidget *spHTML;
GtkWidget *statusbarNE;
GtkWidget *statusbarSP;  	
GtkWidget *tgbtnEDEdit;



/*****************************************************************************
 *       Editor  callbacks
 *
 *****************************************************************************/


/*****************************************************************************
 *       Editor button callbacks
 *
 *****************************************************************************/

static void on_btnEDNew_clicked(GtkButton * button, gboolean ispercom)
{
	if(!ispercom) g_warning("this is not percom");
		else g_warning("this is percom");
}


static void on_btnEDOpen_clicked(GtkButton * button, gboolean ispercom)
{

}


static void on_btnEDSave_clicked(GtkButton * button, gboolean ispercom)
{

}


static void on_btnEDSaveAs_clicked(GtkButton * button, gboolean ispercom)
{

}


static void on_btnEDPrint_clicked(GtkButton * button, gboolean ispercom)
{

}


static void
on_btnEDSaveNote_clicked(GtkButton * button, gboolean ispercom)
{

}


static void
on_btnEDPrintNote_clicked(GtkButton * button, gboolean ispercom)
{

}


static void
on_tgbtnEDEdit_toggled(GtkToggleButton * togglebutton, gboolean ispercom)
{				
	gtk_html_set_editable (GTK_HTML(spHTML), togglebutton->active );
}


static void on_btnEDCut_clicked(GtkButton * button, gboolean ispercom)
{

}


static void on_btnEDCopy_clicked(GtkButton * button, gboolean ispercom)
{

}


static void on_btnEDPaste_clicked(GtkButton * button, gboolean ispercom)
{

}


static void on_btnEDBold_clicked(GtkButton * button, gboolean ispercom)
{
	   if (GTK_TOGGLE_BUTTON (tgbtnEDEdit)->active){
	   	gtk_html_set_font_style (GTK_HTML (spHTML),
	   		GTK_HTML_FONT_STYLE_MAX,
	   		GTK_HTML_FONT_STYLE_BOLD);  	   	
	   }
}


static void on_btnEDItalics_clicked(GtkButton * button, gboolean ispercom)
{	
	if (GTK_TOGGLE_BUTTON (tgbtnEDEdit)->active){
	   	gtk_html_set_font_style (GTK_HTML (spHTML),
	   		GTK_HTML_FONT_STYLE_MAX,
	   		GTK_HTML_FONT_STYLE_ITALIC);  	   	
	   }			


}


static void
on_btnEDUnderline_clicked(GtkButton * button, gboolean ispercom)
{

}


static void
on_btnEDStriketh_clicked(GtkButton * button, gboolean ispercom)
{

}


static void on_btnEDIndent_clicked(GtkButton * button, gboolean ispercom)
{

}


static void
on_btnEDUnindent_clicked(GtkButton * button, gboolean ispercom)
{

}
/*
static void on_btnEDSpell_clicked(GtkButton * button, gboolean ispercom)
{

}
 */


/******************************************************************************
 * editor gui
 ******************************************************************************/
GtkWidget *create_StudyPad(GtkWidget *app, GtkWidget *vbox)
{
	//GtkHTMLControlData *cd;
	//EDITOR *ed;
	  GtkWidget *editor;
  
  GtkWidget *vbox34;
  GtkWidget *vbox35;
  GtkWidget *hbox29;
  GtkWidget *hboxEDStudyPad;
  GtkWidget *toolbarSP;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnEDNew;
  GtkWidget *btnEDOpen;
  GtkWidget *btnEDSave;
  GtkWidget *btnEDSaveAs;
  GtkWidget *vseparator17;
  GtkWidget *btnEDPrint;
  GtkWidget *handlebox21;
  GtkWidget *toolbarEdit;
  GtkWidget *vseparator20;
  GtkWidget *btnEDCut;
  GtkWidget *btnEDCopy;
  GtkWidget *btnEDPaste;
  GtkWidget *vseparator19;
  GtkWidget *btnEDBold;
  GtkWidget *btnEDItalics;
  GtkWidget *btnEDUnderline;
  GtkWidget *btnEDStriketh;
  GtkWidget *vseparator22;
  GtkWidget *btnEDIndent;
  GtkWidget *btnEDUnindent;
  GtkWidget *vseparator21;
  GtkWidget *btnEDSpell;
  GtkWidget *swEDHTML;
  GtkWidget *statusbar1;

  //ed = &ed_widgets;
  
  editor = gtk_frame_new (NULL);
  gtk_widget_ref (editor);
  gtk_object_set_data_full (GTK_OBJECT (app), "editor", editor,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (editor);
  gtk_container_add (GTK_CONTAINER (vbox), editor);

  vbox34 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox34);
  gtk_object_set_data_full (GTK_OBJECT (app), "vbox34", vbox34,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox34);
  gtk_container_add (GTK_CONTAINER (editor), vbox34);

  vbox35 = gtk_vbox_new (TRUE, 0);
  gtk_widget_ref (vbox35);
  gtk_object_set_data_full (GTK_OBJECT (app), "vbox35", vbox35,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox35);
  gtk_box_pack_start (GTK_BOX (vbox34), vbox35, FALSE, TRUE, 0);

  hbox29 = gtk_hbox_new (TRUE, 0);
  gtk_widget_ref (hbox29);
  gtk_object_set_data_full (GTK_OBJECT (app), "hbox29", hbox29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox29);
  gtk_box_pack_start (GTK_BOX (vbox35), hbox29, TRUE, TRUE, 0);

  hboxEDStudyPad = gtk_handle_box_new ();
  gtk_widget_ref (hboxEDStudyPad);
  gtk_object_set_data_full (GTK_OBJECT (app), "hboxEDStudyPad", hboxEDStudyPad,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hboxEDStudyPad);
  gtk_box_pack_start (GTK_BOX (hbox29), hboxEDStudyPad, FALSE, TRUE, 0);
  gtk_handle_box_set_shadow_type (GTK_HANDLE_BOX (hboxEDStudyPad), GTK_SHADOW_NONE);

  toolbarSP = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbarSP);
  gtk_object_set_data_full (GTK_OBJECT (app), "toolbarSP", toolbarSP,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbarSP);
  gtk_container_add (GTK_CONTAINER (hboxEDStudyPad), toolbarSP);
  gtk_container_set_border_width (GTK_CONTAINER (toolbarSP), 2);
  gtk_toolbar_set_space_style (GTK_TOOLBAR (toolbarSP), GTK_TOOLBAR_SPACE_LINE);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbarSP), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_NEW);
  btnEDNew = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarSP),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button7"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDNew);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDNew", btnEDNew,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDNew);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_OPEN);
  btnEDOpen = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarSP),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button7"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDOpen);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDOpen", btnEDOpen,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDOpen);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_SAVE);
  btnEDSave = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarSP),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button7"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDSave);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDSave", btnEDSave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDSave);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_SAVE_AS);
  btnEDSaveAs = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarSP),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button10"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDSaveAs);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDSaveAs", btnEDSaveAs,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDSaveAs);

  vseparator17 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator17);
  gtk_object_set_data_full (GTK_OBJECT (app), "vseparator17", vseparator17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator17);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarSP), vseparator17, NULL, NULL);
  gtk_widget_set_usize (vseparator17, 7, 7);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_PRINT);
  btnEDPrint = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarSP),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button11"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDPrint);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDPrint", btnEDPrint,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDPrint);

  handlebox21 = gtk_handle_box_new ();
  gtk_widget_ref (handlebox21);
  gtk_object_set_data_full (GTK_OBJECT (app), "handlebox21", handlebox21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (handlebox21);
  gtk_box_pack_start (GTK_BOX (vbox35), handlebox21, FALSE, TRUE, 0);
  gtk_handle_box_set_shadow_type (GTK_HANDLE_BOX (handlebox21), GTK_SHADOW_NONE);

  toolbarEdit = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbarEdit);
  gtk_object_set_data_full (GTK_OBJECT (app), "toolbarEdit", toolbarEdit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbarEdit);
  gtk_container_add (GTK_CONTAINER (handlebox21), toolbarEdit);
  gtk_container_set_border_width (GTK_CONTAINER (toolbarEdit), 2);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbarEdit), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_PROPERTIES);
  tgbtnEDEdit = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
                                NULL,
                                _("togglebutton1"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (tgbtnEDEdit);
  gtk_object_set_data_full (GTK_OBJECT (app), "tgbtnEDEdit", tgbtnEDEdit,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tgbtnEDEdit);

  vseparator20 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator20);
  gtk_object_set_data_full (GTK_OBJECT (app), "vseparator20", vseparator20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator20);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarEdit), vseparator20, NULL, NULL);
  gtk_widget_set_usize (vseparator20, 7, 7);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_CUT);
  btnEDCut = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button15"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDCut);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDCut", btnEDCut,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDCut);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_COPY);
  btnEDCopy = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button15"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDCopy);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDCopy", btnEDCopy,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDCopy);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_PASTE);
  btnEDPaste = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button15"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDPaste);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDPaste", btnEDPaste,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDPaste);

  vseparator19 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator19);
  gtk_object_set_data_full (GTK_OBJECT (app), "vseparator19", vseparator19,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator19);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarEdit), vseparator19, NULL, NULL);
  gtk_widget_set_usize (vseparator19, 7, 7);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_TEXT_BOLD);
  btnEDBold = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button15"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDBold);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDBold", btnEDBold,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDBold);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_TEXT_ITALIC);
  btnEDItalics = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button15"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDItalics);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDItalics", btnEDItalics,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDItalics);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_TEXT_UNDERLINE);
  btnEDUnderline = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button15"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDUnderline);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDUnderline", btnEDUnderline,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDUnderline);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_TEXT_STRIKEOUT);
  btnEDStriketh = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button21"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDStriketh);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDStriketh", btnEDStriketh,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDStriketh);

  vseparator22 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator22);
  gtk_object_set_data_full (GTK_OBJECT (app), "vseparator22", vseparator22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator22);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarEdit), vseparator22, NULL, NULL);
  gtk_widget_set_usize (vseparator22, 7, 7);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_TEXT_INDENT);
  btnEDIndent = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button22"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDIndent);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDIndent", btnEDIndent,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDIndent);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_TEXT_UNINDENT);
  btnEDUnindent = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button23"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDUnindent);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDUnindent", btnEDUnindent,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDUnindent);

  vseparator21 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator21);
  gtk_object_set_data_full (GTK_OBJECT (app), "vseparator21", vseparator21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator21);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbarEdit), vseparator21, NULL, NULL);
  gtk_widget_set_usize (vseparator21, 7, 7);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (app, GNOME_STOCK_PIXMAP_SPELLCHECK);
  btnEDSpell = gtk_toolbar_append_element (GTK_TOOLBAR (toolbarEdit),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                _("button7"),
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEDSpell);
  gtk_object_set_data_full (GTK_OBJECT (app), "btnEDSpell", btnEDSpell,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEDSpell);

  swEDHTML = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (swEDHTML);
  gtk_object_set_data_full (GTK_OBJECT (app), "swEDHTML", swEDHTML,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (swEDHTML);
  gtk_box_pack_start (GTK_BOX (vbox34), swEDHTML, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (swEDHTML), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

   spHTML = gtk_html_new();
   gtk_widget_ref(spHTML);
   gtk_object_set_data_full(GTK_OBJECT(app),
				 "spHTML",spHTML ,
				 (GtkDestroyNotify) gtk_widget_unref);
    gtk_widget_show(spHTML);
    gtk_container_add(GTK_CONTAINER(swEDHTML),
			  spHTML);
    gtk_html_load_empty(GTK_HTML(spHTML));


  statusbar1 = gtk_statusbar_new ();
  gtk_widget_ref (statusbar1);
  gtk_object_set_data_full (GTK_OBJECT (app), "statusbar1", statusbar1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (statusbar1);
  gtk_box_pack_start (GTK_BOX (vbox34), statusbar1, FALSE, FALSE, 0);

  gtk_signal_connect (GTK_OBJECT (btnEDNew), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDNew_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDOpen), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDOpen_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDSave), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDSave_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDSaveAs), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDSaveAs_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDPrint), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDPrint_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (tgbtnEDEdit), "toggled",
                      GTK_SIGNAL_FUNC (on_tgbtnEDEdit_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDCut), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDCut_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDCopy), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDCopy_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDPaste), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDPaste_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDBold), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDBold_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDItalics), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDItalics_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDUnderline), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDUnderline_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDStriketh), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDStriketh_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDIndent), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDIndent_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEDUnindent), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEDUnindent_clicked),
                      NULL);

  return editor;
}


