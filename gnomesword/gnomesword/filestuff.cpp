/***************************************************************************
                          filestuff.cpp  -  description
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

#include "support.h"
#include "filestuff.h"
#include "menustuff.h"
#include "GnomeSword.h"
#include "listeditor.h"

#define BUFFER_SIZE 8192	//-- input buffer size	

//extern SETTINGS *settings;
extern GtkWidget *MainFrm; //-- main window (form) widget
extern gchar *current_filename; //-- file loaded into study pad
extern gchar bmarks[50][80];	//-- array to store bookmarks - read in form file when program starts - saved to file after edit
extern gboolean file_changed;	//-- set to true if text is study pad has changed - and file is not saved
char	 	 *homedir,	//-- store $home directory
				*gSwordDir,	//-- store GnomeSword directory
				*fnbookmarks,	//-- store filename for bookmark file
				*fnbookmarksnew,
				*fnconfigure;	//-- store filename for configure file - options
FILE 		 *configfile, //--  file pointer to configuration file - options
				*flbookmarks;	
		
gint 			ibookmarks;	//-- number of bookmark menu items		
gchar remembersubtree[256], //-- we need to use this when add to bookmarks
			rememberlastitem[255];//-- we need to use this when add to bookmarks

/*-----------------------------------------------------
check for GnomeSword dir
if not found - create it and
the files needed to run program
-----------------------------------------------------*/
void
setDiretory(void)
{
	if ((homedir = getenv("HOME")) == NULL) //-- get home dir
	{
		g_error("$HOME is not set!"); //-- if not found in env exit
		exit(0);
	}
 	gSwordDir = g_new(char, strlen(homedir) + strlen(".GnomeSword")+ 2); //-- set gSwordDir to $home + .GnomeSword
	sprintf(gSwordDir, "%s/%s", homedir,".GnomeSword" );
	fnbookmarks = g_new(char, strlen(gSwordDir) + strlen("bookmarks.txt")+ 2); //-- set fnbookmarks to gSwordDir + bookmarks.txt
	sprintf(fnbookmarks, "%s/%s", gSwordDir,"bookmarks.txt" );
	
	fnbookmarksnew = g_new(char, strlen(gSwordDir) + strlen("bookmarksnew.gs")+ 2); //-- set fnbookmarks to gSwordDir + bookmarks.txt
	sprintf(fnbookmarksnew, "%s/%s", gSwordDir,"bookmarksnew.gs" );
	
	fnconfigure = g_new(char, strlen(gSwordDir) + strlen("settings3.cfg")+ 2); //-- set fnconfigure to gSwordDir + gsword.cfg
	sprintf(fnconfigure, "%s/%s", gSwordDir,"settings3.cfg" );
	if(access(gSwordDir,F_OK) == -1)  //-- if gSwordDir does not exist create it
	{
		if((mkdir(gSwordDir,S_IRWXU)) == 0)
		{
			createFiles();
		}
		else
		{
			printf(".GnomeSword does not exist"); //-- if we can not create gSwordDir exit
			gtk_exit(1);
		}
	}	
}

//-------------------------------------------------------------------------------------------
void
loadbookmarks_programstart(void)
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flbookmarksnew;
	gchar 	  subtreelabel[255],
						firstsubtree[255];
	gint      ifirsttime = 1;	
	gint i=0;
	gchar tmpbuf[255];
	long filesize;
	
	struct stat stat_p;
	
	stat (fnbookmarksnew, &stat_p);
  filesize = stat_p.st_size;
  ibookmarks = (filesize / (sizeof(mylist)));
	p_mylist = &mylist;
	
	if((flbookmarksnew = open(fnbookmarksnew, O_RDONLY)) == -1) //-- try to open file
	{
		createFiles(); //-- create bookmark file if we cannot open the file
	}
	flbookmarksnew = open(fnbookmarksnew, O_RDONLY);
	while(i < ibookmarks)
	{
	  read(flbookmarksnew,(char *)&mylist,sizeof(mylist));					
		if(p_mylist->type == 1)  //--- if type is 1 it is a subtree (submenu)
		{			
			if(p_mylist->level == 0)
			{
				sprintf(subtreelabel, "%s%s",p_mylist->menu, p_mylist->preitem );
			}
			else
			{
			  sprintf(subtreelabel, "%s%s%s",p_mylist->menu, p_mylist->subitem, p_mylist->preitem );
			}
			sprintf(remembersubtree,p_mylist->item);
			addsubtreeitem(MainFrm, subtreelabel, p_mylist->item);			
		}		
		else
		{
			sprintf(subtreelabel, "%s%s%s",p_mylist->menu, p_mylist->subitem, p_mylist->preitem ); //-- create subitem label
			additemtosubtree(MainFrm,subtreelabel,p_mylist->item); //--- add menu item to subtree
		}		
	  ++i;
	}
	sprintf(rememberlastitem,"%s",p_mylist->item);
	//ibookmarks = i;
	close(flbookmarksnew);
}

//-------------------------------------------------------------------------------------------
void
loadbookmarks_afterSeparator(void)
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flbookmarksnew;
	gchar 	  subtreelabel[255];
	gint i=0;
	
	p_mylist = &mylist;
	flbookmarksnew = open(fnbookmarksnew, O_RDONLY);
	while(i < ibookmarks)
	{
	  read(flbookmarksnew,(char *)&mylist,sizeof(mylist));					
		if(p_mylist->type == 0 && p_mylist->level == 0)  //--- if type is 1 it is a subtree (submenu)
		{			
			sprintf(subtreelabel, "%s%s%s",p_mylist->menu, p_mylist->subitem, p_mylist->preitem ); //-- create subitem label
			additemtosubtree(MainFrm,subtreelabel,p_mylist->item); //--- add menu item to subtree
		}		
	  ++i;
	}
	sprintf(rememberlastitem,"%s",p_mylist->item);
	//ibookmarks = i+1;
	close(flbookmarksnew);
}

void
savebookmark(gchar *item) //-- save a bookmark that has been added to the bookmark menu by the add bookmark item
{
	LISTITEM mylist; //-- structure for bookmark item
	LISTITEM *p_mylist; //-- pointer to structure
	int flbookmarksnew; //-- bookmark file handle
	
	p_mylist = &mylist; //-- set pointer to structure
	flbookmarksnew = open(fnbookmarksnew, O_WRONLY|O_APPEND); //-- open file to append one record	
	p_mylist->type = 0; //-- item is bookmark not submenu		
	p_mylist->level = 0; //-- item is added at level one (show when you click bookmark menu)
	strcpy(p_mylist->item, item); //-- bookmark label (verse)
	strcpy(p_mylist->preitem, rememberlastitem), //-- item in menu to follow (you have to tell gnome where to insert the item)
	strcpy(p_mylist->subitem, ""); //-- item does not belong to a submenu
	strcpy(p_mylist->menu	,"_Bookmarks/"); //-- item does belong to the bookmark menu
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist)); //-- write the record to the bookmark file
	close(flbookmarksnew); //-- close the file we are done for now	
} 

//--------------------------------------------------------------------------------------------
void
createFiles(void) //--------------- create bookmark file nessary for GomeSword if it does not exist
{
	LISTITEM mylist;
	LISTITEM *p_mylist;
	int flbookmarksnew;
	
	p_mylist = &mylist;	
	flbookmarksnew = open(fnbookmarksnew, O_WRONLY|O_CREAT,S_IREAD|S_IWRITE);	
	
	p_mylist->type = 1;		
	p_mylist->level = 0;
	strcpy(p_mylist->item, "[What must I do to be saved?]");
	strcpy(p_mylist->preitem,"Edit Bookmarks"),
	strcpy(p_mylist->subitem	,"");
	strcpy(p_mylist->menu	,"_Bookmarks/");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
		
	p_mylist->type = 0;		
	p_mylist->level = 1;
	strcpy(p_mylist->item	,"Romans 1:16");
	strcpy(p_mylist->preitem,""),
	strcpy(p_mylist->subitem	,"[What must I do to be saved?]/");
	strcpy(p_mylist->menu	,"_Bookmarks/");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
	
 	p_mylist->type = 0;		
	p_mylist->level = 1;
	strcpy(p_mylist->item	,"Eph 2:8");
	strcpy(p_mylist->preitem,"Romans 1:16"),
	strcpy(p_mylist->subitem	,"[What must I do to be saved?]/");
	strcpy(p_mylist->menu	,"_Bookmarks/");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
		
	p_mylist->type = 0;		
	p_mylist->level = 1;
	strcpy(p_mylist->item	,"Acts  16:31");
	strcpy(p_mylist->preitem,"Eph 2:8"),
	strcpy(p_mylist->subitem	,"[What must I do to be saved?]/");
	strcpy(p_mylist->menu	,"_Bookmarks/");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
		
	p_mylist->type = 1;		
	p_mylist->level = 0;
	strcpy(p_mylist->item	,"[What is the Gospel?]");
	strcpy(p_mylist->preitem,"[What must I do to be saved?]"),
	strcpy(p_mylist->subitem	,"");
	strcpy(p_mylist->menu	,"_Bookmarks/");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
		
	p_mylist->type = 0;		
	p_mylist->level = 1;
	strcpy(p_mylist->item	,"1 Cor 15:1");
	strcpy(p_mylist->preitem,""),
	strcpy(p_mylist->subitem	,"[What is the Gospel?]/");
	strcpy(p_mylist->menu	,"_Bookmarks/");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
		
	p_mylist->type = 0;		
	p_mylist->level = 0;
	strcpy(p_mylist->item	,"Romans 8:28");
	strcpy(p_mylist->preitem,""),
	strcpy(p_mylist->subitem	,"");
	strcpy(p_mylist->menu	,"_Bookmarks/<Separator>");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
		
	p_mylist->type = 0;		
	p_mylist->level = 0;
	strcpy(p_mylist->item	,"Rev 1:5");
	strcpy(p_mylist->preitem,"Romans 8:28"),
	strcpy(p_mylist->subitem	,"");
	strcpy(p_mylist->menu	,"_Bookmarks/");
	write(flbookmarksnew,(char *)&mylist,sizeof(mylist));
		
	close(flbookmarksnew);
	ibookmarks = 8;
}

//-------------------------------------------------------------------------------------------
void
saveFile (gchar *filename)  //-- save studypad file
{
  	gchar *data, charbuf[255];
  	FILE *fp;
  	gint bytes_written, len;
	gint  context_id2;
	GtkWidget *statusbar;

	statusbar = lookup_widget(MainFrm, "statusbar2");
	context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "GtkSword");
	if (current_filename == NULL || strcmp (current_filename, filename))
    {
      g_free (current_filename);
      current_filename = g_strdup (filename);
    }
	data = gtk_editable_get_chars (GTK_EDITABLE (lookup_widget(MainFrm,"text3")), 0, -1);

	fp = fopen (filename, "w");
	if (fp == NULL)
    {
      g_free (data);
      return;
    }

	len = strlen (data);
	bytes_written = fwrite (data, sizeof (gchar), len, fp);
	fclose (fp);
	g_free (data);

	gtk_statusbar_pop (GTK_STATUSBAR (statusbar),context_id2 );
	if (len != bytes_written)
    {
      gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, "Error saving file.");
      return;
    }
	sprintf(charbuf,"%s - saved.",current_filename);

	gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, charbuf);
	file_changed = FALSE;	
}

//-------------------------------------------------------------------------------------------
void
loadFile (GtkWidget *filesel)  //-- load file into studypad
{
 	GtkWidget 	*statusbar,
 				 	 *text;
  	gchar filename[255];
  	FILE *fp;
  	gchar buffer[BUFFER_SIZE];
  	gint bytes_read;
  	gint  context_id2;

  	statusbar = lookup_widget(MainFrm,"statusbar2");
  	text  = lookup_widget(MainFrm,"text3");
  	context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "GnomeSword");
  	sprintf(filename,"%s", gtk_file_selection_get_filename (GTK_FILE_SELECTION (filesel)));
  	gtk_statusbar_pop (GTK_STATUSBAR (statusbar),context_id2 );
  	gtk_text_freeze (GTK_TEXT (text));
  	gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
  	g_free (current_filename);
  	current_filename = NULL;
  	file_changed = FALSE;
  	fp = fopen (filename, "r");
  	if (fp == NULL)
    {
      gtk_text_thaw (GTK_TEXT (text));
      gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, "Could not open file");
      return;
    }
  	for (;;)
    {
	    bytes_read = fread (buffer, sizeof (gchar), BUFFER_SIZE, fp);
      	if (bytes_read > 0)
			gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, buffer,
			 	bytes_read);

      	if (bytes_read != BUFFER_SIZE && (feof (fp) || ferror (fp)))
			break;
	}


  	// If there is an error while loading, we reset everything to a good state.   	

  	if (ferror (fp))
    {
      	fclose (fp);
      	gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      	gtk_text_thaw (GTK_TEXT (text));
      	gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, "Error loading file.");
      	return;
    }

  	fclose (fp);
  	gtk_text_thaw (GTK_TEXT (text));
  	current_filename = g_strdup (filename);
  	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), context_id2);
  	gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, current_filename);
}

//-------------------------------------------------------------------------------------------
void
loadStudyPadFile (gchar *filename)  //-- load file into studypad during program startup
{
 	GtkWidget 	*statusbar,
 				 	 *text;
  	//gchar filename[255];
  	FILE *fp;
  	gchar buffer[BUFFER_SIZE];
  	gint bytes_read;
  	gint  context_id2;

  	
  	
  	statusbar = lookup_widget(MainFrm,"statusbar2");
  	text  = lookup_widget(MainFrm,"text3");
  	context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "GnomeSword");
  	//sprintf(filename,"%s", file_name);
  	
  	gtk_statusbar_pop (GTK_STATUSBAR (statusbar),context_id2 );
  	gtk_text_freeze (GTK_TEXT (text));
  	gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
  	g_free (current_filename);
  	current_filename = NULL;
  	file_changed = FALSE;
  	fp = fopen (filename, "r");
  	if (fp == NULL)
    {
      gtk_text_thaw (GTK_TEXT (text));
      gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2,filename); // "Could not open file");
      return;
    }
  	for (;;)
    {
	    bytes_read = fread (buffer, sizeof (gchar), BUFFER_SIZE, fp);
      	if (bytes_read > 0)
			gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, buffer,
			 	bytes_read);

      	if (bytes_read != BUFFER_SIZE && (feof (fp) || ferror (fp)))
			break;
	}


  	// If there is an error while loading, we reset everything to a good state.   	

  	if (ferror (fp))
    {
      	fclose (fp);
      	gtk_editable_delete_text (GTK_EDITABLE (text), 0, -1);
      	gtk_text_thaw (GTK_TEXT (text));
      	gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, "Error loading file.");
      	return;
    }

  	fclose (fp);
  	gtk_text_thaw (GTK_TEXT (text));
  	current_filename = g_strdup (filename);
  	gtk_statusbar_pop (GTK_STATUSBAR (statusbar), context_id2);
  	gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, current_filename);
}

//----------------------------------------------------------------------------------------------
void                     //-- save our settings
writesettings(SETTINGS settings)
{
   int fd;     //-- file handle

   fd = open(fnconfigure, O_WRONLY|O_CREAT,S_IREAD|S_IWRITE);  //-- open file (settings.cfg)
   write(fd,(char *)&settings,sizeof(settings)); //-- save structure to disk
   close(fd);                     //-- close file
}

//----------------------------------------------------------------------------------------------
SETTINGS                  //-- read settings.cfg into settings structure
readsettings(void)
{
   int fd;           //-- file handle
   SETTINGS settings; //-- settings structure
   long filesize;
   struct stat stat_p;
	
	 stat(fnconfigure, &stat_p);
   filesize = stat_p.st_size;

   if(filesize != sizeof(settings)) //-- if file is not the same size as the structure
   {
      settings = createsettings();  //-- we will create new file
      return(settings); //-- return settings structure to initSword() in GnomeSword.cpp	
   }

   if((fd = open(fnconfigure, O_RDONLY))== -1) //-- try to open file
   {
   		settings = createsettings();  //-- if we cannot open file we will create new one
   }
   else
   {     		
	 		read(fd,(char *)&settings,sizeof(settings));  //-- read file into structure
	 }
	 close(fd);        //-- close file
	
	 return(settings); //-- return settings structure to initSword() in GnomeSword.cpp	
}


//----------------------------------------------------------------------------------------------
SETTINGS                    //-- create settings structure and settings.cfg file
createsettings(void)
{
	SETTINGS settings;   //-- gnomesword settings structure
	SETTINGS *p_settings; //-- pointer to settings structure
	int fd;              //-- handle for settings file
	
	p_settings = &settings;    //-- set p_settings to point to settings structure - we have to do this set the values of the structure
	strcpy(p_settings->MainWindowModule, "KJV");  //-- set main window module
	strcpy(p_settings->Interlinear1Module, "KJV");//-- set first interlinear window module
	strcpy(p_settings->Interlinear2Module, "KJV");//-- set second interlinear window module
	strcpy(p_settings->Interlinear3Module, "KJV");//-- set third interlinear window module
	strcpy(p_settings->personalcommentsmod, "-+*Personal*+-"); //-- personal comments module
	strcpy(p_settings->currentverse,"Romans 8:28"); //-- set openning verse
	strcpy(p_settings->dictkey, "GRACE"); //-- dictionary key to use at program startup - the one we shut down with
	strcpy(p_settings->studypadfilename, ""); //-- name of file in open in study when we closed or last file in studypad
	p_settings->currentverse_red = 0x0000;  //-- set current verse color to green
	p_settings->currentverse_green = 0x7777;
	p_settings->currentverse_blue = 0x0000;
	p_settings->strongs = false;   //-- set strongs numbers to off
	p_settings->footnotes = false; //-- set footnotes to off
	p_settings->versestyle = true; //-- set versestyle to on
	p_settings->interlinearpage = true; //-- set show interlinear page to on
	p_settings->autosavepersonalcomments = true; //-- set autosave personal comments to on
	p_settings->formatpercom = false; //-- set personal comments formatting to off
	p_settings->notebook3page = 0;  //-- notebook 3 page number
	p_settings->notebook1page = 0;  //-- commentaries notebook
	p_settings->notebook2page = 0;  //-- dict and lex notebook  	
	p_settings->showcomtabs  = false;  //-- show tabs on commentary notebook
	p_settings->showdicttabs = false;  //-- show tabs on dict/lex notebook
	p_settings->showshortcutbar = true; //-- show the shortcut bar;
	
	fd = open(fnconfigure, O_WRONLY|O_CREAT,S_IREAD|S_IWRITE);  //-- create settings file (settings.cfg)
	write(fd,(char *)&settings,sizeof(settings)); //-- save settings structrue to file
	close(fd);	                                  //-- close the file
	return(settings);                             //-- return settings structure to readsettings(void)
}

//---------------------------------------------------------------------------------------------
void					//--  save results list form search dialog *filename = pointer to filename from fileselection dialog
savelist(gchar *filename, GtkWidget *list)    //-- *list is results list widget where results are stored
{	
	GtkWidget 	*label; //-- label from search dialog that show number of finds
	gchar 			*text; //-- pointer to text in the label
	gint				howmany; //-- integer storage for number of finds
	FILE 	*fp; 	//-- file pointer
 	gint 	i = 0,  //-- counters
 				j = 0;
	gchar buf[255], //-- string storage for each list item
				*buftmp; //-- pointer for list to text
		
	label = lookup_widget(list,"lbSearchHits"); //-- get the label
	text = (char *)GTK_LABEL(label)->label; //-- get text from label
	howmany = atoi(text); //-- convert to number
	//cout  << text;	
	if(howmany < 1) return; //-- if less than 1 the list is empty lets quit and go back to our program
	fp = fopen (filename, "w"); //-- open file (filename)
  if (fp == NULL) //-- if fopen fails lets quit and return to our program
  {   	
   	return;
  }
	while(i<howmany)  //-- iterate through the results list
	{
		gtk_clist_get_text( GTK_CLIST(list), i, 0, &buftmp); //-- point to text data in each row (i) 
		sprintf(buf,"%s",buftmp); //-- put text data into our buf string
		for(j=0;j<79;j++) //-- iterate through each string
		{
			if(buf[j] == '\0') //-- look for  null
			{
				buf[j] = '\n'; //-- replace null with newline ('\n')
				buf[j+1] = '\0'; //-- put null after newline
				break; //-- leave for(;;) loop
			}
		}
		fputs(buf,fp); //-- write string to file (filename)
		++i; //-- increment i by one
	}
	fputs("-end-",fp); //-- mark end - 
	fclose(fp);	 //-- close file (filename)
}