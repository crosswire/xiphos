/*
 * GnomeSword Bible Study Tool
 * viewtext.h - sword support for veiwtext dialog
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

#ifndef __VIEWTEXT_H__
#define __VIEWTEXT_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>

	void backend_setup_viewtext(GtkWidget * text);
	void backend_shutdown_viewtext(void);
	void backend_goto_verse_viewtext(char * newkey);
	void backend_load_module_viewtext(char * modName);
	const char *backend_get_book_viewtext(void);
	int backend_get_chapter_viewtext(void);
	int backend_get_verse_viewtext(void);
	void backend_set_global_options_viewtext(char * option,
						 char * onoff);
	char *backend_get_first_module_viewtext(void);
	char *backend_get_next_module_viewtext(void);
	
#ifdef __cplusplus
}
#endif
#endif				/* __VIEWTEXT_H__ */
