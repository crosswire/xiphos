/*
 * Xiphos Bible Study Tool
 * export_passage.h -
 *
 * Copyright (C) 2007-2017 Xiphos Developer Team
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

#ifndef ___EXPORT_PASSAGE_H_
#define ___EXPORT_PASSAGE_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _export_data EXPORT_DATA;
struct _export_data
{
	char *filename;
	char *bookheader;
	char *chapterheader_book;
	char *chapterheader_chapter;
	char *versenumber;
	char *verselayout_single_verse_ref_last;
	char *verselayout_single_verse_ref_first;
	char *verse_range_ref_last;
	char *verse_range_ref_first;
	char *plain_bookheader;
	char *plain_chapterheader_book;
	char *plain_chapterheader_chapter;
	char *plain_versenumber;
	char *plain_verselayout_single_verse_ref_last;
	char *plain_verselayout_single_verse_ref_first;
	char *plain_verse_range_ref_last;
	char *plain_verse_range_ref_first;
	char *verse_range_verse;
	int passage_type;
	int start_verse;
	int end_verse;
	int verse_num;
	int reference_last;
	int version;
};

enum {
	BOOK,
	CHAPTER,
	VERSE,
	VERSE_RANGE,
	HTML,
	PLAIN
};

int main_get_max_verses(const char *name);
int main_get_current_verse(const char *name);
void main_export_content(EXPORT_DATA data, gint format);

#ifdef __cplusplus
}
#endif
#endif /* ___EXPORT_PASSAGE_H_ */
