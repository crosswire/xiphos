/***************************************************************************
                          noteeditor.cpp  -  description
                             -------------------
    begin                : Tue Jul 18 2000
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

#include <gnome.h>

#include "noteeditor.h"
NoteEditor *noteeditor;
//----------------------------------------------------------------------------------------
NoteEditor::NoteEditor()
{
}

//----------------------------------------------------------------------------------------
NoteEditor::~NoteEditor(void)
{
	
}

//----------------------------------------------------------------------------------------
bool
NoteEditor::setBOLD(GtkWidget *text) //-- insert bold tags
{
	if(GTK_EDITABLE(text)->has_selection)  //-- do we have a selection?
	{
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "<B>", -1);
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "</b>", -1);
	  return true;
	}
	else return false;
}

//----------------------------------------------------------------------------------------
bool
NoteEditor::setITALIC(GtkWidget *text) //-- insert italic tags
{
	if(GTK_EDITABLE(text)->has_selection)  //-- do we have a selection?
	{
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "<I>", -1);
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "</i>", -1);
		return true;
	}
	else return false;
}

//----------------------------------------------------------------------------------------
bool
NoteEditor::setREFERENCE(GtkWidget *text) //--insert reference tags
{
	gchar			*buf,
						tmpbuf[256];
	
	if(GTK_EDITABLE(text)->has_selection)  //-- do we have a selection?
	{
		buf = gtk_editable_get_chars(GTK_EDITABLE(text), GTK_EDITABLE(text)->selection_start_pos, GTK_EDITABLE(text)->selection_end_pos);	
		sprintf(tmpbuf,"<a href=\"%s\">",buf);
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, tmpbuf, -1);
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "</a>", -1);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool
NoteEditor::setUNDERLINE(GtkWidget *text) //-- insert underline tags
{
	if(GTK_EDITABLE(text)->has_selection)  //-- do we have a selection?
	{
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "<U>", -1);
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "</u>", -1);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool
NoteEditor::setGREEK(GtkWidget *text) //-- insert greek tags
{
	if(GTK_EDITABLE(text)->has_selection)  //-- do we have a selection?
	{
	  gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_start_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "<FONT FACE=\"symbol\">", -1);
		gtk_text_set_point(GTK_TEXT(text), GTK_EDITABLE(text)->selection_end_pos);
		gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "</font>", -1);
		return true;
	}
	return false;
}

//----------------------------------------------------------------------------------------
bool
NoteEditor::setNEWLINE(GtkWidget *text) //-- insert greek tags
{
	gtk_text_set_point(GTK_TEXT(text), gtk_editable_get_position(GTK_EDITABLE(text)));
	gtk_text_insert(GTK_TEXT(text), NULL, &text->style->black, NULL, "<br>", -1); 	
	return true;	
}