/*
 * GnomeSword Bible Study Tool
 * sword_defs.h - backend defs
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

#ifndef _SWORD_DEFS_H_
#define _SWORD_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
	
#include <swmgr.h>
#include <treekeyidx.h>
	


using namespace sword;
typedef struct _sword SWORD;
struct  _sword {	
	SWMgr *main_mgr;
	SWMgr *search_mgr;
	SWMgr *module_mgr;
	SWMgr *text_mgr;
	SWMgr *inter_mgr;
	SWMgr *comm_mgr;
	SWMgr *dict_mgr;
	SWMgr *gbs_mgr;
	SWMgr *percom_mgr;
	SWMgr * results;
	
	SWModule *text_mod;
	SWModule *comm_mod;
	SWModule *dict_mod;
	SWModule *gbs_mod;
	SWModule *percom_mod;
	
	TreeKeyIdx *treeKey;	
};

	
extern SWORD sw;

#ifdef __cplusplus
}
#endif

#endif /* _SWORD_DEFS_H_ */
