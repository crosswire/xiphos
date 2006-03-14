/***************************************************************************
 * GnomeSword Bible Study Tool
 * embed.h - gtkmozembed stuff 
 *
 * Copyright (C) 2004 GnomeSword Developer Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#ifndef _EMBED_DIALOGS_H
#define _EMBED_DIALOGS_H

#ifdef __cplusplus
extern "C"
{
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#ifdef USE_GTKMOZEMBED
	
void embed_dialogs_print(gboolean preview, GtkMozEmbed *embed);
void embed_dialogs_copy_selection(GtkMozEmbed *mozilla_embed);
void embed_dialogs_go_to_anchor(GtkMozEmbed *mozilla_embed, const char *anchor);
void embed_dialogs_navigate_to_anchor(GtkMozEmbed *mozilla_embed, const char *anchor);
GtkWidget *embed_dialogs_new(gpointer dialog);

#endif /* USE_GTKMOZEMBED */

#ifdef __cplusplus
}
#endif

#endif /* _EMBED_DIALOGS_H */
