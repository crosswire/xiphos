/*
 * Xiphos Bible Study Tool
 * xiphos.h -
 *
 * Copyright (C) 2000-2015 Xiphos Developer Team
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

#ifndef __XIPHOS_H__
#define __XIPHOS_H__

#ifdef __cplusplus
extern "C" {
#endif


#define MAIN_TEXT_WINDOW 0
#define parallel_WINDOW 1
#define COMMENTARY_WINDOW 2
#define DICTIONARY_WINDOW 3
#define BOOK_WINDOW 4
#define PERCOMM_WINDOW 5
#define STUDYPAD_WINDOW 6
#define BIBLE_DIALOG 7
#define COMMENTARY_DIALOG 8
#define DICTIONARY_DIALOG 9
#define BOOK_DIALOG 10

/*** function prototypes ***/

	void frontend_init(void);
	void frontend_display(const char *);
	void shutdown_frontend(void);

#ifdef __cplusplus
}
#endif
#endif
