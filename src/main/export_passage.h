/*
 * Xiphos Bible Study Tool
 * export_passage.h -
 *
 * Copyright (C) 2007-2009 Xiphos Developer Team
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
struct _export_data {
	char *filename;
	int passage_type;
	int start_verse;
	int end_verse;
};

enum {
	BOOK,
	CHAPTER,
	VERSE,
	VERSE_RANGE,
	HTML,
	PLAIN
};

int main_get_max_verses (void);
int main_get_current_verse (void);
void main_export_html (EXPORT_DATA data);
void main_export_plain (EXPORT_DATA data);

#ifdef __cplusplus
}
#endif

#endif /* ___EXPORT_PASSAGE_H_ */
