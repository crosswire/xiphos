/* 
 * added to gnomesword 2001-01-06 for spell checking
*/

/* Bluefish HTML Editor
 * char_table.h - character convertion functions and data structures
 *
 * Copyright (C) 2000 Olivier Sessink & Pablo De Napoli (for this module)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#include <glib.h>

typedef struct {
	            unsigned char id;
               char *entity;
         } Tchar_entity;

extern Tchar_entity ascii_charset[];

extern Tchar_entity iso8859_1_charset[];

gchar convert_from_html_chars (gchar* character,Tchar_entity charset[]);

#define ANY_CHAR_SET NULL

gboolean isalpha_iso(unsigned char c);

gchar* convert_string_iso_to_html (gchar* string);

gchar* convert_char_iso_to_html (unsigned char c);

