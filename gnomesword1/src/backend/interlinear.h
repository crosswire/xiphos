/*
 * GnomeSword Bible Study Tool
 * interlinear.h - support for displaying multiple modules
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


#ifndef __INTERLINEAR_H_
#define __INTERLINEAR_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "gs_gnomesword.h"

	gboolean backend_setup_interlinear(SETTINGS *s);
	void backend_shutdown_interlinear(void);
	char *backend_get_interlinear_module_text(char *mod_name,
						  char *key);
	void backend_set_interlinear_global_option(char *option,
						   char *yesno);
	
#ifdef __cplusplus
}
#endif
#endif				/* __INTERLINEAR_H_ */
