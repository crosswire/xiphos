/***************************************************************************
                                  gs_shordcutbar.h
                             -------------------
    begin                : Thu Jun 14 2001
    copyright            : (C) 2001 by tbiggs
    email                : tbiggs@users.sf.net
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
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GS_SHORTCUTBAR_H_
#define __GS_SHORTCUTBAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include  <gal/shortcut-bar/e-shortcut-bar.h>

void on_btnSB_clicked(GtkButton * button, gpointer user_data);
void setupSB(GList *text, GList *commentary, GList *dictionary);
gint add_sb_group(EShortcutBar * shortcut_bar,
		gchar * group_name);

void on_shortcut_bar_item_selected(EShortcutBar * shortcut_bar,
				       GdkEvent * event,
				       gint group_num, gint item_num);

				
#ifdef __cplusplus
}
#endif

#endif  /* __GS_SHORTCUTBAR_H_ */



