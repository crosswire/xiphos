/***************************************************************************
                          display.cpp  -  description
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

/********************************************************************\
**********************************************************************
**  this code was taken from the Sword Cheatah program				**
**  and modfied to handle some of the GBF stuff. Also added			**
**  suport for the x symbol font when using greek modules.  		**
**  2000/07/10 - added some support for the RWP module				** 
**********************************************************************
\********************************************************************/

#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>
#include <versekey.h>

#include "display.h"
#include "support.h"
#include "GnomeSword.h"

GdkColor GTKEntryDisp::colourBlue;
GdkColor GTKEntryDisp::colourGreen;
GdkColor GTKEntryDisp::colourRed;
GdkColor myGreen;
GdkColor BGcolor;	
GdkFont  *roman_font,
 	*italic_font,
 	*versenum_font,
 	*bold_font;
				 	
gchar	*font_mainwindow ="-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
	*font_italic_mainwindow = "-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1",
	*font_interlinear="-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
	*font_italic_interlinear="-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1",
	*font_currentverse="-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
	*font_italic_currentverse="-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1";
				 	
extern SWMgr *mainMgr;
extern SWMgr *mainMgr1;
extern bool bVerseStyle;
extern GtkWidget *MainFrm; //-- pointer to app -- declared in GnomeSword.cpp

//-----------------------------------------------------------------------------------------------
char 
GTKEntryDisp::Display(SWModule &imodule) 
{
	char tmpBuf[255];
	GdkFont *sword_font;
	ModMap::iterator it;	
	
    /* Load a  font */
	sword_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");

	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete (GTK_TEXT (gtkText), gtk_text_get_length((GTK_TEXT(gtkText))));
	int curPos = 0;
	(const char *)imodule;	// snap to entry
	gtk_text_freeze (GTK_TEXT(gtkText));
	
	//-- let's find out if we have a comment or dict module	
	it = mainMgr->Modules.find(imodule.Name());
	if (strcmp((*it).second->Type(), "Commentaries"))
		sprintf(tmpBuf, "[%s][ %s] ", imodule.Name(),imodule.KeyText()); //-- if not commentaries add module name to text widget
	else
		sprintf(tmpBuf, "[%s] ",imodule.KeyText());    //-- else just the keytext
	if(((*mainMgr->config->Sections[imodule.Name()].find("ModDrv")).second == "RawFiles") &&  //-- check for personal comments by finding ModDrv=RawFiles
				      (GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active))            //-- check for edit mode
	{
	  GtkWidget *statusbar;  //-- pointer to comments statusbar
  	gint  context_id2;     //-- statusbar context_id ???
  	sprintf(tmpBuf, "[%s][ %s] ", imodule.Name(),imodule.KeyText()); //-- add module name and verse to edit note statusbar
   	//-- setup statusbar for personal comments
		statusbar = lookup_widget(MainFrm, "sbNotes"); //-- get stutusbar
		context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "GnomeSword"); //-- get context id
		gtk_statusbar_pop (GTK_STATUSBAR (statusbar),context_id2 );   //-- ready status
		gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, tmpBuf); //-- show modName and verse ref in statusbar
		gtk_text_insert(GTK_TEXT(gtkText),sword_font , &gtkText->style->black, NULL, " ", -1);		
	}
	else //-- not using personal comment module in edit mode
	{	
		gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL, tmpBuf, -1);	//-- show modName and verse ref in text widget
	}
  //-- show module text for current key
  
	gtk_text_insert(GTK_TEXT(gtkText),sword_font , &gtkText->style->black, NULL, (const char *)imodule, -1);
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
}

//-----------------------------------------------------------------------------------------------
char 
GTKPerComDisp::Display(SWModule &imodule) 
{
	char tmpBuf[255];
	GdkFont *sword_font;
	ModMap::iterator it;	
	
    /* Load a  font */
	sword_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");

	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete (GTK_TEXT (gtkText), gtk_text_get_length((GTK_TEXT(gtkText))));
	int curPos = 0;
	(const char *)imodule;	// snap to entry
	gtk_text_freeze (GTK_TEXT(gtkText));
	
	//-- let's find out if we have a comment or dict module	
	it = mainMgr->Modules.find(imodule.Name());
	if (strcmp((*it).second->Type(), "Commentaries"))
		sprintf(tmpBuf, "[%s][ %s] ", imodule.Name(),imodule.KeyText()); //-- if not commentaries add module name to text widget
	else
		sprintf(tmpBuf, "[%s] ",imodule.KeyText());    //-- else just the keytext
	if(((*mainMgr->config->Sections[imodule.Name()].find("ModDrv")).second == "RawFiles") &&  //-- check for personal comments by finding ModDrv=RawFiles
				      (GTK_TOGGLE_BUTTON(lookup_widget(MainFrm,"btnEditNote"))->active))            //-- check for edit mode
	{
	  GtkWidget *statusbar;  //-- pointer to comments statusbar
  	gint  context_id2;     //-- statusbar context_id ???
  	sprintf(tmpBuf, "[%s][ %s] ", imodule.Name(),imodule.KeyText()); //-- add module name and verse to edit note statusbar
   	//-- setup statusbar for personal comments
		statusbar = lookup_widget(MainFrm, "sbNotes"); //-- get stutusbar
		context_id2 = gtk_statusbar_get_context_id(GTK_STATUSBAR(statusbar), "GnomeSword"); //-- get context id
		gtk_statusbar_pop (GTK_STATUSBAR (statusbar),context_id2 );   //-- ready status
		gtk_statusbar_push (GTK_STATUSBAR (statusbar), context_id2, tmpBuf); //-- show modName and verse ref in statusbar
		gtk_text_insert(GTK_TEXT(gtkText),sword_font , &gtkText->style->black, NULL, " ", -1);		
	}
	else //-- not useing personal comment module in edit mode
	{	
		gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL, tmpBuf, -1);	//-- show modName and verse ref in text widget
	}
  //-- show module text for current key
  
	gtk_text_insert(GTK_TEXT(gtkText),sword_font , &gtkText->style->black, NULL, (const char *)imodule, -1);
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
}

//-----------------------------------------------------------------------------------------------
char                                   //-- this will handle some of the GBF format
GTKInterlinearDisp::Display(SWModule &imodule) 
{
	char tmpBuf[255];
	GdkFont *sword_font,
					*greek_font;
	gchar    *myname;
	bool greek,
			findclose,
			italics_on=FALSE;
	char 	verseBuf[800],
 				buf[800];
	char 	*myverse,
				*font;
	int i,j,len;	

	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;

	font = "Roman";
	if ((sit = mainMgr1->config->Sections.find(imodule.Name())) != mainMgr1->config->Sections.end()) 
	{
		if ((eit = (*sit).second.find("Font")) != (*sit).second.end()) 
		{
			font = (char *)(*eit).second.c_str();
		}
	}

	/* Load a italic font */
 	italic_font = gdk_font_load ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a bold font */
 	bold_font = gdk_font_load ("-adobe-helvetica-bold-r-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a roman font */
 	roman_font = gdk_font_load(font_interlinear);
	/* Load a verse number font */
 	versenum_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
  /* Load a greek font */
	greek_font = gdk_font_load ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");

	/******************************************************************** check for greek module that use symbol font */
	greek = FALSE;	
	
	if(!strcmp(font,"Symbol"))
	{
		greek=TRUE;
  	sword_font = greek_font;
  }	
	else
	{
		sword_font = roman_font;
		greek=FALSE;
	}
	findclose = FALSE; 
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete (GTK_TEXT (gtkText), gtk_text_get_length((GTK_TEXT(gtkText))));
	int curPos = 0;
	(const char *)imodule;	// snap to entry
	gtk_text_freeze (GTK_TEXT(gtkText));
	sprintf(tmpBuf, "[%s][ %s] ", imodule.Name(),imodule.KeyText());
	gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL, tmpBuf, -1);
	
	if(greek) 
		gtk_text_insert(GTK_TEXT(gtkText),sword_font , &gtkText->style->black, NULL, (const char *)imodule, -1);
	else
	{
		i=j=0;
		verseBuf[0]='\0';
		myverse = g_strdup((const char *)imodule);
		len = strlen(myverse);
		while(i<len)
		{
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='I')
   		{				
				i=i+4;
				gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, verseBuf, -1);
				j=0;
				verseBuf[0]='\0';
    			//myverse[i] = '[';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='i')
   		{
				i=i+4;
				gtk_text_insert(GTK_TEXT(gtkText), italic_font, &gtkText->style->black, NULL, verseBuf, -1);
				j=0;
				verseBuf[0]='\0';
    			//myverse[i] = ']';
    	}			
			if(myverse[i] == '<' && myverse[i+1] =='P' && myverse[i+2]=='P')
   		{				
				i=i+4;
				sprintf(buf,"%s\n\n",verseBuf);
				gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, buf, -1);
				j=0;
				verseBuf[0]='\0';
				italics_on = TRUE;
    	}
			if(myverse[i] == '<' && myverse[i+1] =='P' && myverse[i+2]=='p')
   		{
				i=i+4;
				sprintf(buf,"%s\n",verseBuf);
				gtk_text_insert(GTK_TEXT(gtkText), italic_font, &gtkText->style->black, NULL, buf, -1);
				j=0;
				verseBuf[0]='\0';
    		italics_on = FALSE;
    	}
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='B')
   		{				
				i=i+4;
				gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, verseBuf, -1);
				j=0;
				verseBuf[0]='\0';
    			//myverse[i] = '[';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='b')
   		{
				i=i+4;
				gtk_text_insert(GTK_TEXT(gtkText), bold_font, &gtkText->style->black, NULL, verseBuf, -1);
				j=0;
				verseBuf[0]='\0';
    			//myverse[i] = ']';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='C')
   		{				
    			while(myverse[i] != '>')
    			{
    				++i;
	    		}
					++i;
    	}
			if(myverse[i] == '<' && myverse[i+1] =='R' && myverse[i+2]=='F')
   		{				
				i=i+3;
    			myverse[i] = '{';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='R' && myverse[i+2]=='f')
   		{				
				i=i+3;
    			myverse[i] = '}';
    	}
    	if(myverse[i] == '<' && myverse[i+1] =='R' && myverse[i+2]=='B')
    	{				
				i=i+3;
    		myverse[i] = '*';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='G')
    		{				
					i=i+2;
    			myverse[i] = '<';
    		}
			if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='H')
    		{				
					i=i+2;
    			myverse[i] = '<';
    		}
			if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='T')
    		{				
					i=i+3;
    			myverse[i] = '(';
				findclose = TRUE;			
    		}
			if(findclose)
			{
				if(myverse[i] == '>')
				{
					myverse[i] = ')';
					findclose=FALSE;
				}
			}
			verseBuf[j] = myverse[i];
	    	++i;    		
	    	verseBuf[j+1] = '\0';
	    	++j;
		}		
		if(italics_on) sword_font = italic_font;
		else sword_font = roman_font;
		gtk_text_insert(GTK_TEXT(gtkText), sword_font, &gtkText->style->black, NULL, verseBuf, -1);		
		verseBuf[0]='\0';
	}
	/**************************************************************************************************************************************/
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
}

//-----------------------------------------------------------------------------------------------
char                                   //-- this will handle  Robertson's Word Pictures in the New Testament (RWP) format?????????
GTKRWPDisp::Display(SWModule &imodule)
{
	GdkFont	 *sword_font, //-- pointers to fonts
				*greek_font;
	//gchar   	*myname; 
	bool		italics_on=FALSE, //-- boolean switches
				greek_on=false,
				bold_on=false,
				first_time=true,
				first_letter=true;
	char		verseBuf[800], //-- work strings
				tmpBuf[800],
				buf[800];
	char		*myverse, //-- pointers to strings
				*font; 
	int		 i,j,len; //-- integer vars	


	/* Load a italic font */
	italic_font = gdk_font_load ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a bold font */
	bold_font = gdk_font_load ("-adobe-helvetica-bold-r-normal-*-*-120-*-*-p-*-iso8859-1");
	/* Load a roman font */
	roman_font = gdk_font_load(font_interlinear);
	/* Load a verse number font */
	versenum_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
	/* Load a greek font */
	greek_font = gdk_font_load ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");
	//-- prepare text widget 
	gtk_text_set_point(GTK_TEXT(gtkText), 0); //-- set position to begining of text
	gtk_text_forward_delete (GTK_TEXT (gtkText), gtk_text_get_length((GTK_TEXT(gtkText)))); //-- clear to end of text
	gtk_text_freeze (GTK_TEXT(gtkText)); //-- to pervent flicker and movement
	
	(const char *)imodule;	// snap to entry

	i=j=0; //-- set counters to 0 - i is counter for myverse[] - j is counter for verseBuf
	verseBuf[0]='\0'; //-- set buf to null
	myverse = g_strdup((const char *)imodule); //-- get string from sword module
	len = strlen(myverse); //-- get length of string
	while(i<len) //-- loop until we reach the end of the text string
	{
		if(myverse[i] == 32  || myverse[i] == '\\') first_letter = true; //-- we need to know if we have a space or a backslash -- first letter of a word follows
		if(myverse[i] == '{' ) //-- remove the open curly bracket ({)
		{				
			++i; //-- increment i to revome the {
			if(first_time) //-- this is the first line to go to the text widget - we don't want any line feed here
			{
				sprintf(tmpBuf,"%s",verseBuf);
				first_time = false;
			}
			else //-- else add two line feeds to seperate sections of text
			{
				sprintf(tmpBuf,"%s\n\n",verseBuf);		
			}
			gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, tmpBuf, -1); //-- print what is in the buffer  so we can bold the what follows (text between {})
			j=0; //-- set verseBuf counter to 0
			verseBuf[0]='\0'; //-- empty verseBuf 
			bold_on =  true; 		
    	}
		if(myverse[i] == '}') //-- remove the close curly bracket (}) and print bold text
		{
			++i;//-- increment i to revome the }
			gtk_text_insert(GTK_TEXT(gtkText), bold_font, &gtkText->style->black, NULL, verseBuf, -1); //-- print what is in the buffer 
			j=0; //-- set verseBuf counter to 0
			verseBuf[0]='\0'; //-- empty verseBuf 
			bold_on = false;  //-- turn bold font off
		}			
		if(myverse[i ] == '\\') //-- remove first backslash and turn greek font on
		{				
			if(!greek_on) //-- we don't want to be here is greek is already on
			{
				++i; //-- increment i to remove the backslash
				gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, verseBuf, -1); //-- print what is in the buffer so we can use greek font
				j=0; //-- set verseBuf counter to 0
				verseBuf[0]='\0'; //-- empty verseBuf 
				greek_on = TRUE;
			}
		}
		if(myverse[i] == '\\') //-- remove secound backslash and turn greek font off
		{
			if(greek_on) //-- only go here is greek is on
			{
				++i; //-- increment i to remove the backslash
				gtk_text_insert(GTK_TEXT(gtkText), greek_font, &gtkText->style->black, NULL, verseBuf, -1);
				j=0; //-- set verseBuf counter to 0;
				verseBuf[0]='\0'; //-- empty verseBuf 
    				greek_on = FALSE; //-- turn greet font off
    			}
		}
		if(greek_on) //-- if greek is on let's try to replace characters that don't print right with the symbol font with some that will
		{
    		//gchar buftext[80];    			
    		//sprintf(buftext,"%3.3d\n",myverse[i]);
    		//cout << buftext;    			
    		if(myverse[i] == 't' && myverse[i+1] == 'h') //-- replace th with q (theta)
    		{										
    			++i;
    			myverse[i] = 'q';
    		}
    		if(myverse[i] == 's')	 //-- end of word s with V
    		{						 //-- sigma with end of word sigma
    			if(myverse[i+1] == ' ' || myverse[i+1] == ')' || myverse[i+1] == '\\')
    			myverse[i] = 'V';     				
    		}
				if(myverse[i] == 'c' && myverse[i+1] == 'h') //-- ch with c (chi)
    		{										
    			++i;
    			myverse[i] = 'c';
    		}
				if(myverse[i] == 'p' && myverse[i+1] == 'h') //-- ph with f (phi)
    		{											
    			++i;
    			myverse[i] = 'f';
    		}
    		if(myverse[i] == 39) //-- skip ' (39)  - i don't know what this is
    		{
    			++i;
    		}
    		if(myverse[i] == -120) //-- replace nonprintable with h (eta)
    		{
    			if(myverse[i+1] == 'i') ++i;
    			myverse[i] = 'h';
    		}
    		if(myverse[i] == -125) //-- replace nonprintable with a (alpha)
    		{
    			if(myverse[i+1] == 'i') ++i;
    			myverse[i] = 'a';
    		}
    		if(myverse[i] == -109) //-- replace nonprintable with w (omega)
    		{
    			if(myverse[i+1] == 'i') ++i;
    			myverse[i] = 'w';
    		}    		
    		if(first_letter) //-- if first letter of greek word -- if a vowel we must remove the h which i guess to a rough breathing we can not show this with the symbol font so we remove it
    		{
    			if(myverse[i] == 'h' && myverse[i+1] == 'o')
    			{
    				++i;
    				first_letter = false;
    			}
    			if(myverse[i] == 'h' && myverse[i+1] == 'a')
    			{
    				++i;
    				first_letter = false;
    			}
    			if(myverse[i] == 'h' && myverse[i+1] == 'w')
    			{
    				++i;
    				first_letter = false;
    			}
    			if(myverse[i] == 'h' && myverse[i+1] == 'u')
    			{
    				++i;
    				first_letter = false;
    			}
    			if(myverse[i] == 'h' && myverse[i+1] == -120)
    			{
    				++i;
    				myverse[i] = 'h';
    				first_letter = false;
    			}
    			if(myverse[i] == 'h' && myverse[i+1] == 'i')
    			{
    				++i;
    				first_letter = false;
    			}    			    				   				
    		}
    	}     					
		if(myverse[i] == '#') //-- remove # and start reference and red fond
		{			
			++i; //-- remove #
			gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, verseBuf, -1); //-- print current buffer and start new for reference
			j=0;
			verseBuf[0]='\0';
    	}
		if(myverse[i] == '|') //-- remove | and end reference and red fond
		{			
			++i; //-- remove #
			gtk_text_insert(GTK_TEXT(gtkText),roman_font, &colourRed, NULL, verseBuf, -1); //-- show ref
			j=0;
			verseBuf[0]='\0';
		}
		verseBuf[j] = myverse[i]; //-- move current char form myverse to verseBuf
    	++i;    		
    	verseBuf[j+1] = '\0'; //-- put null at end of verseBuf
    	++j;
	}
	//-- let's print what ever is left	
	if(greek_on) sword_font = greek_font;
	else if(bold_on) sword_font = bold_font;
	else sword_font = roman_font;
	gtk_text_insert(GTK_TEXT(gtkText), sword_font, &gtkText->style->black, NULL, verseBuf, -1);		
	verseBuf[0]='\0';
	//-- finish with the text widget
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_thaw(GTK_TEXT(gtkText));
}

//-----------------------------------------------------------------------------------------------
char                                //-- this will handle some GBF format a chapter at a time
GTKChapDisp::Display(SWModule &imodule) 
{
	char tmpBuf[255];
	char 	verseBuf[800],
 				buf[800];
	char 	*myverse,
 				*font;
	int i,j;	
	bool 	findclose=FALSE,
				italics_on=FALSE,
				poetry_on=FALSE,
				niv_on=false,
				FO_on=false;
	GdkFont *sword_font,
					*greek_font,
					*fo_font,
					*fo_italic_font;
	ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;

	font = "Roman";
  if(!strcmp(imodule.Name(),"NIV-GnomeSword")) niv_on=true; //-- we need to know if we are using the niv
  else niv_on=false;
	
		if ((sit = mainMgr->config->Sections.find(imodule.Name())) != mainMgr->config->Sections.end()) 
		{
			if ((eit = (*sit).second.find("Font")) != (*sit).second.end()) 
			{
				font = (char *)(*eit).second.c_str();
			}
		}
	
	
	//-- Load an italic font
        italic_font = gdk_font_load ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	//-- Load a roman font
        roman_font = gdk_font_load(font_mainwindow);
 	//-- Load FO font
 	     fo_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-80-*-*-p-*-iso8859-1");
 	//-- Load FO italic font
 	     fo_italic_font = gdk_font_load("-adobe-helvetica-medium-o-normal-*-*-80-*-*-p-*-iso8859-1");
	//-- Load a verse number font
	if(bVerseStyle)
        versenum_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
	else
        versenum_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-80-*-*-p-*-iso8859-1");

	//-- Load a greek font
	greek_font = gdk_font_load ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");
	
	if(!strcmp(font,"Symbol")) sword_font = greek_font;	
	else sword_font = roman_font;
	
	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete (GTK_TEXT (gtkText), gtk_text_get_length((GTK_TEXT(gtkText))));
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	int len;

	gtk_text_freeze (GTK_TEXT(gtkText));
	for (key->Verse(1); (key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error()); imodule++) 
	{
		sprintf(tmpBuf, " %d. ", key->Verse());
		gtk_text_insert(GTK_TEXT(gtkText), versenum_font, &colourBlue, NULL, tmpBuf, -1);
		i=j=0;
		verseBuf[0]='\0';
		myverse = g_strdup((const char *)imodule);
		len = strlen(myverse);
		while(i<len)
		{
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='O') //-- begin gbf italic
   		{				
				i=i+8;
				//-- we print what we have so far in standard font
				if (key->Verse() == curVerse) gtk_text_insert(GTK_TEXT(gtkText), roman_font, &myGreen, NULL, verseBuf, -1);
				else gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, verseBuf, -1);
				j=0;
				verseBuf[0]='\0'; //-- clear buffer and start over for italics
				FO_on=true;
    				//myverse[i] = '[';
    	}
    	if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='o') //-- begin gbf italic
   		{				
				i=i+7;
				//-- we print what we have so far in FO font
				gtk_text_insert(GTK_TEXT(gtkText), fo_font, &gtkText->style->black, NULL, verseBuf, -1);
				j=0;
				myverse[i]='\n';
				verseBuf[0]='\0'; //-- clear buffer and start over for italics
				FO_on=false;
    				//myverse[i] = '[';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='I') //-- begin gbf italic
   		{				
				i=i+4;
				//-- we print what we have so far in standard font
				if(FO_on)
				{
				   gtk_text_insert(GTK_TEXT(gtkText),fo_font ,&gtkText->style->black, NULL, verseBuf, -1);
				}
				else
				{
					if (key->Verse() == curVerse) gtk_text_insert(GTK_TEXT(gtkText), roman_font, &myGreen, NULL, verseBuf, -1);
					else gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, verseBuf, -1);
				}
				j=0;
				verseBuf[0]='\0'; //-- clear buffer and start over for italics
    				//myverse[i] = '[';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='i') //-- end gbf italic
   		{
				i=i+4;
				//-- we print what we have now in italic font
				if(FO_on)
				{
				   gtk_text_insert(GTK_TEXT(gtkText),fo_italic_font , &gtkText->style->black, NULL, verseBuf, -1);
				}
				else
				{
					if (key->Verse() == curVerse) gtk_text_insert(GTK_TEXT(gtkText), italic_font, &myGreen,NULL , verseBuf, -1);
					else gtk_text_insert(GTK_TEXT(gtkText), italic_font, &gtkText->style->black, NULL, verseBuf, -1);
				}
				j=0;
				verseBuf[0]='\0'; //-- clear buffer and start over
    				//myverse[i] = ']';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='P' && myverse[i+2]=='P') //-- start gbf poetry
   		{				
				i=i+4;
				sprintf(buf,"%s",verseBuf);
				if (key->Verse() == curVerse) gtk_text_insert(GTK_TEXT(gtkText), roman_font, &myGreen, NULL, buf, -1);
				else gtk_text_insert(GTK_TEXT(gtkText),roman_font , &gtkText->style->black, NULL, buf, -1);
				j=0;
				verseBuf[0]='\0';
				italics_on = TRUE;
				poetry_on = TRUE;
    				//myverse[i] = '[';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='P' && myverse[i+2]=='p') //-- end gbf poetry
   		{
				i=i+4;
				sprintf(buf,"%s",verseBuf);
				if (key->Verse() == curVerse) gtk_text_insert(GTK_TEXT(gtkText), italic_font, &myGreen,NULL , buf, -1);
				else gtk_text_insert(GTK_TEXT(gtkText), italic_font, &gtkText->style->black, NULL, buf, -1);
				j=0;
				verseBuf[0]='\0';
    		italics_on = FALSE;
    		poetry_on = FALSE;
    	}
			if(myverse[i] == '<' && myverse[i+1] =='C' && myverse[i+2] == 'M')	//-- gbf new line
			{
				if((!bVerseStyle) || poetry_on || niv_on) //-- we only need new line if we are not
				{                                         //-- in verse style or poetry is on or
					i=i+3;                                  //-- we are using the niv
					myverse[i] = '\t'; //-- add a tab to beginning of next line
					verseBuf[j] = '\n';//-- add new line to end of this line
					++j; 					
				}
			}		
			if(myverse[i] == '<' && myverse[i+1] =='C') //-- catch any C format stuff left
    		{				
    			while(myverse[i] != '>')
    			{
    				++i;
    			}
				++i;
    		}
			if(myverse[i] == '<' && myverse[i+1] =='R' && myverse[i+2]=='F')
    		{				
				i=i+3;
    			myverse[i] = '{';
    		}
			if(myverse[i] == '<' && myverse[i+1] =='R' && myverse[i+2]=='f')
    		{				
				i=i+3;
    			myverse[i] = '}';
    		}
    	if(myverse[i] == '<' && myverse[i+1] =='R' && myverse[i+2]=='B')
    	{				
				i=i+3;
    			myverse[i] = '*';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='G')
   		{				
				i=i+2;
    			myverse[i] = '<';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='H')
   		{				
				i=i+2;
    			myverse[i] = '<';
    	}
			if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='T' && myverse[i+3]=='G')
   		{				
				i=i+3;
    			myverse[i] = '(';
				findclose = TRUE;			
	   	}
			if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='T' && myverse[i+3]=='H')
   		{				
				i=i+3;
    			myverse[i] = '(';
				findclose = TRUE;			
	   	} 	    	
			if(findclose)
			{
				if(myverse[i] == '>')
				{
					myverse[i] = ')';
					findclose=FALSE;
				}
			}if(myverse[i] == '<' && myverse[i+1] =='W' && myverse[i+2]=='T' && myverse[i+3]=='P')
   		{				
				i=i+5;
    		//myverse[i] = ' ';	
	   	}
			verseBuf[j] = myverse[i];
	    	++i;    		
	    	verseBuf[j+1] = '\0';
	    	++j;
		}
		if(strcmp(font,"Symbol")) //-- make sure we are not using greek font
		{
			if(italics_on) sword_font = italic_font;
			else sword_font = roman_font;
		} 		
		if (key->Verse() == curVerse) 
		{
			gtk_text_thaw(GTK_TEXT(gtkText));
			adjVal = GTK_TEXT(gtkText)->vadj->upper;
			curPos = gtk_text_get_length(GTK_TEXT(gtkText));
			gtk_text_insert(GTK_TEXT(gtkText), sword_font, &myGreen, NULL, verseBuf, -1);
			gtk_text_freeze (GTK_TEXT(gtkText));
		}
		else 
		{
			gtk_text_insert(GTK_TEXT(gtkText), sword_font, &gtkText->style->black, NULL, verseBuf, -1);
		}
		verseBuf[0]='\0';
		//---------------------------------------------------------------------------- toggle paragraph style 
		if(bVerseStyle && (!poetry_on))
			gtk_text_insert(GTK_TEXT(gtkText), 	roman_font, &gtkText->style->black, NULL, "\n", -1); 

	}
	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
	adjVal -= (GTK_TEXT(gtkText)->vadj->page_size / 2);
	gtk_adjustment_set_value(GTK_TEXT(gtkText)->vadj, (adjVal>0)?adjVal:0);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
}

//-------------------------------------------------------------------------------------------
void
AboutModsDisplay(GtkWidget* text, gchar *aboutinfo) //-- to display Sword module about information
{
	gchar 	textBuf[800]; //-- text buffer to play with
	gint  	i,            //-- counter
					j,            //-- counter
					len;          //-- length of string aboutinfo
	bool 		printnow = false;  //-- tells us when to put our text to the screen
	
   	i=j=0;     //-- set to 0
		textBuf[0]='\0'; //-- empty text buffer
		gtk_text_set_point(GTK_TEXT(text), 0); //-- set position to begining of text widget
		gtk_text_forward_delete (GTK_TEXT (text), gtk_text_get_length((GTK_TEXT(text)))); //-- clear text widget
		len = strlen(aboutinfo);  //-- set len to length of aboutinfo
		while(i<len) //-- loop through string aboutinfo
		{
			if(aboutinfo[i] == '\\' && aboutinfo[i+1] =='p' && aboutinfo[i+2]=='a')//-- if we find \par replace with /n
   		{				
				if(aboutinfo[i+4] == 'd') i=i+4; //-- fix our counter to jump over \pard
				else i=i+3;		//-- fix our counter to jump over \par	
				aboutinfo[i]='\n'; //-- add the \n (new line)
				printnow = true;	//-- we want to show this much now			
    	}
    	if(aboutinfo[i] == '\\' && aboutinfo[i+1] =='q' && aboutinfo[i+2]=='c') //-- replace \qc with nothing
   		{				
				i=i+3;	//-- fix counter to jump over \qc
    	}					
			textBuf[j] = aboutinfo[i]; //-- move aboutinfo to textBuf one char at a time			
	 		++i;   //-- add one to i
	 		++j;   //-- add one to j		
	  	textBuf[j] = '\0';	//-- terminate our string with \0 (null)
	  	if(printnow)  //-- if true we print to text widget	
	  	{
	  		printnow = false; //-- set to false now
	  		gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, textBuf, -1);	  //-- put string to text widget
	  		 textBuf[0]='\0'; //-- set string to null
	  		 j = 0;  //-- set j to 0
	  	}
		}
		gtk_text_insert(GTK_TEXT(text), NULL, NULL, NULL, textBuf, -1);	//-- incase there is no \par at end of info	
		textBuf[0]='\0';
}
