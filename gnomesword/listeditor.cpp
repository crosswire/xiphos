/***************************************************************************
                          listeditor.cpp  -  description
                             -------------------
    begin                : Wed Jun 21 2000
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
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

#include "display.h"
#include "callback.h"
#include "interface.h"
#include "support.h"
#include "listeditor.h"

SWDisplay *GBFlistDisplay; //-- to display modules in list editor
SWMgr *listMgr;  //-- sword mgr for ListEditor
SWModule *listMod;	//-- module for ListEditor
extern SWModule *curMod; //-- module for main text window (GnomeSword.cpp)

extern gint ibookmarks;

//-------------------------------------------------------------------------------------------
void
addsubitme(GtkWidget *list, gint row)
{
  gchar *buf[1][6];
  ++row;
  buf[0][0] = g_strdup(" ");
  buf[0][1] = g_strdup("1");
  buf[0][2] = g_strdup("0");
  buf[0][3] = g_strdup(" ");
  buf[0][4] = g_strdup(" ");
  buf[0][5] = g_strdup(" ");
  gtk_clist_insert(GTK_CLIST(list), row, buf[0]); //-- insert item in new position
	gtk_clist_select_row(GTK_CLIST(list), row, 0); //-- seletct row (new item)	
	++ibookmarks;
	
}

//-------------------------------------------------------------------------------------------
void
addverse(GtkWidget *list, gint row, gchar *item)
{
  gchar *buf[1][6],
  			*tmpbuf;

  buf[0][0] = g_strdup(item);
  buf[0][1] = g_strdup("0");
  gtk_clist_get_text(GTK_CLIST(list), row, 2, &tmpbuf); //-- get item from list
  buf[0][2] = g_strdup(tmpbuf);
  gtk_clist_get_text(GTK_CLIST(list), row, 3, &tmpbuf); //-- get item from list
  buf[0][3] = g_strdup(tmpbuf);
  gtk_clist_get_text(GTK_CLIST(list), row, 4, &tmpbuf); //-- get item from list
  buf[0][4] = g_strdup(tmpbuf);
  gtk_clist_get_text(GTK_CLIST(list), row, 0, &tmpbuf); //-- get item from list
  buf[0][5] = g_strdup(tmpbuf);
  ++row;
  gtk_clist_insert(GTK_CLIST(list), row, buf[0]); //-- insert item in new position
	//gtk_clist_select_row(GTK_CLIST(list), row, 0); //-- seletct row (new item)	
	++ibookmarks;
	if(row < ibookmarks)
	{
		++row;
		gtk_clist_get_text(GTK_CLIST(list), row, 0, &tmpbuf); //-- get item from list
  	buf[0][0] = g_strdup(tmpbuf);  	
  	gtk_clist_get_text(GTK_CLIST(list), row, 1, &tmpbuf); //-- get type from list
  	buf[0][1] = g_strdup(tmpbuf);
  	gtk_clist_get_text(GTK_CLIST(list), row, 2, &tmpbuf); //-- get level from list
  	buf[0][2] = g_strdup(tmpbuf);
  	gtk_clist_get_text(GTK_CLIST(list), row, 3, &tmpbuf); //-- get subitem from list
  	buf[0][3] = g_strdup(tmpbuf);
  	gtk_clist_get_text(GTK_CLIST(list), row, 4, &tmpbuf); //-- get menu from list
  	buf[0][4] = g_strdup(tmpbuf);   	
  	buf[0][5] = g_strdup(item); //---------------------- previous item
  	if(buf[0][1][0] == 0 && buf[0][0][0] != '<') //-- change only if not a subitem and not following a <Separator>
  	{
  		gtk_clist_remove(GTK_CLIST(list), row); //-- remove item
  		gtk_clist_insert(GTK_CLIST(list), row, buf[0]); //-- insert item in new position
  	}
  }
	gtk_clist_select_row(GTK_CLIST(list), row-1, 0); //-- seletct row (new item)	
}

//-------------------------------------------------------------------------------------------
void
changeLEverse(gchar *verse)
{
   if(listMod)
   {
   	listMod->SetKey(verse);
   	listMod->Display();
   }
}
//-------------------------------------------------------------------------------------------
GtkWidget *
createListEditor(void) //-- init ListEditor
{
		GtkWidget *ListEditor,
							*text;
		
		
		listMgr					= new SWMgr();
		listMod					= NULL;
		
		
		ListEditor = create_ListEditor();
		text = lookup_widget(ListEditor,"text7");
		ModMap::iterator it;  //-- sword manager iterator
		//gtk_text_set_word_wrap(GTK_TEXT (lookup_widget(searchDlg,"txtSearch")) , TRUE ); //-- set text window to word wrap
		GBFlistDisplay = new GTKInterlinearDisp(text); //-- set sword display
    	//--------------------------------------------------------------------------------------- searchmodule	
		for (it = listMgr->Modules.begin(); it != listMgr->Modules.end(); it++) //-- iterator through modules
		{
			if (!strcmp((*it).second->Type(), "Biblical Texts"))
			{
				listMod  = (*it).second;  //-- set searchMod
				listMod->Disp(GBFlistDisplay); //-- set search display for modules
			}
		}
		it = listMgr->Modules.find(curMod->Name()); //-- set listMod to curMod
		if (it !=listMgr->Modules.end())
		{
			listMod = (*it).second;			
		}	
		gtk_signal_connect (GTK_OBJECT (ListEditor), "destroy",
                      GTK_SIGNAL_FUNC (on_listeditor_destroy),
                      NULL);
		return(ListEditor);
}

//-------------------------------------------------------------------------------------------
void
destroyListEditor(void) //-- destroy ListEditor
{
	delete listMgr;
	if (GBFlistDisplay)    //-- delete Sword display
		delete GBFlistDisplay;
}

//-----------------------------------------------------------------------------------------------
void
applylistchanges(GtkWidget *widget, gint row)
{
  GtkWidget *editor,
  					*entry,
  					*list;
  gchar			*buf[1][6];  //-- pointer to string storage

  editor = gtk_widget_get_toplevel(widget); //-- set pointer to editor dialog
  list =  lookup_widget(editor,"clLElist");  //-- get item list
  entry = lookup_widget(editor,"entryListItem");  //-- get item entry
  buf[0][0]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  entry = lookup_widget(editor,"entryType");  //-- get item entry
  buf[0][1]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  entry = lookup_widget(editor,"entryLevel");  //-- get item entry
  buf[0][2]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  entry = lookup_widget(editor,"entrySubitem");  //-- get item entry
  buf[0][3]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  entry = lookup_widget(editor,"entryMenu");  //-- get item entry
  buf[0][4]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  entry = lookup_widget(editor,"entryPreitem");  //-- get item entry
  buf[0][5]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  gtk_clist_remove(GTK_CLIST(list), row); //-- remove item from current position
	gtk_clist_insert(GTK_CLIST(list), row, buf[0]); //-- insert item in new position
	gtk_clist_select_row(GTK_CLIST(list), row, 0); //-- seletct row (same item)
}
//-----------------------------------------------------------------------------------------------
void
selectrow(GtkWidget *list, gint row, gint column)
{
	GtkWidget *editor,
						*entry;
	gchar 		*buf;	
				
	editor = gtk_widget_get_toplevel(list); //-- set pointer to editor dialog
	entry = lookup_widget(editor,"entryListItem");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 0, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryType");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 1, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryLevel");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row,2, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entrySubitem");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 3, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryMenu");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row,4, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryPreitem");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 5, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
}

//-----------------------------------------------------------------------------------------------
void                   //-- for moving list items up and down - and adding and removing tabs(changing level of item)
movelistitem(GtkWidget *widget, gint direction, gint listrow)
{
	GtkWidget *editor, //-- pointer to list editor
						*list; //-- pointer to list
	gchar			*buf[1][6],  //-- pointer to string storage
						*tmpbuf, //-- pointer to string storage
						listbuf[255], //-- string storage
						newbuf[255];  //-- string storage
	gint 			ilevel, //-- for level item
						i;	  //-- for counting
								
	editor = gtk_widget_get_toplevel(widget); //-- set pointer to editor dialog
	list =  lookup_widget(editor,"clLElist");  //-- get item list
	//---- read items from list so we can move them or show changes
	gtk_clist_get_text(GTK_CLIST(list), listrow, 0, &tmpbuf); //-- get item from list
	buf[0][0] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 1, &tmpbuf); //-- get type from list
	buf[0][1] = g_strdup(tmpbuf);		
	gtk_clist_get_text(GTK_CLIST(list), listrow, 2, &tmpbuf); //-- get level from list
	buf[0][2] = g_strdup(tmpbuf);
	ilevel = atoi(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 3, &tmpbuf); //-- get subitem from list
	buf[0][3] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 4, &tmpbuf); //-- get menu from list
	buf[0][4] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 5, &tmpbuf); //-- get extra item from list
	buf[0][5] = g_strdup(tmpbuf);			
	switch(direction)  //-- make sure that we want to do something
	{
		case	0:    if(listrow > 1)  //-- don't do anything if we are in first row and press up button 	
								{
		            	gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position
									--listrow;  //-- subtract one form our position
								}
								else return;
								break;  	
		case  1:    if(listrow < ibookmarks) //-- don't do anything if we are in last row and press down button
								{
									gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position
									++listrow; //-- add one to listrow 																		
								}
								else return;
								break;
		
		case  2:		if(buf[0][0][0] == '\t')   //-- don't do anything is there is not a tab
								{
	   							for(i=0;i<(strlen(buf[0][0]))-1;i++) //-- remove one tab
	   							{
	   	  						listbuf[i] = buf[0][0][i+1];
	   	  						listbuf[i+1] = '\0';
	   							}
									gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position
									buf[0][0] = g_strdup(listbuf);
									--ilevel;
									sprintf(buf[0][2],"%d",ilevel);
								}
								else return;
								break;
		
		case  3:    sprintf(listbuf,"%s",buf[0][0]);
								if(listbuf[2] == '\t') return; //-- do nothing if we have three tabs
								newbuf[0] = '\t';
	
								for(i=1;i<(strlen(listbuf))+1;i++) //-- add one tab
								{ 	
	  							newbuf[i] = listbuf[i-1];
	  							newbuf[i+1] = '\0';
								} 	
								gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position
								buf[0][0] = g_strdup(newbuf);
								++ilevel;
								sprintf(buf[0][2],"%d",ilevel);	
		 						break;   //-- stay if right button pressed
		
		default: 		return;
	} 	
	gtk_clist_insert(GTK_CLIST(list), listrow, buf[0]); //-- insert item in new position
	gtk_clist_select_row(GTK_CLIST(list), listrow, 0); //-- seletct row (same item)
}


GtkWidget*
create_ListEditor (void)
{
  GtkWidget *ListEditor;
  GtkWidget *dialog_vbox5;
  GtkWidget *frame24;
  GtkWidget *toolbar26;
  GtkWidget *tmp_toolbar_icon;
  GtkWidget *btnVerseListNew;
  GtkWidget *btnVerseListOpen;
  GtkWidget *btnVerseListSave;
  GtkWidget *btnVerseListPrint;
  GtkWidget *hbox22;
  GtkWidget *vbox26;
  GtkWidget *scrolledwindow34;
  GtkWidget *clLElist;
  GtkWidget *label103;
  GtkWidget *label104;
  GtkWidget *label105;
  GtkWidget *label106;
  GtkWidget *label107;
  GtkWidget *label108;
  GtkWidget *toolbar29;
  GtkWidget *btnLEup;
  GtkWidget *btnLEdown;
  GtkWidget *btnLEleft;
  GtkWidget *btnLEright;
  GtkWidget *vbox27;
  GtkWidget *frame22;
  GtkWidget *vbox28;
  GtkWidget *toolbar31;
  GtkWidget *entryVerseLookup;
  GtkWidget *btnGotoVerse;
  GtkWidget *toolbar32;
  GtkWidget *combo1;
  GtkWidget *cbLEBook;
  GtkObject *spLEChapter_adj;
  GtkWidget *spLEChapter;
  GtkObject *spLEVerse_adj;
  GtkWidget *spLEVerse;
  GtkWidget *frame23;
  GtkWidget *vbox29;
  GtkWidget *entryListItem;
  GtkWidget *toolbar33;
  GtkWidget *btnLEAddVerse;
  GtkWidget *btnLEAddItem;
  GtkWidget *btnLEmakesub;
  GtkWidget *btnLEDelete;
  GtkWidget *hbox23;
  GtkWidget *label109;
  GtkWidget *entryType;
  GtkWidget *label110;
  GtkWidget *entryLevel;
  GtkWidget *table8;
  GtkWidget *label111;
  GtkWidget *label112;
  GtkWidget *entrySubitem;
  GtkWidget *entryMenu;
  GtkWidget *label113;
  GtkWidget *entryPreitem;
  GtkWidget *hbuttonbox1;
  GtkWidget *btnLEapplylistchanges;
  GtkWidget *btnLEcancelistchanges;
  GtkWidget *scrolledwindow33;
  GtkWidget *text7;
  GtkWidget *dialog_action_area5;
  GtkWidget *btnLEok;
  GtkWidget *btnLEapply;
  GtkWidget *btnLEcancel;

  ListEditor = gnome_dialog_new ("GnomeSword - List Editor", NULL);
  gtk_object_set_data (GTK_OBJECT (ListEditor), "ListEditor", ListEditor);
  gtk_container_set_border_width (GTK_CONTAINER (ListEditor), 4);
  gtk_window_set_policy (GTK_WINDOW (ListEditor), FALSE, FALSE, TRUE);

  dialog_vbox5 = GNOME_DIALOG (ListEditor)->vbox;
  gtk_object_set_data (GTK_OBJECT (ListEditor), "dialog_vbox5", dialog_vbox5);
  gtk_widget_show (dialog_vbox5);

  frame24 = gtk_frame_new (NULL);
  gtk_widget_ref (frame24);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "frame24", frame24,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame24);
  gtk_box_pack_start (GTK_BOX (dialog_vbox5), frame24, FALSE, FALSE, 0);

  toolbar26 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar26);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "toolbar26", toolbar26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar26);
  gtk_container_add (GTK_CONTAINER (frame24), toolbar26);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar26), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_NEW);
  btnVerseListNew = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "New List",
                                "Start New Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListNew);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnVerseListNew", btnVerseListNew,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListNew);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_OPEN);
  btnVerseListOpen = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Open List",
                                "Open Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListOpen);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnVerseListOpen", btnVerseListOpen,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListOpen);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_SAVE);
  btnVerseListSave = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Save List",
                                "Save Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListSave);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnVerseListSave", btnVerseListSave,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListSave);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_PRINT);
  btnVerseListPrint = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar26),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Print List",
                                "Print Verse List", NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnVerseListPrint);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnVerseListPrint", btnVerseListPrint,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnVerseListPrint);
  gtk_widget_set_sensitive (btnVerseListPrint, FALSE);

  hbox22 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox22);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "hbox22", hbox22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox22);
  gtk_box_pack_start (GTK_BOX (dialog_vbox5), hbox22, TRUE, TRUE, 0);

  vbox26 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox26);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "vbox26", vbox26,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox26);
  gtk_box_pack_start (GTK_BOX (hbox22), vbox26, TRUE, TRUE, 0);

  scrolledwindow34 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow34);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "scrolledwindow34", scrolledwindow34,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow34);
  gtk_box_pack_start (GTK_BOX (vbox26), scrolledwindow34, TRUE, TRUE, 0);
  gtk_widget_set_usize (scrolledwindow34, 240, 206);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow34), GTK_POLICY_NEVER, GTK_POLICY_ALWAYS);

  clLElist = gtk_clist_new (6);
  gtk_widget_ref (clLElist);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "clLElist", clLElist,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (clLElist);
  gtk_container_add (GTK_CONTAINER (scrolledwindow34), clLElist);
  gtk_clist_set_column_width (GTK_CLIST (clLElist), 0, 207);
  gtk_clist_set_column_width (GTK_CLIST (clLElist), 1, 80);
  gtk_clist_set_column_width (GTK_CLIST (clLElist), 2, 80);
  gtk_clist_set_column_width (GTK_CLIST (clLElist), 3, 80);
  gtk_clist_set_column_width (GTK_CLIST (clLElist), 4, 80);
  gtk_clist_set_column_width (GTK_CLIST (clLElist), 5, 80);
  gtk_clist_column_titles_hide (GTK_CLIST (clLElist));

  label103 = gtk_label_new ("label103");
  gtk_widget_ref (label103);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label103", label103,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label103);
  gtk_clist_set_column_widget (GTK_CLIST (clLElist), 0, label103);

  label104 = gtk_label_new ("label104");
  gtk_widget_ref (label104);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label104", label104,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label104);
  gtk_clist_set_column_widget (GTK_CLIST (clLElist), 1, label104);

  label105 = gtk_label_new ("label105");
  gtk_widget_ref (label105);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label105", label105,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label105);
  gtk_clist_set_column_widget (GTK_CLIST (clLElist), 2, label105);

  label106 = gtk_label_new ("label106");
  gtk_widget_ref (label106);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label106", label106,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label106);
  gtk_clist_set_column_widget (GTK_CLIST (clLElist), 3, label106);

  label107 = gtk_label_new ("label107");
  gtk_widget_ref (label107);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label107", label107,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label107);
  gtk_clist_set_column_widget (GTK_CLIST (clLElist), 4, label107);

  label108 = gtk_label_new ("label108");
  gtk_widget_ref (label108);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label108", label108,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label108);
  gtk_clist_set_column_widget (GTK_CLIST (clLElist), 5, label108);

  toolbar29 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar29);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "toolbar29", toolbar29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar29);
  gtk_box_pack_start (GTK_BOX (vbox26), toolbar29, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar29), GTK_RELIEF_NONE);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_UP);
  btnLEup = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button8",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnLEup);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEup", btnLEup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEup);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_DOWN);
  btnLEdown = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button9",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnLEdown);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEdown", btnLEdown,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEdown);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_BACK);
  btnLEleft = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button14",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnLEleft);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEleft", btnLEleft,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEleft);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_FORWARD);
  btnLEright = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar29),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button15",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnLEright);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEright", btnLEright,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEright);

  vbox27 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox27);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "vbox27", vbox27,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox27);
  gtk_box_pack_start (GTK_BOX (hbox22), vbox27, TRUE, TRUE, 0);

  frame22 = gtk_frame_new ("Lookup Verse");
  gtk_widget_ref (frame22);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "frame22", frame22,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame22);
  gtk_box_pack_start (GTK_BOX (vbox27), frame22, FALSE, TRUE, 0);

  vbox28 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox28);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "vbox28", vbox28,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox28);
  gtk_container_add (GTK_CONTAINER (frame22), vbox28);

  toolbar31 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar31);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "toolbar31", toolbar31,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar31);
  gtk_box_pack_start (GTK_BOX (vbox28), toolbar31, FALSE, FALSE, 0);
  gtk_toolbar_set_button_relief (GTK_TOOLBAR (toolbar31), GTK_RELIEF_NONE);

  entryVerseLookup = gtk_entry_new ();
  gtk_widget_ref (entryVerseLookup);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "entryVerseLookup", entryVerseLookup,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryVerseLookup);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar31), entryVerseLookup, NULL, NULL);
  gtk_widget_set_usize (entryVerseLookup, 248, -2);

  tmp_toolbar_icon = gnome_stock_pixmap_widget (ListEditor, GNOME_STOCK_PIXMAP_JUMP_TO);
  btnGotoVerse = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar31),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "button10",
                                NULL, NULL,
                                tmp_toolbar_icon, NULL, NULL);
  gtk_widget_ref (btnGotoVerse);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnGotoVerse", btnGotoVerse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnGotoVerse);

  toolbar32 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
  gtk_widget_ref (toolbar32);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "toolbar32", toolbar32,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar32);
  gtk_box_pack_start (GTK_BOX (vbox28), toolbar32, FALSE, FALSE, 0);

  combo1 = gtk_combo_new ();
  gtk_widget_ref (combo1);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "combo1", combo1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (combo1);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar32), combo1, NULL, NULL);
  gtk_widget_set_usize (combo1, 190, -2);

  cbLEBook = GTK_COMBO (combo1)->entry;
  gtk_widget_ref (cbLEBook);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "cbLEBook", cbLEBook,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (cbLEBook);

  spLEChapter_adj = gtk_adjustment_new (1, 0, 151, 1, 10, 10);
  spLEChapter = gtk_spin_button_new (GTK_ADJUSTMENT (spLEChapter_adj), 1, 0);
  gtk_widget_ref (spLEChapter);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "spLEChapter", spLEChapter,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (spLEChapter);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar32), spLEChapter, NULL, NULL);

  spLEVerse_adj = gtk_adjustment_new (1, 0, 178, 1, 10, 10);
  spLEVerse = gtk_spin_button_new (GTK_ADJUSTMENT (spLEVerse_adj), 1, 0);
  gtk_widget_ref (spLEVerse);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "spLEVerse", spLEVerse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (spLEVerse);
  gtk_toolbar_append_widget (GTK_TOOLBAR (toolbar32), spLEVerse, NULL, NULL);

  frame23 = gtk_frame_new ("List Items");
  gtk_widget_ref (frame23);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "frame23", frame23,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (frame23);
  gtk_box_pack_start (GTK_BOX (vbox27), frame23, FALSE, TRUE, 0);

  vbox29 = gtk_vbox_new (FALSE, 0);
  gtk_widget_ref (vbox29);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "vbox29", vbox29,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (vbox29);
  gtk_container_add (GTK_CONTAINER (frame23), vbox29);

  entryListItem = gtk_entry_new ();
  gtk_widget_ref (entryListItem);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "entryListItem", entryListItem,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryListItem);
  gtk_box_pack_start (GTK_BOX (vbox29), entryListItem, FALSE, FALSE, 0);

  toolbar33 = gtk_toolbar_new (GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
  gtk_widget_ref (toolbar33);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "toolbar33", toolbar33,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (toolbar33);
  gtk_box_pack_start (GTK_BOX (vbox29), toolbar33, FALSE, FALSE, 0);

  btnLEAddVerse = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar33),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Add Verse",
                                "Add verse in text window to list", NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (btnLEAddVerse);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEAddVerse", btnLEAddVerse,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEAddVerse);

  btnLEAddItem = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar33),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Add SubItem",
                                "Add new item and make is a subitem", NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (btnLEAddItem);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEAddItem", btnLEAddItem,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEAddItem);

  btnLEmakesub = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar33),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Make Subitem",
                                "Make selected item a subitem", NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (btnLEmakesub);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEmakesub", btnLEmakesub,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEmakesub);

  btnLEDelete = gtk_toolbar_append_element (GTK_TOOLBAR (toolbar33),
                                GTK_TOOLBAR_CHILD_BUTTON,
                                NULL,
                                "Delete",
                                "Delete selected item", NULL,
                                NULL, NULL, NULL);
  gtk_widget_ref (btnLEDelete);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEDelete", btnLEDelete,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEDelete);

  hbox23 = gtk_hbox_new (FALSE, 0);
  gtk_widget_ref (hbox23);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "hbox23", hbox23,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbox23);
  gtk_box_pack_start (GTK_BOX (vbox27), hbox23, TRUE, TRUE, 0);

  label109 = gtk_label_new ("type");
  gtk_widget_ref (label109);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label109", label109,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label109);
  gtk_box_pack_start (GTK_BOX (hbox23), label109, FALSE, FALSE, 0);
  gtk_widget_set_usize (label109, 30, -2);

  entryType = gtk_entry_new ();
  gtk_widget_ref (entryType);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "entryType", entryType,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryType);
  gtk_box_pack_start (GTK_BOX (hbox23), entryType, TRUE, TRUE, 0);
  gtk_widget_set_usize (entryType, 82, -2);

  label110 = gtk_label_new ("level");
  gtk_widget_ref (label110);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label110", label110,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label110);
  gtk_box_pack_start (GTK_BOX (hbox23), label110, FALSE, FALSE, 0);
  gtk_widget_set_usize (label110, 34, -2);

  entryLevel = gtk_entry_new ();
  gtk_widget_ref (entryLevel);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "entryLevel", entryLevel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryLevel);
  gtk_box_pack_start (GTK_BOX (hbox23), entryLevel, TRUE, TRUE, 0);
  gtk_widget_set_usize (entryLevel, 92, -2);

  table8 = gtk_table_new (3, 2, FALSE);
  gtk_widget_ref (table8);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "table8", table8,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (table8);
  gtk_box_pack_start (GTK_BOX (vbox27), table8, TRUE, TRUE, 0);

  label111 = gtk_label_new ("subitem");
  gtk_widget_ref (label111);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label111", label111,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label111);
  gtk_table_attach (GTK_TABLE (table8), label111, 0, 1, 1, 2,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  label112 = gtk_label_new ("menu");
  gtk_widget_ref (label112);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label112", label112,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label112);
  gtk_table_attach (GTK_TABLE (table8), label112, 0, 1, 2, 3,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);

  entrySubitem = gtk_entry_new ();
  gtk_widget_ref (entrySubitem);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "entrySubitem", entrySubitem,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entrySubitem);
  gtk_table_attach (GTK_TABLE (table8), entrySubitem, 1, 2, 1, 2,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  entryMenu = gtk_entry_new ();
  gtk_widget_ref (entryMenu);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "entryMenu", entryMenu,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryMenu);
  gtk_table_attach (GTK_TABLE (table8), entryMenu, 1, 2, 2, 3,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  label113 = gtk_label_new ("previous Item");
  gtk_widget_ref (label113);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "label113", label113,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (label113);
  gtk_table_attach (GTK_TABLE (table8), label113, 0, 1, 0, 1,
                    (GtkAttachOptions) (0),
                    (GtkAttachOptions) (0), 0, 0);
  gtk_widget_set_usize (label113, 66, -2);

  entryPreitem = gtk_entry_new ();
  gtk_widget_ref (entryPreitem);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "entryPreitem", entryPreitem,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (entryPreitem);
  gtk_table_attach (GTK_TABLE (table8), entryPreitem, 1, 2, 0, 1,
                    (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),
                    (GtkAttachOptions) (0), 0, 0);

  hbuttonbox1 = gtk_hbutton_box_new ();
  gtk_widget_ref (hbuttonbox1);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "hbuttonbox1", hbuttonbox1,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (hbuttonbox1);
  gtk_box_pack_start (GTK_BOX (vbox27), hbuttonbox1, TRUE, TRUE, 0);

  btnLEapplylistchanges = gnome_stock_button (GNOME_STOCK_BUTTON_APPLY);
  gtk_widget_ref (btnLEapplylistchanges);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEapplylistchanges", btnLEapplylistchanges,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEapplylistchanges);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnLEapplylistchanges);
  GTK_WIDGET_SET_FLAGS (btnLEapplylistchanges, GTK_CAN_DEFAULT);

  btnLEcancelistchanges = gnome_stock_button (GNOME_STOCK_BUTTON_CANCEL);
  gtk_widget_ref (btnLEcancelistchanges);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEcancelistchanges", btnLEcancelistchanges,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEcancelistchanges);
  gtk_container_add (GTK_CONTAINER (hbuttonbox1), btnLEcancelistchanges);
  GTK_WIDGET_SET_FLAGS (btnLEcancelistchanges, GTK_CAN_DEFAULT);

  scrolledwindow33 = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_ref (scrolledwindow33);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "scrolledwindow33", scrolledwindow33,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (scrolledwindow33);
  gtk_box_pack_start (GTK_BOX (dialog_vbox5), scrolledwindow33, TRUE, TRUE, 0);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow33), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);

  text7 = gtk_text_new (NULL, NULL);
  gtk_widget_ref (text7);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "text7", text7,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (text7);
  gtk_container_add (GTK_CONTAINER (scrolledwindow33), text7);

  dialog_action_area5 = GNOME_DIALOG (ListEditor)->action_area;
  gtk_object_set_data (GTK_OBJECT (ListEditor), "dialog_action_area5", dialog_action_area5);
  gtk_widget_show (dialog_action_area5);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area5), GTK_BUTTONBOX_END);
  gtk_button_box_set_spacing (GTK_BUTTON_BOX (dialog_action_area5), 8);

  gnome_dialog_append_button (GNOME_DIALOG (ListEditor), GNOME_STOCK_BUTTON_OK);
  btnLEok = g_list_last (GNOME_DIALOG (ListEditor)->buttons)->data;
  gtk_widget_ref (btnLEok);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEok", btnLEok,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEok);
  GTK_WIDGET_SET_FLAGS (btnLEok, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (ListEditor), GNOME_STOCK_BUTTON_APPLY);
  btnLEapply = g_list_last (GNOME_DIALOG (ListEditor)->buttons)->data;
  gtk_widget_ref (btnLEapply);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEapply", btnLEapply,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEapply);
  GTK_WIDGET_SET_FLAGS (btnLEapply, GTK_CAN_DEFAULT);

  gnome_dialog_append_button (GNOME_DIALOG (ListEditor), GNOME_STOCK_BUTTON_CANCEL);
  btnLEcancel = g_list_last (GNOME_DIALOG (ListEditor)->buttons)->data;
  gtk_widget_ref (btnLEcancel);
  gtk_object_set_data_full (GTK_OBJECT (ListEditor), "btnLEcancel", btnLEcancel,
                            (GtkDestroyNotify) gtk_widget_unref);
  gtk_widget_show (btnLEcancel);
  GTK_WIDGET_SET_FLAGS (btnLEcancel, GTK_CAN_DEFAULT);

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
  gtk_signal_connect (GTK_OBJECT (clLElist), "select_row",
                      GTK_SIGNAL_FUNC (on_clLElist_select_row),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEup), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEup_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEdown), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEdown_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEleft), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEleft_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEright), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEright_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnGotoVerse), "clicked",
                      GTK_SIGNAL_FUNC (on_btnGotoVerse_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (cbLEBook), "changed",
                      GTK_SIGNAL_FUNC (on_cbLEBook_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (spLEChapter), "changed",
                      GTK_SIGNAL_FUNC (on_spLEChapter_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (spLEVerse), "changed",
                      GTK_SIGNAL_FUNC (on_spLEVerse_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEAddVerse), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEAddVerse_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEAddItem), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEAddItem_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEmakesub), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEmakesub_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEDelete), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEDelete_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (entryPreitem), "changed",
                      GTK_SIGNAL_FUNC (on_entryPreitem_changed),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEapplylistchanges), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEapplylistchanges_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEcancelistchanges), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEcancelistchanges_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEok), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEok_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEapply), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEapply_clicked),
                      NULL);
  gtk_signal_connect (GTK_OBJECT (btnLEcancel), "clicked",
                      GTK_SIGNAL_FUNC (on_btnLEcancel_clicked),
                      NULL);

  return ListEditor;
}