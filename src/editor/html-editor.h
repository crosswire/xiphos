/*
 * GnomeSword Bible Study Tool
 * html-editor.h - the html editor
 *
 * Copyright (C) 2005 GnomeSword Developer Team
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
 
#ifndef _HTML_EDITOR_H
#define _HTML_EDITOR_H

#ifdef __cplusplus
extern "C"
{
#endif

#include <config.h>

typedef struct _editor EDITOR;

void editor_sync_with_main(void);
gboolean editor_close_all(void);
gint editor_create_new(const gchar * filename, const gchar * key, gint note);
gint load_file (EDITOR * e);	


#ifdef __cplusplus
}
#endif

#endif /* _HTML_EDITOR_H */
