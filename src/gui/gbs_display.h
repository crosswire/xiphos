/*
 * GnomeSword Bible Study Tool
 * gui/gbs_display.h - support for displaying General Book Format modules
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

#ifndef _GBS_DISPALY_H_
#define _GBS_DISPALY_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gui/gbs_dialog.h"
	
#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"></head>"

void gbs_display(GBS_DATA * gbs, gint level);

	
#ifdef __cplusplus
}
#endif

#endif /* _GBS_DISPALY_H_ */
