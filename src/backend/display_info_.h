/*
    GnomeSword Bible Study Tool
    *  display_info_.h - display for view dialogs
    *
    *  Copyright (C) 2000,2001,2002 GnomeSword Developer Team
    *
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

#ifndef __DISPLAY_INFO__H__
#define __DISPLAY_INFO__H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

void backend_display_info_setup(GtkWidget *text);
void backend_dispaly_info_shutdown(void);
void backend_dispaly_info_load_modudle(gchar *modName, gchar *newkey) ;
void backend_dispaly_info_change_key(gchar *newkey);
        
#ifdef __cplusplus
}
#endif

#endif /* __DISPLAY_INFO__H__ */

