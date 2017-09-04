/*
 * Xiphos Bible Study Tool
 * prayerlists.h -  code to create several different prayer lists
 *
 * Copyright (C) 2008-2017 Xiphos Developer Team
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef _PRAYERLISTS_H
#define _PRAYERLISTS_H

#ifdef __cplusplus
extern "C" {
#endif

gboolean main_prayerlist_basic_create(void);
gboolean main_prayerlist_subject_create(void);
gboolean main_prayerlist_monthly_create(void);
gboolean main_prayerlist_journal_create(void);
gboolean main_prayerlist_outlined_topic_create(void);
gboolean main_prayerlist_book_chapter_create(void);

#ifdef __cplusplus
}
#endif
#endif /* _PRAYERLISTS_H */
