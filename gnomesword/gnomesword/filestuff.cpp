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
#include "support.h"
#include "filestuff.h"

#define BUFFER_SIZE 8192	//-- input buffer size	
extern GtkWidget *MainFrm; //-- main window (form) widget
extern gchar *current_filename; //-- file loaded into study pad
extern gchar bmarks[50][80];	//-- array to store bookmarks - read in form file when program starts - saved to file after edit
extern gchar options[11][80];	//-- array to store a number of setting - read in form file when program starts - saved to file at normal shut down
extern gboolean file_changed;	//-- set to true if text is study pad has changed - and file is not saved
char	 	 *homedir,	//-- store $home directory
				*gSwordDir,	//-- store GnomeSword directory
				*fnbookmarks,	//-- store filename for bookmark file
				*fnconfigure;	//-- store filename for configure file - options
FILE 		 *configfile, //--  file pointer to configuration file - options
				*flbookmarks;	 //-- file pointer to bookmarks file	
		
gint 			ibookmarks;	//-- number of bookmark menu items		


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
	fnconfigure = g_new(char, strlen(gSwordDir) + strlen("gsword.cfg")+ 2); //-- set fnconfigure to gSwordDir + gsword.cfg
	sprintf(fnconfigure, "%s/%s", gSwordDir,"gsword.cfg" );
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
			if(mybuf[j] =='\n')
			{
 				mybuf[j] = '\0';
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
}

//--------------------------------------------------------------------------------------------
void
loadoptionarray(void) //---------------------------------------------- load settings form file
{
	gint i,j;
	gchar mybuf[255];
	if((configfile = fopen(fnconfigure,"r")) == NULL)
	{
		printf("open failed!");
		gtk_exit(1);
	}
	for(i=0;i<15;i++)
	{
		fgets(mybuf,79,configfile);
		for(j=0;j<79;j++)
		{
			if(mybuf[j] =='\n')
			{
 				mybuf[j] = '\0';
				break;
			}
			
		}
		strcpy(options[i],mybuf);
		//++i;
	}
	fclose(configfile);		
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
saveoptions(void) 
{
	FILE *configfile;	
	gint i = 0;
	gint j;

	configfile = fopen(fnconfigure,"w"); //-------------------------- save op setting, so we can open where we closed
	while(i<15)
	{
		for(j=0;j<79;j++)
		{
			if(options[i][j] == '\0') 
			{
				options[i][j] = '\n';
				options[i][j+1] = '\0';
				break;
			}
		}
		fputs(options[i],configfile);
		++i;
	}
	fclose(configfile);	
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
	if((f = fopen(fnconfigure,"w"))== NULL) //-------------------------- create settings file
	{
		printf("Can not create settings file");
		gtk_exit(1);
	}
	else
	{
		fputs("[GnomeSword]\n",f);
		fputs("KJV\n",f);
		fputs("KJV\n",f);
		fputs("KJV\n",f);
		fputs("KJV\n",f);
		fputs("TRUE\n",f);
		fputs("FALSE\n",f);
		fputs("FALSE\n",f);
		fputs("TRUE\n",f);
		fputs("Revelation of John 1:5\n",f);
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
