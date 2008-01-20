/*
 * GnomeSword Bible Study Tool
 * mod_global_ops.c - setup for SWORD global options in the gui
 *
 * Copyright (C) 2000-2008 GnomeSword Developer Team
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

#include "gui/parallel_view.h"
#include "gui/utilities.h"

#include "main/global_ops.hh"
#include "main/lists.h"
#include "main/module_dialogs.h"
#include "main/settings.h"
#include "main/parallel_view.h"

//#include "backend/sword.h"
#include "backend/sword_main.hh"

static void set_dialog_global_option(BackEnd *be,
				     char *option,
				     gboolean choice)
{
	//BackEnd* be = (BackEnd*)t->backend;	
	SWMgr *mgr = be->get_display_mgr();
	char *on_off;

	on_off = gui_tf2of(choice);
	mgr->setGlobalOption(option, on_off);
}


static void set_global_option(int manager,
			      char *option,
			      gboolean choice)
{
	SWMgr *mgr = backend->get_display_mgr();
	SWMgr *main_mgr = backend->get_main_mgr();
	char *on_off;

	on_off = gui_tf2of(choice);
	mgr->setGlobalOption(option, on_off);
	main_mgr->setGlobalOption(option, on_off);
}


/******************************************************************************
 * Name
 *   main_save_module_options
 *
 * Synopsis
 *   #include "main/global_ops.h"
 *
 *   int main_save_module_options(char * mod_name, char * option,
 *				    int choice)	
 *
 * Description
 *
 *
 * Return value
 *   int
 */

int main_save_module_options(char *mod_name,
			     char *option,
			     int choice)
{
	gchar *buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);
	
	module_options.Load();
	g_free(buf);	

	module_options[mod_name][option] = (choice ? "On" : "Off");

	module_options.Save();
	return true;
}

/******************************************************************************
 * Name
 *  main_dialog_set_global_options
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void main_dialog_set_global_options(gpointer backend, GLOBAL_OPS * ops)
 *
 * Description
 *   set module global options
 *
 * Return value
 *   void
 */

void main_dialog_set_global_options(gpointer backend,
				    GLOBAL_OPS * ops)
{
	BackEnd* b = (BackEnd*)backend;
	
	set_dialog_global_option(b, "Strong's Numbers",
				 ops->strongs);
	set_dialog_global_option(b, "Morphological Tags",
				 ops->morphs);
	set_dialog_global_option(b, "Footnotes",
				 ops->footnotes);
	set_dialog_global_option(b, "Greek Accents",
				 ops->greekaccents);
	set_dialog_global_option(b, "Lemmas",
				 ops->lemmas);
	set_dialog_global_option(b, "Cross-references",
				 ops->scripturerefs);
	set_dialog_global_option(b, "Hebrew Vowel Points",
				 ops->hebrewpoints);
	set_dialog_global_option(b, "Hebrew Cantillation",
				 ops->hebrewcant);
	set_dialog_global_option(b, "Headings",
				 ops->headings);
	set_dialog_global_option(b, "Words of Christ in Red",
				 ops->words_in_red);
	set_dialog_global_option(b, "Primary Reading",
				 ops->variants_primary);
	set_dialog_global_option(b, "Secondary Reading",
				 ops->variants_secondary);
	set_dialog_global_option(b, "All Readings",
				 ops->variants_all);
}


/******************************************************************************
 * Name
 *  main_set_global_options
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void main_set_global_options(GLOBAL_OPS * ops)	
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
	set_global_option(ops->module_type, "Primary Reading",
			  ops->variants_primary);
	set_global_option(ops->module_type, "Secondary Reading",
			  ops->variants_secondary);
	set_global_option(ops->module_type, "All Readings",
			  ops->variants_all);
}


/******************************************************************************
 * Name
 *   main_set_strongs_morphs_off
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void main_set_strongs_morphs_off(GLOBAL_OPS * ops)	
 *
 * Description
 *   set module global options
 *
 * Return value
 *   void
 */

void main_set_strongs_morphs_off(GLOBAL_OPS * ops)
{
	set_global_option(ops->module_type, "Strong's Numbers",
			  FALSE);
	set_global_option(ops->module_type, "Morphological Tags",
			  FALSE);

}


/******************************************************************************
 * Name
 *   main_set_strongs_morphs
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void main_set_strongs_morphs(GLOBAL_OPS * ops)	
 *
 * Description
 *   set module global options
 *
 * Return value
 *   void
 */

void main_set_strongs_morphs(GLOBAL_OPS * ops)
{
	set_global_option(ops->module_type, "Strong's Numbers",
			  TRUE); //ops->strongs);
	set_global_option(ops->module_type, "Morphological Tags",
			  TRUE); //ops->morphs);

}


/******************************************************************************
 * Name
 *   main_set_dialog_strongs_morphs_off
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void main_set_dialog_strongs_morphs_off(GLOBAL_OPS * ops)	
 *
 * Description
 *   set module global options
 *
 * Return value
 *   void
 */

void main_set_dialog_strongs_morphs_off(gpointer backend,
					GLOBAL_OPS * ops)
{
	BackEnd* be = (BackEnd*)backend;
	
	set_dialog_global_option(be, "Strong's Numbers", FALSE);
	set_dialog_global_option(be, "Morphological Tags", FALSE);
}


/******************************************************************************
 * Name
 *   main_set_dialog_strongs_morphs
 *
 * Synopsis
 *   #include "gui/mod_global_ops.h"
 *
 *   void main_set_dialog_strongs_morphs(GLOBAL_OPS * ops)	
 *
 * Description
 *   set module global options
 *
 * Return value
 *   void
 */

void main_set_dialog_strongs_morphs(gpointer backend,
				    GLOBAL_OPS * ops)
{
	BackEnd* be = (BackEnd*)backend;
	
	set_dialog_global_option(be, "Strong's Numbers", TRUE);
	set_dialog_global_option(be, "Morphological Tags", TRUE);
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
	bool retval = false;
	gchar *buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);
	
	module_options.Load();
	g_free(buf);	

	ops = g_new0(GLOBAL_OPS, 1);
	ops->module_type = 0;
	ops->words_in_red =
	    gui_of2tf(module_options[mod_name]["Words of Christ in Red"].c_str());
	ops->strongs =
	    gui_of2tf(module_options[mod_name]["Strong's Numbers"].c_str());
	ops->morphs =
	    gui_of2tf(module_options[mod_name]["Morphological Tags"].c_str());
	ops->footnotes =
	    gui_of2tf(module_options[mod_name]["Footnotes"].c_str());
	ops->greekaccents =
	    gui_of2tf(module_options[mod_name]["Greek Accents"].c_str());
	ops->lemmas =
	    gui_of2tf(module_options[mod_name]["Lemmas"].c_str());
	ops->scripturerefs =
	    gui_of2tf(module_options[mod_name]["Scripture Cross-references"].c_str());
	ops->hebrewpoints =
	    gui_of2tf(module_options[mod_name]["Hebrew Vowel Points"].c_str());
	ops->hebrewcant =
	    gui_of2tf(module_options[mod_name]["Hebrew Cantillation"].c_str());
	ops->headings =
	    gui_of2tf(module_options[mod_name]["Headings"].c_str());

	ops->variants_all =
	    gui_of2tf(module_options[mod_name]["All Readings"].c_str());
	ops->variants_primary =
	    gui_of2tf(module_options[mod_name]["Primary Reading"].c_str());
	ops->variants_secondary =
	    gui_of2tf(module_options[mod_name]["Secondary Reading"].c_str());
	if ((ops->variants_all == FALSE) &&
	    (ops->variants_primary == FALSE) &&
	    (ops->variants_secondary == FALSE)) {
		ops->variants_primary = TRUE;
		//main_save_module_options(mod_name, "Primary Reading", 1);
	}

	ops->image_content =
	    (*(module_options[mod_name]["Image Content"].c_str()) == '\0')
	    ? -1	// "unknown"; otherwise, it's like the others.
	    : gui_of2tf(module_options[mod_name]["Image Content"].c_str());

	return ops;
}
