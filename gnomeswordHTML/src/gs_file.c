/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/***************************************************************************
                          gs_file.c  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
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
#include "support.h"
#include "gs_file.h"
#include "gs_menu.h"
#include "gs_sword.h"
#include "gs_listeditor.h"

#define BUFFER_SIZE 8192	/* input buffer size */

/*****************************************************************************
 *globals
*****************************************************************************/
char 	*homedir,		/* store $home directory */
	*gSwordDir,		/* store GnomeSword directory */
	*fnbookmarks,		/* store filename for bookmark file */
	*fnbookmarksnew,
	*fnconfigure;		/* store filename for configure file - options */
FILE 	*configfile,		/*  file pointer to configuration file - options */
	*flbookmarks;
gint 	ibookmarks;		/* number of bookmark menu items */
gchar 	remembersubtree[256],	/* we need to use this when add to bookmarks */
 	rememberlastitem[255];	/* we need to use this when add to bookmarks */

/*****************************************************************************
 *externals
*****************************************************************************/
extern GtkWidget *MainFrm;	/* main window (form) widget */
extern gchar *current_filename;	/* file loaded into study pad */
extern gchar bmarks[50][80];	/* array to store bookmarks - read in form file
				when program starts - saved to file after edit */
extern gboolean file_changed;	/* set to TRUE if text is study pad has changed
					- and file is not saved */

/******************************************************************************
*******************************************************************************
 *callbacks fileselection dialogs
*******************************************************************************
******************************************************************************/

/******************************************************************************
 *on_ok_button1_clicked - fileselection dialog
******************************************************************************/
static void on_ok_button1_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	loadFile(filesel);
	gtk_widget_destroy(filesel);
}

//----------------------------------------------
static void on_cancel_button1_clicked(GtkButton * button, gpointer user_data)
{
	gtk_widget_destroy(gtk_widget_get_toplevel(GTK_WIDGET(button)));
}

//----------------------------------------------
void on_ok_button2_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;
	gchar filename[255];

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	gtk_widget_destroy(filesel);
	saveFile(filename);
}

 //------------------------------------------
 //-- save verse list - fileselection dialog ok button clicke
 //-- user data - the list widget dialog ok button clicked
void on_ok_button4_clicked(GtkButton * button, gpointer user_data)
{
	GtkWidget *filesel;	//-- pointer to fileselection dialog
	gchar filename[255];	//-- string to store filename from fileselection dialog

	filesel = gtk_widget_get_toplevel(GTK_WIDGET(button));	//-- get fileselection dialog
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION(filesel)));	//-- get filename
	gtk_widget_destroy(filesel);	//-- destroy fileselection dialog
	savelist(filename, (GtkWidget *) user_data);	//-- send filename and list widget to savelist function (gs_file.c)
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
void setDiretory(void)
{
	/* get home dir */
	if ((homedir = getenv("HOME")) == NULL) { 		
		g_error("$HOME is not set!");	
		/* if not found in env exit */
		exit(0);
	}
	/* set gSwordDir to $home + .GnomeSword */
	gSwordDir = g_new(char, strlen(homedir) + strlen(".GnomeSword") + 2);	
	sprintf(gSwordDir, "%s/%s", homedir, ".GnomeSword");
	/* set fnbookmarks to gSwordDir + bookmarks.txt */
	fnbookmarks =
	    g_new(char, strlen(gSwordDir) + strlen("bookmarks.txt") + 2);	
	sprintf(fnbookmarks, "%s/%s", gSwordDir, "bookmarks.txt");
        /* set fnbookmarks to gSwordDir + bookmarks.txt */
	fnbookmarksnew =
	    g_new(char, strlen(gSwordDir) + strlen("bookmarksnew.gs") + 2);	
	sprintf(fnbookmarksnew, "%s/%s", gSwordDir, "bookmarksnew.gs");
        /* set fnconfigure to gSwordDir + gsword.cfg */
	fnconfigure =
	    g_new(char, strlen(gSwordDir) + strlen("settings3.cfg") + 2);	
	sprintf(fnconfigure, "%s/%s", gSwordDir, "settings3.cfg");
	if (access(gSwordDir, F_OK) == -1) {	/* if gSwordDir does not exist create it */
		if ((mkdir(gSwordDir, S_IRWXU)) == 0) {
			createFiles();
		} else {
			printf(".GnomeSword does not exist");	
			/* if we can not create gSwordDir exit */
			gtk_exit(1);
		}
	}
}

/*****************************************************************************
 * load bookmarks program start
 *****************************************************************************/
void loadbookmarks_programstart(void)
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flbookmarksnew;
	gchar subtreelabel[255];
	gint i = 0;
	long filesize;
	struct stat stat_p;

	stat(fnbookmarksnew, &stat_p);
	filesize = stat_p.st_size;
	ibookmarks = (filesize / (sizeof(mylist)));
	p_mylist = &mylist;
        /* try to open file */
	if ((flbookmarksnew = open(fnbookmarksnew, O_RDONLY)) == -1) {	
	        /* create bookmark file if we cannot open the file */
		createFiles();	
	}
	flbookmarksnew = open(fnbookmarksnew, O_RDONLY);
	while (i < ibookmarks) {
		read(flbookmarksnew, (char *) &mylist, sizeof(mylist));		
		if (p_mylist->type == 1) {  /* if type is 1 it is a subtree (submenu) */

			if (p_mylist->level == 0) {
				sprintf(subtreelabel, "%s%s",
					p_mylist->menu, p_mylist->preitem);
			} else {
				sprintf(subtreelabel, "%s%s%s",
					p_mylist->menu, p_mylist->subitem,
					p_mylist->preitem);
			}
			sprintf(remembersubtree, p_mylist->item);
			addsubtreeitem(MainFrm, subtreelabel,
				       p_mylist->item);
		} else {
			sprintf(subtreelabel, "%s%s%s", p_mylist->menu,
				p_mylist->subitem, p_mylist->preitem);	/* create subitem label */
			additemtosubtree(MainFrm, subtreelabel, p_mylist->item);  /* add menu item to subtree */
		}
		++i;
	}
	sprintf(rememberlastitem, "%s", p_mylist->item);
	close(flbookmarksnew);
}

/*****************************************************************************
 * load bookmarks after menu separator
 *****************************************************************************/
void loadbookmarks_afterSeparator(void)
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flbookmarksnew;
	gchar subtreelabel[255];
	gint i = 0;

	p_mylist = &mylist;
	flbookmarksnew = open(fnbookmarksnew, O_RDONLY);
	while (i < ibookmarks) {
		read(flbookmarksnew, (char *) &mylist, sizeof(mylist));
		if (p_mylist->type == 0 && p_mylist->level == 0) { /*- if type is 1 it is a subtree (submenu) */
			sprintf(subtreelabel, "%s%s%s", p_mylist->menu,
				p_mylist->subitem, p_mylist->preitem);	/* create subitem label */
			additemtosubtree(MainFrm, subtreelabel, p_mylist->item);	/*- add menu item to subtree */
		}
		++i;
	}
	sprintf(rememberlastitem, "%s", p_mylist->item);
	//ibookmarks = i+1;
	close(flbookmarksnew);
}

/*****************************************************************************
 * save a bookmark that has been added to the bookmark menu
 * by the add bookmark item
 *****************************************************************************/
void savebookmark(gchar * item)
{				
	LISTITEM mylist;	/* structure for bookmark item */
	LISTITEM *p_mylist;	/* pointer to structure */
	int flbookmarksnew;	/* bookmark file handle */

	p_mylist = &mylist;	/* set pointer to structure */
	flbookmarksnew = open(fnbookmarksnew, O_WRONLY | O_APPEND);	/* open file to append one record  */
	p_mylist->type = 0;	/* item is bookmark not submenu   */
	p_mylist->level = 0;	/* item is added at level one (show when you click bookmark menu) */
	strcpy(p_mylist->item, item);	/* bookmark label (verse) */
	strcpy(p_mylist->preitem, rememberlastitem),	/* item in menu to follow (you have to tell gnome where to insert the item) */
	    strcpy(p_mylist->subitem, "");	/* item does not belong to a submenu */
	strcpy(p_mylist->menu, "_Bookmarks/");	/* item does belong to the bookmark menu */
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));	/* write the record to the bookmark file */
	close(flbookmarksnew);	/* close the file we are done for now */
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
	    open(fnbookmarksnew, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);

	p_mylist->type = 1;
	p_mylist->level = 0;
	strcpy(p_mylist->item, "[What must I do to be saved?]");
	strcpy(p_mylist->preitem, "Edit Bookmarks"),
	    strcpy(p_mylist->subitem, "");
	strcpy(p_mylist->menu, "_Bookmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	p_mylist->type = 0;
	p_mylist->level = 1;
	strcpy(p_mylist->item, "Romans 1:16");
	strcpy(p_mylist->preitem, ""),
	    strcpy(p_mylist->subitem, "[What must I do to be saved?]/");
	strcpy(p_mylist->menu, "_Bookmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	p_mylist->type = 0;
	p_mylist->level = 1;
	strcpy(p_mylist->item, "Eph 2:8");
	strcpy(p_mylist->preitem, "Romans 1:16"),
	    strcpy(p_mylist->subitem, "[What must I do to be saved?]/");
	strcpy(p_mylist->menu, "_Bookmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	p_mylist->type = 0;
	p_mylist->level = 1;
	strcpy(p_mylist->item, "Acts  16:31");
	strcpy(p_mylist->preitem, "Eph 2:8"),
	    strcpy(p_mylist->subitem, "[What must I do to be saved?]/");
	strcpy(p_mylist->menu, "_Bookmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	p_mylist->type = 1;
	p_mylist->level = 0;
	strcpy(p_mylist->item, "[What is the Gospel?]");
	strcpy(p_mylist->preitem, "[What must I do to be saved?]"),
	    strcpy(p_mylist->subitem, "");
	strcpy(p_mylist->menu, "_Bookmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	p_mylist->type = 0;
	p_mylist->level = 1;
	strcpy(p_mylist->item, "1 Cor 15:1");
	strcpy(p_mylist->preitem, ""),
	    strcpy(p_mylist->subitem, "[What is the Gospel?]/");
	strcpy(p_mylist->menu, "_Bookmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	p_mylist->type = 0;
	p_mylist->level = 0;
	strcpy(p_mylist->item, "Romans 8:28");
	strcpy(p_mylist->preitem, ""), strcpy(p_mylist->subitem, "");
	strcpy(p_mylist->menu, "_Bookmarks/<Separator>");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	p_mylist->type = 0;
	p_mylist->level = 0;
	strcpy(p_mylist->item, "Rev 1:5");
	strcpy(p_mylist->preitem, "Romans 8:28"),
	    strcpy(p_mylist->subitem, "");
	strcpy(p_mylist->menu, "_Bookmarks/");
	write(flbookmarksnew, (char *) &mylist, sizeof(mylist));

	close(flbookmarksnew);
	ibookmarks = 8;
}

/*****************************************************************************
 * save studypad file
 *****************************************************************************/
void saveFile(gchar * filename)
{				
	gchar *data, charbuf[255];
	FILE *fp;
	gint bytes_written, len;
	gint context_id2;
	GtkWidget *statusbar;

	statusbar = lookup_widget(MainFrm, "statusbar2");
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
					 "GtkSword");
	if (current_filename == NULL || strcmp(current_filename, filename)) {
		g_free(current_filename);
		current_filename = g_strdup(filename);
	}
	data =
	    gtk_editable_get_chars(GTK_EDITABLE
				   (lookup_widget(MainFrm, "text3")), 0,
				   -1);
	fp = fopen(filename, "w");
	if (fp == NULL) {
		g_free(data);
		return;
	}
	len = strlen(data);
	bytes_written = fwrite(data, sizeof(gchar), len, fp);
	fclose(fp);
	g_free(data);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);
	if (len != bytes_written) {
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2,
				   "Error saving file.");
		return;
	}
	sprintf(charbuf, "%s - saved.", current_filename);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2, charbuf);
	file_changed = FALSE;
}

/*****************************************************************************
 * load file into studypad
 *****************************************************************************/
void loadFile(GtkWidget * filesel)
{				
	GtkWidget *statusbar, *text;
	gchar filename[255];
	FILE *fp;
	gchar buffer[BUFFER_SIZE];
	gint bytes_read;
	gint context_id2;

	statusbar = lookup_widget(MainFrm, "statusbar2");
	text = lookup_widget(MainFrm, "text3");
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
					 "GnomeSword");
	sprintf(filename, "%s",
		gtk_file_selection_get_filename(GTK_FILE_SELECTION
						(filesel)));
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);
	gtk_text_freeze(GTK_TEXT(text));
	gtk_editable_delete_text(GTK_EDITABLE(text), 0, -1);
	g_free(current_filename);
	current_filename = NULL;
	file_changed = FALSE;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		gtk_text_thaw(GTK_TEXT(text));
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2,
				   "Could not open file");
		return;
	}
	for (;;) {
		bytes_read = fread(buffer, sizeof(gchar), BUFFER_SIZE, fp);
		if (bytes_read > 0)
			gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
					buffer, bytes_read);

		if (bytes_read != BUFFER_SIZE && (feof(fp) || ferror(fp)))
			break;
	}
	/* If there is an error while loading, we reset everything to a good state. */
	if (ferror(fp)) {
		fclose(fp);
		gtk_editable_delete_text(GTK_EDITABLE(text), 0, -1);
		gtk_text_thaw(GTK_TEXT(text));
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2,
				   "Error loading file.");
		return;
	}
	fclose(fp);
	gtk_text_thaw(GTK_TEXT(text));
	current_filename = g_strdup(filename);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2,
			   current_filename);
}

/*****************************************************************************
 * load file into studypad during program startup
 *****************************************************************************/
void loadStudyPadFile(gchar * filename)
{				
	GtkWidget *statusbar, *text;
	FILE *fp;
	gchar buffer[BUFFER_SIZE];
	gint bytes_read;
	gint context_id2;

	statusbar = lookup_widget(MainFrm, "statusbar2");
	text = lookup_widget(MainFrm, "text3");
	context_id2 =
	    gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar),
					 "GnomeSword");
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);
	gtk_text_freeze(GTK_TEXT(text));
	gtk_editable_delete_text(GTK_EDITABLE(text), 0, -1);
	g_free(current_filename);
	current_filename = NULL;
	file_changed = FALSE;
	fp = fopen(filename, "r");
	if (fp == NULL) {
		gtk_text_thaw(GTK_TEXT(text));
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2, filename);	/* Could not open file */
		return;
	}
	for (;;) {
		bytes_read = fread(buffer, sizeof(gchar), BUFFER_SIZE, fp);
		if (bytes_read > 0)
			gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL,
					buffer, bytes_read);

		if (bytes_read != BUFFER_SIZE && (feof(fp) || ferror(fp)))
			break;
	}
	/* If there is an error while loading, we reset everything to a good state */
	if (ferror(fp)) {
		fclose(fp);
		gtk_editable_delete_text(GTK_EDITABLE(text), 0, -1);
		gtk_text_thaw(GTK_TEXT(text));
		gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2,
				   "Error loading file.");
		return;
	}
	fclose(fp);
	gtk_text_thaw(GTK_TEXT(text));
	current_filename = g_strdup(filename);
	gtk_statusbar_pop(GTK_STATUSBAR(statusbar), context_id2);
	gtk_statusbar_push(GTK_STATUSBAR(statusbar), context_id2,
			   current_filename);
}

/*****************************************************************************
 * save our program settings
 *****************************************************************************/
void writesettings(SETTINGS settings)
{
	int fd;	/* file handle */
        //g_warning("saved com page = %d",settings.notebook1page);
	fd = open(fnconfigure, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);	/* open file (settings.cfg) */
	write(fd, (char *) &settings, sizeof(settings));	/* save structure to disk */
	close(fd);		/* close file */
}


/**********************************************************************************************
 * read settings.cfg into settings structure
 *
 **********************************************************************************************/
SETTINGS readsettings(void)
{
	int fd;			/* file handle */
	SETTINGS settings;	/* settings structure */
	long filesize;
	struct stat stat_p;

	stat(fnconfigure, &stat_p);
	filesize = stat_p.st_size;

	if (filesize != sizeof(settings)) {	/* if file is not the same size as the structure */
		settings = createsettings();	/* we will create new file */
		return (settings);	/* return settings structure to initSword() in GnomeSword.cpp  */
	}

	if ((fd = open(fnconfigure, O_RDONLY)) == -1) {	/* try to open file */
		settings = createsettings();	/* if we cannot open file we will create new one */
	} else {
		read(fd, (char *) &settings, sizeof(settings));	/* read file into structure */
	}
	close(fd);		/* close file */

	return (settings);	/* return settings structure to initSword() in GnomeSword.cpp  */
}



/**************************************************************************************************
 *create settings structure and settings.cfg file
 *
 **************************************************************************************************/
SETTINGS createsettings(void)
{
	SETTINGS settings;	/* gnomesword settings structure */
	SETTINGS *p_settings;	/* pointer to settings structure */
	int fd;			/* handle for settings file */

	p_settings = &settings;	/* set p_settings to point to settings structure - we have to do this set the values of the structure */
	strcpy(p_settings->MainWindowModule, "KJV");	/* set main window module */
	strcpy(p_settings->Interlinear1Module, "BBE");	/* set first interlinear window module */
	strcpy(p_settings->Interlinear2Module, "WEB");	/* set second interlinear window module */
	strcpy(p_settings->Interlinear3Module, "Byz");	/* set third interlinear window module */
	strcpy(p_settings->Interlinear4Module, "NIV");	/* set second interlinear window module */
	strcpy(p_settings->Interlinear5Module, "NASB");	/* set third interlinear window module */	
	strcpy(p_settings->personalcommentsmod, "-+*Personal*+-");	/* personal comments module */
	strcpy(p_settings->currentverse, "Romans 8:28");	/* set openning verse */
	strcpy(p_settings->dictkey, "GRACE");	/* dictionary key to use at program startup - the one we shut down with */
	strcpy(p_settings->studypadfilename, "");	/* name of file in open in study when we closed or last file in studypad */
	strcpy(p_settings->currentverse_color, "#2A9C83");  /* current verse color */
	p_settings->currentverse_red = 0x0000;	/* set current verse color to green */
	p_settings->currentverse_green = 0x7777;
	p_settings->currentverse_blue = 0x0000;
	p_settings->strongs = FALSE;	/* set strongs numbers to off */
	p_settings->footnotes = FALSE;	/* set footnotes to off */
	p_settings->versestyle = TRUE;	/* set versestyle to on */
	p_settings->interlinearpage = TRUE;	/* set show interlinear page to on */
	p_settings->autosavepersonalcomments = TRUE;	/* set autosave personal comments to on */
	p_settings->formatpercom = FALSE;	/* set personal comments formatting to off */
	p_settings->notebook3page = 0;	/* notebook 3 page number */
	p_settings->notebook1page = 0;	/* commentaries notebook */
	p_settings->notebook2page = 0;	/* dict and lex notebook  */
	p_settings->shortcutbarsize = 120; /*width of shortcutbar */
	p_settings->showcomtabs = FALSE;	/* show tabs on commentary notebook */
	p_settings->showdicttabs = FALSE;	/* show tabs on dict/lex notebook */
	p_settings->showshortcutbar = TRUE;	/* show the shortcut bar */
	p_settings->showtextgroup = FALSE;	/* show text group */
	p_settings->showcomgroup = TRUE;	/* show com group */
	p_settings->showdictgroup = TRUE;	/* show dict/lex group */
	p_settings->showbookmarksgroup = FALSE;	/* show bookmark group */
	p_settings->showhistorygroup = TRUE;	/* show history group */
	fd = open(fnconfigure, O_WRONLY | O_CREAT, S_IREAD | S_IWRITE);	/* create settings file (settings.cfg) */
	write(fd, (char *) &settings, sizeof(settings));	/* save settings strucTRUE to file */
	close(fd);		/* close the file */
	return (settings);	/* return settings structure to readsettings(void) */
}


/********************************************************************************************************** 
 * save results list form search dialog *filename = pointer to filename from fileselection dialogure
 * list is results list widget where results are stored 
 **********************************************************************************************************/
void savelist(gchar * filename, GtkWidget * list)
{
	GtkWidget *label;	/* label from search dialog that show number of finds */
	gchar *text;		/* pointer to text in the label */
	gint howmany;		/* integer storage for number of finds */
	FILE *fp;		/* file pointer */
	gint i = 0,		/* counters */
	 j = 0;
	gchar buf[255],		/* string storage for each list item */
	*buftmp;		/* pointer for list to text */

	label = lookup_widget(list, "lbSearchHits");	/* get the label */
	text = (char *) GTK_LABEL(label)->label;	/* get text from label */
	howmany = atoi(text);	/* convert to number */
	//cout  << text;        
	if (howmany < 1)
		return;		/* if less than 1 the list is empty lets quit and go back to our program */
	fp = fopen(filename, "w");	/* open file (filename) */
	if (fp == NULL) {	/* if fopen fails lets quit and return to our program */
		return;
	}
	while (i < howmany) {	/* iterate through the results list */
		gtk_clist_get_text(GTK_CLIST(list), i, 0, &buftmp);	/* point to text data in each row (i)  */
		sprintf(buf, "%s", buftmp);	/* put text data into our buf string */
		for (j = 0; j < 79; j++) {	/* iterate through each string */
			if (buf[j] == '\0') {	/* look for  null */
				buf[j] = '\n';	/* replace null with newline ('\n') */
				buf[j + 1] = '\0';	/* put null after newline */
				break;	/* leave for(;;) loop */
			}
		}
		fputs(buf, fp);	/* write string to file (filename) */
		++i;		/* increment i by one */
	}
	fputs("-end-", fp);	/* mark end -  */
	fclose(fp);		/* close file (filename) */
}


GtkWidget *create_fileselection1(void)
{
	GtkWidget *fileselection1;
	GtkWidget *ok_button1;
	GtkWidget *cancel_button1;

	fileselection1 =
	    gtk_file_selection_new("GtkSword - Open Note File");
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
			   GTK_SIGNAL_FUNC(on_ok_button1_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(cancel_button1), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button1_clicked),
			   NULL);

	return fileselection1;
}

GtkWidget *create_fileselectionSave(void)
{
	GtkWidget *fileselectionSave;
	GtkWidget *ok_button2;
	GtkWidget *cancel_button2;

	fileselectionSave =
	    gtk_file_selection_new("GtkSword - Save Note File");
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
			   GTK_SIGNAL_FUNC(on_ok_button2_clicked), NULL);
	gtk_signal_connect(GTK_OBJECT(cancel_button2), "clicked",
			   GTK_SIGNAL_FUNC(on_cancel_button2_clicked),
			   NULL);

	return fileselectionSave;
}


