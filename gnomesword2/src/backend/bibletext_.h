/*
 * GnomeSword Bible Study Tool
 * bibletext_.h - support for Sword commentary modules
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


#ifndef __BIBLETEXT__H_
#define __BIBLETEXT__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "settings.h"
	
const char* backend_get_text_module_description(int modnum);
void backend_nav_text_module(int modnum, int direction);
void backend_new_text_display(GtkWidget * html, char *modname,
					      SETTINGS * s);
void backend_setup_text(SETTINGS * s);
void backend_shutdown_text(void);
void backend_display_text(int modnum, char *key);
void backend_set_text_global_option(char *option, char *yesno);
void backend_set_module_unlocked(char *mod_name, int mod_num,char *key);
gboolean backend_check_for_global_option(int mod_num, char *option);
	
#ifdef __cplusplus
}
#endif
#endif				/* __BIBLETEXT__H_ */
