/***************************************************************************
                          dialogs.cpp  -  description
                             -------------------
    begin                : Fri Jun 16 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>

#include <gnome.h>

#include "callback.h"
#include "support.h"
#include "dialogs.h"


//-----------------------------------------------------------------------------------------------
GtkWidget*
create_propertybox1 (void)
{
  GtkWidget *propertybox1;
  GtkWidget *notebook6;
  GtkWidget *table4;
  GtkWidget *cpfgMainwindow;
  GtkWidget *fpMainwindow;
  GtkWidget *label79;
  GtkWidget *fpMainwindowitalic;
  GtkWidget *label84;
  GtkWidget *entry2;
  GtkWidget *entry3;
  GtkWidget *cpfgCurrentverse;
  GtkWidget *fbMainwindowbold;
  GtkWidget *entry5;
  GtkWidget *label80;
  GtkWidget *label78;
  GtkWidget *label74;
  GtkWidget *empty_notebook_page;
  GtkWidget *label86;
  GtkWidget *table7;
  GtkWidget *label88;
  GtkWidget *entryfontSP;
  GtkWidget *fpStudypad;
  GtkWidget *label89;
  GtkWidget *entryfontSPPrinter;
  GtkWidget *fpSPPrinter;
  GtkWidget *label87;
  GtkWidget *table3;
  GtkWidget *label76;
  GtkWidget *label77;
  GtkWidget *cpbgMainwindow;
  GtkWidget *cpbgInterlinear;
  GtkWidget *cpbgCurrentverse;
  GtkWidget *label75;
  GtkWidget *label73;
  GtkWidget *paperselector1;
  GtkWidget *label91;

  propertybox1 = gnome_property_box_new ();
  gtk_object_set_data (GTK_OBJECT (propertybox1), "propertybox1", propertybox1);
  gtk_container_set_border_width (GTK_CONTAINER (propertybox1), 5);

  notebook6 = GNOME_PROPERTY_BOX (propertybox1)->notebook;
  gtk_object_set_data (GTK_OBJECT (propertybox1), "notebook6", notebook6);
  gtk_widget_show (notebook6);

  table4 = gtk_table_new (4, 4, FALSE);
  gtk_widget_ref (table4);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "table4", table4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table4);
  gtk_container_add (GTK_CONTAINER (notebook6), table4);

  cpfgMainwindow = gnome_color_picker_new ();
  gtk_widget_ref (cpfgMainwindow);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "cpfgMainwindow", cpfgMainwindow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cpfgMainwindow);
  gtk_table_attach (GTK_TABLE (table4), cpfgMainwindow, 3, 4, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  fpMainwindow = gnome_font_picker_new ();
  gtk_widget_ref (fpMainwindow);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "fpMainwindow", fpMainwindow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpMainwindow);
  gtk_table_attach (GTK_TABLE (table4), fpMainwindow, 2, 3, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label79 = gtk_label_new ("Standard font");
  gtk_widget_ref (label79);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label79", label79,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label79);
  gtk_table_attach (GTK_TABLE (table4), label79, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label79), GTK_JUSTIFY_LEFT);

  fpMainwindowitalic = gnome_font_picker_new ();
  gtk_widget_ref (fpMainwindowitalic);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "fpMainwindowitalic", fpMainwindowitalic,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpMainwindowitalic);
  gtk_table_attach (GTK_TABLE (table4), fpMainwindowitalic, 2, 3, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label84 = gtk_label_new ("Italics font");
  gtk_widget_ref (label84);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label84", label84,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label84);
  gtk_table_attach (GTK_TABLE (table4), label84, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label84), GTK_JUSTIFY_LEFT);

  entry2 = gtk_entry_new ();
  gtk_widget_ref (entry2);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "entry2", entry2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry2);
  gtk_table_attach (GTK_TABLE (table4), entry2, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entry3 = gtk_entry_new ();
  gtk_widget_ref (entry3);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "entry3", entry3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry3);
  gtk_table_attach (GTK_TABLE (table4), entry3, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  cpfgCurrentverse = gnome_color_picker_new ();
  gtk_widget_ref (cpfgCurrentverse);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "cpfgCurrentverse", cpfgCurrentverse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cpfgCurrentverse);
  gtk_table_attach (GTK_TABLE (table4), cpfgCurrentverse, 3, 4, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  fbMainwindowbold = gnome_font_picker_new ();
  gtk_widget_ref (fbMainwindowbold);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "fbMainwindowbold", fbMainwindowbold,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fbMainwindowbold);
  gtk_table_attach (GTK_TABLE (table4), fbMainwindowbold, 2, 3, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  entry5 = gtk_entry_new ();
  gtk_widget_ref (entry5);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "entry5", entry5,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry5);
  gtk_table_attach (GTK_TABLE (table4), entry5, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label80 = gtk_label_new ("Bold font");
  gtk_widget_ref (label80);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label80", label80,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label80);
  gtk_table_attach (GTK_TABLE (table4), label80, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label80), GTK_JUSTIFY_LEFT);

  label78 = gtk_label_new ("Current verse");
  gtk_widget_ref (label78);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label78", label78,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label78);
  gtk_table_attach (GTK_TABLE (table4), label78, 0, 1, 3, 4,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_label_set_justify (GTK_LABEL (label78), GTK_JUSTIFY_LEFT);

  label74 = gtk_label_new ("Fonts - Main Window");
  gtk_widget_ref (label74);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label74", label74,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label74);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook6), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook6), 0), label74);

  empty_notebook_page = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (empty_notebook_page);
  gtk_container_add (GTK_CONTAINER (notebook6), empty_notebook_page);

  label86 = gtk_label_new ("Fonts - Interlinear Window");
  gtk_widget_ref (label86);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label86", label86,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook6), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook6), 1), label86);

  table7 = gtk_table_new (3, 3, FALSE);
  gtk_widget_ref (table7);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "table7", table7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table7);
  gtk_container_add (GTK_CONTAINER (notebook6), table7);

  label88 = gtk_label_new ("Screen Font ");
  gtk_widget_ref (label88);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label88", label88,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label88);
  gtk_table_attach (GTK_TABLE (table7), label88, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  entryfontSP = gtk_entry_new ();
  gtk_widget_ref (entryfontSP);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "entryfontSP", entryfontSP,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryfontSP);
  gtk_table_attach (GTK_TABLE (table7), entryfontSP, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  fpStudypad = gnome_font_picker_new ();
  gtk_widget_ref (fpStudypad);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "fpStudypad", fpStudypad,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpStudypad);
  gtk_table_attach (GTK_TABLE (table7), fpStudypad, 2, 3, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label89 = gtk_label_new ("Printer Font");
  gtk_widget_ref (label89);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label89", label89,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label89);
  gtk_table_attach (GTK_TABLE (table7), label89, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  entryfontSPPrinter = gtk_entry_new ();
  gtk_widget_ref (entryfontSPPrinter);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "entryfontSPPrinter", entryfontSPPrinter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryfontSPPrinter);
  gtk_table_attach (GTK_TABLE (table7), entryfontSPPrinter, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  fpSPPrinter = gnome_font_picker_new ();
  gtk_widget_ref (fpSPPrinter);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "fpSPPrinter", fpSPPrinter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (fpSPPrinter);
  gtk_table_attach (GTK_TABLE (table7), fpSPPrinter, 2, 3, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label87 = gtk_label_new ("StudyPad");
  gtk_widget_ref (label87);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label87", label87,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label87);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook6), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook6), 2), label87);

  table3 = gtk_table_new (3, 2, FALSE);
  gtk_widget_ref (table3);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "table3", table3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table3);
  gtk_container_add (GTK_CONTAINER (notebook6), table3);

  label76 = gtk_label_new ("  Interlinear                                              ");
  gtk_widget_ref (label76);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label76", label76,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label76);
  gtk_table_attach (GTK_TABLE (table3), label76, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (label76, 234, -2);

  label77 = gtk_label_new ("  Current Verse                                        ");
  gtk_widget_ref (label77);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label77", label77,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label77);
  gtk_table_attach (GTK_TABLE (table3), label77, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (label77, 234, -2);
  gtk_label_set_justify (GTK_LABEL (label77), GTK_JUSTIFY_LEFT);

  cpbgMainwindow = gnome_color_picker_new ();
  gtk_widget_ref (cpbgMainwindow);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "cpbgMainwindow", cpbgMainwindow,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cpbgMainwindow);
  gtk_table_attach (GTK_TABLE (table3), cpbgMainwindow, 1, 2, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (cpbgMainwindow, FALSE);

  cpbgInterlinear = gnome_color_picker_new ();
  gtk_widget_ref (cpbgInterlinear);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "cpbgInterlinear", cpbgInterlinear,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cpbgInterlinear);
  gtk_table_attach (GTK_TABLE (table3), cpbgInterlinear, 1, 2, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (cpbgInterlinear, FALSE);

  cpbgCurrentverse = gnome_color_picker_new ();
  gtk_widget_ref (cpbgCurrentverse);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "cpbgCurrentverse", cpbgCurrentverse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cpbgCurrentverse);
  gtk_table_attach (GTK_TABLE (table3), cpbgCurrentverse, 1, 2, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_sensitive (cpbgCurrentverse, FALSE);

  label75 = gtk_label_new ("Main Window                                       ");
  gtk_widget_ref (label75);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label75", label75,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label75);
  gtk_table_attach (GTK_TABLE (table3), label75, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (label75, 233, -2);
  gtk_label_set_justify (GTK_LABEL (label75), GTK_JUSTIFY_LEFT);

  label73 = gtk_label_new ("Background Colors");
  gtk_widget_ref (label73);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label73", label73,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook6), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook6), 3), label73);

  paperselector1 = gnome_paper_selector_new ();
  gtk_widget_ref (paperselector1);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "paperselector1", paperselector1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (paperselector1);
  gtk_container_add (GTK_CONTAINER (notebook6), paperselector1);

  label91 = gtk_label_new ("Paper");
  gtk_widget_ref (label91);
  gtk_object_set_data_full (GTK_OBJECT (propertybox1), "label91", label91,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label91);
  gtk_notebook_set_tab_label (GTK_NOTEBOOK (notebook6), gtk_notebook_get_nth_page (GTK_NOTEBOOK (notebook6), 4), label91);

  gtk_signal_connect (GTK_OBJECT (propertybox1), "apply",
                      GTK_SIGNAL_FUNC (on_propertybox1_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cpfgMainwindow), "color_set",
                      GTK_SIGNAL_FUNC (on_cpfgMainwindow_color_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fpMainwindow), "font_set",
                      GTK_SIGNAL_FUNC (on_fpMainwindow_font_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fpMainwindowitalic), "font_set",
                      GTK_SIGNAL_FUNC (on_fpMainwindowitalic_font_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cpfgCurrentverse), "color_set",
                      GTK_SIGNAL_FUNC (on_cpfgCurrentverse_color_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fbMainwindowbold), "font_set",
                      GTK_SIGNAL_FUNC (on_fbMainwindowbold_font_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fpStudypad), "font_set",
                      GTK_SIGNAL_FUNC (on_fpStudypad_font_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (fpSPPrinter), "font_set",
                      GTK_SIGNAL_FUNC (on_fpSPPrinter_font_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cpbgMainwindow), "color_set",
                      GTK_SIGNAL_FUNC (on_cpbgMainwindow_color_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cpbgInterlinear), "color_set",
                      GTK_SIGNAL_FUNC (on_colorpicker2_color_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cpbgInterlinear), "color_set",
                      GTK_SIGNAL_FUNC (on_cpbgInterlinear_color_set),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cpbgCurrentverse), "color_set",
                      GTK_SIGNAL_FUNC (on_cpbgCurrentverse_color_set),
                      NULL);

  return propertybox1;
}

//-----------------------------------------------------------------------------------------------
GtkWidget*
create_BMEditor (void)
{
  GtkWidget *BMEditor;
  GtkWidget *dialog_vbox9;
  GtkWidget *hbox22;
  GtkWidget *scrolledwindow31;
  GtkWidget *viewport1;
  GtkWidget *tree1;
  GtkWidget *vbox26;
  GtkWidget *label100;
  GtkWidget *entryNewItem;
  GtkWidget *toolbar28;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnBMEAddItem;
  GtkWidget *btnBMEDelete;
  GtkWidget *vseparator15;
  GtkWidget *btnBMEsave;
  GtkWidget *toolbar27;
  GtkWidget *btnBMEup;
  GtkWidget *btnBMEdown;
  GtkWidget *btnBMEleft;
  GtkWidget *btnBMEright;
  GtkWidget *dialog_action_area9;
  GtkWidget *btnBMEok;
  GtkWidget *btnBMEapply;
  GtkWidget *btnBMEcancel;
  GtkTooltips *tooltips;

  tooltips = gtk_tooltips_new ();

  BMEditor = gnome_dialog_new ("Bookmark Editor", NULL);
  gtk_object_set_data (GTK_OBJECT (BMEditor), "BMEditor", BMEditor);
  gtk_container_set_border_width (GTK_CONTAINER (BMEditor), 4);
  gtk_window_set_policy (GTK_WINDOW (BMEditor), FALSE, FALSE, FALSE);

  dialog_vbox9 = GNOME_DIALOG (BMEditor)->vbox;
  gtk_object_set_data (GTK_OBJECT (BMEditor), "dialog_vbox9", dialog_vbox9);
  gtk_widget_show (dialog_vbox9);

  hbox22 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox22);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "hbox22", hbox22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox22);
  gtk_box_pack_start (GTK_BOX (dialog_vbox9), hbox22, TRUE, TRUE, 0);

  scrolledwindow31 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow31);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "scrolledwindow31", scrolledwindow31,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow31);
  gtk_box_pack_start (GTK_BOX (hbox22), scrolledwindow31, TRUE, TRUE, 0);
  gtk_widget_set_usize (scrolledwindow31, 236, 271);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow31), GTK_POLICY_AUTOMATIC, GTK_POLICY_ALWAYS);

  viewport1 = gtk_viewport_new (NULL, NULL);
  gtk_widget_ref (viewport1);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "viewport1", viewport1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (viewport1);
  gtk_container_add (GTK_CONTAINER (scrolledwindow31), viewport1);

  tree1 = gtk_tree_new ();
  gtk_widget_ref (tree1);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "tree1", tree1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (tree1);
  gtk_container_add (GTK_CONTAINER (viewport1), tree1);

  vbox26 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox26);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "vbox26", vbox26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox26);
  gtk_box_pack_start (GTK_BOX (hbox22), vbox26, TRUE, TRUE, 0);

  label100 = gtk_label_new ("Bookmark Item:");
  gtk_widget_ref (label100);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "label100", label100,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label100);
  gtk_box_pack_start (GTK_BOX (vbox26), label100, FALSE, FALSE, 0);
  gtk_widget_set_usize (label100, -2, 20);

  entryNewItem = gtk_entry_new ();
  gtk_widget_ref (entryNewItem);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "entryNewItem", entryNewItem,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryNewItem);
  gtk_box_pack_start (GTK_BOX (vbox26), entryNewItem, FALSE, FALSE, 0);

  toolbar28 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar28);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "toolbar28", toolbar28,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar28);
  gtk_box_pack_start (GTK_BOX (vbox26), toolbar28, FALSE, FALSE, 0);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (BMEditor, GNOME_STOCK_PIXMAP_ADD);
  btnBMEAddItem = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar28),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button8",
                                "Add new bookmark ", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBMEAddItem);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEAddItem", btnBMEAddItem,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEAddItem);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (BMEditor, GNOME_STOCK_PIXMAP_REMOVE);
  btnBMEDelete = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar28),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button9",
                                "Delete bookmark item", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBMEDelete);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEDelete", btnBMEDelete,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEDelete);

  vseparator15 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator15);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "vseparator15", vseparator15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator15);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar28), vseparator15, NULL, NULL);
  gtk_widget_set_usize (vseparator15, 5, 7);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (BMEditor, GNOME_STOCK_PIXMAP_SAVE);
  btnBMEsave = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar28),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button12",
                                "Save bookmarks", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBMEsave);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEsave", btnBMEsave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEsave);

  toolbar27 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar27);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "toolbar27", toolbar27,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar27);
  gtk_box_pack_start (GTK_BOX (dialog_vbox9), toolbar27, FALSE, FALSE, 0);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (BMEditor, GNOME_STOCK_PIXMAP_UP);
  btnBMEup = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar27),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button4",
                                "Move item up", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBMEup);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEup", btnBMEup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEup);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (BMEditor, GNOME_STOCK_PIXMAP_DOWN);
  btnBMEdown = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar27),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button5",
                                "Move item down", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBMEdown);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEdown", btnBMEdown,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEdown);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (BMEditor, GNOME_STOCK_PIXMAP_BACK);
  btnBMEleft = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar27),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button6",
                                "Move item to left", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBMEleft);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEleft", btnBMEleft,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEleft);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (BMEditor, GNOME_STOCK_PIXMAP_FORWARD);
  btnBMEright = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar27),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button7",
                                "Move item to right", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnBMEright);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEright", btnBMEright,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEright);

  dialog_action_area9 = GNOME_DIALOG (BMEditor)->action_area;
  gtk_object_set_data (GTK_OBJECT (BMEditor), "dialog_action_area9", dialog_action_area9);
  gtk_widget_show (dialog_action_area9);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area9), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area9), 8);

  gnome_dialog_append_button (GNOME_DIALOG (BMEditor), GNOME_STOCK_BUTTON_OK);
  btnBMEok = g_list_last(GNOME_DIALOG(BMEditor)->buttons)->data;
  gtk_widget_ref (btnBMEok);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEok", btnBMEok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEok);
  GTK_WIDGET_SET_FLAGS (btnBMEok, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, btnBMEok, "Apply changes and close editor", NULL);

  gnome_dialog_append_button (GNOME_DIALOG (BMEditor), GNOME_STOCK_BUTTON_APPLY);
  btnBMEapply = g_list_last (GNOME_DIALOG (BMEditor)->buttons)->data;
  gtk_widget_ref (btnBMEapply);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEapply", btnBMEapply,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEapply);
  GTK_WIDGET_SET_FLAGS (btnBMEapply, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, btnBMEapply, "Apply changes", NULL);

  gnome_dialog_append_button (GNOME_DIALOG (BMEditor), GNOME_STOCK_BUTTON_CANCEL);
  btnBMEcancel = g_list_last(GNOME_DIALOG (BMEditor)->buttons)->data;
  gtk_widget_ref (btnBMEcancel);
  gtk_object_set_data_full (GTK_OBJECT (BMEditor), "btnBMEcancel", btnBMEcancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnBMEcancel);
  GTK_WIDGET_SET_FLAGS (btnBMEcancel, GTK_CAN_DEFAULT);
  gtk_tooltips_set_tip (tooltips, btnBMEcancel, "Close editor", NULL);

  gtk_signal_connect (GTK_OBJECT (tree1), "select_child",
                      GTK_SIGNAL_FUNC (on_tree1_select_child),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (tree1), "selection_changed",
                      GTK_SIGNAL_FUNC (on_tree1_selection_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (tree1), "unselect_child",
                      GTK_SIGNAL_FUNC (on_tree1_unselect_child),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEAddItem), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEAddItem_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEDelete), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEDelete_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEup), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEup_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEdown), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEdown_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEleft), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEleft_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEright), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEright_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEok), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEok_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEapply), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEapply_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnBMEcancel), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEcancel_clicked),
                      NULL);

  gtk_object_set_data (GTK_OBJECT (BMEditor), "tooltips", tooltips);

  return BMEditor;
}


