/*
 * GnomeSword Bible Study Tool
 * mod_mgr.h
 * 
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#ifndef __MOD_MGR_H__
#define __MOD_MGR_H__

#ifdef __cplusplus
extern "C" {
#endif	

typedef struct _mm MOD_MGR;
struct  _mm {
	char *name;
	char *type;
	const char *language;
	const char *old_version;
	char *new_version;
	char *description;
	int installed;
	int locked;
};

typedef struct _mms MOD_MGR_SOURCE;
struct  _mms {
	const char *caption;
	const char *type;
	const char *source;
	const char *directory;
};

MOD_MGR *mod_mgr_get_next_module(void);
void update_install_status(long total, long done, const char *message);
void update_install_progress(double fraction);
GList *mod_mgr_list_local_modules(const char *dir);
GList *mod_mgr_list_remote_sources(void);
GList *mod_mgr_list_local_sources(void);
int mod_mgr_uninstall(const char *dir, const char *mod_name);
int mod_mgr_remote_install(const char *source_name, const char *mod_name);
int mod_mgr_local_install_module(const char *dir, const char *mod_name);
GList *mod_mgr_remote_list_modules(const char *source_name);
int mod_mgr_refresh_remote_source(const char *source_name);
void mod_mgr_init_config(void);
void mod_mgr_add_source(const char * type, const char * caption,
				   const char * source, const char * directory);
void mod_mgr_init(const char *dir);
void mod_mgr_shut_down(void);

#ifdef __cplusplus
}
#endif

#endif
