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

#include "gs_editor.h"




GtkWidget *textedit;

static GtkWidget *editor;
static GtkWidget *tgbtnEDEdit;
static GtkWidget *texthtmlpc;
static GtkWidget *texthtmlsp;
static GtkWidget *texthtml;
static GtkWidget *btnEDSpell;
static GtkWidget *nbEditor;
static GtkWidget *swEditorHTML;
static usepercom;

/*****************************************************************************
 *       Editor button callbacks
 *
 *****************************************************************************/

static void on_btnEDNew_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDOpen_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDSave_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDSaveAs_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDPrint_clicked(GtkButton * button, gpointer user_data)
{

}


static void
on_btnEDSaveNote_clicked(GtkButton * button, gpointer user_data)
{

}


static void
on_btnEDPrintNote_clicked(GtkButton * button, gpointer user_data)
{

}


static void
on_tgbtnEDEdit_toggled(GtkToggleButton * togglebutton, gpointer user_data)
{
	if (togglebutton->active)
		gtk_notebook_set_page(GTK_NOTEBOOK(nbEditor),	1);
	else
		gtk_notebook_set_page(GTK_NOTEBOOK(nbEditor),	0);
		
}


static void on_btnEDCut_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDCopy_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDPaste_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDBold_clicked(GtkButton * button, gpointer user_data)
{
	if (!GTK_TOGGLE_BUTTON(GTK_BUTTON(tgbtnEDEdit))->active) {
		if (GTK_EDITABLE(textedit)->has_selection) {	//-- do we have a selection?        
			gtk_text_set_point(GTK_TEXT(textedit),
					   GTK_EDITABLE(textedit)->
					   selection_start_pos);
			gtk_text_insert(GTK_TEXT(textedit), NULL,
					&textedit->style->black, NULL,
					"<b>", -1);
			gtk_text_set_point(GTK_TEXT(textedit),
					   GTK_EDITABLE(textedit)->
					   selection_end_pos);
			gtk_text_insert(GTK_TEXT(textedit), NULL,
					&textedit->style->black, NULL,
					"</b>", -1);
		}

	} else {
		/*if(!block_font_style_change) {
		   if (gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (widget)))
		   gtk_html_set_font_style (GTK_HTML (html_widget),
		   GTK_HTML_FONT_STYLE_MAX,
		   GTK_HTML_FONT_STYLE_BOLD);
		   else
		   gtk_html_set_font_style (GTK_HTML (html_widget), ~GTK_HTML_FONT_STYLE_BOLD, 0);
		   noteModified = TRUE;
		   }   */
	}
}


static void on_btnEDItalics_clicked(GtkButton * button, gpointer user_data)
{
	if (GTK_EDITABLE(textedit)->has_selection) {	//-- do we have a selection?        
		gtk_text_set_point(GTK_TEXT(textedit),
				   GTK_EDITABLE(textedit)->
				   selection_start_pos);
		gtk_text_insert(GTK_TEXT(textedit), NULL,
				&textedit->style->black, NULL, "<i>", -1);
		gtk_text_set_point(GTK_TEXT(textedit),
				   GTK_EDITABLE(textedit)->
				   selection_end_pos);
		gtk_text_insert(GTK_TEXT(textedit), NULL,
				&textedit->style->black, NULL, "</i>", -1);
	}
}


static void
on_btnEDUnderline_clicked(GtkButton * button, gpointer user_data)
{

}


static void
on_btnEDStriketh_clicked(GtkButton * button, gpointer user_data)
{

}


static void on_btnEDIndent_clicked(GtkButton * button, gpointer user_data)
{

}


static void
on_btnEDUnindent_clicked(GtkButton * button, gpointer user_data)
{

}

static void on_btnEDSpell_clicked(GtkButton * button, gpointer user_data)
{

}


static void
on_nbEditor_switch_page(GtkNotebook * notebook,
			GtkNotebookPage * page,
			gint page_num, gpointer user_data)
{
	if(page_num == 1){
		gchar *buf;
		GString *str;
		
		buf = gtk_editable_get_chars(GTK_EDITABLE(textedit), 0,-1);
		str = g_string_new(buf);
		texthtml = gtk_html_new_from_string (str->str, str->len);
		//texthtml = gtk_html_new();
		gtk_widget_ref(texthtml);
		gtk_object_set_data_full(GTK_OBJECT(editor),
				 "texthtml", texthtml,
				 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(texthtml);
		gtk_container_add(GTK_CONTAINER(swEditorHTML),
			  texthtml);
		//gtk_html_load_empty(GTK_HTML(texthtml));
		g_string_free(str,FALSE);
	}else{
		gtk_widget_destroy(texthtml);
	}

}



/******************************************************************************
 * editor gui
 ******************************************************************************/
GtkWidget *create_editor(GtkWidget * vbox, gboolean ispercom)
{
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
	GtkWidget *hboxPerCom;
	GtkWidget *toolbarPC;
	GtkWidget *btnEDSaveNote;
	GtkWidget *btnEDClearNote;
	GtkWidget *vseparator18;
	GtkWidget *btnEDPrintNote;
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
	GtkWidget *vseparator21;
	GtkWidget *btnEDIndent;
	GtkWidget *btnEDUnindent;
	GtkWidget *swEditorText;
	GtkWidget *label139;
	GtkWidget *label140;
	GtkWidget *statusbar1;

                  usepercom = ispercom;
	editor = gtk_frame_new(NULL);
	gtk_widget_ref(editor);
	gtk_object_set_data_full(GTK_OBJECT(vbox), "editor", editor,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(editor);
	gtk_container_add(GTK_CONTAINER(vbox), editor);

	vbox34 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox34);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vbox34", vbox34,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox34);
	gtk_container_add(GTK_CONTAINER(editor), vbox34);

	vbox35 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox35);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vbox35", vbox35,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox35);
	gtk_box_pack_start(GTK_BOX(vbox34), vbox35, FALSE, TRUE, 0);

	hbox29 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox29);
	gtk_object_set_data_full(GTK_OBJECT(editor), "hbox29", hbox29,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox29);
	gtk_box_pack_start(GTK_BOX(vbox35), hbox29, TRUE, TRUE, 0);

	hboxEDStudyPad = gtk_handle_box_new();
	gtk_widget_ref(hboxEDStudyPad);
	gtk_object_set_data_full(GTK_OBJECT(editor), "hboxEDStudyPad",
				 hboxEDStudyPad,
				 (GtkDestroyNotify) gtk_widget_unref);
	if(!ispercom) gtk_widget_show(hboxEDStudyPad);
	gtk_box_pack_start(GTK_BOX(hbox29), hboxEDStudyPad, FALSE, TRUE,
			   0);

	toolbarSP =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarSP);
	gtk_object_set_data_full(GTK_OBJECT(editor), "toolbarSP",
				 toolbarSP,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarSP);
	gtk_container_add(GTK_CONTAINER(hboxEDStudyPad), toolbarSP);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarSP),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_NEW);
	btnEDNew =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarSP),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDNew);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDNew", btnEDNew,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDNew);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_OPEN);
	btnEDOpen =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarSP),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDOpen);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDOpen",
				 btnEDOpen,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDOpen);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_SAVE);
	btnEDSave =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarSP),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDSave);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDSave",
				 btnEDSave,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDSave);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_SAVE_AS);
	btnEDSaveAs =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarSP),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button10"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDSaveAs);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDSaveAs",
				 btnEDSaveAs,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDSaveAs);

	vseparator17 = gtk_vseparator_new();
	gtk_widget_ref(vseparator17);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vseparator17",
				 vseparator17,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator17);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarSP), vseparator17,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator17, 7, 9);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_PRINT);
	btnEDPrint =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarSP),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button11"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDPrint);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDPrint",
				 btnEDPrint,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDPrint);

	hboxPerCom = gtk_handle_box_new();
	gtk_widget_ref(hboxPerCom);
	gtk_object_set_data_full(GTK_OBJECT(editor), "hboxPerCom",
				 hboxPerCom,
				 (GtkDestroyNotify) gtk_widget_unref);
	if(ispercom) gtk_widget_show(hboxPerCom);
	gtk_box_pack_start(GTK_BOX(hbox29), hboxPerCom, FALSE, TRUE, 0);

	toolbarPC =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarPC);
	gtk_object_set_data_full(GTK_OBJECT(editor), "toolbarPC",
				 toolbarPC,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarPC);
	gtk_container_add(GTK_CONTAINER(hboxPerCom), toolbarPC);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarPC),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_SAVE);
	btnEDSaveNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarPC),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button12"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDSaveNote);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDSaveNote",
				 btnEDSaveNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDSaveNote);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_CLEAR);
	btnEDClearNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarPC),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button13"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDClearNote);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDClearNote",
				 btnEDClearNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDClearNote);

	vseparator18 = gtk_vseparator_new();
	gtk_widget_ref(vseparator18);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vseparator18",
				 vseparator18,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator18);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarPC), vseparator18,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator18, 7, 9);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_PRINT);
	btnEDPrintNote =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarPC),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button14"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDPrintNote);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDPrintNote",
				 btnEDPrintNote,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDPrintNote);

	handlebox21 = gtk_handle_box_new();
	gtk_widget_ref(handlebox21);
	gtk_object_set_data_full(GTK_OBJECT(editor), "handlebox21",
				 handlebox21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox21);
	gtk_box_pack_start(GTK_BOX(vbox35), handlebox21, TRUE, TRUE, 0);

	toolbarEdit =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbarEdit);
	gtk_object_set_data_full(GTK_OBJECT(editor), "toolbarEdit",
				 toolbarEdit,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbarEdit);
	gtk_container_add(GTK_CONTAINER(handlebox21), toolbarEdit);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbarEdit),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_PROPERTIES);
	tgbtnEDEdit =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_TOGGLEBUTTON,
				       NULL, _("togglebutton1"), NULL,
				       NULL, tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(tgbtnEDEdit);
	gtk_object_set_data_full(GTK_OBJECT(editor), "tgbtnEDEdit",
				 tgbtnEDEdit,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(tgbtnEDEdit);

	vseparator20 = gtk_vseparator_new();
	gtk_widget_ref(vseparator20);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vseparator20",
				 vseparator20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator20);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarEdit), vseparator20,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator20, 7, 9);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_CUT);
	btnEDCut =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button15"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDCut);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDCut", btnEDCut,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDCut);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_COPY);
	btnEDCopy =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button15"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDCopy);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDCopy",
				 btnEDCopy,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDCopy);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor, GNOME_STOCK_PIXMAP_PASTE);
	btnEDPaste =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button15"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDPaste);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDPaste",
				 btnEDPaste,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDPaste);

	vseparator19 = gtk_vseparator_new();
	gtk_widget_ref(vseparator19);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vseparator19",
				 vseparator19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator19);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarEdit), vseparator19,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator19, 7, 9);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_TEXT_BOLD);
	btnEDBold =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button15"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDBold);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDBold",
				 btnEDBold,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDBold);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_TEXT_ITALIC);
	btnEDItalics =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button15"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDItalics);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDItalics",
				 btnEDItalics,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDItalics);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_TEXT_UNDERLINE);
	btnEDUnderline =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button15"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDUnderline);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDUnderline",
				 btnEDUnderline,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDUnderline);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_TEXT_STRIKEOUT);
	btnEDStriketh =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button21"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDStriketh);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDStriketh",
				 btnEDStriketh,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDStriketh);

	vseparator21 = gtk_vseparator_new();
	gtk_widget_ref(vseparator21);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vseparator21",
				 vseparator21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator21);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarEdit), vseparator21,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator21, 7, 9);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_TEXT_INDENT);
	btnEDIndent =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button22"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDIndent);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDIndent",
				 btnEDIndent,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDIndent);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_TEXT_UNINDENT);
	btnEDUnindent =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button23"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDUnindent);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDUnindent",
				 btnEDUnindent,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDUnindent);


	vseparator21 = gtk_vseparator_new();
	gtk_widget_ref(vseparator21);
	gtk_object_set_data_full(GTK_OBJECT(editor), "vseparator21",
				 vseparator21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vseparator21);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbarEdit), vseparator21,
				  NULL, NULL);
	gtk_widget_set_usize(vseparator21, 7, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(editor,
				      GNOME_STOCK_PIXMAP_SPELLCHECK);
	btnEDSpell =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbarEdit),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       _("button7"), NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnEDSpell);
	gtk_object_set_data_full(GTK_OBJECT(editor), "btnEDSpell",
				 btnEDSpell,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnEDSpell);

	nbEditor = gtk_notebook_new();
	gtk_widget_ref(nbEditor);
	gtk_object_set_data_full(GTK_OBJECT(editor), "nbEditor", nbEditor,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(nbEditor);
	gtk_box_pack_start(GTK_BOX(vbox34), nbEditor, TRUE, TRUE, 0);
	GTK_WIDGET_UNSET_FLAGS(nbEditor, GTK_CAN_FOCUS);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(nbEditor), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(nbEditor), FALSE);

	swEditorText = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swEditorText);
	gtk_object_set_data_full(GTK_OBJECT(editor), "swEditorText",
				 swEditorText,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swEditorText);
	gtk_container_add(GTK_CONTAINER(nbEditor), swEditorText);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swEditorText),
				       GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);
	
		textedit = gtk_text_new(NULL, NULL);
		gtk_widget_ref(textedit);
		gtk_object_set_data_full(GTK_OBJECT(editor), "textedit", textedit,
				 (GtkDestroyNotify) gtk_widget_unref);
		gtk_widget_show(textedit);
		gtk_container_add(GTK_CONTAINER(swEditorText), textedit);
		gtk_text_set_editable(GTK_TEXT(textedit), TRUE);
	
	
	label139 = gtk_label_new(_("label139"));
	gtk_widget_ref(label139);
	gtk_object_set_data_full(GTK_OBJECT(editor), "label139", label139,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label139);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbEditor),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbEditor),
							     0), label139);

	swEditorHTML = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(swEditorHTML);
	gtk_object_set_data_full(GTK_OBJECT(editor), "swEditorHTML",
				 swEditorHTML,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(swEditorHTML);
	gtk_container_add(GTK_CONTAINER(nbEditor), swEditorHTML);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(swEditorHTML),
				       GTK_POLICY_AUTOMATIC,
				       GTK_POLICY_AUTOMATIC);

	
	label140 = gtk_label_new(_("label140"));
	gtk_widget_ref(label140);
	gtk_object_set_data_full(GTK_OBJECT(editor), "label140", label140,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label140);
	gtk_notebook_set_tab_label(GTK_NOTEBOOK(nbEditor),
				   gtk_notebook_get_nth_page(GTK_NOTEBOOK
							     (nbEditor),
							     1), label140);

	statusbar1 = gtk_statusbar_new();
	gtk_widget_ref(statusbar1);
	gtk_object_set_data_full(GTK_OBJECT(editor), "statusbar1",
				 statusbar1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(statusbar1);
	gtk_box_pack_start(GTK_BOX(vbox34), statusbar1, FALSE, FALSE, 0);

	gtk_signal_connect(GTK_OBJECT(btnEDNew), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDNew_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDOpen), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDOpen_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDSave), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDSave_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDSaveAs), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDSaveAs_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDPrint), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDPrint_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDSaveNote), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDSaveNote_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDPrintNote), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDPrintNote_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(tgbtnEDEdit), "toggled",
			   GTK_SIGNAL_FUNC(on_tgbtnEDEdit_toggled), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDCut), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDCut_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDCopy), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDCopy_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDPaste), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDPaste_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDBold), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDBold_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDItalics), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDItalics_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDUnderline), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDUnderline_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDStriketh), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDStriketh_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDIndent), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDIndent_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDUnindent), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDUnindent_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnEDSpell), "clicked",
			   GTK_SIGNAL_FUNC(on_btnEDSpell_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(nbEditor), "switch_page",
			   GTK_SIGNAL_FUNC(on_nbEditor_switch_page), NULL);

	return editor;
}
