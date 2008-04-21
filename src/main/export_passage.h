/*
 * GnomeSword Bible Study Tool
 * export_passage.h - 
 *
 * Copyright (C) 2007 GnomeSword Developer Team
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

#ifndef ___EXPORT_PASSAGE_H_
#define ___EXPORT_PASSAGE_H_


#ifdef __cplusplus
extern "C" {
#endif	

enum {
	BOOK,
	CHAPTER,
	VERSE,
	HTML,
	PLAIN
};

void main_export_html(char *filename, int what_to_export);
void main_export_plain(char *filename, int what_to_export);

#ifdef __cplusplus
}
#endif

#endif /* ___EXPORT_PASSAGE_H_ */
