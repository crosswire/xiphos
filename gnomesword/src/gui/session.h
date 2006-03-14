/*
 * GnomeSword Bible Study Tool
 * session.h - create and maintain a GNOME session
 *
 * based on code from gedit
 * gedit-session - Basic session management for gedit
 * Copyright (C) 2002 Ximian, Inc.
 * Authors Federico Mena-Quintero <federico@ximian.com>
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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


#ifndef __GS_SESSION_H__
#define __GS_SESSION_H__

void gs_session_init (const char *argv0);
gboolean gs_session_is_restored (void);
gboolean gs_session_load (void);

#endif
