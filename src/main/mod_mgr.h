/*
 * Xiphos Bible Study Tool
 * mod_mgr.h
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

#ifndef __MOD_MGR_H__
#define __MOD_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <glib.h>

typedef struct _mm MOD_MGR;
struct _mm
{
	char *name;
	char *abbreviation;
	char *type;
	const char *language;
	char *old_version;
	char *new_version;
	char *min_version;	/* req'd level of sword support */
	char *description;
	int installed;
	int is_devotional;
	int is_maps;
	int is_images;
	int is_glossary;
	int is_cult;
	int locked;
	char *installsize;
	char *about;
};

typedef struct _mms MOD_MGR_SOURCE;
struct _mms
{
	const char *caption;
	const char *type;
	const char *source;
	const char *directory;
	const char *user;
	const char *pass;
	const char *uid;
};
int main_module_mgr_index_mod(char *module_name);
int main_module_mgr_delete_index_mod(char *module_name);
void main_index_percent_update(char percent, void *userData);
void main_update_module_lists(void);
MOD_MGR *mod_mgr_get_next_module(void);
void update_install_status(long total,
			   long done, const char *message);
void update_install_progress(double fraction);
GList *mod_mgr_list_local_modules(const char *dir,
				  gboolean augment);
GList *mod_mgr_list_remote_sources(void);
GList *mod_mgr_list_local_sources(void);
int mod_mgr_uninstall(const char *dir, const char *mod_name);
int mod_mgr_remote_install(const char *dir,
			   const char *source_name,
			   const char *mod_name);
int mod_mgr_local_install_module(const char *destdir,
				 const char *srcdir,
				 const char *mod_name);
GList *mod_mgr_remote_list_modules(const char *source_name);
int mod_mgr_refresh_remote_source(const char *source_name);
void mod_mgr_init_config(void);
int mod_mgr_init_config_extras(void);
void mod_mgr_clear_config(void);
void mod_mgr_reread_config(void);
void mod_mgr_add_source(const char *vtype,
			const char *type,
			const char *caption,
			const char *source,
			const char *directory,
			const char *user,
			const char *pass, const char *uid);
void mod_mgr_init(const char *dir,
		  gboolean augment, gboolean regular);
void mod_mgr_shut_down(void);
void mod_mgr_terminate(void);

#ifdef __cplusplus
}
#endif
#endif
