/*
 * Xiphos Bible Study Tool
 * biblesync_glue.h
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

#ifndef _BIBLESYNC_GLUE_H__
#define _BIBLESYNC_GLUE_H__

#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "main/configs.h"

#ifdef __cplusplus

#include <map>
#include <string>

using namespace std;

extern "C" {

typedef struct _speaker
{
	string uuid;
	string user;
	string ipaddr;
	string app;
	string device;
	string ref;
	string direct;
	bool listen;
} BSP_Speaker;

typedef std::map<string, BSP_Speaker> BSP_SpeakerMap;
typedef BSP_SpeakerMap::iterator BSP_SpeakerMapIterator;

int biblesync_compare_speaker(const void *Lvoid,
			      const void *Rvoid);

#endif // __cplusplus

void biblesync_navigate(char cmd, char *speaker,
			char *bible, char *verse, char *alt,
			char *group, char *domain,
			char *info, char *dump);
void biblesync_update_speaker();
int biblesync_mode_select(int m, char *p);
int biblesync_personal();
int biblesync_active();
int biblesync_active_xmit_allowed();
const char *biblesync_get_passphrase();
void biblesync_transmit_verse_list(char *modname, char *vlist);
void biblesync_privacy(gboolean privacy);
void biblesync_prep_and_xmit(const char *mod_name,
			     const char *key);
void biblesync_set_clear_all_listen(gboolean listen);

#ifdef __cplusplus
}
#endif
#endif /* _BIBLESYNC_GLUE_H__ */
