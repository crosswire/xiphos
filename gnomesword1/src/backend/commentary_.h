/*
 * GnomeSword Bible Study Tool
 * commentary_.h - support for Sword commentary modules
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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


#ifndef __COMMENTARY__H_
#define __COMMENTARY__H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"

const char* backend_nav_commentary( gint modnum,
				   gint direction );
void backend_new_display_commentary( GtkWidget * html, char *modname,
			      SETTINGS * s );
void backend_setup_commentary( SETTINGS * s );
void backend_shutdown_commentary( void );
void backend_display_commentary( int modnnum, gchar * key );

#ifdef __cplusplus
}
#endif
#endif  /* __COMMENTARY__H_ */
