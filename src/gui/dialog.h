/*
 * Xiphos Bible Study Tool
 * dialog.h -
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#include <gtk/gtk.h>

#ifndef _DIALOG_H_
#define _DIALOG_H_

#ifdef __cplusplus
extern "C" {
#endif

#define GS_YES 0
#define GS_NO 1
#define GS_OK 2
#define GS_CANCEL 3

typedef struct _gs_dialog GS_DIALOG;
struct _gs_dialog
{

	GtkWidget *dialog;

	gchar *stock_icon;

	gchar *title;

	gchar *text1;
	gchar *text2;
	gchar *text3;
	gchar *text4;
	gchar *text5;
	gchar *text6;

	gchar *label_top;
	gchar *label_middle;
	gchar *label1;
	gchar *label2;
	gchar *label3;
	gchar *label4;
	gchar *label5;
	gchar *label6;
	gchar *label_bottom;

	gboolean no_save;
	gboolean save;
	gboolean ok;
	gboolean cancel;
	gboolean yes;
	gboolean no;
	gboolean apply;
};

GS_DIALOG *gui_new_dialog(void);
void gui_generic_warning(const char *);
void gui_generic_warning_modal(const char *);
gint gui_gs_dialog(GS_DIALOG *info);
gint gui_alert_dialog(GS_DIALOG *info);
gint gui_yes_no_dialog(char *, char *);
gint gui_close_confirmation_dialog(GS_DIALOG *info);

#ifdef __cplusplus
}
#endif
#endif
