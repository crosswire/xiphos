/*
 * Xiphos Bible Study Tool
 * import_andbible.h - import bookmarks from an AndBible sqlite backup
 *
 * Copyright (C) 2026 Xiphos Developer Team
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

#ifndef __IMPORT_ANDBIBLE_H__
#define __IMPORT_ANDBIBLE_H__

#include <glib.h>
#include <gtk/gtk.h>

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************
 * Name
 *   andbible_import_to_temp_xml
 *
 * Synopsis
 *   #include "gui/import_andbible.h"
 *
 *   gboolean andbible_import_to_temp_xml(const gchar *sqlite_path,
 *                                        gchar **out_xml_path,
 *                                        gint *n_imported,
 *                                        gint *n_skipped,
 *                                        GError **error)
 *
 * Description
 *   Reads an AndBible bookmark backup (the Room sqlite database found in
 *   AndBible's "bookmarks.sqlite3" export/backup) and converts it into a
 *   temporary bookmarks.xml file using the same <SwordBookmarks> schema
 *   Xiphos itself uses (see main/xml.c). AndBible bookmarks are stored as
 *   an ordinal position within a named SWORD versification system
 *   (KJVA, Catholic, Catholic2, ...); this function uses libsword's own
 *   VerseKey/versification support to turn that ordinal back into a
 *   human verse reference, so no separate canon table needs to be
 *   maintained by Xiphos.
 *
 *   AndBible labels become Folder elements (carrying the label's color
 *   when available); notes are appended to the bookmark description.
 *
 *   The caller is responsible for g_free()-ing *out_xml_path and
 *   removing the temporary file (g_unlink()) once it has been consumed,
 *   e.g. via gui_parse_bookmarks().
 *
 * Return value
 *   TRUE on success (out_xml_path is set); FALSE on failure (error is
 *   set, if provided).
 */
gboolean andbible_import_to_temp_xml(const gchar *sqlite_path,
				      const gchar *top_folder_name,
				      gchar **out_xml_path,
				      gint *n_imported,
				      gint *n_skipped,
				      GError **error);

/* Menu/GtkBuilder signal handler: prompts for an AndBible sqlite backup
 * file, converts it, and merges the result into the bookmarks tree - see
 * bibletime_bookmarks_activate() in bookmarks_menu.c for the analogous,
 * existing BibleTime import. */
G_MODULE_EXPORT void andbible_bookmarks_activate(GtkMenuItem *menuitem,
						  gpointer user_data);

#ifdef __cplusplus
}
#endif

#endif /* __IMPORT_ANDBIBLE_H__ */
