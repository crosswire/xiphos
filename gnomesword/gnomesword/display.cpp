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
**  this code was taken from the Sword Cheatah program							**
**  and modfied to handle some of the BTF stuff. Also added	 				**
**  suport for the x symbol font when using greek modules.     			**
**                             																			**                                                    	 **
**********************************************************************
\********************************************************************/

#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>
#include <versekey.h>

#include "display.h"

GdkColor GTKEntryDisp::colourBlue;
GdkColor GTKEntryDisp::colourGreen;
GdkColor GTKEntryDisp::colourRed;
GdkColor myGreen;
GdkColor BGcolor;	
GdkFont 	*roman_font,
				 	*italic_font,
				 	*versenum_font,
				 	*bold_font;
				 	
gchar			*font_mainwindow ="-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
					*font_interlinear="-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1",
					*font_currentverse="-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1";
				 	
extern SWMgr *mainMgr;
extern SWMgr *mainMgr1;
extern bool bVerseStyle;

//-----------------------------------------------------------------------------------------------
char 
GTKEntryDisp::Display(SWModule &imodule) 
{
	char tmpBuf[255];
	GdkFont *sword_font;

    /* Load a  font */
	sword_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");

	gtk_text_set_point(GTK_TEXT(gtkText), 0);
	gtk_text_forward_delete (GTK_TEXT (gtkText), gtk_text_get_length((GTK_TEXT(gtkText))));
	int curPos = 0;
	(const char *)imodule;	// snap to entry
	gtk_text_freeze (GTK_TEXT(gtkText));
	sprintf(tmpBuf, "[%s][ %s] ", imodule.Name(),imodule.KeyText());
	gtk_text_insert(GTK_TEXT(gtkText), NULL, &colourBlue, NULL, tmpBuf, -1);	

	gtk_text_insert(GTK_TEXT(gtkText),sword_font , &gtkText->style->black, NULL, (const char *)imodule, -1);

	gtk_text_set_point(GTK_TEXT(gtkText), curPos);
	gtk_text_thaw(GTK_TEXT(gtkText));
}

//-----------------------------------------------------------------------------------------------
char 
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
		myverse = (const char *)imodule;
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
char 
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
		myverse = (const char *)imodule;
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
				//-- we print what we have so far in standard font
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
					myverse[i] = '\t';
					verseBuf[j] = '\n';
					++j;
				}
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

/*
//-------------------------------------------------------------------------------------------
void
BFTextVerse(char* key, GtkWidget* textWindow, SWModule *module)
{

	char tmpBuf[255];
	GdkFont *sword_font;
	gchar    *myname;
	bool 	greek,
			findclose;
	char verseBuf[800];
	char 	*myverse,
			*font;
	int i,j,len;	
	GdkFont *roman_font,
		*italic_font,
		*versenum_font;
	ModMap::iterator it;
	SectionMap::iterator sit;
	ConfigEntMap::iterator eit;		

	font = "roman";
	
	// Load a italic font \\
        italic_font = gdk_font_load ("-adobe-helvetica-medium-o-normal-*-*-120-*-*-p-*-iso8859-1");
	// Load a roman font \\
        roman_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-120-*-*-p-*-iso8859-1");
	// Load a verse number font \\
        versenum_font = gdk_font_load("-adobe-helvetica-medium-r-normal-*-*-100-*-*-p-*-iso8859-1");
    // Load a greek font \\
	sword_font = gdk_font_load ("-adobe-symbol-medium-r-normal-*-*-140-*-*-p-*-adobe-fontspecific");

	//--------------------------------------------------------- check for greek module that uses symbol font
	greek = FALSE;	

	it = searchMgr->Modules.find(module->Name());
	if (it != searchMgr->Modules.end())
	{
		if ((sit = searchMgr->config->Sections.find((*it).second->Name())) != searchMgr->config->Sections.end())
		{
			if ((eit = (*sit).second.find("Font")) != (*sit).second.end())
			{
				font = (char *)(*eit).second.c_str();
			}
		}
	}
	if(!strcmp(font,"Symbol")) greek=TRUE;

	findclose = FALSE;
	gtk_text_set_point(GTK_TEXT(textWindow), 0);
	gtk_text_forward_delete (GTK_TEXT (textWindow), gtk_text_get_length((GTK_TEXT(textWindow))));
	int curPos = 0;
	(const char *)module;	// snap to entry
	module->SetKey(key);
	gtk_text_freeze (GTK_TEXT(textWindow));
	sprintf(tmpBuf, "[%s][ %s] ", module->Name(),module->KeyText());
	gtk_text_insert(GTK_TEXT(textWindow), NULL, NULL, NULL, tmpBuf, -1);
	
	if(greek)
		gtk_text_insert(GTK_TEXT(textWindow),sword_font , &textWindow->style->black, NULL, (const char *)*module, -1);
	else
	{
		i=j=0;
		verseBuf[0]='\0';
		strcpy(myverse,(const char *)*module);
		len = strlen(myverse);
		while(i<len)
		{
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='I')
    		{				
				i=i+4;
				gtk_text_insert(GTK_TEXT(textWindow),roman_font , &textWindow->style->black, NULL, verseBuf, -1);
				j=0;
				verseBuf[0]='\0';
    			//myverse[i] = '[';
    		}
			if(myverse[i] == '<' && myverse[i+1] =='F' && myverse[i+2]=='i')
    		{
				i=i+4;
				gtk_text_insert(GTK_TEXT(textWindow), italic_font, &textWindow->style->black, NULL, verseBuf, -1);
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
		gtk_text_insert(GTK_TEXT(textWindow), roman_font, &textWindow->style->black, NULL, verseBuf, -1);		
		verseBuf[0]='\0';
	}
	gtk_text_set_point(GTK_TEXT(textWindow), curPos);
	gtk_text_thaw(GTK_TEXT(textWindow));		
}
*/