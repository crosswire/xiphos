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
create_fileselection1 (void)
{
  GtkWidget *fileselection1;
  GtkWidget *ok_button1;
  GtkWidget *cancel_button1;

  fileselection1 = gtk_file_selection_new ("GtkSword - Open Note File");
  gtk_object_set_data (GTK_OBJECT (fileselection1), "fileselection1", fileselection1);
  gtk_container_set_border_width (GTK_CONTAINER (fileselection1), 10);
  gtk_file_selection_hide_fileop_buttons (GTK_FILE_SELECTION (fileselection1));

  ok_button1 = GTK_FILE_SELECTION (fileselection1)->ok_button;
  gtk_object_set_data (GTK_OBJECT (fileselection1), "ok_button1", ok_button1);
  gtk_widget_show (ok_button1);
  GTK_WIDGET_SET_FLAGS (ok_button1, GTK_CAN_DEFAULT);

  cancel_button1 = GTK_FILE_SELECTION (fileselection1)->cancel_button;
  gtk_object_set_data (GTK_OBJECT (fileselection1), "cancel_button1", cancel_button1);
  gtk_widget_show (cancel_button1);
  GTK_WIDGET_SET_FLAGS (cancel_button1, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (ok_button1), "clicked",
                      GTK_SIGNAL_FUNC (on_ok_button1_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button1), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_button1_clicked),
                      NULL);

  return fileselection1;
}

//-----------------------------------------------------------------------------------------------
GtkWidget*
create_fileselectionSave (void)
{
  GtkWidget *fileselectionSave;
  GtkWidget *ok_button2;
  GtkWidget *cancel_button2;

  fileselectionSave = gtk_file_selection_new ("GtkSword - Save Note File");
  gtk_object_set_data (GTK_OBJECT (fileselectionSave), "fileselectionSave", fileselectionSave);
  gtk_container_set_border_width (GTK_CONTAINER (fileselectionSave), 10);

  ok_button2 = GTK_FILE_SELECTION (fileselectionSave)->ok_button;
  gtk_object_set_data (GTK_OBJECT (fileselectionSave), "ok_button2", ok_button2);
  gtk_widget_show (ok_button2);
  GTK_WIDGET_SET_FLAGS (ok_button2, GTK_CAN_DEFAULT);

  cancel_button2 = GTK_FILE_SELECTION (fileselectionSave)->cancel_button;
  gtk_object_set_data (GTK_OBJECT (fileselectionSave), "cancel_button2", cancel_button2);
  gtk_widget_show (cancel_button2);
  GTK_WIDGET_SET_FLAGS (cancel_button2, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (ok_button2), "clicked",
                      GTK_SIGNAL_FUNC (on_ok_button2_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cancel_button2), "clicked",
                      GTK_SIGNAL_FUNC (on_cancel_button2_clicked),
                      NULL);

  return fileselectionSave;
}

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
create_about2 (void)
{
  const gchar *authors[] = {
    "\t\t\t  Terry Biggs    tbiggs@infinet.com",
    NULL
  };
  GtkWidget *about2;

  about2 = gnome_about_new ("gnomesword", VERSION,
                        "                Copyright                 2000",
                        authors,
                        "This is a faithful saying,and worthy of all acceptation, that Christ Jesus came into the world to save sinners; of whom I am chief. - I Timothy 1:15",
                        "gnomesword/GnomeSword.png");
  gtk_object_set_data (GTK_OBJECT (about2), "about2", about2);
  gtk_container_set_border_width (GTK_CONTAINER (about2), 4);
  gtk_window_set_modal (GTK_WINDOW (about2), TRUE);

  return about2;
}

//-----------------------------------------------------------------------------------------------
GtkWidget*
create_dlgSearch (void)
{
  GtkWidget *dlgSearch;
  GtkWidget *dialog_vbox1;
  GtkWidget *frame8;
  GtkWidget *searchPanel;
  GtkWidget *hbox4;
  GtkWidget *toolbar24;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnSearchSaveList;
  GtkWidget *entry1;
  GtkWidget *btnSearch1;
  GtkWidget *searchSettingsPanel;
  GtkWidget *frame15;
  GtkWidget *searchTypePanel;
  GSList *searchType_group = NULL;
  GtkWidget *regexSearch;
  GtkWidget *phraseSearch;
  GtkWidget *multiWordSearch;
  GtkWidget *frame16;
  GtkWidget *searchOptionsPanel;
  GtkWidget *caseSensitive;
  GtkWidget *ckbCom;
  GtkWidget *cbpercom;
  GtkWidget *frame20;
  GtkWidget *hbox17;
  GSList *__group = NULL;
  GtkWidget *rbNoScope;
  GtkWidget *rbUseBounds;
  GtkWidget *rbLastSearch;
  GtkWidget *table6;
  GtkWidget *label82;
  GtkWidget *label83;
  GtkWidget *entryLower;
  GtkWidget *entryUpper;
  GtkWidget *scrolledwindow2;
  GtkWidget *resultList;
  GtkWidget *label46;
  GtkWidget *scrolledwindow20;
  GtkWidget *txtSearch;
  GtkWidget *cbContext;
  GtkWidget *hbox15;
  GtkWidget *label57;
  GtkWidget *lbSearchHits;
  GtkWidget *dialog_action_area1;
  GtkWidget *btnSearchOK;

  dlgSearch = gnome_dialog_new ("GnomeSword - Search", NULL);
  gtk_object_set_data (GTK_OBJECT (dlgSearch), "dlgSearch", dlgSearch);
  gtk_container_set_border_width (GTK_CONTAINER (dlgSearch), 4);
  GTK_WINDOW (dlgSearch)->type = GTK_WINDOW_DIALOG;

  dialog_vbox1 = GNOME_DIALOG (dlgSearch)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlgSearch), "dialog_vbox1", dialog_vbox1);
  gtk_widget_show (dialog_vbox1);

  frame8 = gtk_frame_new (NULL);
  gtk_widget_ref (frame8);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame8", frame8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame8);
  gtk_box_pack_start (GTK_BOX (dialog_vbox1), frame8, TRUE, TRUE, 0);

  searchPanel = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (searchPanel);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "searchPanel", searchPanel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (searchPanel);
  gtk_container_add (GTK_CONTAINER (frame8), searchPanel);
  gtk_widget_set_usize (searchPanel, 279, -2);
  gtk_container_set_border_width (GTK_CONTAINER (searchPanel), 2);

  hbox4 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox4);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "hbox4", hbox4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox4);
  gtk_box_pack_start (GTK_BOX (searchPanel), hbox4, TRUE, TRUE, 0);

  toolbar24 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar24);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "toolbar24", toolbar24,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar24);
  gtk_box_pack_start (GTK_BOX (hbox4), toolbar24, TRUE, TRUE, 0);
  gtk_widget_set_usize (toolbar24, -2, 27);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar24), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgSearch, GNOME_STOCK_PIXMAP_SAVE);
  btnSearchSaveList = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar24),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Save List",
                                "Save Results of search", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnSearchSaveList);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "btnSearchSaveList", btnSearchSaveList,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSearchSaveList);

  entry1 = gtk_entry_new ();
  gtk_widget_ref (entry1);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "entry1", entry1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entry1);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar24), entry1, NULL, NULL);
  gtk_widget_set_usize (entry1, 216, 24);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgSearch, GNOME_STOCK_PIXMAP_SEARCH);
  btnSearch1 = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar24),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button4",
                                "Start Search", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnSearch1);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "btnSearch1", btnSearch1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSearch1);

  searchSettingsPanel = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (searchSettingsPanel);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "searchSettingsPanel", searchSettingsPanel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (searchSettingsPanel);
  gtk_box_pack_start (GTK_BOX (searchPanel), searchSettingsPanel, FALSE, FALSE, 0);

  frame15 = gtk_frame_new ("Search Type");
  gtk_widget_ref (frame15);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame15", frame15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame15);
  gtk_box_pack_start (GTK_BOX (searchSettingsPanel), frame15, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame15), 1);

  searchTypePanel = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (searchTypePanel);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "searchTypePanel", searchTypePanel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (searchTypePanel);
  gtk_container_add (GTK_CONTAINER (frame15), searchTypePanel);

  regexSearch = gtk_radio_button_new_with_label (searchType_group, "Regular Expression");
  searchType_group = gtk_radio_button_group (GTK_RADIO_BUTTON (regexSearch));
  gtk_widget_ref (regexSearch);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "regexSearch", regexSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (regexSearch);
  gtk_box_pack_start (GTK_BOX (searchTypePanel), regexSearch, FALSE, FALSE, 0);

  phraseSearch = gtk_radio_button_new_with_label (searchType_group, "Exact Phrase");
  searchType_group = gtk_radio_button_group (GTK_RADIO_BUTTON (phraseSearch));
  gtk_widget_ref (phraseSearch);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "phraseSearch", phraseSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (phraseSearch);
  gtk_box_pack_start (GTK_BOX (searchTypePanel), phraseSearch, FALSE, FALSE, 0);

  multiWordSearch = gtk_radio_button_new_with_label (searchType_group, "Multi Word");
  searchType_group = gtk_radio_button_group (GTK_RADIO_BUTTON (multiWordSearch));
  gtk_widget_ref (multiWordSearch);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "multiWordSearch", multiWordSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (multiWordSearch);
  gtk_box_pack_start (GTK_BOX (searchTypePanel), multiWordSearch, FALSE, FALSE, 0);

  frame16 = gtk_frame_new ("Search Options");
  gtk_widget_ref (frame16);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame16", frame16,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame16);
  gtk_box_pack_start (GTK_BOX (searchSettingsPanel), frame16, FALSE, FALSE, 0);
  gtk_container_set_border_width (GTK_CONTAINER (frame16), 1);

  searchOptionsPanel = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (searchOptionsPanel);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "searchOptionsPanel", searchOptionsPanel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (searchOptionsPanel);
  gtk_container_add (GTK_CONTAINER (frame16), searchOptionsPanel);

  caseSensitive = gtk_check_button_new_with_label ("Case Sensitive");
  gtk_widget_ref (caseSensitive);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "caseSensitive", caseSensitive,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (caseSensitive);
  gtk_box_pack_start (GTK_BOX (searchOptionsPanel), caseSensitive, FALSE, FALSE, 0);

  //----------------------------------------------------------------------------------
  ckbCom = gtk_check_button_new_with_label ("Search Commentary");
  gtk_widget_ref (ckbCom);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "ckbCom", ckbCom,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (ckbCom);
  gtk_box_pack_start (GTK_BOX (searchOptionsPanel), ckbCom, FALSE, FALSE, 0);
  //----------------------------------------------------------------------------------
  cbpercom = gtk_check_button_new_with_label ("Search Personal");
  gtk_widget_ref (cbpercom);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "cbpercom", cbpercom,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbpercom);
  gtk_box_pack_start (GTK_BOX (searchOptionsPanel), cbpercom, FALSE, FALSE, 0);
  //----------------------------------------------------------------------------------

  frame20 = gtk_frame_new ("Search Scope");
  gtk_widget_ref (frame20);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "frame20", frame20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame20);
  gtk_box_pack_start (GTK_BOX (searchPanel), frame20, TRUE, TRUE, 0);

  hbox17 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox17);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "hbox17", hbox17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox17);
  gtk_container_add (GTK_CONTAINER (frame20), hbox17);

  rbNoScope = gtk_radio_button_new_with_label (__group, "No Scope");
  __group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbNoScope));
  gtk_widget_ref (rbNoScope);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "rbNoScope", rbNoScope,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbNoScope);
  gtk_box_pack_start (GTK_BOX (hbox17), rbNoScope, FALSE, FALSE, 0);
  gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (rbNoScope), TRUE);

  rbUseBounds = gtk_radio_button_new_with_label (__group, "Use Bounds");
  __group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbUseBounds));
  gtk_widget_ref (rbUseBounds);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "rbUseBounds", rbUseBounds,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbUseBounds);
  gtk_box_pack_start (GTK_BOX (hbox17), rbUseBounds, FALSE, FALSE, 0);

  rbLastSearch = gtk_radio_button_new_with_label (__group, "Last Search");
  __group = gtk_radio_button_group (GTK_RADIO_BUTTON (rbLastSearch));
  gtk_widget_ref (rbLastSearch);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "rbLastSearch", rbLastSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (rbLastSearch);
  gtk_box_pack_start (GTK_BOX (hbox17), rbLastSearch, FALSE, FALSE, 0);

  table6 = gtk_table_new (2, 2, FALSE);
  gtk_widget_ref (table6);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "table6", table6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table6);
  gtk_box_pack_start (GTK_BOX (searchPanel), table6, TRUE, TRUE, 0);

  label82 = gtk_label_new ("Lower bounds");
  gtk_widget_ref (label82);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label82", label82,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label82);
  gtk_table_attach (GTK_TABLE (table6), label82, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label83 = gtk_label_new ("Uppen bounds");
  gtk_widget_ref (label83);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label83", label83,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label83);
  gtk_table_attach (GTK_TABLE (table6), label83, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  entryLower = gtk_entry_new ();
  gtk_widget_ref (entryLower);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "entryLower", entryLower,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryLower);
  gtk_table_attach (GTK_TABLE (table6), entryLower, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_text (GTK_ENTRY (entryLower), "Genesis");

  entryUpper = gtk_entry_new ();
  gtk_widget_ref (entryUpper);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "entryUpper", entryUpper,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryUpper);
  gtk_table_attach (GTK_TABLE (table6), entryUpper, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_text (GTK_ENTRY (entryUpper), "Revelation");

  scrolledwindow2 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow2);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "scrolledwindow2", scrolledwindow2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow2);
  gtk_box_pack_start (GTK_BOX (searchPanel), scrolledwindow2, TRUE, TRUE, 0);
  gtk_widget_set_usize (scrolledwindow2, -2, 185);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow2), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  resultList = gtk_clist_new (1);
  gtk_widget_ref (resultList);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "resultList", resultList,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (resultList);
  gtk_container_add (GTK_CONTAINER (scrolledwindow2), resultList);
  gtk_widget_set_usize (resultList, -2, 185);
  gtk_clist_set_column_width (GTK_CLIST (resultList), 0, 80);
  gtk_clist_column_titles_hide (GTK_CLIST (resultList));

  label46 = gtk_label_new ("label46");
  gtk_widget_ref (label46);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label46", label46,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label46);
  gtk_clist_set_column_widget (GTK_CLIST (resultList), 0, label46);

  scrolledwindow20 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow20);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "scrolledwindow20", scrolledwindow20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow20);
  gtk_box_pack_start (GTK_BOX (searchPanel), scrolledwindow20, FALSE, TRUE, 0);
  gtk_widget_set_usize (scrolledwindow20, -2, 87);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow20), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  txtSearch = gtk_text_new (NULL, NULL);
  gtk_widget_ref (txtSearch);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "txtSearch", txtSearch,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (txtSearch);
  gtk_container_add (GTK_CONTAINER (scrolledwindow20), txtSearch);

  cbContext = gtk_check_button_new_with_label ("Show Verse in Main window");
  gtk_widget_ref (cbContext);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "cbContext", cbContext,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbContext);
  gtk_box_pack_start (GTK_BOX (searchPanel), cbContext, FALSE, FALSE, 0);

  hbox15 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox15);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "hbox15", hbox15,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox15);
  gtk_box_pack_start (GTK_BOX (searchPanel), hbox15, FALSE, TRUE, 0);

  label57 = gtk_label_new ("Found: ");
  gtk_widget_ref (label57);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "label57", label57,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label57);
  gtk_box_pack_start (GTK_BOX (hbox15), label57, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (label57), GTK_JUSTIFY_RIGHT);
  gtk_misc_set_padding (GTK_MISC (label57), 4, 0);

  lbSearchHits = gtk_label_new ("0");
  gtk_widget_ref (lbSearchHits);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "lbSearchHits", lbSearchHits,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lbSearchHits);
  gtk_box_pack_start (GTK_BOX (hbox15), lbSearchHits, FALSE, FALSE, 0);
  gtk_label_set_justify (GTK_LABEL (lbSearchHits), GTK_JUSTIFY_LEFT);

  dialog_action_area1 = GNOME_DIALOG (dlgSearch)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlgSearch), "dialog_action_area1", dialog_action_area1);
  gtk_widget_show (dialog_action_area1);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area1), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area1), 8);

  gnome_dialog_append_button (GNOME_DIALOG (dlgSearch), GNOME_STOCK_BUTTON_CLOSE);
  btnSearchOK = g_list_last (GNOME_DIALOG (dlgSearch)->buttons)->data;
  gtk_widget_ref (btnSearchOK);
  gtk_object_set_data_full (GTK_OBJECT (dlgSearch), "btnSearchOK", btnSearchOK,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnSearchOK);
  GTK_WIDGET_SET_FLAGS (btnSearchOK, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (btnSearchSaveList), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSearchSaveList_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnSearch1), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSearch1_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (resultList), "select_row",
                      GTK_SIGNAL_FUNC (on_resultList_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cbContext), "toggled",
                      GTK_SIGNAL_FUNC (on_cbContext_toggled),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnSearchOK), "clicked",
                      GTK_SIGNAL_FUNC (on_btnSearchOK_clicked),
                      NULL);

  return dlgSearch;
}

//-----------------------------------------------------------------------------------------------
GtkWidget*
create_wdwEditBookmarks (void)
{
  GtkWidget *wdwEditBookmarks;
  GtkWidget *dialog_vbox2;
  GtkWidget *vbox17;
  GtkWidget *handlebox17;
  GtkWidget *toolbar23;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnEBMcut;
  GtkWidget *btnEBMcopy;
  GtkWidget *btnEBMpaste;
  GtkWidget *scrolledwindow25;
  GtkWidget *text4;
  GtkWidget *dialog_action_area2;
  GtkWidget *btnEBMOK;
  GtkWidget *btnEBMApply;
  GtkWidget *btnEBMCancel;

  wdwEditBookmarks = gnome_dialog_new ("GnomeSword - Edit Bookmarks", NULL);
  gtk_object_set_data (GTK_OBJECT (wdwEditBookmarks), "wdwEditBookmarks", wdwEditBookmarks);
  gtk_container_set_border_width (GTK_CONTAINER (wdwEditBookmarks), 4);

  dialog_vbox2 = GNOME_DIALOG (wdwEditBookmarks)->vbox;
  gtk_object_set_data (GTK_OBJECT (wdwEditBookmarks), "dialog_vbox2", dialog_vbox2);
  gtk_widget_show (dialog_vbox2);

  vbox17 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox17);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "vbox17", vbox17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox17);
  gtk_box_pack_start (GTK_BOX (dialog_vbox2), vbox17, TRUE, TRUE, 0);

  handlebox17 = gtk_handle_box_new ();
  gtk_widget_ref (handlebox17);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "handlebox17", handlebox17,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (handlebox17);
  gtk_box_pack_start (GTK_BOX (vbox17), handlebox17, FALSE, TRUE, 0);
  gtk_widget_set_usize (handlebox17, -2, 36);

  toolbar23 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar23);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "toolbar23", toolbar23,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar23);
  gtk_container_add (GTK_CONTAINER (handlebox17), toolbar23);
  gtk_widget_set_usize (toolbar23, -2, 34);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar23), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (wdwEditBookmarks, GNOME_STOCK_PIXMAP_CUT);
  btnEBMcut = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar23),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Cut",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEBMcut);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "btnEBMcut", btnEBMcut,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEBMcut);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (wdwEditBookmarks, GNOME_STOCK_PIXMAP_COPY);
  btnEBMcopy = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar23),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Copy",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEBMcopy);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "btnEBMcopy", btnEBMcopy,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEBMcopy);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (wdwEditBookmarks, GNOME_STOCK_PIXMAP_PASTE);
  btnEBMpaste = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar23),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Paste",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnEBMpaste);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "btnEBMpaste", btnEBMpaste,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEBMpaste);

  scrolledwindow25 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow25);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "scrolledwindow25", scrolledwindow25,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow25);
  gtk_box_pack_start (GTK_BOX (vbox17), scrolledwindow25, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow25), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  text4 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text4);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "text4", text4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text4);
  gtk_container_add (GTK_CONTAINER (scrolledwindow25), text4);
  gtk_widget_set_usize (text4, -2, 219);
  gtk_text_set_editable (GTK_TEXT (text4), TRUE);

  dialog_action_area2 = GNOME_DIALOG (wdwEditBookmarks)->action_area;
  gtk_object_set_data (GTK_OBJECT (wdwEditBookmarks), "dialog_action_area2", dialog_action_area2);
  gtk_widget_show (dialog_action_area2);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area2), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area2), 8);

  gnome_dialog_append_button (GNOME_DIALOG (wdwEditBookmarks), GNOME_STOCK_BUTTON_OK);
  btnEBMOK = g_list_last (GNOME_DIALOG (wdwEditBookmarks)->buttons)->data;
  gtk_widget_ref (btnEBMOK);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "btnEBMOK", btnEBMOK,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEBMOK);
  GTK_WIDGET_SET_FLAGS (btnEBMOK, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (wdwEditBookmarks), GNOME_STOCK_BUTTON_APPLY);
  btnEBMApply = g_list_last (GNOME_DIALOG (wdwEditBookmarks)->buttons)->data;
  gtk_widget_ref (btnEBMApply);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "btnEBMApply", btnEBMApply,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEBMApply);
  GTK_WIDGET_SET_FLAGS (btnEBMApply, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (wdwEditBookmarks), GNOME_STOCK_BUTTON_CANCEL);
  btnEBMCancel = g_list_last (GNOME_DIALOG (wdwEditBookmarks)->buttons)->data;
  gtk_widget_ref (btnEBMCancel);
  gtk_object_set_data_full (GTK_OBJECT (wdwEditBookmarks), "btnEBMCancel", btnEBMCancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnEBMCancel);
  GTK_WIDGET_SET_FLAGS (btnEBMCancel, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (btnEBMcut), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEBMcut_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEBMcopy), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEBMcopy_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEBMpaste), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEBMpaste_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (text4), "changed",
                      GTK_SIGNAL_FUNC (on_text4_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEBMOK), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEBMOK_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEBMApply), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEBMApply_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnEBMCancel), "clicked",
                      GTK_SIGNAL_FUNC (on_btnEBMCancel_clicked),
                      NULL);

  gtk_widget_grab_default (btnEBMCancel);
  return wdwEditBookmarks;
}

//-----------------------------------------------------------------------------------------------
GtkWidget*
create_dlgVerseList (void)
{
  GtkWidget *dlgVerseList;
  GtkWidget *dialog_vbox5;
  GtkWidget *toolbar26;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnVerseListNew;
  GtkWidget *btnVerseListOpen;
  GtkWidget *btnVerseListSave;
  GtkWidget *btnVerseListPrint;
  GtkWidget *vseparator14;
  GtkWidget *btnVerseListCut;
  GtkWidget *btnVerseListCopy;
  GtkWidget *btnVerseListPaste;
  GtkWidget *frame21;
  GtkWidget *scrolledwindow29;
  GtkWidget *verselist;
  GtkWidget *label90;
  GtkWidget *dialog_action_area5;
  GtkWidget *btnVerseListClose;

  dlgVerseList = gnome_dialog_new ("GnomeSword - Verse List", NULL);
  gtk_object_set_data (GTK_OBJECT (dlgVerseList), "dlgVerseList", dlgVerseList);
  gtk_container_set_border_width (GTK_CONTAINER (dlgVerseList), 4);
  gtk_window_set_policy (GTK_WINDOW (dlgVerseList), FALSE, FALSE, TRUE);

  dialog_vbox5 = GNOME_DIALOG (dlgVerseList)->vbox;
  gtk_object_set_data (GTK_OBJECT (dlgVerseList), "dialog_vbox5", dialog_vbox5);
  gtk_widget_show (dialog_vbox5);

  toolbar26 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar26);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "toolbar26", toolbar26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar26);
  gtk_box_pack_start (GTK_BOX (dialog_vbox5), toolbar26, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar26), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgVerseList, GNOME_STOCK_PIXMAP_NEW);
  btnVerseListNew = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "New List",
                                "Start New Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListNew);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListNew", btnVerseListNew,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListNew);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgVerseList, GNOME_STOCK_PIXMAP_OPEN);
  btnVerseListOpen = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Open List",
                                "Open Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListOpen);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListOpen", btnVerseListOpen,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListOpen);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgVerseList, GNOME_STOCK_PIXMAP_SAVE);
  btnVerseListSave = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Save List",
                                "Save Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListSave);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListSave", btnVerseListSave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListSave);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgVerseList, GNOME_STOCK_PIXMAP_PRINT);
  btnVerseListPrint = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Print List",
                                "Print Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListPrint);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListPrint", btnVerseListPrint,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListPrint);

  vseparator14 = gtk_vseparator_new ();
  gtk_widget_ref (vseparator14);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "vseparator14", vseparator14,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vseparator14);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar26), vseparator14, NULL, NULL);
  gtk_widget_set_usize (vseparator14, 5, 7);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgVerseList, GNOME_STOCK_PIXMAP_CUT);
  btnVerseListCut = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Cut",
                                "Cut Selected Verse(s)", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListCut);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListCut", btnVerseListCut,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListCut);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgVerseList, GNOME_STOCK_PIXMAP_COPY);
  btnVerseListCopy = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Copy",
                                "Copy Selected Verse(s)", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListCopy);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListCopy", btnVerseListCopy,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListCopy);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (dlgVerseList, GNOME_STOCK_PIXMAP_PASTE);
  btnVerseListPaste = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Paste",
                                "Paste Verse", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListPaste);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListPaste", btnVerseListPaste,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListPaste);

  frame21 = gtk_frame_new ("VerseList");
  gtk_widget_ref (frame21);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "frame21", frame21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame21);
  gtk_box_pack_start (GTK_BOX (dialog_vbox5), frame21, TRUE, TRUE, 0);

  scrolledwindow29 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow29);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "scrolledwindow29", scrolledwindow29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow29);
  gtk_container_add (GTK_CONTAINER (frame21), scrolledwindow29);
  gtk_widget_set_usize (scrolledwindow29, 257, 303);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow29), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  verselist = gtk_clist_new (1);
  gtk_widget_ref (verselist);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "verselist", verselist,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (verselist);
  gtk_container_add (GTK_CONTAINER (scrolledwindow29), verselist);
  gtk_clist_set_column_width (GTK_CLIST (verselist), 0, 80);
  gtk_clist_set_selection_mode (GTK_CLIST (verselist), GTK_SELECTION_MULTIPLE);
  gtk_clist_column_titles_hide (GTK_CLIST (verselist));

  label90 = gtk_label_new ("label90");
  gtk_widget_ref (label90);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "label90", label90,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label90);
  gtk_clist_set_column_widget (GTK_CLIST (verselist), 0, label90);

  dialog_action_area5 = GNOME_DIALOG (dlgVerseList)->action_area;
  gtk_object_set_data (GTK_OBJECT (dlgVerseList), "dialog_action_area5", dialog_action_area5);
  gtk_widget_show (dialog_action_area5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area5), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area5), 8);

  gnome_dialog_append_button (GNOME_DIALOG (dlgVerseList), GNOME_STOCK_BUTTON_CLOSE);
  btnVerseListClose = g_list_last (GNOME_DIALOG (dlgVerseList)->buttons)->data;
  gtk_widget_ref (btnVerseListClose);
  gtk_object_set_data_full (GTK_OBJECT (dlgVerseList), "btnVerseListClose", btnVerseListClose,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListClose);
  GTK_WIDGET_SET_FLAGS (btnVerseListClose, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (btnVerseListNew), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVerseListNew_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVerseListOpen), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVerseListOpen_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVerseListSave), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVerseListSave_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVerseListPrint), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVerseListPrint_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVerseListCopy), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVerseListCopy_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVerseListPaste), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVerseListPaste_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (verselist), "select_row",
                      GTK_SIGNAL_FUNC (on_verselist_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnVerseListClose), "clicked",
                      GTK_SIGNAL_FUNC (on_btnVerseListClose_clicked),
                      NULL);

  return dlgVerseList;
}

//-----------------------------------------------------------------------------------------------
GtkWidget*
create_InfoBox (void)
{
  GtkWidget *InfoBox;
  GtkWidget *dialog_vbox6;
  GtkWidget *hbox20;
  GtkWidget *pixmap2;
  GtkWidget *vbox24;
  GtkWidget *lbInfoBox1;
  GtkWidget *lbInfoBox2;
  GtkWidget *lbInfoBox3;
  GtkWidget *dialog_action_area6;
  GtkWidget *btnInfoBoxYes;
  GtkWidget *btnInfoBoxNo;

  InfoBox = gnome_dialog_new ("GnomeSword - InfoBox", NULL);
  gtk_object_set_data (GTK_OBJECT (InfoBox), "InfoBox", InfoBox);
  gtk_widget_set_usize (InfoBox, 216, -2);
  gtk_container_set_border_width (GTK_CONTAINER (InfoBox), 4);
  gtk_window_set_modal (GTK_WINDOW (InfoBox), TRUE);

  dialog_vbox6 = GNOME_DIALOG (InfoBox)->vbox;
  gtk_object_set_data (GTK_OBJECT (InfoBox), "dialog_vbox6", dialog_vbox6);
  gtk_widget_show (dialog_vbox6);

  hbox20 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox20);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "hbox20", hbox20,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox20);
  gtk_box_pack_start (GTK_BOX (dialog_vbox6), hbox20, TRUE, TRUE, 0);

  pixmap2 = create_pixmap (InfoBox, "gnomesword/GnomeSword.xpm", FALSE);
  gtk_widget_ref (pixmap2);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "pixmap2", pixmap2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap2);
  gtk_box_pack_start (GTK_BOX (hbox20), pixmap2, TRUE, TRUE, 0);

  vbox24 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox24);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "vbox24", vbox24,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox24);
  gtk_box_pack_start (GTK_BOX (hbox20), vbox24, TRUE, TRUE, 0);

  lbInfoBox1 = gtk_label_new ("File");
  gtk_widget_ref (lbInfoBox1);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "lbInfoBox1", lbInfoBox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lbInfoBox1);
  gtk_box_pack_start (GTK_BOX (vbox24), lbInfoBox1, FALSE, FALSE, 0);

  lbInfoBox2 = gtk_label_new ("in StudyPad is not saved!");
  gtk_widget_ref (lbInfoBox2);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "lbInfoBox2", lbInfoBox2,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lbInfoBox2);
  gtk_box_pack_start (GTK_BOX (vbox24), lbInfoBox2, FALSE, FALSE, 0);

  lbInfoBox3 = gtk_label_new ("Shall I save it?");
  gtk_widget_ref (lbInfoBox3);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "lbInfoBox3", lbInfoBox3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lbInfoBox3);
  gtk_box_pack_start (GTK_BOX (vbox24), lbInfoBox3, FALSE, FALSE, 0);

  dialog_action_area6 = GNOME_DIALOG (InfoBox)->action_area;
  gtk_object_set_data (GTK_OBJECT (InfoBox), "dialog_action_area6", dialog_action_area6);
  gtk_widget_show (dialog_action_area6);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area6), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area6), 8);

  gnome_dialog_append_button (GNOME_DIALOG (InfoBox), GNOME_STOCK_BUTTON_YES);
  btnInfoBoxYes = g_list_last (GNOME_DIALOG (InfoBox)->buttons)->data;
  gtk_widget_ref (btnInfoBoxYes);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "btnInfoBoxYes", btnInfoBoxYes,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnInfoBoxYes);
  GTK_WIDGET_SET_FLAGS (btnInfoBoxYes, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (InfoBox), GNOME_STOCK_BUTTON_NO);
  btnInfoBoxNo = g_list_last (GNOME_DIALOG (InfoBox)->buttons)->data;
  gtk_widget_ref (btnInfoBoxNo);
  gtk_object_set_data_full (GTK_OBJECT (InfoBox), "btnInfoBoxNo", btnInfoBoxNo,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnInfoBoxNo);
  GTK_WIDGET_SET_FLAGS (btnInfoBoxNo, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (btnInfoBoxYes), "clicked",
                      GTK_SIGNAL_FUNC (on_btnInfoBoxYes_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnInfoBoxNo), "clicked",
                      GTK_SIGNAL_FUNC (on_btnInfoBoxNo_clicked),
                      NULL);

  return InfoBox;
}

//-----------------------------------------------------------------------------------------------
GtkWidget*
create_AboutSword (void)
{
  GtkWidget *AboutSword;
  GtkWidget *dialog_vbox7;
  GtkWidget *vbox21;
  GtkWidget *pixmap1;
  GtkWidget *scrolledwindow26;
  GtkWidget *txtAboutSword;
  GtkWidget *label95;
  GtkWidget *href1;
  GtkWidget *label96;
  GtkWidget *text6;
  GtkWidget *dialog_action_area7;
  GtkWidget *btnAboutSwordOK;

  AboutSword = gnome_dialog_new ("About The Sword Project", NULL);
  gtk_object_set_data (GTK_OBJECT (AboutSword), "AboutSword", AboutSword);
  gtk_container_set_border_width (GTK_CONTAINER (AboutSword), 4);
  gtk_window_set_policy (GTK_WINDOW (AboutSword), FALSE, FALSE, FALSE);

  dialog_vbox7 = GNOME_DIALOG (AboutSword)->vbox;
  gtk_object_set_data (GTK_OBJECT (AboutSword), "dialog_vbox7", dialog_vbox7);
  gtk_widget_show (dialog_vbox7);

  vbox21 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox21);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "vbox21", vbox21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox21);
  gtk_box_pack_start (GTK_BOX (dialog_vbox7), vbox21, TRUE, TRUE, 0);

  pixmap1 = create_pixmap (AboutSword, "gnomesword/sword.xpm", FALSE);
  gtk_widget_ref (pixmap1);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "pixmap1", pixmap1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap1);
  gtk_box_pack_start (GTK_BOX (vbox21), pixmap1, FALSE, FALSE, 0);
  gtk_misc_set_padding (GTK_MISC (pixmap1), 4, 4);

  scrolledwindow26 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow26);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "scrolledwindow26", scrolledwindow26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow26);
  gtk_box_pack_start (GTK_BOX (vbox21), scrolledwindow26, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow26), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  txtAboutSword = gtk_text_new (NULL, NULL);
  gtk_widget_ref (txtAboutSword);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "txtAboutSword", txtAboutSword,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (txtAboutSword);
  gtk_container_add (GTK_CONTAINER (scrolledwindow26), txtAboutSword);
  gtk_widget_set_usize (txtAboutSword, -2, 103);
  gtk_widget_realize (txtAboutSword);
  gtk_text_insert (GTK_TEXT (txtAboutSword), NULL, NULL, NULL,
                   "The SWORD Project is an effort to create an ever expanding software package for research and study of God and His Word.  The SWORD Bible Framework allows easy manipulation of Bible texts, commentaries, lexicons, dictionaries, etc.  Many frontends are build using this framework.  An installed module set may be shared between any frontend using the framework.", 359);

  label95 = gtk_label_new ("Modules can be downloaded from the Sword Project ");
  gtk_widget_ref (label95);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "label95", label95,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label95);
  gtk_box_pack_start (GTK_BOX (vbox21), label95, FALSE, FALSE, 0);
  gtk_widget_set_usize (label95, -2, 24);

  href1 = gnome_href_new ("www.crosswire.org", "The Sword Project");
  gtk_widget_ref (href1);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "href1", href1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (href1);
  gtk_box_pack_start (GTK_BOX (vbox21), href1, FALSE, FALSE, 0);

  label96 = gtk_label_new ("Current Module");
  gtk_widget_ref (label96);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "label96", label96,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label96);
  gtk_box_pack_start (GTK_BOX (vbox21), label96, FALSE, FALSE, 0);
  gtk_widget_set_usize (label96, -2, 19);

  text6 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text6);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "text6", text6,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text6);
  gtk_box_pack_start (GTK_BOX (vbox21), text6, TRUE, TRUE, 0);

  dialog_action_area7 = GNOME_DIALOG (AboutSword)->action_area;
  gtk_object_set_data (GTK_OBJECT (AboutSword), "dialog_action_area7", dialog_action_area7);
  gtk_widget_show (dialog_action_area7);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area7), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area7), 8);

  gnome_dialog_append_button (GNOME_DIALOG (AboutSword), GNOME_STOCK_BUTTON_OK);
  btnAboutSwordOK = g_list_last (GNOME_DIALOG (AboutSword)->buttons)->data;
  gtk_widget_ref (btnAboutSwordOK);
  gtk_object_set_data_full (GTK_OBJECT (AboutSword), "btnAboutSwordOK", btnAboutSwordOK,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnAboutSwordOK);
  GTK_WIDGET_SET_FLAGS (btnAboutSwordOK, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (btnAboutSwordOK), "clicked",
                      GTK_SIGNAL_FUNC (on_btnAboutSwordOK_clicked),
                      NULL);

  return AboutSword;
}

//================================== about sword modules dialog ===============================\\
//-----------------------------------------------------------------------------------------------
GtkWidget*
create_aboutmodules (void)
{
  GtkWidget *dialog1;
  GtkWidget *dialog_vbox8;
  GtkWidget *vbox25;
  GtkWidget *hbox21;
  GtkWidget *pixmap3;
  GtkWidget *label97;
  GtkWidget *pixmap4;
  GtkWidget *lbModName;
  GtkWidget *scrolledwindow30;
  GtkWidget *textModAbout;
  GtkWidget *dialog_action_area8;
  GtkWidget *btnAboutModuleOK;

  dialog1 = gnome_dialog_new ("About Sword Modules", NULL);
  gtk_object_set_data (GTK_OBJECT (dialog1), "dialog1", dialog1);
  gtk_container_set_border_width (GTK_CONTAINER (dialog1), 4);
  gtk_window_set_policy (GTK_WINDOW (dialog1), FALSE, FALSE, FALSE);

  dialog_vbox8 = GNOME_DIALOG (dialog1)->vbox;
  gtk_object_set_data (GTK_OBJECT (dialog1), "dialog_vbox8", dialog_vbox8);
  gtk_widget_show (dialog_vbox8);

  vbox25 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox25);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "vbox25", vbox25,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox25);
  gtk_box_pack_start (GTK_BOX (dialog_vbox8), vbox25, TRUE, TRUE, 0);

  hbox21 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox21);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "hbox21", hbox21,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox21);
  gtk_box_pack_start (GTK_BOX (vbox25), hbox21, TRUE, TRUE, 0);

  pixmap3 = create_pixmap (dialog1, "gnomesword/sword.xpm", FALSE);
  gtk_widget_ref (pixmap3);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "pixmap3", pixmap3,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap3);
  gtk_box_pack_start (GTK_BOX (hbox21), pixmap3, TRUE, TRUE, 0);

  label97 = gtk_label_new ("The Sword Project");
  gtk_widget_ref (label97);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "label97", label97,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label97);
  gtk_box_pack_start (GTK_BOX (hbox21), label97, FALSE, FALSE, 0);

  pixmap4 = create_pixmap (dialog1, "gnomesword/GnomeSword.xpm", FALSE);
  gtk_widget_ref (pixmap4);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "pixmap4", pixmap4,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (pixmap4);
  gtk_box_pack_start (GTK_BOX (hbox21), pixmap4, TRUE, TRUE, 0);

  lbModName = gtk_label_new ("KJV");
  gtk_widget_ref (lbModName);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "lbModName", lbModName,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (lbModName);
  gtk_box_pack_start (GTK_BOX (vbox25), lbModName, FALSE, FALSE, 0);
  gtk_widget_set_usize (lbModName, -2, 21);

  scrolledwindow30 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow30);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "scrolledwindow30", scrolledwindow30,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow30);
  gtk_box_pack_start (GTK_BOX (vbox25), scrolledwindow30, TRUE, TRUE, 0);
  gtk_widget_set_usize (scrolledwindow30, 304, 183);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow30), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  textModAbout = gtk_text_new (NULL, NULL);
  gtk_widget_ref (textModAbout);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "textModAbout", textModAbout,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (textModAbout);
  gtk_container_add (GTK_CONTAINER (scrolledwindow30), textModAbout);

  dialog_action_area8 = GNOME_DIALOG (dialog1)->action_area;
  gtk_object_set_data (GTK_OBJECT (dialog1), "dialog_action_area8", dialog_action_area8);
  gtk_widget_show (dialog_action_area8);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area8), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area8), 8);

  gnome_dialog_append_button (GNOME_DIALOG (dialog1), GNOME_STOCK_BUTTON_OK);
  btnAboutModuleOK = g_list_last (GNOME_DIALOG (dialog1)->buttons)->data;
  gtk_widget_ref (btnAboutModuleOK);
  gtk_object_set_data_full (GTK_OBJECT (dialog1), "btnAboutModuleOK", btnAboutModuleOK,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnAboutModuleOK);
  GTK_WIDGET_SET_FLAGS (btnAboutModuleOK, GTK_CAN_DEFAULT);

  gtk_signal_connect (GTK_OBJECT (btnAboutModuleOK), "clicked",
                      GTK_SIGNAL_FUNC (on_btnAboutModuleOK_clicked),
                      NULL);

  return dialog1;
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
  btnBMEok = g_list_last (GNOME_DIALOG (BMEditor)->buttons)->data;
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
  btnBMEcancel = g_list_last (GNOME_DIALOG (BMEditor)->buttons)->data;
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
  gtk_signal_connect (GTK_OBJECT (btnBMEsave), "clicked",
                      GTK_SIGNAL_FUNC (on_btnBMEsave_clicked),
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


