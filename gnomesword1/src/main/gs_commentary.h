/*
 * GnomeSword Bible Study Tool
 * gs_commentary.h - support for commentary modules
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GS_COMMENTARY_H_
#define __GS_COMMENTARY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"

	typedef struct _COMMFindDialog COMMFindDialog;
	struct _COMMFindDialog {
		GnomeDialog *dialog;
		GtkWidget *htmlwidget;
		GtkWidget *entry;
		GtkWidget *backward;
		GtkWidget *case_sensitive;
		gboolean regular;
	};

	typedef struct _commdata COMM_DATA;
	struct _commdata {
		GtkWidget *html;
		GtkWidget *btnCOMMSync;
		GtkWidget *btnCOMMBack;
		GtkWidget *btnCOMMForward;
		GtkWidget *btnCOMMPrint;
		GtkWidget *showtabs;
		gchar *modName;
		gchar *modDescription;
		gchar *searchstring;
		gchar *key;
		gint modnum;
		gboolean has_key;
		COMMFindDialog *find_dialog;
	};
	void gui_set_commentary_page_and_key(gint page_num,
					     gchar * key);
	GList* gui_setup_comm(SETTINGS * s);
	void gui_shutdownCOMM(void);
	void gui_displayCOMM(gchar * key);

#ifdef __cplusplus
}
#endif
#endif				/* __GS_COMMENTARY_H_ */
/******  end of file  ******/
