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
#include <sys/stat.h>
#include <fcntl.h>

#include "support.h"
#include "filestuff.h"
#include "menustuff.h"
#include "GnomeSword.h"

#define BUFFER_SIZE 8192	//-- input buffer size	
//extern SETTINGS *settings;
extern GtkWidget *MainFrm; //-- main window (form) widget
extern gchar *current_filename; //-- file loaded into study pad
extern gchar bmarks[50][80];	//-- array to store bookmarks - read in form file when program starts - saved to file after edit
extern gboolean file_changed;	//-- set to true if text is study pad has changed - and file is not saved
char	 	 *homedir,	//-- store $home directory
				*gSwordDir,	//-- store GnomeSword directory
				*fnbookmarks,	//-- store filename for bookmark file
				*fnconfigure;	//-- store filename for configure file - options
FILE 		 *configfile, //--  file pointer to configuration file - options
				*flbookmarks;	 //-- file pointer to bookmarks file	
		
gint 			ibookmarks;	//-- number of bookmark menu items		
gchar remembersubtree[256];

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
	fnconfigure = g_new(char, strlen(gSwordDir) + strlen("settings.cfg")+ 2); //-- set fnconfigure to gSwordDir + gsword.cfg
	sprintf(fnconfigure, "%s/%s", gSwordDir,"settings.cfg" );
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

//--------------------------------------------------------------------------------------------
void
loadbookmarkarray(void) //------------------------------------------- load bookmarks form file
{                       //------------------------------------------- store in bmarks[]
	gint i,j;
	gchar mybuf[255];
	
	if((flbookmarks = fopen(fnbookmarks,"r")) == NULL) //-- open file
	{
		printf("bookmarks open failed!");  //-- if file does not exist exit
		gtk_exit(1);                       //-- this should not happen
	} 
	for(i=0;i<50;i++)
	{
		fgets(mybuf,79,flbookmarks);
		if(mybuf[0] == '-') //-- marks last line in file
		{
			ibookmarks = i;		
 			break;		
		}
		for(j=0;j<79;j++)   //-- remove '\n' line feed
		{
			if(mybuf[j] =='\n') //-- when we find a line feed
			{
 				mybuf[j] = '\0';  //-- replace it with a null
				break;
			}			
		}
		strcpy(bmarks[i],mybuf);
	}	
	fclose(flbookmarks);
	
}


//-------------------------------------------------------------------------------------------
void
editbookmarksSave(GtkWidget *editdlg)  //-------------------------- save bookmarks after edit
{
	GtkWidget *text; //-- for text widget from edit dlg
	gchar *data;     //-- data from text widget
	gint 	len,       //-- length of data
			 bytes_written; //-- number of bytes written to file
	FILE *flbookmarks;  //-- file handle

	text = lookup_widget(editdlg,"text4"); //-- get text widget
 	data = gtk_editable_get_chars (GTK_EDITABLE (text), 0, -1);	//-- get data from text widget
	flbookmarks = fopen (fnbookmarks, "w"); //-- open bookmarks file
	if (flbookmarks == NULL) //-- return if we don't get a handle
 	{
 		g_free (data);
  	return;
 	}
 	len = strlen (data); //-- get length of text data
 	bytes_written = fwrite (data, sizeof (gchar), len, flbookmarks ); //-- write data to file
 	fclose (flbookmarks); //-- close bookmarks file
 	g_free (data); //-- free data - do we need to do this?
  removemenuitems(MainFrm, remembersubtree, ibookmarks); //-- remove old bookmarks form menu -- menustuff.cpp	
  loadbookmarkarray(); //-- load edited bookmarks  -- filestuff.cpp
  loadbookmarks(MainFrm); //-- let's show what we did -- GnomeSword.cpp
}

//--------------------------------------------------------------------------------------------
void
savebookmarks(void) 
{
	gint i,j;
	flbookmarks = fopen(fnbookmarks,"w"); //-------------------------- save bookmarks
	i=0;
	while(i<ibookmarks)
	{
		for(j=0;j<79;j++)
		{
			if(bmarks[i][j] == '\0') 
			{
				bmarks[i][j] = '\n';
				bmarks[i][j+1] = '\0';
				break;
			}
		}
		fputs(bmarks[i],flbookmarks);
		++i;
	}
	fputs("-end-",flbookmarks);
	fclose(flbookmarks);		
}

//--------------------------------------------------------------------------------------------
void
createFiles(void) //--------------- files nessary for GomeSword if they do not exist
{
	FILE *f;
	if((f = fopen(fnbookmarks,"w"))==NULL)  //-------------------------- create bookmark file
	{
		printf("Can not create bookmark file");
		gtk_exit(1);
	}
	else
	{
		fputs("[What must I do to be saved?]\n",f);
		fputs("Romans 1:16\n",f);
		fputs("Eph 2:8\n",f);
		fputs("Acts  16:31\n",f);
		fputs("[What is  the Gospel?]\n",f);
		fputs("1 Cor 15:1\n",f);
		fputs("<personal>\n",f);
		fputs("Romans 8:28\n",f);
		fputs("Rev 1:5\n",f);
		fputs("John 3:16\n",f);
		fputs("-end-\n",f);
		fclose(f);	
	} 	
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
	p_settings->currentverse_red = 0x0000;  //-- set current verse color to green
	p_settings->currentverse_green = 0x7777;
	p_settings->currentverse_blue = 0x0000;
	p_settings->strongs = false;   //-- set strongs numbers to off
	p_settings->footnotes = false; //-- set footnotes to off
	p_settings->versestyle = true; //-- set versestyle to on
	p_settings->interlinearpage = true; //-- set show interlinear page to on
	p_settings->autosavepersonalcomments = true; //-- set autosave personal comments to on
	p_settings->notebook3page = 0;  //-- notebook 3 page number
	p_settings->notebook1page = 0;  //-- commentaries notebook
	p_settings->notebook2page = 0;  //-- dict and lex notebook
	
	
	
	fd = open(fnconfigure, O_WRONLY|O_CREAT,S_IREAD|S_IWRITE);  //-- create settings file (settings.cfg)
	write(fd,(char *)&settings,sizeof(settings)); //-- save settings structrue to file
	close(fd);	                                  //-- close the file
	return(settings);                             //-- return settings structure to readsettings(void)
}

