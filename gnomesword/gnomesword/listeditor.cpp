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
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <gnome.h>
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

#include "GnomeSword.h"
#include "display.h"
#include "callback.h"
#include "interface.h"
#include "support.h"
#include "listeditor.h"
#include "filestuff.h"
#include "menustuff.h"



typedef struct _listrow LISTROW;
struct _listrow
{
 	gchar *item;
	gint 	type;
	gint  level;  	
};


SWDisplay *GBFlistDisplay; //-- to display modules in list editor
SWMgr *listMgr;  //-- sword mgr for ListEditor
SWModule *listMod;	//-- module for ListEditor
extern SWModule *curMod; //-- module for main text window (GnomeSword.cpp)
extern gint ibookmarks;
extern gchar *fnbookmarksnew; //-- bookmarks file name - declared and set in filesturr.cpp
extern gchar rememberlastitem[255];//-- we need to use this when add to bookmarks
extern GtkWidget *MainFrm; //------------- main form (app)
extern gchar remembersubtree[256];  //-- used for bookmark menus declared in filestuff.cpp

LISTITEM mylistitem;
LISTITEM *p_mylistitem;
gchar *remfirstsub;
gchar	*title;
bool	newsave = true;
gint 	currentrow;
static void  setbuttonson(GtkWidget *listeditor, bool choice);
static void  loadbookmarksnew(GtkWidget *list);
static gint  findpresubitem(GtkWidget *list, gint row, gint ilevel);
static gint  findnextsubitem(GtkWidget *list, gint row, gint ilevel);
static gint  findhowmany(GtkWidget * clist, gint row);
static void  remove_item_and_children (GtkWidget * clist, LISTROW *items, gint row);
static void  insert_items (GtkWidget * clist,LISTROW * items, gint howmany, gint row);
static void  insert_item (GtkCList * clist, LISTROW* item, gint row);
static void  getitem(gchar *buf[6]);

//-------------------------------------------------------------------------------------------
void
setbuttonson(GtkWidget *listeditor, bool choice)  //--------------------------
{
	GtkWidget *ok,
						*apply;
	
	ok = lookup_widget(listeditor,"btnLEok");
	apply = lookup_widget(listeditor,"btnLEapply");
	gtk_widget_set_sensitive (ok, choice);
	gtk_widget_set_sensitive (apply, choice);
}

//-------------------------------------------------------------------------------------------
void
editbookmarksSave(GtkWidget *editdlg)  //-------------------------- save bookmarks after edit
{
	GtkWidget *list;
	gint 	i,
				x;
	gint 	j=0;
	gchar *buf[6],
				*tmpbuf;
	gint 	flbookmarksnew;
	newsave = true;
	flbookmarksnew = open(fnbookmarksnew, O_WRONLY|O_TRUNC);	
	list = lookup_widget(editdlg,"clLElist");
	for(i=0;i<GTK_CLIST(list)->rows;i++)   //ibookmarks
	{
		getrow(list,i, buf);
		getitem(buf);
		strcpy(p_mylistitem->item,buf[0]);
		p_mylistitem->type = atoi(buf[1]);
		p_mylistitem->level = atoi(buf[2]);
		strcpy(p_mylistitem->subitem,buf[3]);
		strcpy(p_mylistitem->menu,"_Bookmarks/");
		strcpy(p_mylistitem->preitem,buf[5]);
		write(flbookmarksnew,(char *)&mylistitem,sizeof(mylistitem));
	}
	sprintf(rememberlastitem,"%s",p_mylistitem->item);
	close(flbookmarksnew);
	
}

//-------------------------------------------------------------------------------------------
void
getitem(gchar *buf[6])
{
	static gchar 	subs[6][80],
								tmpbuf[255],	
								*preitem = "",
								*presubitem = "Edit Bookmarks";
	static gint		oldlevel = 0;
	gint					level,
								type;							
	static bool		firsttime = true,
								firstsub  = true,
								first0item = true;
	gint					i;
	gchar					mybuf[80];
	
	if(newsave)
	{
	  for(i=0; i < 5; i++) sprintf(subs[i], "%s" ,"");
	  sprintf(tmpbuf, "%s", "");
		preitem = "";
		presubitem = "Edit Bookmarks";
		oldlevel = 0;
		firsttime = true;
		firstsub  = true;
		first0item = true;	
		newsave = false;
	}
	type = atoi(buf[1]);
	level = atoi(buf[2]);
	if(type == 1)
	{  		
		sprintf(subs[level],"%s/",buf[0]);
		sprintf(tmpbuf, "%s", subs[level]);
	  if(level == 0)
	  {
	  	buf[3] = g_strdup("");
	  	buf[5] = g_strdup(presubitem);
	  	preitem = g_strdup(buf[0]);
	  	presubitem = g_strdup(buf[0]);
	  	firstsub = false;
	  }
	  if(level > 0)
	  {
	  	sprintf(tmpbuf, "%s", "");
	  	for(i=0; i < level; i++)
	  	{
	  		strcat(tmpbuf,subs[i]);
	  	}
	  	buf[3] = g_strdup(tmpbuf);
	  	if(oldlevel == level)	buf[5] = g_strdup(preitem);
	  	else buf[5] = g_strdup("");
	  	preitem = g_strdup(buf[0]);
	  }
	  firsttime = true;
	}
	if(type == 0)							
	{
	 	if(level == 0)
	  {
	  	buf[3] = g_strdup("");
	  	if(first0item) buf[5] = g_strdup("<Separator>");
	  	else buf[5] = g_strdup(preitem);
	  	preitem = g_strdup(buf[0]);
	  	firsttime = false;
	  	first0item = false;
	  }
	  if(level > 0)
	  {
	  	sprintf(tmpbuf, "%s", "");
	  	for(i=0; i < level; i++)
	  	{
	  		strcat(tmpbuf,subs[i]);
	  	} 	  	
	  	buf[3] = g_strdup(tmpbuf);
	  	if(firsttime)	buf[5] = g_strdup("");
	  	else buf[5] = g_strdup(preitem);
	  	preitem = g_strdup(buf[0]);
	  	firsttime = false;
	  }
	}
	oldlevel = level;	
}

//-------------------------------------------------------------------------------------------
void                                                 //-- load bookmarks file into listeditor
loadbookmarksnew(GtkWidget *list)
{
	int flbookmarksnew; //-- file handle	
	gint i=0;           //-- counter
	gchar *buf[6],      //-- strings for clist
				tmpbuf[255];    //-- work buffer
	struct stat stat_p;
	long filesize;
	
	gtk_clist_clear(GTK_CLIST(list)); //-- clear the list widget
	stat (fnbookmarksnew, &stat_p);   //-- get file info
  filesize = stat_p.st_size;        //-- filesize in bytes
  ibookmarks = (filesize / (sizeof(mylistitem))); //-- find number of records in file (filesize / record size)
	flbookmarksnew = open(fnbookmarksnew, O_RDONLY);  //-- open bookmarks file
	while(i < ibookmarks) //-- loop until we have all the records
	{	
	  read(flbookmarksnew,(char *)&mylistitem,sizeof(mylistitem));//-- read in one record at a time 	
	 	p_mylistitem = &mylistitem; //-- set pointer to record	
	  buf[0] = g_strdup(p_mylistitem->item);
	  remfirstsub = g_strdup(p_mylistitem->item);
	  sprintf(tmpbuf,"%d", p_mylistitem->type); 	
	  buf[1] = g_strdup(tmpbuf);
	  sprintf(tmpbuf,"%d", p_mylistitem->level);
	  buf[2] = g_strdup(tmpbuf);
	  buf[3] = g_strdup(p_mylistitem->subitem);
	  buf[4] = g_strdup(p_mylistitem->menu);
	  buf[5] = g_strdup(p_mylistitem->preitem);	
	  	//if(buf[0][0] == '-') break;	  	
	  gtk_clist_insert(GTK_CLIST(list), i, buf); //-- insert item in editor list
	  gtk_clist_set_shift( GTK_CLIST(list), i, 0, 0, (p_mylistitem->level*10)); //-- show level by amount of shift
		++i;
	}
	sprintf(rememberlastitem,"%s",p_mylistitem->item);	
	close(flbookmarksnew);
}

//-------------------------------------------------------------------------------------------
void
addsubitme(GtkWidget *list, gint row)
{
  gchar *buf[1][6];
  ++row;
  buf[0][0] = g_strdup("");
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
deleteitem(GtkWidget *list)
{
   gtk_clist_remove(GTK_CLIST(list), currentrow); //-- remove current row
   setbuttonson(list,true);
}

//-------------------------------------------------------------------------------------------
void
addverse(GtkWidget *list, gint row, gchar *item)
{
  gchar 		*buf[6],
  					*tmpbuf;

	
  buf[0] = g_strdup(item);  //-- set item to verse

  buf[1] = g_strdup("0");     //-- set type to item (0)

  gtk_clist_get_text(GTK_CLIST(list), row, 2, &tmpbuf); //-- get level from previous item in list
  buf[2] = g_strdup(tmpbuf);

  gtk_clist_get_text(GTK_CLIST(list), row, 3, &tmpbuf); //-- get item from list
  buf[3] = g_strdup(tmpbuf);

  gtk_clist_get_text(GTK_CLIST(list), row, 4, &tmpbuf); //-- get item from list
  buf[4] = g_strdup(tmpbuf);

  gtk_clist_get_text(GTK_CLIST(list), row, 0, &tmpbuf); //-- get item from list
  buf[5] = g_strdup(tmpbuf);

  ++row;
  gtk_clist_insert(GTK_CLIST(list), row, buf); //-- insert item in new position
	++ibookmarks;
	if(row < ibookmarks)
	{
		++row;
		getrow(list,row, buf);
  	tmpbuf = g_strdup(item); //---------------------- previous item
  	if(buf[1][0] == 0 && buf[0][0] != '<') //-- change only if not a subitem and not following a <Separator>
  	{
  		gtk_clist_set_text(GTK_CLIST(list), row, 5,tmpbuf);
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
		
		title = g_strdup("GnomeSword - Bookmark Editor");
		p_mylistitem = &mylistitem;
		ListEditor = create_listeditor();
		text = lookup_widget(ListEditor,"text7");
		ModMap::iterator it;  //-- sword manager iterator
		gtk_text_set_word_wrap(GTK_TEXT (text), TRUE ); //-- set text window to word wrap
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
editbookmarks(GtkWidget *editdlg) //-- load bookmarks into an editor dialog
{
	GtkWidget *list;
	
	list = lookup_widget(editdlg,"clLElist");
	loadbookmarksnew(list);
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
applychanges(GtkWidget *widget)
{
  GtkWidget *listeditor,
  					*list;
  gchar			tmpbuf[255];
  					
  listeditor = lookup_widget(widget, "listeditor");
  list = lookup_widget(listeditor, "clLElist");
  editbookmarksSave(listeditor);  //-- save to file so we don't forget -- local

	sprintf(tmpbuf,"%s%s", "_Bookmarks/","What must I do to be saved?");
	removemenuitems(MainFrm, tmpbuf, ibookmarks); //-- remove old bookmarks form menu -- menustuff.cpp	
  //sprintf(tmpbuf,"%s%s", "_Bookmarks/",remembersubtree);
  addseparator(MainFrm, "_Bookmarks/Edit Bookmarks"); //-- add Separator it was deleted with old menus
  loadbookmarks_programstart(); //-- let's show what we did -- filestuff.cpp
  loadbookmarksnew(list);
  setbuttonson(listeditor,false);
}

//-----------------------------------------------------------------------------------------------
void
applylistchanges(GtkWidget *widget, gint row)
{
  GtkWidget *editor,
  					*entry,
  					*list;
  gchar			*buf[6];  //-- pointer to string storage
  gint			level;


  editor = gtk_widget_get_toplevel(widget); //-- set pointer to editor dialog
  list =  lookup_widget(editor,"clLElist");  //-- get item list
  getrow(list,row,buf);

  entry = lookup_widget(editor,"entryListItem");  //-- get item entry
  buf[0]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

  entry = lookup_widget(editor,"entryType");  //-- get type entry
  buf[1]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

  entry = lookup_widget(editor,"entryLevel");  //-- get level entry
  buf[2]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  level = atoi(buf[2]);
  /*
  entry = lookup_widget(editor,"entrySubitem");  //-- get subitem entry
  buf[3]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

  entry = lookup_widget(editor,"entryMenu");  //-- get menu entry
  buf[4]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

  entry = lookup_widget(editor,"entryPreitem");  //-- get preitem entry
  buf[5]= g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
  */
  gtk_clist_remove(GTK_CLIST(list), row); //-- remove item from current position

	gtk_clist_insert(GTK_CLIST(list), row, buf); //-- insert item in new position
	gtk_clist_set_shift( GTK_CLIST(list), row, 0, 0, (level*10)); //-- set indent
	gtk_clist_select_row(GTK_CLIST(list), row, 0); //-- seletct row (same item)
}
//-----------------------------------------------------------------------------------------------
void
selectrow(GtkWidget *list, gint row, gint column)
{
	GtkWidget *editor,
						*entry;
	gchar 		*buf,
						*submenu;	
	gint			level,
						type;	
	currentrow = row;	
	editor = gtk_widget_get_toplevel(list); //-- set pointer to editor dialog
	entry = lookup_widget(editor,"entryListItem");  //-- get item entry
	
	gtk_clist_get_text(GTK_CLIST(list), row, 0, &buf); //-- get item from list
	submenu = g_strdup(buf);
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryType");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 1, &buf); //-- get item from list
	//type = atoi(buf);
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryLevel");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row,2, &buf); //-- get item from list
	//level = (atoi(buf)+1);
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry 	
	/*
	entry = lookup_widget(editor,"entrySubitem");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 3, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryMenu");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 4, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	
	entry = lookup_widget(editor,"entryPreitem");  //-- get item entry
	gtk_clist_get_text(GTK_CLIST(list), row, 5, &buf); //-- get item from list
	gtk_entry_set_text(GTK_ENTRY(entry),buf);  	//-- put item into ListItem entry
	*/
}

//-----------------------------------------------------------------------------------------------
gint                 //-- for finding sub items up
findpresubitem(GtkWidget *list, gint row, gint ilevel)
{
  gint 	i,
  			level;
  gchar	*buf,
  			tmpbuf[80];

	for(i=row-1; i > 0; i--)
	{
		 gtk_clist_get_text(GTK_CLIST(list), i, 2, &buf);
		 level = atoi(buf);
		
		 if(level == ilevel) return(i);
	}
	return(0);
}

//-----------------------------------------------------------------------------------------------
gint                 //-- for finding sub items up
findnextsubitem(GtkWidget *list, gint row, gint ilevel)
{
  gint 	i,
  			level,
  			howmany;
  gchar	*buf,
  			tmpbuf[80];

	for(i=row+1; i < GTK_CLIST(list)->rows; i++)
	{
		gtk_clist_get_text(GTK_CLIST(list), i, 2, &buf);
	 	level = atoi(buf);
		
		if(level == ilevel)
		{
			howmany = findhowmany(list, i); 			
			return(i+howmany);
		}	
	}
	return(0);
}

//-----------------------------------------------------------------------------------------------
void                   //-- for moving list items up and down - and adding and removing tabs(changing level of item)
movelistitem(GtkWidget *widget, gint direction, gint listrow)
{
	GtkWidget *editor, //-- pointer to list editor
						*list;						
	gchar				*buf[6],  //-- pointer to string storage
						*prebuf[6], //-- item before
						*nextbuf[6],//-- item after
						*tmpbuf, //-- pointer to string storage
						listbuf[255], //-- string storage
						newbuf[255],  //-- string storage
						mybuf[80];
	gint 				ilevel, //-- for level item
						itype,
						prelevel,
						nextlevel,
						pretype,
						nexttype,
						newrow,
						i;	  //-- for counting
	LISTROW 	*items;
//	GList 		*items;
	
	editor = gtk_widget_get_toplevel(widget); //-- set pointer to editor dialog
	list = lookup_widget(widget,"clLElist");
	getrow(list,listrow, buf); //---- read items from list so we can move them or show changes 	
	ilevel = atoi(buf[2]);	//-- we need to know level so we can add tabs	(increment and deincrement level)
	itype = atoi(buf[1]);
	if(listrow > 0)
	{
		getrow(list,listrow-1, prebuf);
		prelevel = atoi(prebuf[2]);
		pretype = atoi(prebuf[1]);
	}
	if(listrow < GTK_CLIST(list)->rows) //-- make sure there is a row for us to access
	{
	  getrow(list,listrow+1, nextbuf);
		nextlevel = atoi(nextbuf[2]);
		nexttype = atoi(nextbuf[1]);
	}
	switch(direction)  //-- make sure that we want to do something
	{
		case	0:    //-- up
								if(listrow >= 1)  //-- don't do anything if we are in first row and press up button 	
								{
		            	if(listrow == 1 && (atoi(buf[1]) == 0)) return; //-- if item is not a sumitem (submenu) do nothing
		            	
									/*
									if(listrow == 1 && (atoi(buf[1]) == 1))  //-- if secound item is a sumitem (submenu)
									{
									   buf[2] = g_strdup("0");
									   buf[3] = g_strdup("");
									   buf[5] = g_strdup("Edit Bookmarks");
									   tmpbuf = g_strdup(buf[0]);
									   gtk_clist_set_text(GTK_CLIST(list), 0, 5,tmpbuf);
									   --listrow; //-- subtract one form our position
									   break; 									
									} */
									if(atoi(buf[1]) == 1) //-- we are moving a sub item
									{
										gint 	lrow,
													howmany;
										
										lrow = listrow;																			
									  newrow = findpresubitem(list, lrow, ilevel);
									
									  howmany = findhowmany(list, lrow);
									 	items = new LISTROW[howmany];
									 	remove_item_and_children(list, items, lrow);
									  //sprintf(mybuf,"%d\n",howmany);
		 								//cout << mybuf;	
									 	insert_items(list, items, howmany, newrow);
									 	delete items;
									  return;
									}
									else       //-- we are moving an item
									{
										if(atoi(prebuf[1]) == 0) //-- if we jumped an item
									 	{
									   	buf[2] = g_strdup(prebuf[2]);
									   	buf[3] = g_strdup(prebuf[3]);
									   	buf[5] = g_strdup(prebuf[5]);
									   	tmpbuf = g_strdup(buf[0]);
									   	gtk_clist_set_text(GTK_CLIST(list), listrow-1, 5,tmpbuf);
									  }
									  else   //-- if we jumped a sub item
									  {
									  	getrow(list,listrow-2, prebuf);
									  	if(atoi(prebuf[1]) == 0) //-- if row we will follow is an item
									  	{
									  		buf[2] = g_strdup(prebuf[2]);
									   		buf[3] = g_strdup(prebuf[3]);
									   		buf[5] = g_strdup(prebuf[0]);	
									   								   		
									   	}
									   	else   //-- if row we will follow is a sub item
									   	{
									   		sprintf(buf[2],"%d",(atoi(prebuf[2])+1)); //-- move one to the right of the sub item
									   		buf[3] = g_strdup(prebuf[0]);
									   		buf[5] = g_strdup("");			
									   	}
									   	getcolumn(list, listrow, 1, tmpbuf);
									   	if(atoi(tmpbuf) == 0) //-- what we left behind was an item not a sub item
									   	{
									   		tmpbuf = g_strdup("");
									   		gtk_clist_set_text(GTK_CLIST(list), listrow, 5,tmpbuf); //-- we have already removed our row so listrom now points to the row we left behind
									   	}			
									  }
									}
									gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position
									--listrow; //-- subtract one form our position
								}
								else return;
								break;  	
						case  1:    //-- down
		 						if(listrow < GTK_CLIST(list)->rows) //-- don't do anything if we are in last row and press down button
								{
									if(atoi(buf[1]) == 1) //-- we are moving a sub item
									{
										gint 	lrow,
													howmany;
										
										lrow = listrow;																			
									  newrow = findnextsubitem(list, lrow, ilevel);
									
									  howmany = findhowmany(list, lrow);
									  newrow = newrow - howmany;
									 	items = new LISTROW[howmany];
									 	remove_item_and_children(list, items, lrow);
									  sprintf(mybuf,"howmany = %d\n newrow = %d\n",howmany,newrow);
		 								cout << mybuf;	
									 	insert_items(list, items, howmany, newrow);
									 	delete items;
									  return;
									}
									if(atoi(buf[1]) == 0)
									{
										gint l;
										
										if(atoi(buf[2])!= atoi(nextbuf[2])) return; //-- do not move item to a new level
									}
									gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position
									++listrow; //-- add one to listrow 																		
								}
								else return;
								break;
		
						case  2:	if(ilevel > 0)
									{
										--ilevel;
										sprintf(buf[2],"%d",ilevel);
										gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position								
									}
									else return;								
									break;
		
						case  3:	if(itype == 1)
									{
										++ilevel;
										sprintf(buf[2],"%d",ilevel);
										gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position									
									}
									else if(itype == 0)
									{										
										if(pretype == 1 && prelevel >= ilevel) //-- if the previous item is a submenu and it's level equal to or grater than our level
										{
											++ilevel;
											sprintf(buf[2],"%d",ilevel);
											gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position								
										}
										else if(pretype == 0 && prelevel > ilevel)
										{
											++ilevel;
											sprintf(buf[2],"%d",ilevel);
											gtk_clist_remove(GTK_CLIST(list), listrow); //-- remove item from current position								
										}
										else return;
									}
									else return;										
		 						   break;  
		
					default:	    return;
	} 	
	setbuttonson(editor, true);	
	gtk_clist_insert(GTK_CLIST(list), listrow, buf); //-- insert item in new position
	gtk_clist_set_shift( GTK_CLIST(list), listrow, 0, 0, (ilevel*10)); //-- show level by shifting
	gtk_clist_select_row(GTK_CLIST(list), listrow, 0); //-- seletct row (same item)
}

//-----------------------------------------------------------------------------------------------
void
getrow(GtkWidget *list, gint listrow, gchar *buf[6])
{
	gchar *tmpbuf;
	//---- read items from list so we can move them or show changes
	gtk_clist_get_text(GTK_CLIST(list), listrow, 0, &tmpbuf); //-- get item from list
	buf[0] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 1, &tmpbuf); //-- get type from list
	buf[1] = g_strdup(tmpbuf);		
	gtk_clist_get_text(GTK_CLIST(list), listrow, 2, &tmpbuf); //-- get level from list
	buf[2] = g_strdup(tmpbuf);	
	gtk_clist_get_text(GTK_CLIST(list), listrow, 3, &tmpbuf); //-- get subitem from list
	buf[3] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 4, &tmpbuf); //-- get menu from list
	buf[4] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 5, &tmpbuf); //-- get extra item from list
	buf[5] = g_strdup(tmpbuf);
}

//-----------------------------------------------------------------------------------------------
void
getcolumn(GtkWidget *list, gint row, gint column, gchar *buf)
{
	//---- read item from list
	gtk_clist_get_text(GTK_CLIST(list), row, column, &buf); //-- get item from list  	
}

//--------------------------------------------------------------------------------------------
//--
gint
findhowmany(GtkWidget * clist, gint row)
{
  gchar 	*buf[6];
  gint 		level,
  				type;
  gchar tmpbuf[80];
  gint i=1;

  getrow(clist,row,buf);
  level = atoi(buf[2]);
  type =  atoi(buf[1]);
  ++row;
  while (row < GTK_CLIST (clist)->rows)
  {
      getrow(clist,row,buf);
  		if (atoi(buf[2]) > level)
			{
				++row;
				++i;			  		
			}
      else break;
  }
  //delete item;
  return i;
}

//--------------------------------------------------------------------------------------------
//-- This removes an item and its children from the clist, and returns a list of the removed items. (code from glade_menu_editor.c)
void
remove_item_and_children (GtkWidget * clist, LISTROW *items, gint row)
{
  gchar 	*buf[6];
  gint 		level,
  				type;
  gchar tmpbuf[80];
  gint i=0;

  getrow(clist,row,buf);
  level = atoi(buf[2]);
  type =  atoi(buf[1]);
  items[i].item = g_strdup(buf[0]);
  items[i].type = type;
  items[i].level = level;
  gtk_clist_remove (GTK_CLIST (clist), row);

  while (row < GTK_CLIST (clist)->rows)
  {
      getrow(clist,row,buf);
      ++i;
  		if (atoi(buf[2]) > level)
			{
			  items[i].item = g_strdup(buf[0]);
  			items[i].type = atoi(buf[1]);
  			items[i].level = atoi(buf[2]);
	  		gtk_clist_remove (GTK_CLIST (clist), row);	  		
			}
      else break;
  }
  //delete item;
  return;
}

//--------------------------------------------------------------------------------------------
//-- This inserts the given list of items at the given position in the clist. (code from glade_menu_editor.c)
static void
insert_items (GtkWidget * clist, LISTROW *items, gint howmany, gint row)
{
  LISTROW *item;
  gint i=0;

  while (i < howmany)
    {
    	item = &items[i];
      insert_item (GTK_CLIST (clist), item, row++);
      ++i;
    }
}


/* This adds the item to the clist at the given position. */
static void
insert_item (GtkCList * clist, LISTROW *item, gint row)
{
  gchar *buf[6],
  			tmpbuf[80];
  gint	level;
  //LISTROW *item;

 // item = &rowitem;
  /* Empty labels are understood to be separators. */

  buf[0] = g_strdup(item->item);
  sprintf(tmpbuf, "%d", item->type);
  buf[1] = g_strdup(tmpbuf);
  sprintf(tmpbuf, "%d", item->level);
  buf[2] = g_strdup(tmpbuf);
  level = item->level;
  buf[3] = g_strdup("");
  buf[4] = g_strdup("");
  buf[5] = g_strdup("");
  if (row >= 0)
    gtk_clist_insert (GTK_CLIST (clist), row, buf);
  else
    row = gtk_clist_append (GTK_CLIST (clist), buf);

  //gtk_clist_set_row_data (GTK_CLIST (clist), row, item);
  gtk_clist_set_shift (GTK_CLIST (clist), row, 0, 0,(level * 10));
}
