/*
 * Xiphos Bible Study Tool
 * import_andbible.cc - import bookmarks from an AndBible sqlite backup
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <cstring>
#include <unistd.h>

#include <libxml/parser.h>

#include <glib.h>
#include <glib/gi18n.h>
#include <glib/gstdio.h>
#include <sqlite3.h>
#include <versekey.h>

#include "gui/import_andbible.h"
#include "main/xml.h"

using sword::VerseKey;

/* AndBible's system labels are not real, user-visible categories. */
static const char *const SKIP_LABEL_NAMES[] = {
    "__UNLABELED__", "__SPEAK_LABEL__", NULL};

struct LabelInfo {
	gchar *name;
	gchar *color; /* "#RRGGBB", may be NULL */
};

static void label_info_free(gpointer p) {
	LabelInfo *li = (LabelInfo *)p;
	g_free(li->name);
	g_free(li->color);
	g_free(li);
}

static gboolean is_system_label(const gchar *name) {
	for (int i = 0; SKIP_LABEL_NAMES[i]; ++i)
		if (name && !strcmp(name, SKIP_LABEL_NAMES[i]))
			return TRUE;
	return FALSE;
}

/* sqlite BLOB ids (16-byte UUIDs) -> stable hex string, for use as a
 * GHashTable key. */
static gchar *blob_to_hex(const void *data, int len) {
	static const char hexch[] = "0123456789abcdef";
	gchar *out = (gchar *)g_malloc(len * 2 + 1);
	const guint8 *b = (const guint8 *)data;
	for (int i = 0; i < len; ++i) {
		out[i * 2] = hexch[b[i] >> 4];
		out[i * 2 + 1] = hexch[b[i] & 0xf];
	}
	out[len * 2] = '\0';
	return out;
}

/* AndBible/Android stores label colors as signed 32-bit ARGB ints
 * (Java convention); alpha is discarded. */
static gchar *argb_to_hex(gint64 argb_signed) {
	guint32 v = ((guint32)argb_signed) & 0xFFFFFFu;
	return g_strdup_printf("#%06X", v);
}

/* Decodes an AndBible ordinal range, in a named SWORD versification
 * system (e.g. "KJVA", "Catholic2"), into a human verse key such as
 * "Romans 3:23-Romans 3:24" - using libsword's own VerseKey rather than
 * a hand-maintained canon table, so any versification SWORD knows about
 * is supported automatically. Returns NULL (caller should skip the
 * bookmark) if the versification name is unknown to this SWORD build. */
static gchar *ordinal_range_to_key(const gchar *v11n, glong ord_start,
				   glong ord_end) {
	if (!v11n || !*v11n)
		return NULL;

	VerseKey vk;
	vk.setVersificationSystem(v11n);
	if (strcmp(vk.getVersificationSystem(), v11n) != 0)
		/* SWORD silently falls back to KJV for unknown systems -
		 * refuse to produce a misleading reference. */
		return NULL;

	vk.setIndex(ord_start);
	if (ord_end == ord_start) {
		const char *text = vk.getText();
		return text ? g_strdup(text) : NULL;
	}

	VerseKey lb(vk);
	VerseKey ub;
	ub.setVersificationSystem(v11n);
	ub.setIndex(ord_end);
	vk.setLowerBound(lb);
	vk.setUpperBound(ub);
	const char *text = vk.getRangeText();
	return text ? g_strdup(text) : NULL;
}

/* Returns a newly-allocated GList of label-id hex strings (caller frees
 * both the list and its contents) attached to the given bookmark id,
 * in AndBible's own display order. */
static GList *fetch_label_ids(sqlite3_stmt *st, const void *bookmark_id,
			      int bookmark_id_len) {
	GList *result = NULL;
	sqlite3_reset(st);
	sqlite3_bind_blob(st, 1, bookmark_id, bookmark_id_len, SQLITE_STATIC);
	while (sqlite3_step(st) == SQLITE_ROW) {
		const void *lidblob = sqlite3_column_blob(st, 0);
		int lidlen = sqlite3_column_bytes(st, 0);
		result = g_list_append(result, blob_to_hex(lidblob, lidlen));
	}
	return result;
}

static gchar *fetch_note(sqlite3_stmt *st, const void *bookmark_id,
			 int bookmark_id_len) {
	gchar *note = NULL;
	sqlite3_reset(st);
	sqlite3_bind_blob(st, 1, bookmark_id, bookmark_id_len, SQLITE_STATIC);
	if (sqlite3_step(st) == SQLITE_ROW) {
		const unsigned char *text = sqlite3_column_text(st, 0);
		if (text)
			note = g_strdup((const gchar *)text);
	}
	return note;
}

extern "C" gboolean andbible_import_to_temp_xml(const gchar *sqlite_path,
						 const gchar *top_folder_name,
						 gchar **out_xml_path,
						 gint *n_imported,
						 gint *n_skipped,
						 GError **error) {
	g_return_val_if_fail(sqlite_path != NULL, FALSE);
	g_return_val_if_fail(out_xml_path != NULL, FALSE);

	if (!top_folder_name || !*top_folder_name)
		top_folder_name = _("Import AndBible");
	if (n_imported)
		*n_imported = 0;
	if (n_skipped)
		*n_skipped = 0;

	sqlite3 *db = NULL;
	if (sqlite3_open_v2(sqlite_path, &db, SQLITE_OPEN_READONLY, NULL) !=
	    SQLITE_OK) {
		g_set_error(error, g_quark_from_static_string("andbible-import"),
			    1, _("Unable to open '%s': %s"), sqlite_path,
			    db ? sqlite3_errmsg(db) : _("unknown error"));
		if (db)
			sqlite3_close(db);
		return FALSE;
	}

	/* --- Load labels (small table, load fully up-front). --- */
	GHashTable *labels = g_hash_table_new_full(
	    g_str_hash, g_str_equal, g_free, label_info_free);
	sqlite3_stmt *label_st = NULL;
	if (sqlite3_prepare_v2(db, "SELECT id, name, color FROM Label", -1,
				&label_st, NULL) == SQLITE_OK) {
		while (sqlite3_step(label_st) == SQLITE_ROW) {
			const void *idblob = sqlite3_column_blob(label_st, 0);
			int idlen = sqlite3_column_bytes(label_st, 0);
			const unsigned char *name =
			    sqlite3_column_text(label_st, 1);

			LabelInfo *li = g_new0(LabelInfo, 1);
			li->name = g_strdup(name ? (const gchar *)name : "");
			li->color = argb_to_hex(sqlite3_column_int64(label_st, 2));
			g_hash_table_insert(labels, blob_to_hex(idblob, idlen),
					     li);
		}
	}
	sqlite3_finalize(label_st);

	sqlite3_stmt *link_st = NULL, *note_st = NULL, *bm_st = NULL;
	sqlite3_prepare_v2(db,
			   "SELECT labelId FROM BibleBookmarkToLabel "
			   "WHERE bookmarkId = ? ORDER BY orderNumber",
			   -1, &link_st, NULL);
	sqlite3_prepare_v2(db,
			   "SELECT notes FROM BibleBookmarkNotes "
			   "WHERE bookmarkId = ?",
			   -1, &note_st, NULL);

	if (sqlite3_prepare_v2(
		db,
		"SELECT id, v11n, ordinalStart, ordinalEnd, book "
		"FROM BibleBookmark",
		-1, &bm_st, NULL) != SQLITE_OK) {
		g_set_error(error, g_quark_from_static_string("andbible-import"),
			    2, _("'%s' does not look like an AndBible "
				 "bookmarks backup (table BibleBookmark not "
				 "found)."),
			    sqlite_path);
		g_hash_table_destroy(labels);
		sqlite3_finalize(link_st);
		sqlite3_finalize(note_st);
		sqlite3_close(db);
		return FALSE;
	}

	/* --- Build the SwordBookmarks XML tree. --- */
	xmlDocPtr doc = xmlNewDoc((const xmlChar *)"1.0");
	xmlNodePtr root = xmlNewNode(NULL, (const xmlChar *)"SwordBookmarks");
	xmlNewProp(root, (const xmlChar *)"syntaxVersion",
		   (const xmlChar *)"1.0");
	xmlDocSetRootElement(doc, root);
	/* Folders are attached directly under the document root; the
	 * caller (andbible_bookmarks_activate) nests the whole thing under
	 * a single new tree row of its own, the same way the existing
	 * BibleTime import does. */
	xmlNodePtr top = root;
	(void)top_folder_name; /* now only used as the tree-row caption */

	GHashTable *folder_by_label = g_hash_table_new(g_str_hash, g_str_equal);
	xmlNodePtr unfiled = NULL;
	gint imported = 0, skipped = 0;

	while (sqlite3_step(bm_st) == SQLITE_ROW) {
		const void *idblob = sqlite3_column_blob(bm_st, 0);
		int idlen = sqlite3_column_bytes(bm_st, 0);
		const unsigned char *v11n_txt = sqlite3_column_text(bm_st, 1);
		glong ord_start = (glong)sqlite3_column_int64(bm_st, 2);
		glong ord_end = (glong)sqlite3_column_int64(bm_st, 3);
		const unsigned char *module_txt = sqlite3_column_text(bm_st, 4);

		gchar *key = ordinal_range_to_key(
		    v11n_txt ? (const gchar *)v11n_txt : NULL, ord_start,
		    ord_end);
		if (!key) {
			++skipped;
			continue;
		}

		gchar *note = fetch_note(note_st, idblob, idlen);
		gchar *description =
		    note ? g_strdup_printf("%s - %s", key, note)
			 : g_strdup(key);

		GList *label_ids = fetch_label_ids(link_st, idblob, idlen);
		GList *targets = NULL;
		for (GList *l = label_ids; l; l = l->next) {
			const gchar *lid_hex = (const gchar *)l->data;
			LabelInfo *li =
			    (LabelInfo *)g_hash_table_lookup(labels, lid_hex);
			if (!li || is_system_label(li->name))
				continue;

			xmlNodePtr folder = (xmlNodePtr)g_hash_table_lookup(
			    folder_by_label, lid_hex);
			if (!folder) {
				folder = xml_add_folder_to_parent_colored(
				    top, li->name, li->color);
				g_hash_table_insert(folder_by_label,
						     g_strdup(lid_hex), folder);
			}
			targets = g_list_append(targets, folder);
		}
		if (!targets) {
			if (!unfiled)
				unfiled = xml_add_folder_to_parent(
				    top, _("Uncategorized"));
			targets = g_list_append(targets, unfiled);
		}

		for (GList *t = targets; t; t = t->next)
			xml_add_bookmark_to_parent(
			    (xmlNodePtr)t->data, description, key,
			    module_txt ? (gchar *)module_txt : (gchar *)"",
			    NULL);
		++imported;

		g_list_free(targets);
		g_list_free_full(label_ids, g_free);
		g_free(note);
		g_free(description);
		g_free(key);
	}

	sqlite3_finalize(bm_st);
	sqlite3_finalize(link_st);
	sqlite3_finalize(note_st);
	g_hash_table_destroy(labels);
	g_hash_table_destroy(folder_by_label);
	sqlite3_close(db);

	/* --- Write to a temp file for gui_parse_bookmarks() to consume. --- */
	gchar *tmp_path = NULL;
	gint fd = g_file_open_tmp("xiphos-andbible-import-XXXXXX.xml",
				  &tmp_path, error);
	if (fd < 0) {
		xmlFreeDoc(doc);
		return FALSE;
	}
	close(fd);

	if (xmlSaveFormatFileEnc(tmp_path, doc, "UTF-8", 1) < 0) {
		g_set_error(error, g_quark_from_static_string("andbible-import"),
			    3, _("Could not write temporary file '%s'."),
			    tmp_path);
		g_free(tmp_path);
		xmlFreeDoc(doc);
		return FALSE;
	}

	xmlFreeDoc(doc);
	*out_xml_path = tmp_path;
	if (n_imported)
		*n_imported = imported;
	if (n_skipped)
		*n_skipped = skipped;
	return TRUE;
}
