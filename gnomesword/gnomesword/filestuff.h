/***************************************************************************
                          filestuff.h  -  description
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
#include "GnomeSword.h"

void
saveFile 					(gchar *filename);

void
loadFile 					(GtkWidget *filesel);

void
setDiretory					(void);

void
loadbookmarkarray		(void);

void
loadoptionarray			(void);

void
savebookmarks				(void);

void
editbookmarksSave	  (GtkWidget *editdlg);
 
void
saveoptions				  (void) ;

void
createFiles					(void);

void
writesettings				(SETTINGS settings);

SETTINGS
readsettings				(void);

SETTINGS
createsettings			(void);

void
loadStudyPadFile 		(gchar *filename);

void
savelist						(GtkWidget *list,
										gint howmany);