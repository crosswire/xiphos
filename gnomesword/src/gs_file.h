/***************************************************************************
                          gs_file.h  -  description
                             -------------------
    begin                : Mon May 8 2000
    copyright            : (C) 2000 by Terry Biggs
    email                :  tbiggs@infinet.com
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
#ifdef __cplusplus
extern "C" {
#endif
//#include "sw_gnomesword.h"
	
void on_ok_button2_clicked(GtkButton * button, gpointer user_data);
void on_ok_button4_clicked(GtkButton * button, gpointer user_data);
void saveFile(gchar * filename);
void loadFile(GtkWidget * filesel);
gint setDiretory(void);
void loadbookmarkarray(void);
void loadbookmarks_programstart(void);
void loadbookmarks_afterSeparator(void);
void loadoptionarray(void);
void savebookmark(gchar * item);
void saveoptions(void);
void createFiles(void);
void loadStudyPadFile(gchar * filename);
void savelist(gchar * filename, GtkWidget * list);
GtkWidget *create_fileselection1(void);
GtkWidget *create_fileselectionSave(void);

#ifdef __cplusplus
}
#endif

