/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          listeditor.c  -  description
                             -------------------
    begin                : Wed Jun 21 2000
    copyright            : (C) 2000 by Terry Biggs
    email                : tbiggs@infinet.com
 ***************************************************************************/

/***********************************************************************************
 *                                                                                 *
 *   This program is free software; you can redistribute it and/or modify      *
 *   it under the terms of the GNU General Public License as published by *
 *   the Free Software Foundation; either version 2 of the License, or        *
 *   (at your option) any later version.                                           *
 *                                                                                 *
 ***********************************************************************************/

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <gnome.h>

#include "gs_gnomesword.h"
#include "gs_sword.h"
#include "callback.h"
#include "interface.h"
#include "support.h"
#include "gs_listeditor.h"
#include "gs_file.h"
#include "gs_menu.h"


/*****************************************************************************
 *globals
*****************************************************************************/
typedef struct _listrow LISTROW;
struct _listrow {
	gchar *item;
	gint type;
	gint level;
};
LISTITEM mylistitem;
LISTITEM *p_mylistitem;
LISTITEM listitem;		/* structure for ListEditor items
				   (verse lists and bookmarks) */
gint listrow;			/* current row in ListEditor clist widget */
gchar *remfirstsub;
gchar *title;
gboolean newsave = TRUE;
gint currentrow;

/*****************************************************************************
 *externals
*****************************************************************************/
extern gint ibookmarks;
extern gchar *fnbookmarksnew;	/* bookmarks file name - declared and set in filesturr.cpp */
extern gchar rememberlastitem[255];	/* we need to use this when add to bookmarks */
extern GtkWidget *MainFrm;	/*----------- main form (app) */
extern gchar remembersubtree[256];	/* used for bookmark menus declared in filestuff.cpp */
extern GtkWidget *listeditor;	/* pointer to ListEditor */

/*****************************************************************************
 *internal functions - visable to this file only
*****************************************************************************/
static void setbuttonson(GtkWidget * listeditor, gboolean choice);
static void loadbookmarksnew(GtkWidget * list);
static gint findpresubitem(GtkWidget * list, gint row, gint ilevel);
static gint findnextsubitem(GtkWidget * list, gint row, gint ilevel);
static gint findhowmany(GtkWidget * clist, gint row);
static void remove_item_and_children(GtkWidget * clist, LISTROW * items,
				     gint row);
static void insert_items(GtkWidget * clist, LISTROW * items, gint howmany,
			 gint row);
static void insert_item(GtkCList * clist, LISTROW * item, gint row);
static void getitem(gchar * buf[6]);



/*********************************************************************************
**********************************************************************************
 *callbacks liseditor
**********************************************************************************
*********************************************************************************/

void
on_clLElist_select_row(GtkCList * clist,
		       gint row,
		       gint column, GdkEvent * event, gpointer user_data)
{
	listrow = row;
	selectrow(GTK_WIDGET(clist), listrow, column);
}


void on_btnLEup_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 0, listrow);	/* send to movelistitem function for processing (listeditor.cpp) */
}


void on_btnLEdown_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 1, listrow);	/* send to movelistitem function for processing (listeditor.cpp) */
}


void on_btnLEleft_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 2, listrow);	/* send to movelistitem function for processing (listeditor.cpp) */
}


void on_btnLEright_clicked(GtkButton * button, gpointer user_data)
{
	movelistitem(GTK_WIDGET(button), 3, listrow);	/* send to movelistitem function for processing (listeditor.cpp) */
}


void on_btnLEgotoverse_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *entry;
	gchar *buf;
	entry = lookup_widget(GTK_WIDGET(button), "entryVerseLookup");
	buf = gtk_entry_get_text(GTK_ENTRY(entry));
	changeLEverseSWORD(buf);
}


void on_btnLEAddVerse_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list, *entry;
	gchar *item;

	list = lookup_widget(GTK_WIDGET(button), "clLElist");
	entry = lookup_widget(GTK_WIDGET(button), "entryVerseLookup");
	item = gtk_entry_get_text(GTK_ENTRY(entry));
	addverse(list, listrow, item);	/* function to add item to list (listeditor.cpp) */
}


void on_btnLEAddItem_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clLElist");
	addsubitme(list, listrow);
}


void on_btnLEok_clicked(GtkButton * button, gpointer user_data)
{
	applychanges(GTK_WIDGET(button));
	gtk_widget_hide(listeditor);
}


void on_btnLEapply_clicked(GtkButton * button, gpointer user_data)
{
	applychanges(GTK_WIDGET(button));
}


void on_btnLEcancel_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_hide(listeditor);
}


void
on_btnLEapplylistchanges_clicked(GtkButton * button, gpointer user_data)
{
	applylistchanges(GTK_WIDGET(button), listrow);
}


void on_btnLEdelete_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *list;

	list = lookup_widget(GTK_WIDGET(button), "clLElist");
	deleteitem(list);
}


void on_btnLEnew_clicked(GtkButton * button, gpointer user_data)
{

}


void on_btnLEopen_clicked(GtkButton * button, gpointer user_data)
{

}


void on_btnLEclose_clicked(GtkButton * button, gpointer user_data)
{

}


void on_btnLErefresh_clicked(GtkButton * button, gpointer user_data)
{

}


void on_btnLEsave_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *listeditor;

	listeditor = gtk_widget_get_toplevel(GTK_WIDGET(button));
	editbookmarksSave(listeditor);
}


/*****************************************************************************
 *****************************************************************************
 *listeditor functions
 *****************************************************************************
*****************************************************************************/

/******************************************************************************
 * set ok and apply button sensitivity
 *****************************************************************************/
void setbuttonson(GtkWidget * listeditor, gboolean choice)
{
	GtkWidget *ok, *apply;

	ok = lookup_widget(listeditor, "btnLEok");
	apply = lookup_widget(listeditor, "btnLEapply");
	gtk_widget_set_sensitive(ok, choice);
	gtk_widget_set_sensitive(apply, choice);
}

/******************************************************************************
 * save bookmarks after edit
 *****************************************************************************/
void editbookmarksSave(GtkWidget * editdlg)
{
	GtkWidget *list;
	gint i;
	gchar *buf[6];
	gint flbookmarksnew;
	newsave = TRUE;
	flbookmarksnew = open(fnbookmarksnew, O_WRONLY | O_TRUNC);
	list = lookup_widget(editdlg, "clLElist");
	for (i = 0; i < GTK_CLIST(list)->rows; i++) {	/* ibookmarks */
		getrow(list, i, buf);
		getitem(buf);
		strcpy(p_mylistitem->item, buf[0]);
		p_mylistitem->type = atoi(buf[1]);
		p_mylistitem->level = atoi(buf[2]);
		strcpy(p_mylistitem->subitem, buf[3]);
		strcpy(p_mylistitem->menu, "_Bookmarks/");
		strcpy(p_mylistitem->preitem, buf[5]);
		write(flbookmarksnew, (char *) &mylistitem,
		      sizeof(mylistitem));
	}
	sprintf(rememberlastitem, "%s", p_mylistitem->item);
	close(flbookmarksnew);

}

/******************************************************************************
 * getitem
 *****************************************************************************/
void getitem(gchar * buf[6])
{
	static gchar subs[6][80],
	    tmpbuf[255], *preitem = "", *presubitem = "Edit Bookmarks";
	static gint oldlevel = 0;
	gint level, type;
	static gboolean firsttime = TRUE, firstsub = TRUE, first0item =
	    TRUE;
	gint i;
	// gchar mybuf[80];

	if (newsave) {
		for (i = 0; i < 5; i++)
			sprintf(subs[i], "%s", "");
		sprintf(tmpbuf, "%s", "");
		preitem = "";
		presubitem = "Edit Bookmarks";
		oldlevel = 0;
		firsttime = TRUE;
		firstsub = TRUE;
		first0item = TRUE;
		newsave = FALSE;
	}
	type = atoi(buf[1]);
	level = atoi(buf[2]);
	if (type == 1) {
		sprintf(subs[level], "%s/", buf[0]);
		sprintf(tmpbuf, "%s", subs[level]);
		if (level == 0) {
			buf[3] = "";
			buf[5] = g_strdup(presubitem);
			preitem = g_strdup(buf[0]);
			presubitem = g_strdup(buf[0]);
			firstsub = FALSE;
		}
		if (level > 0) {
			sprintf(tmpbuf, "%s", "");
			for (i = 0; i < level; i++) {
				strcat(tmpbuf, subs[i]);
			}
			buf[3] = g_strdup(tmpbuf);
			if (oldlevel == level)
				buf[5] = g_strdup(preitem);
			else
				buf[5] = g_strdup("");
			preitem = g_strdup(buf[0]);
		}
		firsttime = TRUE;
	}
	if (type == 0) {
		if (level == 0) {
			buf[3] = g_strdup("");
			if (first0item)
				buf[5] = g_strdup("<Separator>");
			else
				buf[5] = g_strdup(preitem);
			preitem = g_strdup(buf[0]);
			firsttime = FALSE;
			first0item = FALSE;
		}
		if (level > 0) {
			sprintf(tmpbuf, "%s", "");
			for (i = 0; i < level; i++) {
				strcat(tmpbuf, subs[i]);
			}
			buf[3] = g_strdup(tmpbuf);
			if (firsttime)
				buf[5] = g_strdup("");
			else
				buf[5] = g_strdup(preitem);
			preitem = g_strdup(buf[0]);
			firsttime = FALSE;
		}
	}
	oldlevel = level;
}

/******************************************************************************
 * load bookmarks file into listeditor
 *****************************************************************************/
void loadbookmarksnew(GtkWidget * list)
{
	int flbookmarksnew;	/* file handle */
	gint i = 0, j;		/* counter */
	gchar *buf[6],		/* strings for clist */
	 tmpbuf[255];		/* work buffer */
	struct stat stat_p;
	long filesize;

	gtk_clist_clear(GTK_CLIST(list));	/* clear the list widget */
	stat(fnbookmarksnew, &stat_p);	/* get file info */
	filesize = stat_p.st_size;	/* filesize in bytes */
	/* find number of records in file (filesize / record size) */
	ibookmarks = (filesize / (sizeof(mylistitem)));
	flbookmarksnew = open(fnbookmarksnew, O_RDONLY);	/* open bookmarks file */
	while (i < ibookmarks) {	/* loop until we have all the records */
		/* read in one record at a time */
		read(flbookmarksnew, (char *) &mylistitem,
		     sizeof(mylistitem));
		p_mylistitem = &mylistitem;	/* set pointer to record */
		buf[0] = g_strdup(p_mylistitem->item);
		remfirstsub = g_strdup(p_mylistitem->item);
		sprintf(tmpbuf, "%d", p_mylistitem->type);
		buf[1] = g_strdup(tmpbuf);
		sprintf(tmpbuf, "%d", p_mylistitem->level);
		buf[2] = g_strdup(tmpbuf);
		buf[3] = g_strdup(p_mylistitem->subitem);
		buf[4] = g_strdup(p_mylistitem->menu);
		buf[5] = g_strdup(p_mylistitem->preitem);
		/* insert item in editor list */
		gtk_clist_insert(GTK_CLIST(list), i, buf);
		/* show level by amount of shift */
		gtk_clist_set_shift(GTK_CLIST(list), i, 0, 0,
				    (p_mylistitem->level * 10));
		++i;
		for (j = 0; j < 6; j++)
			g_free(buf[j]);
	}
	sprintf(rememberlastitem, "%s", p_mylistitem->item);
	close(flbookmarksnew);
}

/******************************************************************************
 * add subitme to list
 *****************************************************************************/
void addsubitme(GtkWidget * list, gint row)
{
	gchar *buf[6];
	gint j;

	++row;
	buf[0] = g_strdup("");
	buf[1] = g_strdup("1");
	buf[2] = g_strdup("0");
	buf[3] = g_strdup(" ");
	buf[4] = g_strdup(" ");
	buf[5] = g_strdup(" ");
	gtk_clist_insert(GTK_CLIST(list), row, buf);	/* insert item in new position */
	gtk_clist_select_row(GTK_CLIST(list), row, 0);	/* seletct row (new item) */
	++ibookmarks;
	for (j = 0; j < 6; j++)
		g_free(buf[j]);
}

/******************************************************************************
 * delete an item from the list
 *****************************************************************************/
void deleteitem(GtkWidget * list)
{
	gtk_clist_remove(GTK_CLIST(list), currentrow);	/* remove current row */
	setbuttonson(list, TRUE);
}

/******************************************************************************
 * add a verse reference to the list
 *****************************************************************************/
void addverse(GtkWidget * list, gint row, gchar * item)
{
	gchar *buf[6], *tmpbuf;
	gint j;

	buf[0] = g_strdup(item);	/* set item to verse */
	buf[1] = g_strdup("0");	/* set type to item (0) */
	/* get level from previous item in list */
	gtk_clist_get_text(GTK_CLIST(list), row, 2, &tmpbuf);
	buf[2] = g_strdup(tmpbuf);
	/* get item from list */
	gtk_clist_get_text(GTK_CLIST(list), row, 3, &tmpbuf);
	buf[3] = g_strdup(tmpbuf);
	/* get item from list */
	gtk_clist_get_text(GTK_CLIST(list), row, 4, &tmpbuf);
	buf[4] = g_strdup(tmpbuf);
	/* get item from list */
	gtk_clist_get_text(GTK_CLIST(list), row, 0, &tmpbuf);
	buf[5] = g_strdup(tmpbuf);
	/* insert item in new position */
	++row;
	gtk_clist_insert(GTK_CLIST(list), row, buf);
	++ibookmarks;
	if (row < ibookmarks) {
		++row;
		getrow(list, row, buf);
		tmpbuf = g_strdup(item);	/* previous item */
		/* change only if not a subitem and not following a <Separator> */
		if (buf[1][0] == 0 && buf[0][0] != '<') {
			gtk_clist_set_text(GTK_CLIST(list), row, 5,
					   tmpbuf);
		}
		for (j = 0; j < 6; j++)
			g_free(buf[j]);
	}
	gtk_clist_select_row(GTK_CLIST(list), row - 1, 0);	/* seletct row (new item)  */
}


/******************************************************************************
 *create and init ListEditor
 *****************************************************************************/
GtkWidget *createListEditor(void)
{
	GtkWidget *ListEditor, *text;

	title = "GnomeSword - Bookmark Editor";
	p_mylistitem = &mylistitem;
	ListEditor = create_listeditor();
	text = lookup_widget(ListEditor, "text7");
	/* set text window to word wrap */
	gtk_text_set_word_wrap(GTK_TEXT(text), TRUE);
	/* init listmodule */
	setuplisteditSWORD(text);
	gtk_signal_connect(GTK_OBJECT(ListEditor), "destroy",
			   GTK_SIGNAL_FUNC(on_listeditor_destroy), NULL);
	return (ListEditor);
}

/******************************************************************************
 *load bookmarks into an editor dialog
 *****************************************************************************/
void editbookmarks(GtkWidget * editdlg)
{
	GtkWidget *list;

	list = lookup_widget(editdlg, "clLElist");
	loadbookmarksnew(list);
}

/******************************************************************************
 *up date bookmark menu with our changes
 *****************************************************************************/
void applychanges(GtkWidget * widget)
{
	GtkWidget *listeditor, *list;
	gchar tmpbuf[255];
	gchar *buf[6];

	listeditor = lookup_widget(widget, "listeditor");
	list = lookup_widget(listeditor, "clLElist");
	/* save to file so we don't forget -- local */
	editbookmarksSave(listeditor);
	getrow(list, 0, buf);
	getitem(buf);
	sprintf(tmpbuf, "%s%s", "_Bookmarks/", buf[0]);
	/* remove old bookmarks form menu -- menustuff.cpp */
	removemenuitems(MainFrm, tmpbuf, ibookmarks);
	/* add Separator it was deleted with old menus */
	addseparator(MainFrm, "_Bookmarks/Edit Bookmarks");
	/* let's show what we did -- filestuff.cpp */
	loadbookmarks_programstart();
	loadbookmarksnew(list);
	setbuttonson(listeditor, FALSE);
}

/******************************************************************************
 *up date clist with info from form
 *****************************************************************************/
void applylistchanges(GtkWidget * widget, gint row)
{
	GtkWidget *editor, *entry, *list;
	gchar *buf[6];		/* pointer to string storage */
	gint j, level;


	editor = gtk_widget_get_toplevel(widget);	/* set pointer to editor dialog */
	list = lookup_widget(editor, "clLElist");	/* get item list */
	getrow(list, row, buf);

	entry = lookup_widget(editor, "entryListItem");	/* get item entry */
	buf[0] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

	entry = lookup_widget(editor, "entryType");	/* get type entry */
	buf[1] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));

	entry = lookup_widget(editor, "entryLevel");	/* get level entry */
	buf[2] = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
	level = atoi(buf[2]);

	gtk_clist_remove(GTK_CLIST(list), row);	/* remove item from current position */
	gtk_clist_insert(GTK_CLIST(list), row, buf);	/* insert item in new position */
	gtk_clist_set_shift(GTK_CLIST(list), row, 0, 0, (level * 10));	/* set indent */
	gtk_clist_select_row(GTK_CLIST(list), row, 0);	/* seletct row (same item) */
	/* free buf */
	for (j = 0; j < 3; j++)
		g_free(buf[j]);
}

/******************************************************************************
 *for getting info from selected row and displaying in form
 *****************************************************************************/
void selectrow(GtkWidget * list, gint row, gint column)
{
	GtkWidget *editor, *entry;
	gchar *buf;
	currentrow = row;
	editor = gtk_widget_get_toplevel(list);	/* set pointer to editor dialog */

	entry = lookup_widget(editor, "entryListItem");	/* get item entry */
	gtk_clist_get_text(GTK_CLIST(list), row, 0, &buf);	/* get item from list */
	gtk_entry_set_text(GTK_ENTRY(entry), buf);	/* put item into ListItem entry */

	entry = lookup_widget(editor, "entryType");	/* get item entry */
	gtk_clist_get_text(GTK_CLIST(list), row, 1, &buf);	/* get item from list */
	gtk_entry_set_text(GTK_ENTRY(entry), buf);	/* put item into ListItem entry */

	entry = lookup_widget(editor, "entryLevel");	/* get item entry */
	gtk_clist_get_text(GTK_CLIST(list), row, 2, &buf);	/* get item from list */
	gtk_entry_set_text(GTK_ENTRY(entry), buf);	/* put item into ListItem entry */
}

/******************************************************************************
 *for finding sub items above
 *****************************************************************************/
gint findpresubitem(GtkWidget * list, gint row, gint ilevel)
{
	gint i, level;
	gchar *buf;

	for (i = row - 1; i > 0; i--) {
		gtk_clist_get_text(GTK_CLIST(list), i, 2, &buf);
		level = atoi(buf);

		if (level == ilevel)
			return (i);
	}
	return (0);
}

/******************************************************************************
 *for finding sub items below 
 *****************************************************************************/
gint findnextsubitem(GtkWidget * list, gint row, gint ilevel)
{
	gint i, level, howmany;
	gchar *buf;

	for (i = row + 1; i < GTK_CLIST(list)->rows; i++) {
		gtk_clist_get_text(GTK_CLIST(list), i, 2, &buf);
		level = atoi(buf);

		if (level == ilevel) {
			howmany = findhowmany(list, i);
			return (i + howmany);
		}
	}
	return (0);
}

/****************************************************************************** 
 *for moving list items up and down  
 *and adding and removing tabs(changing level of item) 
 *****************************************************************************/
void movelistitem(GtkWidget * widget, gint direction, gint listrow)
{
	GtkWidget *editor,	/* pointer to list editor */
	*list;
	gchar *buf[6],		/* pointer to string storage */
	*prebuf[6],		/* item before */
	*nextbuf[6],		/* item after */
	*tmpbuf = "\0",		/* pointer to string storage */
	 mybuf[80];
	gint ilevel = 0,	/* for level item */
	 itype = 0,
	    prelevel = 0, nextlevel = 0, pretype = 0, nexttype =
	    0, newrow = 0; /* for counting */
	gint    lastrow;	
	
	LISTROW *items;
	/* set pointer to editor dialog */
	editor = gtk_widget_get_toplevel(widget);
	/* set list pointer to clLElist (clist widget ) */
	list = lookup_widget(widget, "clLElist");
	lastrow =  (GTK_CLIST(list)->rows - 1);
	/*  read items from list so we can move them or show changes */
	getrow(list, listrow, buf);
	/* we need to know level so we can add tabs
	   (increment and deincrement level) */
	ilevel = atoi(buf[2]);
	itype = atoi(buf[1]);
	/* make sure there is a row for us to access */
	if (listrow > 0) {
		/* get previous item (item before the one we want to change) */
		getrow(list, listrow - 1, prebuf);
		prelevel = atoi(prebuf[2]);
		pretype = atoi(prebuf[1]);
	}
	/* make sure there is a row for us to access */	
	if (listrow < lastrow) {
		/* get next item (item after the one we want to change) */
		getrow(list, listrow + 1, nextbuf);
		nextlevel = atoi(nextbuf[2]);
		nexttype = atoi(nextbuf[1]);
	}
	switch (direction) {	/* make sure that we want to do something */
	case 0:	/**** up ****/
		/* don't do anything if we are in first row and press up button */
		if (listrow >= 1) {
			if (listrow == 1 && (atoi(buf[1]) == 0))
				return;	/* if item is not a sumitem (submenu) do nothing */
			if (atoi(buf[1]) == 1) {	/* we are moving a sub item */
				gint lrow, howmany;

				lrow = listrow;
				newrow =
				    findpresubitem(list, lrow, ilevel);

				howmany = findhowmany(list, lrow);
				items =
				    (LISTROW *) g_malloc(sizeof(LISTROW) *
							 howmany);
				remove_item_and_children(list, items,
							 lrow);
				insert_items(list, items, howmany, newrow);
				g_free(items);
				setbuttonson(editor, TRUE);
				return;
			} else {	/* we are moving an item */

				if (atoi(prebuf[1]) == 0) {	/* if we jumped an item */
				        gtk_clist_swap_rows (GTK_CLIST(list),
			                                listrow,
			                                listrow-1);
			                --listrow;
			                gtk_clist_select_row(GTK_CLIST(list), listrow, 0);
			                setbuttonson(editor, TRUE);
			                return;
			
				} else {	/* if we jumped a sub item */
					getrow(list, listrow - 2, prebuf);
					if (atoi(prebuf[1]) == 0) {	/* if row we will follow is an item */
						buf[2] =
						    g_strdup(prebuf[2]);
						buf[3] =
						    g_strdup(prebuf[3]);
						buf[5] =
						    g_strdup(prebuf[0]);

					} else {	/* if row we will follow is a sub item */
						sprintf(buf[2], "%d",
							(atoi(prebuf[2]) + 1));	/* move one to the right of the sub item */
						buf[3] =
						    g_strdup(prebuf[0]);
						buf[5] = g_strdup("");
					}
					getcolumn(list, listrow, 1,
						  tmpbuf);
					if (atoi(tmpbuf) == 0) {	/* what we left behind was an item not a sub item */
						tmpbuf = g_strdup("");
						gtk_clist_set_text
						    (GTK_CLIST(list),
						     listrow, 5, tmpbuf);	/* we have already removed our row so listrom now points to the row we left behind */
					}
				}
			}
			gtk_clist_remove(GTK_CLIST(list), listrow);	/* remove item from current position */
			--listrow;	/* subtract one form our position */
		} else
			return;
		break;
	case 1:	/**** down ****/
		if (listrow < GTK_CLIST(list)->rows -1) {	/* don't do anything if we are in last row and press down button */
			if (atoi(buf[1]) == 1) {	/* we are moving a sub item */
				gint lrow, howmany;

				lrow = listrow;
				newrow =
				    findnextsubitem(list, lrow, ilevel);

				howmany = findhowmany(list, lrow);
				newrow = newrow - howmany;
				items =
				    (LISTROW *) g_malloc(sizeof(LISTROW) *
							 howmany);
				remove_item_and_children(list, items,
							 lrow);
				sprintf(mybuf,
					"howmany = %d\n newrow = %d\n",
					howmany, newrow);
				insert_items(list, items, howmany, newrow);
				g_free(items);
				setbuttonson(editor, TRUE);
				return;
			}
			if (atoi(buf[1]) == 0) {
				if (atoi(buf[2]) != atoi(nextbuf[2]))
					return;	/*       //-- do not move item to a new level */
			}
			gtk_clist_remove(GTK_CLIST(list), listrow);	/* remove item from current position */
			++listrow;	/* add one to listrow */
		} else
			return;
		break;
	case 2:
		if (ilevel > 0) {
			--ilevel;
			sprintf(buf[2], "%d", ilevel);
			gtk_clist_remove(GTK_CLIST(list), listrow);	/* remove item from current position */
		} else
			return;
		break;
	case 3:
		if (itype == 1) {
			++ilevel;
			sprintf(buf[2], "%d", ilevel);
			gtk_clist_remove(GTK_CLIST(list), listrow);	/* remove item from current position */
		} else if (itype == 0) {
			if (pretype == 1 && prelevel >= ilevel) {	/* if the previous item is a submenu and it's level equal to or grater than our level */
				++ilevel;
				sprintf(buf[2], "%d", ilevel);
				gtk_clist_remove(GTK_CLIST(list), listrow);	/* remove item from current position */
			} else if (pretype == 0 && prelevel > ilevel) {
				++ilevel;
				sprintf(buf[2], "%d", ilevel);
				gtk_clist_remove(GTK_CLIST(list), listrow);	/* remove item from current position */
			} else
				return;
		} else
			return;
		break;
	default:
		return;
	}
	setbuttonson(editor, TRUE);
	gtk_clist_insert(GTK_CLIST(list), listrow, buf);	/* insert item in new position */
	gtk_clist_set_shift(GTK_CLIST(list), listrow, 0, 0, (ilevel * 10));	/* show level by shifting */
	gtk_clist_select_row(GTK_CLIST(list), listrow, 0);	/* seletct row (same item) */
}

/****************************************************************************** 
 * gets info from one row all columns
******************************************************************************/
void getrow(GtkWidget * list, gint listrow, gchar * buf[6])
{
	gchar *tmpbuf;
	/* read items from list so we can move them or show changes */
	gtk_clist_get_text(GTK_CLIST(list), listrow, 0, &tmpbuf);	/* get item from list */
	buf[0] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 1, &tmpbuf);	/* get type from list */
	buf[1] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 2, &tmpbuf);	/* get level from list */
	buf[2] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 3, &tmpbuf);	/* get subitem from list */
	buf[3] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 4, &tmpbuf);	/* get menu from list */
	buf[4] = g_strdup(tmpbuf);
	gtk_clist_get_text(GTK_CLIST(list), listrow, 5, &tmpbuf);	/* get extra item from list */
	buf[5] = g_strdup(tmpbuf);
}

/****************************************************************************** 
 * gets info from one column of one row
******************************************************************************/
void getcolumn(GtkWidget * list, gint row, gint column, gchar * buf)
{
	/*-- read item from list */
	gtk_clist_get_text(GTK_CLIST(list), row, column, &buf);	/* get item from list */
}

/****************************************************************************** 
 * returns the number of rows (bookmarks)
******************************************************************************/
gint findhowmany(GtkWidget * clist, gint row)
{
	gchar *buf[6];
	gint level, type;
	gint i = 1;

	getrow(clist, row, buf);
	level = atoi(buf[2]);
	type = atoi(buf[1]);
	++row;
	while (row < GTK_CLIST(clist)->rows) {
		getrow(clist, row, buf);
		if (atoi(buf[2]) > level) {
			++row;
			++i;
		} else
			break;
	}
	return i;
}

/****************************************************************************** 
 *This removes an item and its children from the clist, 
 *and returns a list of the removed items. (code from glade_menu_editor.c) 
******************************************************************************/
void remove_item_and_children(GtkWidget * clist, LISTROW * items, gint row)
{
	gchar *buf[6];
	gint level, type;
	gint i = 0;

	getrow(clist, row, buf);
	level = atoi(buf[2]);
	type = atoi(buf[1]);
	items[i].item = g_strdup(buf[0]);
	items[i].type = type;
	items[i].level = level;
	gtk_clist_remove(GTK_CLIST(clist), row);

	while (row < GTK_CLIST(clist)->rows) {
		getrow(clist, row, buf);
		++i;
		if (atoi(buf[2]) > level) {
			items[i].item = g_strdup(buf[0]);
			items[i].type = atoi(buf[1]);
			items[i].level = atoi(buf[2]);
			gtk_clist_remove(GTK_CLIST(clist), row);
		} else
			break;
	}
	/* delete item; */
	return;
}


/******************************************************************************  
 *This inserts the given list of items at the 
 *given position in the clist. (code from glade_menu_editor.c) 
******************************************************************************/
static void
insert_items(GtkWidget * clist, LISTROW * items, gint howmany, gint row)
{
	LISTROW *item;
	gint i = 0;

	while (i < howmany) {
		item = &items[i];
		insert_item(GTK_CLIST(clist), item, row++);
		++i;
	}
}

/******************************************************************************
 * This adds the item to the clist at the given position.
******************************************************************************/
static void insert_item(GtkCList * clist, LISTROW * item, gint row)
{
	gchar *buf[6], tmpbuf[80];
	gint level;

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
		gtk_clist_insert(GTK_CLIST(clist), row, buf);
	else
		row = gtk_clist_append(GTK_CLIST(clist), buf);

	gtk_clist_set_shift(GTK_CLIST(clist), row, 0, 0, (level * 10));
}


/******************************************************************************
 *create_listeditor - create the listeditor dialog
******************************************************************************/
GtkWidget *create_listeditor(void)
{
	GtkWidget *listeditor;
	GtkWidget *dialog_vbox1;
	GtkWidget *handlebox1;
	GtkWidget *toolbar3;
	GtkWidget *tmp_toolbar_icon;
	GtkWidget *btnLEnew;
	GtkWidget *btnLEopen;
	GtkWidget *btnLEclose;
	GtkWidget *btnLEsave;
	GtkWidget *vseparator4;
	GtkWidget *btnLErefresh;
	GtkWidget *vseparator6;
	GtkWidget *btnLEprint;
	GtkWidget *hbox1;
	GtkWidget *frame3;
	GtkWidget *vbox1;
	GtkWidget *scrolledwindow1;
	GtkWidget *clLElist;
	GtkWidget *label1;
	GtkWidget *label2;
	GtkWidget *label3;
	GtkWidget *label4;
	GtkWidget *label5;
	GtkWidget *toolbar1;
	GtkWidget *btnLEup;
	GtkWidget *btnLEdown;
	GtkWidget *btnLEleft;
	GtkWidget *btnLEright;
	GtkWidget *vbox3;
	GtkWidget *frame1;
	GtkWidget *toolbar4;
	GtkWidget *entryVerseLookup;
	GtkWidget *btnLEgotoverse;
	GtkWidget *frame2;
	GtkWidget *vbox4;
	GtkWidget *hbox2;
	GtkWidget *label19;
	GtkWidget *entryListItem;
	GtkWidget *hbox3;
	GtkWidget *label20;
	GtkWidget *entryType;
	GtkWidget *label21;
	GtkWidget *entryLevel;
	GtkWidget *toolbar6;
	GtkWidget *btnLEAddVerse;
	GtkWidget *btnLEAddItem;
	GtkWidget *btnLEdelete;
	GtkWidget *toolbar9;
	GtkWidget *btnLEapplylistchanges;
	GtkWidget *frame4;
	GtkWidget *scrolledwindow3;
	GtkWidget *text7;
	GtkWidget *dialog_action_area1;
	GtkWidget *btnLEok;
	GtkWidget *btnLEapply;
	GtkWidget *btnLEcancel;

	listeditor = gnome_dialog_new("GnomeSword - ListEditor", NULL);
	gtk_object_set_data(GTK_OBJECT(listeditor), "listeditor",
			    listeditor);
	gtk_container_set_border_width(GTK_CONTAINER(listeditor), 4);
	GTK_WINDOW(listeditor)->type = GTK_WINDOW_DIALOG;
	gtk_window_set_policy(GTK_WINDOW(listeditor), FALSE, FALSE, FALSE);

	dialog_vbox1 = GNOME_DIALOG(listeditor)->vbox;
	gtk_object_set_data(GTK_OBJECT(listeditor), "dialog_vbox1",
			    dialog_vbox1);
	gtk_widget_show(dialog_vbox1);

	handlebox1 = gtk_handle_box_new();
	gtk_widget_ref(handlebox1);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "handlebox1",
				 handlebox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(handlebox1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), handlebox1, TRUE, TRUE,
			   0);
	gtk_widget_set_usize(handlebox1, -2, 33);

	toolbar3 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar3);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "toolbar3",
				 toolbar3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar3);
	gtk_container_add(GTK_CONTAINER(handlebox1), toolbar3);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar3),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor, GNOME_STOCK_PIXMAP_NEW);
	btnLEnew =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar3),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button18", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEnew);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEnew",
				 btnLEnew,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(btnLEnew, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor, GNOME_STOCK_PIXMAP_OPEN);
	btnLEopen =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar3),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button19", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEopen);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEopen",
				 btnLEopen,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(btnLEopen, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor,
				      GNOME_STOCK_PIXMAP_CLOSE);
	btnLEclose =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar3),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button20", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEclose);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEclose",
				 btnLEclose,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(btnLEclose, FALSE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor, GNOME_STOCK_PIXMAP_SAVE);
	btnLEsave =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar3),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button21", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEsave);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEsave",
				 btnLEsave,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEsave);

	vseparator4 = gtk_vseparator_new();
	gtk_widget_ref(vseparator4);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "vseparator4",
				 vseparator4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar3), vseparator4, NULL,
				  NULL);
	gtk_widget_set_usize(vseparator4, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor,
				      GNOME_STOCK_PIXMAP_REFRESH);
	btnLErefresh =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar3),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button26",
				       "Reload menu in ListEditor", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLErefresh);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLErefresh",
				 btnLErefresh,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_set_sensitive(btnLErefresh, FALSE);

	vseparator6 = gtk_vseparator_new();
	gtk_widget_ref(vseparator6);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "vseparator6",
				 vseparator6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar3), vseparator6, NULL,
				  NULL);
	gtk_widget_set_usize(vseparator6, 5, 7);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor,
				      GNOME_STOCK_PIXMAP_PRINT);
	btnLEprint =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar3),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button22", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEprint);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEprint",
				 btnLEprint,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEprint);
	gtk_widget_set_sensitive(btnLEprint, FALSE);

	hbox1 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox1);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "hbox1", hbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox1);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), hbox1, TRUE, TRUE, 0);

	frame3 = gtk_frame_new("Bookmarks");
	gtk_widget_ref(frame3);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "frame3", frame3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame3);
	gtk_box_pack_start(GTK_BOX(hbox1), frame3, TRUE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame3), 3);

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox1);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "vbox1", vbox1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox1);
	gtk_container_add(GTK_CONTAINER(frame3), vbox1);

	scrolledwindow1 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow1);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "scrolledwindow1",
				 scrolledwindow1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow1);
	gtk_box_pack_start(GTK_BOX(vbox1), scrolledwindow1, TRUE, TRUE, 0);
	gtk_widget_set_usize(scrolledwindow1, 225, 181);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow1), GTK_POLICY_NEVER,
				       GTK_POLICY_ALWAYS);

	clLElist = gtk_clist_new(5);
	gtk_widget_ref(clLElist);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "clLElist",
				 clLElist,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(clLElist);
	gtk_container_add(GTK_CONTAINER(scrolledwindow1), clLElist);
	gtk_clist_set_column_width(GTK_CLIST(clLElist), 0, 223);
	gtk_clist_set_column_width(GTK_CLIST(clLElist), 1, 80);
	gtk_clist_set_column_width(GTK_CLIST(clLElist), 2, 80);
	gtk_clist_set_column_width(GTK_CLIST(clLElist), 3, 80);
	gtk_clist_set_column_width(GTK_CLIST(clLElist), 4, 80);
	gtk_clist_column_titles_hide(GTK_CLIST(clLElist));

	label1 = gtk_label_new("label1");
	gtk_widget_ref(label1);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label1", label1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label1);
	gtk_clist_set_column_widget(GTK_CLIST(clLElist), 0, label1);

	label2 = gtk_label_new("label2");
	gtk_widget_ref(label2);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label2", label2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label2);
	gtk_clist_set_column_widget(GTK_CLIST(clLElist), 1, label2);

	label3 = gtk_label_new("label3");
	gtk_widget_ref(label3);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label3", label3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label3);
	gtk_clist_set_column_widget(GTK_CLIST(clLElist), 2, label3);

	label4 = gtk_label_new("label4");
	gtk_widget_ref(label4);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label4", label4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label4);
	gtk_clist_set_column_widget(GTK_CLIST(clLElist), 3, label4);

	label5 = gtk_label_new("label5");
	gtk_widget_ref(label5);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label5", label5,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label5);
	gtk_clist_set_column_widget(GTK_CLIST(clLElist), 4, label5);

	toolbar1 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar1);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "toolbar1",
				 toolbar1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar1);
	gtk_box_pack_start(GTK_BOX(vbox1), toolbar1, FALSE, FALSE, 0);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar1),
				      GTK_RELIEF_NONE);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor, GNOME_STOCK_PIXMAP_UP);
	btnLEup =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button11", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEup);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEup",
				 btnLEup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEup);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor, GNOME_STOCK_PIXMAP_DOWN);
	btnLEdown =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button12", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEdown);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEdown",
				 btnLEdown,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEdown);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor, GNOME_STOCK_PIXMAP_BACK);
	btnLEleft =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button23", "Previous Level", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEleft);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEleft",
				 btnLEleft,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEleft);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor,
				      GNOME_STOCK_PIXMAP_FORWARD);
	btnLEright =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar1),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "button24", "Next level", NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEright);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEright",
				 btnLEright,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEright);
	gtk_widget_set_usize(btnLEright, 56, -2);

	vbox3 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox3);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "vbox3", vbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox3);
	gtk_box_pack_start(GTK_BOX(hbox1), vbox3, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(vbox3), 3);

	frame1 = gtk_frame_new("Lookup Verse");
	gtk_widget_ref(frame1);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "frame1", frame1,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame1);
	gtk_box_pack_start(GTK_BOX(vbox3), frame1, FALSE, TRUE, 0);

	toolbar4 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_ICONS);
	gtk_widget_ref(toolbar4);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "toolbar4",
				 toolbar4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar4);
	gtk_container_add(GTK_CONTAINER(frame1), toolbar4);
	gtk_widget_set_usize(toolbar4, 193, -2);
	gtk_toolbar_set_button_relief(GTK_TOOLBAR(toolbar4),
				      GTK_RELIEF_NONE);

	entryVerseLookup = gtk_entry_new();
	gtk_widget_ref(entryVerseLookup);
	gtk_object_set_data_full(GTK_OBJECT(listeditor),
				 "entryVerseLookup", entryVerseLookup,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryVerseLookup);
	gtk_toolbar_append_widget(GTK_TOOLBAR(toolbar4), entryVerseLookup,
				  NULL, NULL);
	gtk_widget_set_usize(entryVerseLookup, 172, -2);

	tmp_toolbar_icon =
	    gnome_stock_pixmap_widget(listeditor,
				      GNOME_STOCK_PIXMAP_JUMP_TO);
	btnLEgotoverse =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar4),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Go to Verse", NULL, NULL,
				       tmp_toolbar_icon, NULL, NULL);
	gtk_widget_ref(btnLEgotoverse);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEgotoverse",
				 btnLEgotoverse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEgotoverse);

	frame2 = gtk_frame_new("Edit items");
	gtk_widget_ref(frame2);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "frame2", frame2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame2);
	gtk_box_pack_start(GTK_BOX(vbox3), frame2, FALSE, TRUE, 0);

	vbox4 = gtk_vbox_new(FALSE, 0);
	gtk_widget_ref(vbox4);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "vbox4", vbox4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(vbox4);
	gtk_container_add(GTK_CONTAINER(frame2), vbox4);

	hbox2 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox2);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "hbox2", hbox2,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox2);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox2, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox2), 2);

	label19 = gtk_label_new("item");
	gtk_widget_ref(label19);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label19",
				 label19,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label19);
	gtk_box_pack_start(GTK_BOX(hbox2), label19, FALSE, FALSE, 0);
	gtk_widget_set_usize(label19, 46, -2);

	entryListItem = gtk_entry_new();
	gtk_widget_ref(entryListItem);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "entryListItem",
				 entryListItem,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryListItem);
	gtk_box_pack_start(GTK_BOX(hbox2), entryListItem, TRUE, TRUE, 0);

	hbox3 = gtk_hbox_new(FALSE, 0);
	gtk_widget_ref(hbox3);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "hbox3", hbox3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(hbox3);
	gtk_box_pack_start(GTK_BOX(vbox4), hbox3, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(hbox3), 2);

	label20 = gtk_label_new("type");
	gtk_widget_ref(label20);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label20",
				 label20,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label20);
	gtk_box_pack_start(GTK_BOX(hbox3), label20, FALSE, FALSE, 0);
	gtk_widget_set_usize(label20, 45, -2);

	entryType = gtk_entry_new();
	gtk_widget_ref(entryType);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "entryType",
				 entryType,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryType);
	gtk_box_pack_start(GTK_BOX(hbox3), entryType, TRUE, TRUE, 0);
	gtk_widget_set_usize(entryType, 47, -2);

	label21 = gtk_label_new("level");
	gtk_widget_ref(label21);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "label21",
				 label21,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(label21);
	gtk_box_pack_start(GTK_BOX(hbox3), label21, FALSE, FALSE, 0);
	gtk_widget_set_usize(label21, 45, -2);

	entryLevel = gtk_entry_new();
	gtk_widget_ref(entryLevel);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "entryLevel",
				 entryLevel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(entryLevel);
	gtk_box_pack_start(GTK_BOX(hbox3), entryLevel, TRUE, TRUE, 0);
	gtk_widget_set_usize(entryLevel, 52, -2);

	toolbar6 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
	gtk_widget_ref(toolbar6);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "toolbar6",
				 toolbar6,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar6);
	gtk_box_pack_start(GTK_BOX(vbox4), toolbar6, FALSE, FALSE, 0);

	btnLEAddVerse = gtk_toolbar_append_element(GTK_TOOLBAR(toolbar6),
						   GTK_TOOLBAR_CHILD_BUTTON,
						   NULL,
						   "Add Verse",
						   NULL, NULL,
						   NULL, NULL, NULL);
	gtk_widget_ref(btnLEAddVerse);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEAddVerse",
				 btnLEAddVerse,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEAddVerse);
	gtk_widget_set_usize(btnLEAddVerse, 69, -2);

	btnLEAddItem = gtk_toolbar_append_element(GTK_TOOLBAR(toolbar6),
						  GTK_TOOLBAR_CHILD_BUTTON,
						  NULL,
						  "Add Menu",
						  NULL, NULL,
						  NULL, NULL, NULL);
	gtk_widget_ref(btnLEAddItem);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEAddItem",
				 btnLEAddItem,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEAddItem);

	btnLEdelete = gtk_toolbar_append_element(GTK_TOOLBAR(toolbar6),
						 GTK_TOOLBAR_CHILD_BUTTON,
						 NULL,
						 "Delete Item",
						 NULL, NULL, NULL, NULL,
						 NULL);
	gtk_widget_ref(btnLEdelete);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEdelete",
				 btnLEdelete,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEdelete);

	toolbar9 =
	    gtk_toolbar_new(GTK_ORIENTATION_HORIZONTAL, GTK_TOOLBAR_TEXT);
	gtk_widget_ref(toolbar9);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "toolbar9",
				 toolbar9,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(toolbar9);
	gtk_box_pack_start(GTK_BOX(vbox4), toolbar9, FALSE, FALSE, 0);

	btnLEapplylistchanges =
	    gtk_toolbar_append_element(GTK_TOOLBAR(toolbar9),
				       GTK_TOOLBAR_CHILD_BUTTON, NULL,
				       "Apply Changes to List", NULL, NULL,
				       NULL, NULL, NULL);
	gtk_widget_ref(btnLEapplylistchanges);
	gtk_object_set_data_full(GTK_OBJECT(listeditor),
				 "btnLEapplylistchanges",
				 btnLEapplylistchanges,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEapplylistchanges);
	gtk_widget_set_usize(btnLEapplylistchanges, 207, -2);

	frame4 = gtk_frame_new(NULL);
	gtk_widget_ref(frame4);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "frame4", frame4,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(frame4);
	gtk_box_pack_start(GTK_BOX(dialog_vbox1), frame4, FALSE, TRUE, 0);
	gtk_container_set_border_width(GTK_CONTAINER(frame4), 3);

	scrolledwindow3 = gtk_scrolled_window_new(NULL, NULL);
	gtk_widget_ref(scrolledwindow3);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "scrolledwindow3",
				 scrolledwindow3,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(scrolledwindow3);
	gtk_container_add(GTK_CONTAINER(frame4), scrolledwindow3);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW
				       (scrolledwindow3), GTK_POLICY_NEVER,
				       GTK_POLICY_AUTOMATIC);

	text7 = gtk_text_new(NULL, NULL);
	gtk_widget_ref(text7);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "text7", text7,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(text7);
	gtk_container_add(GTK_CONTAINER(scrolledwindow3), text7);

	dialog_action_area1 = GNOME_DIALOG(listeditor)->action_area;
	gtk_object_set_data(GTK_OBJECT(listeditor), "dialog_action_area1",
			    dialog_action_area1);
	gtk_widget_show(dialog_action_area1);
	gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area1),
				  GTK_BUTTONBOX_END);
	gtk_button_box_set_spacing(GTK_BUTTON_BOX(dialog_action_area1), 8);

	gnome_dialog_append_button(GNOME_DIALOG(listeditor),
				   GNOME_STOCK_BUTTON_OK);
	btnLEok = g_list_last(GNOME_DIALOG(listeditor)->buttons)->data;
	gtk_widget_ref(btnLEok);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEok",
				 btnLEok,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEok);
	gtk_widget_set_sensitive(btnLEok, FALSE);
	GTK_WIDGET_SET_FLAGS(btnLEok, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(listeditor),
				   GNOME_STOCK_BUTTON_APPLY);
	btnLEapply = g_list_last(GNOME_DIALOG(listeditor)->buttons)->data;
	gtk_widget_ref(btnLEapply);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEapply",
				 btnLEapply,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEapply);
	gtk_widget_set_sensitive(btnLEapply, FALSE);
	GTK_WIDGET_SET_FLAGS(btnLEapply, GTK_CAN_DEFAULT);

	gnome_dialog_append_button(GNOME_DIALOG(listeditor),
				   GNOME_STOCK_BUTTON_CANCEL);
	btnLEcancel = g_list_last(GNOME_DIALOG(listeditor)->buttons)->data;
	gtk_widget_ref(btnLEcancel);
	gtk_object_set_data_full(GTK_OBJECT(listeditor), "btnLEcancel",
				 btnLEcancel,
				 (GtkDestroyNotify) gtk_widget_unref);
	gtk_widget_show(btnLEcancel);
	GTK_WIDGET_SET_FLAGS(btnLEcancel, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(btnLEnew), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEnew_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEopen), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEopen_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEclose), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEclose_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLErefresh), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLErefresh_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(clLElist), "select_row",
			   GTK_SIGNAL_FUNC(on_clLElist_select_row), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEup), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEup_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEdown), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEdown_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEleft), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEleft_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEright), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEright_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEgotoverse), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEgotoverse_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEAddVerse), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEAddVerse_clicked),
			   NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEAddItem), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEAddItem_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEdelete), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEdelete_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEapplylistchanges), "clicked",
			   GTK_SIGNAL_FUNC
			   (on_btnLEapplylistchanges_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEok), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEok_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEapply), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEapply_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(btnLEcancel), "clicked",
			   GTK_SIGNAL_FUNC(on_btnLEcancel_clicked), NULL);

	return listeditor;
}
