/***************************************************************************
                          noteeditor.h  -  description
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

class NoteEditor
{
    public:
        NoteEditor();
        ~NoteEditor();
        bool setBOLD(GtkWidget *text);
        bool setITALIC(GtkWidget *text);
        bool setREFERENCE(GtkWidget *text);
        bool setUNDERLINE(GtkWidget *text);
        bool setGREEK(GtkWidget *text);
        bool setNEWLINE(GtkWidget *text); 		
};

