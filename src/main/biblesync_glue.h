/*
 * Xiphos Bible Study Tool
 * biblesync_glue.h
 *
 * Copyright (C) 2000-2014 Xiphos Developer Team
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

#ifdef __cplusplus

extern "C" {
#endif
#include <gtk/gtk.h>
#include <glib/gi18n.h>

#include "main/configs.h"

void biblesync_navigate(char cmd,
			     char *bible, char *verse, char *alt,
			     char *info, char *dump);
int biblesync_mode_select(int m, char *p);
int biblesync_personal();
int biblesync_active();
int biblesync_active_xmit_allowed();
const char *biblesync_get_passphrase();
void biblesync_transmit_verse_list(char *modname, char *vlist);
void biblesync_privacy(gboolean privacy);
void biblesync_prep_and_xmit(const char *mod_name, const char *key);
void biblesync_listen(gboolean listen, char *speakerkey);

#ifdef __cplusplus
}
#endif

#endif /* _BIBLESYNC_GLUE_H__ */
