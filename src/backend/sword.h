/*
 * GnomeSword Bible Study Tool
 * sword.h - support for sword libs
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



#ifndef __SWORD_H__
#define __SWORD_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <gnome.h>
#include "gs_gnomesword.h"

/* 
   these strings are not seen by users
 */
#define TEXT_MODS "Biblical Texts"
#define COMM_MODS "Commentaries"
#define DICT_MODS "Lexicons / Dictionaries"
#define BOOK_MODS "Generic Books"
#define CIPHER_KEY_LEN 16

	void backend_first_init(void);
	void backend_init_sword(SETTINGS * s);
	void backend_change_verse_percom(char *key);
	void backend_module_name_from_description(gchar * mod_name,
						  gchar * mod_desc);
	void backend_shutdown(SETTINGS * s);
	void backend_set_locale(char *locale);
	char *backend_get_valid_key(char *key);
	char *backend_get_book_from_key(char *key);
	int backend_get_chapter_from_key(char *key);
	int backend_get_verse_from_key(char *key);
	void backend_save_personal_comment(gchar * buf);
	void backend_delete_personal_comment(void);
	void backend_change_percom_module(gchar * modName);
	void backend_set_global_option(gint window, gchar * option,
				       gchar * yesno);
	GList *backend_get_books(void);
	const char *backend_get_sword_verion(void);
	void backend_display_new_font_color_and_size(SETTINGS * s);
	void backend_save_module_key(char *mod_name, char *key);
	void backend_display_devotional(SETTINGS * s);
	int backend_get_mod_type(gchar * modName);
	GList *backend_get_list_of_mods_by_type(char *mod_type);
	GList *backend_get_list_of_devotion_modules(void);
	GList *backend_get_list_of_percom_modules(void);
	GList *backend_get_mod_description_list_SWORD(char *mod_type);
	gchar *backend_get_module_description(gchar * modName);
	gchar *backend_get_path_to_mods(void);
	gchar *backend_get_mod_aboutSWORD(gchar * modname);
	int backend_get_module_page(char *module_name,
				    char *module_type);
	char *backend_get_module_font_name(char *mod_name);
	gboolean backend_module_is_locked(char *mod_name);
	char *backend_get_cipher_key(char *mod_name);
	
#ifdef __cplusplus
}
#endif
#endif				/* __SWORD_H__ */
/*****  end of file    ******/
