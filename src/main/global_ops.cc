/*
 * GnomeSword Bible Study Tool
 * mod_global_ops.c - setup for SWORD global options in the gui
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <string.h>
#include <swmgr.h>
#include <swmodule.h>

#include "main/global_ops.hh"

#include "main/module_dialogs.h"
#include "main/settings.h"

#include "backend/sword.h"
#include "backend/sword_defs.h"
#include "backend/sword_main.hh"
#include "backend/dialogs.hh"
#include "backend/mgr.hh"

static int of2tf(const gchar * on_off)
{
	if (!strcmp(on_off, "On"))
		return true;
	else
		return false;
}

static gchar *tf2of(int true_false)
{
	if (true_false)
		return "On";
	else
		return "Off";
}


static void set_dialog_global_option(TEXT_DATA * t, char * option, gboolean choice)
{
	ModuleDialogs* be = (ModuleDialogs*)t->backend;	
	SWMgr *mgr = be->get_mgr();
	char *on_off;

	on_off = tf2of(choice);
	
	mgr->setGlobalOption(option, on_off);
}


static void set_global_option(int manager, char * option, gboolean choice)
{
	SWMgr *mgr = backend->get_display_mgr();
	char *on_off;

	on_off = tf2of(choice);
	
	switch(manager) {
		case TEXT_MGR:
			mgr->setGlobalOption(option, on_off);
		break;
		case COMM_MGR:
		case GBS_MGR:			
			mgr->setGlobalOption(option, on_off);
		break;
		case MAIN_MGR:			
			sw.main_mgr->setGlobalOption(option, on_off);
		break;
		case SEARCH_MGR:
			sw.search_mgr->setGlobalOption(option, on_off);
		break;	
		case INTER_MGR:
			sw.inter_mgr->setGlobalOption(option, on_off);
		break;
	}	
	sw.results->setGlobalOption(option, on_off);
}


/******************************************************************************
 * Name
 *  gui_set_global_options
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void gui_set_global_options(GLOBAL_OPS * ops)	
 *
 * Description
 *   set module global options 
 *
 * Return value
 *   void
 */

void main_dialog_set_global_options(TEXT_DATA * t)
{
	set_dialog_global_option(t, "Strong's Numbers",
			  t->ops->strongs);
	set_dialog_global_option(t, "Morphological Tags",
			  t->ops->morphs);
	set_dialog_global_option(t, "Footnotes",
			  t->ops->footnotes);
	set_dialog_global_option(t, "Greek Accents",
			  t->ops->greekaccents);
	set_dialog_global_option(t, "Lemmas", 
			  t->ops->lemmas);
	set_dialog_global_option(t, "Cross-references",
			  t->ops->scripturerefs);
	set_dialog_global_option(t, "Hebrew Vowel Points",
			  t->ops->hebrewpoints);
	set_dialog_global_option(t, "Hebrew Cantillation",
			  t->ops->hebrewcant);
	set_dialog_global_option(t, "Headings", 
			  t->ops->headings);
	set_dialog_global_option(t, "Words of Christ in Red",
			  t->ops->words_in_red);

//	if (!ops->module_type) {	/* if we have Biblical text */
/*		if (ops->variants_primary)
			set_text_global_option("Textual Variants",
					       "Primary Reading");
		else if (ops->variants_secondary)
			set_text_global_option("Textual Variants",
					       "Secondary Reading");
		else
			set_text_global_option("Textual Variants",
					       "All Readings");
*/
	
}


/******************************************************************************
 * Name
 *  gui_set_global_options
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void gui_set_global_options(GLOBAL_OPS * ops)	
 *
 * Description
 *   set module global options 
 *
 * Return value
 *   void
 */

void main_set_global_options(GLOBAL_OPS * ops)
{
	set_global_option(ops->module_type, "Strong's Numbers",
			  ops->strongs);
	set_global_option(ops->module_type, "Morphological Tags",
			  ops->morphs);
	set_global_option(ops->module_type, "Footnotes",
			  ops->footnotes);
	set_global_option(ops->module_type, "Greek Accents",
			  ops->greekaccents);
	set_global_option(ops->module_type, "Lemmas", 
			  ops->lemmas);
	set_global_option(ops->module_type, "Cross-references",
			  ops->scripturerefs);
	set_global_option(ops->module_type, "Hebrew Vowel Points",
			  ops->hebrewpoints);
	set_global_option(ops->module_type, "Hebrew Cantillation",
			  ops->hebrewcant);
	set_global_option(ops->module_type, "Headings", 
			  ops->headings);
	set_global_option(ops->module_type, "Words of Christ in Red",
			  ops->words_in_red);

	if (!ops->module_type) {	/* if we have Biblical text */
/*		if (ops->variants_primary)
			set_text_global_option("Textual Variants",
					       "Primary Reading");
		else if (ops->variants_secondary)
			set_text_global_option("Textual Variants",
					       "Secondary Reading");
		else
			set_text_global_option("Textual Variants",
					       "All Readings");
*/
	}
}


/******************************************************************************
 * Name
 *    gui_new_globals
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   GLOBAL_OPS *gui_new_globals(void)
 *
 * Description
 *   
 *
 * Return value
 *   GLOBAL_OPS *
 */

GLOBAL_OPS *main_new_globals(gchar * mod_name)
{
	GLOBAL_OPS *ops;
	gchar *yesno;
	bool retval = false;
	gchar *buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);
	
	module_options.Load();
		
	ops = g_new0(GLOBAL_OPS, 1);
	ops->module_type = 0;
	ops->words_in_red = 
	      of2tf(module_options[mod_name]["Words of Christ in Red"].c_str());
	ops->strongs =  
		of2tf(module_options[mod_name]["Strong's Numbers"].c_str());
	ops->morphs =  
		of2tf(module_options[mod_name]["Morphological Tags"].c_str());
	ops->footnotes =  
		of2tf(module_options[mod_name]["Footnotes"].c_str());
	ops->greekaccents =  
		of2tf(module_options[mod_name]["Greek Accents"].c_str());
	ops->lemmas =  
		of2tf(module_options[mod_name]["Lemmas"].c_str());
	ops->scripturerefs =  
	  of2tf(module_options[mod_name]["Scripture Cross-references"].c_str());
	ops->hebrewpoints =  
		of2tf(module_options[mod_name]["Hebrew Vowel Points"].c_str());
	ops->hebrewcant =  
		of2tf(module_options[mod_name]["Hebrew Cantillation"].c_str());
	ops->headings =  
		of2tf(module_options[mod_name]["Headings"].c_str());
	ops->variants_all =  
		of2tf(module_options[mod_name]["All Readings"].c_str());
	ops->variants_primary =  
		of2tf(module_options[mod_name]["Primary Reading"].c_str());
	ops->variants_secondary =  
		of2tf(module_options[mod_name]["Secondary Reading"].c_str());
	
	g_free(buf);	
	return ops;
}
