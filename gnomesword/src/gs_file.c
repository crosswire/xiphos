/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */

 /*
    * GnomeSword Bible Study Tool
    * gs_file.c
    * -------------------
    * Mon May 8 2000
    * copyright (C) 2001 by tbiggs
    * tbiggs@users.sourceforge.net
    *
 */

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
    *  You should have received a copy of the GNU Library General Public License
    *  along with this program; if not, write to the Free Software
    *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
  */

#include <gnome.h>
#include <stdio.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>

#include "gs_gnomesword.h"
#include "gs_editor.h"
#include "support.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "sw_gnomesword.h"
//#include "gs_listeditor.h"
#include "gs_setup.h"
#include "sw_properties.h"

#define BUFFER_SIZE 8192	/* input buffer size */

/*****************************************************************************
 *globals
*****************************************************************************/
char 	
	*homedir,		/* store $home directory */
	*gSwordDir,		/* store GnomeSword directory */
	*swbmDir,		/* bookmarks directory */
	*shortcutbarDir,		/* store dir name for shortcutbar files */
	*fnquickmarks,
	*fnconfigure;		/* store filename for configure file - options */
	
FILE 
	*configfile;		/*  file pointer to configuration file - options */
	
gint 	
	iquickmarks;		/* number of quickmark menu items */
	
gchar 	
	remembersubtree[256],	/* we need to use this when add to quickmarks */
 	rememberlastitem[255];	/* we need to use this when add to quickmarks */

LISTITEM 
	*p_mylistitem,
	mylistitem,
	listitem;		/* structure for ListEditor items
				   (verse lists and bookmarks) */
/*****************************************************************************
 *externals
*****************************************************************************/



/******************************************************************************
*******************************************************************************
 *callbacks fileselection dialogs
*******************************************************************************
******************************************************************************/

/******************************************************************************
 *on_ok_button1_clicked - fileselection dialog
******************************************************************************/
static void on_ok_button1_clicked(GtkButton * button, GSHTMLEditorControlData * ecd)
{
	GtkWidget *filesel;
	
	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(ecd->filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	load_file(ecd->filename, ecd);
	gtk_widget_destroy(filesel);
}

//----------------------------------------------
static void on_cancel_button1_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

//----------------------------------------------
static void on_ok_button2_clicked(GtkButton * button, GSHTMLEditorControlData * ecd)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(ecd->filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	gtk_widget_destroy(filesel);
	save_file(ecd->filename, ecd);
}

/* fileselection dialog cancel button clicked */
static void
on_cancel_button2_clicked(GtkButton * button, gpointer user_data)
{	//--destroy fileselection dialog
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

/*****************************************************************************
 * check for GnomeSword dir
 * if not found - create it and
 * the files needed to run program
 *****************************************************************************/
gint setDiretory(void)
{
	gint retval = 0;
	gchar 
		dotswordDir[256],
		*gsbmDir,		/* bookmarks directory for gnomesword-0.7.0 and up */
		bmFile[300],
		genbookdir[300];
	
	/* get home dir */
	if ((homedir = getenv("HOME")) == NULL) { 		
		g_error("$HOME is not set!");	
		/* if not found in env exit */
		exit(0);
	}
	/* set gSwordDir to $home + .GnomeSword */
	gSwordDir = g_new(char, strlen(homedir) + strlen(".GnomeSword") + 2);	
	sprintf(gSwordDir, "%s/%s", homedir, ".GnomeSword");
	sprintf(dotswordDir, "%s/%s",homedir,".sword/bookmarks/");/* for compatability */
	/* set bookmarks dir to homedir + .GnomeSword/bookmarks */
	gsbmDir = g_new(char, strlen(homedir) + strlen(".GnomeSword/bookmarks") + 2);
	sprintf(gsbmDir, "%s/%s",homedir,".GnomeSword/bookmarks/");
	/*shortcutbar dir */
	shortcutbarDir = g_new(char, strlen(gSwordDir) + strlen("shortcutbar") + 2);	
	sprintf(shortcutbarDir, "%s/%s", gSwordDir, "shortcutbar");	
	if (access(shortcutbarDir, F_OK) == -1) {	/* if shortcutbar does not exist create it */
		if ((mkdir(shortcutbarDir, S_IRWXU)) == 0) {
			// do nothing
		} else {
			printf("can't create shortcutbar dir");
		}
	} 	
        /* set fnbookmarks to gSwordDir + bookmarks.txt */
	fnquickmarks =
	    g_new(char, strlen(gSwordDir) + strlen("quickmarks.gs") + 2);	
	sprintf(fnquickmarks, "%s/%s", gSwordDir, "guickmarks.gs");
        /* set fnconfigure to gSwordDir + gsword.cfg */
	fnconfigure =
	    g_new(char, strlen(gSwordDir) + strlen("preferences.conf") + 2);	
	sprintf(fnconfigure, "%s/%s", gSwordDir, "preferences.conf");
	if (access(gSwordDir, F_OK) == -1) {	/* if gSwordDir does not exist create it */
		if ((mkdir(gSwordDir, S_IRWXU)) == 0) {
			createFiles();
		} else {
			printf(".GnomeSword does not exist");	
			/* if we can not create gSwordDir exit */
			gtk_exit(1);
		}
	}
	
	if (access(fnconfigure, F_OK) == -1) {
		retval = 1; 
	}
	
	
/****  find or create dir for local gbs support  ****/	
	sprintf(genbookdir, "%s/%s",homedir,".sword/");/* for local gbs support */
	
	if (access(genbookdir, F_OK) == -1) {
		if((mkdir(genbookdir, S_IRWXU)) == 0) {
			// do nothing
		} else { 
			g_warning("can't create .sword dir");
		}
	}
	sprintf(genbookdir, "%s/%s",homedir,".sword/mods.d");/* for local module support */
	
	if (access(genbookdir, F_OK) == -1) {	
		if((mkdir(genbookdir, S_IRWXU)) == 0) {
			// do nothing
		} else { 
			g_warning("can't create .sword/mods.d dir");
		}
	}
	sprintf(genbookdir, "%s/%s",homedir,".sword/modules");/* for local gbs support */
	
	if (access(genbookdir, F_OK) == -1) {
		if((mkdir(genbookdir, S_IRWXU)) == 0) {
			// do nothing
		} else { 
			g_warning("can't create .sword/modules dir");
		}
	}
	sprintf(genbookdir, "%s/%s",homedir,".sword/modules/genbook");/* for local gbs support */
	
	
	if (access(genbookdir, F_OK) == -1) {
		if((mkdir(genbookdir, S_IRWXU)) == 0) {
			// do nothing
		} else { 
			g_warning("can't create .sword/modules/genbook dir");
		}
	}
	sprintf(genbookdir, "%s/%s",homedir,".sword/modules/genbook/rawbook");/* for local gbs support */
		
	
	if (access(genbookdir, F_OK) == -1) {
		if((mkdir(genbookdir, S_IRWXU)) == 0) {
			// do nothing
		} else { 
			g_warning("can't create .sword/modules/genbook/rawbook dir");
		}
	}
/****  end local gbs dir checks  ****/	
	
	
	if (access(dotswordDir, F_OK) == 0) {	/* if .sword/bookmarks does exist rename it to .GnomeSword/bookmarks*/
		if ((rename(dotswordDir, gsbmDir)) == 0) {
			// do nothing
		} else { 
			g_warning("can't rename bookmarks dir");
		}
	}  
	
	/* set swbmDir to gsbmDir */	
	swbmDir = g_new(char, strlen(gsbmDir) + 2);
	sprintf(swbmDir, "%s",gsbmDir);
	g_free(gsbmDir);
	
	if (access(swbmDir, F_OK) == -1) {	/* if .GnomeSword/bookmarks does not exist create it */
		if ((mkdir(swbmDir, S_IRWXU)) == 0) {
			// do nothing
		} else {
			g_warning("can't create bookmarks dir and files");
		}
	} 
	
	sprintf(bmFile, "%s/%s",swbmDir,"personal.conf");
	if (access(bmFile, F_OK) == -1) {	
		if(retval == 1) retval = 3;
			else retval = 2;		
	} 
	
	return retval; 
}

/*****************************************************************************
 * load bookmarks program start
 *****************************************************************************/
void loadquickmarks_programstart(GtkWidget *MainFrm)
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flquickmarksnew;
	gchar subtreelabel[255];
	gint i = 0;
	long filesize;
	struct stat stat_p;

	stat(fnquickmarks, &stat_p);
	filesize = stat_p.st_size;
	iquickmarks = (filesize / (sizeof(mylist)));
	p_mylist = &mylist;
        /* try to open file */
	if ((flquickmarksnew = open(fnquickmarks, O_RDONLY)) == -1) {	
	        /* create bookmark file if we cannot open the file */
		createFiles();	
	}
	flquickmarksnew = open(fnquickmarks, O_RDONLY);
	while (i < iquickmarks) {
		read(flquickmarksnew, (char *) &mylist, sizeof(mylist));	
		sprintf(subtreelabel, "%s%s", p_mylist->menu,
				p_mylist->preitem);	/* create subitem label */
		additemtosubtree(MainFrm, subtreelabel, p_mylist->item);  /* add menu item to subtree */
		++i;
	}
	if(iquickmarks < 1)
		sprintf(rememberlastitem, "%s","<Separator>");
		
	else
		sprintf(rememberlastitem, "%s", p_mylist->item);
	close(flquickmarksnew);
}

/*****************************************************************************
 * load bookmarks after menu separator
 *****************************************************************************/
void loadquickmarks_afterSeparator(GtkWidget *MainFrm)
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flquickmarksnew;
	gchar subtreelabel[255];
	gint i = 0;

	p_mylist = &mylist;
	flquickmarksnew = open(fnquickmarks, O_RDONLY);
	while (i < iquickmarks) {
		read(flquickmarksnew, (char *) &mylist, sizeof(mylist));
		sprintf(subtreelabel, "%s%s", p_mylist->menu,
				p_mylist->preitem);	/* create subitem label */
			additemtosubtree(MainFrm, subtreelabel, p_mylist->item);	/*- add menu item to subtree */
		++i;
	}
	if(iquickmarks < 1)
		sprintf(rememberlastitem, "%s", "<Separator>");
	else
		sprintf(rememberlastitem, "%s", p_mylist->item);
	//iquickmarks = i+1;
	close(flquickmarksnew);
}

void clearquickmarks(void)
{				
	int flquickmarks;	/* bookmark file handle */
		
	remove(fnquickmarks);
	flquickmarks = open(fnquickmarks, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);
	close(flquickmarks);	/* close the file we are done for now */	
}


/*****************************************************************************
 * save a quickkmark that has been added to the quickkmark menu
 * by the add quickkmark item
 *****************************************************************************/
void savequickmark(gchar * item)
{				
	LISTITEM mylist;	/* structure for bookmark item */
	LISTITEM *p_mylist;	/* pointer to structure */
	int flquickmarksnew;	/* bookmark file handle */

	p_mylist = &mylist;	/* set pointer to structure */
	flquickmarksnew = open(fnquickmarks, O_WRONLY | O_APPEND);	/* open file to append one record  */
	strcpy(p_mylist->item, item);	/* bookmark label (verse) */
	strcpy(p_mylist->preitem, rememberlastitem);	/* item in menu to follow (you have to tell gnome where to insert the item) */
	strcpy(p_mylist->menu, "_Quickmarks/");	/* item belongs to the quickmark menu */
	write(flquickmarksnew, (char *) &mylist, sizeof(mylist));	/* write the record to the bookmark file */
	close(flquickmarksnew);	/* close the file we are done for now */
	sprintf(rememberlastitem,"%s",item);
}

/*****************************************************************************
 * create bookmark file nessary for GomeSword if it does not exist
 *****************************************************************************/
void createFiles(void)
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flbookmarksnew;

	p_mylist = &mylist;
	flbookmarksnew =
	    open(fnquickmarks, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);

	strcpy(p_mylist->item, "Romans 8:28");
	strcpy(p_mylist->preitem, "");
	strcpy(p_mylist->menu, "_Quickmarks/<Separator>");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	strcpy(p_mylist->item, "Rev 1:5");
	strcpy(p_mylist->preitem, "Romans 8:28");
	strcpy(p_mylist->menu, "_Quickmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	close(flbookmarksnew);
	iquickmarks = 2;
}

GtkWidget *create_fileselection1(GSHTMLEditorControlData * ecd)
{
	GtkWidget *fileselection1;
	GtkWidget *ok_button1;
	GtkWidget *cancel_button1;

	fileselection1 =
	    gtk_file_selection_new(_("GnomeSword - Open Note File"));
	gtk_object_set_data(GTK_OBJECT(fileselection1), "fileselection1",
			    fileselection1);
	gtk_container_set_border_width(GTK_CONTAINER(fileselection1), 10);
	gtk_file_selection_hide_fileop_buttons(GTK_FILE_SELECTION
					       (fileselection1));

	ok_button1 = GTK_FILE_SELECTION(fileselection1)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "ok_button1",
			    ok_button1);
	gtk_widget_show(ok_button1);
	GTK_WIDGET_SET_FLAGS(ok_button1, GTK_CAN_DEFAULT);

	cancel_button1 = GTK_FILE_SELECTION(fileselection1)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselection1), "cancel_button1",
			    cancel_button1);
	gtk_widget_show(cancel_button1);
	GTK_WIDGET_SET_FLAGS(cancel_button1, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button1_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(cancel_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button1_clicked),
			   NULL);

	return fileselection1;
}

GtkWidget *create_fileselectionSave(GSHTMLEditorControlData * ecd)
{
	GtkWidget *fileselectionSave;
	GtkWidget *ok_button2;
	GtkWidget *cancel_button2;

	fileselectionSave =
	    gtk_file_selection_new(_("GomeSword - Save Note File"));
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "fileselectionSave", fileselectionSave);
	gtk_container_set_border_width(GTK_CONTAINER(fileselectionSave),
				       10);

	ok_button2 = GTK_FILE_SELECTION(fileselectionSave)->ok_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave), "ok_button2",
			    ok_button2);
	gtk_widget_show(ok_button2);
	GTK_WIDGET_SET_FLAGS(ok_button2, GTK_CAN_DEFAULT);

	cancel_button2 =
	    GTK_FILE_SELECTION(fileselectionSave)->cancel_button;
	gtk_object_set_data(GTK_OBJECT(fileselectionSave),
			    "cancel_button2", cancel_button2);
	gtk_widget_show(cancel_button2);
	GTK_WIDGET_SET_FLAGS(cancel_button2, GTK_CAN_DEFAULT);

	gtk_signal_connect(GTK_OBJECT(ok_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_ok_button2_clicked), ecd);
	gtk_signal_connect(GTK_OBJECT(cancel_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button2_clicked),
			   NULL);

	return fileselectionSave;
}


