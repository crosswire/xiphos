/***************************************************************************
                                    sw_shortcutbar.h
                             -------------------
				    Mon Aug 06 2001
			       (C) 2001 by Terry Biggs
			         tbiggs@users.sf.net
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
#ifndef __SW_SHORTCUTBAR_H__
#define __SW_SHORTCUTBAR_H__

#ifdef __cplusplus
extern "C" {
#endif
	
#include "gs_gnomesword.h"
gint sbtypefromModNameSBSW(gchar *description);
void save_iconsizeSW(gchar * filename, gchar *large_icons);	
GList *loadshortcutbarSW(gchar *filename, gchar *group_name, gchar *use_largeicons);
void saveshortcutbarSW(gchar *filename, gchar *group_name, GList *list, gchar *large_icons);
GList *getModlistSW(gchar *modtype);
	
#ifdef __cplusplus
}
#endif
#endif /* __SW_SHORTCUTBAR_H__ */

