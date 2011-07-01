/*
 * Xiphos Bible Study Tool
 * mod_global_ops.c - setup for SWORD global options and
 *                    a few of our own in the gui
 *
 * Copyright (C) 2000-2011 Xiphos Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
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

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"


static void _set_dialog_global_option(BackEnd *be,
				     const char *option,
				     gboolean choice)
{
	SWMgr *mgr = be->get_mgr();
	char *on_off;

	on_off = gui_tf2of(choice);
	mgr->setGlobalOption(option, on_off);
}


static void _set_dialog_global_textual(BackEnd *be,
				      const char *option,
				      const char *choice)
{
	SWMgr *mgr = be->get_mgr();
	mgr->setGlobalOption(option, choice);
}


static void _set_global_option(int manager,
			      const char *option,
			      gboolean choice)
{
	SWMgr *mgr = backend->get_mgr();
	char *on_off;

	on_off = gui_tf2of(choice);
	mgr->setGlobalOption(option, on_off);
}


static void _set_global_textual(int manager,
			       const char *option,
			       const char *choice)
{
	SWMgr *mgr = backend->get_mgr();
	mgr->setGlobalOption(option, choice);

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

int main_save_module_options(const char *mod_name,
			     const char *option,
			     int choice,
			     int dialog)
{
	gchar *buf = NULL;
#if 0
    	if (dialog)
		buf = g_strdup_printf("%s/modops-dialog.conf", settings.gSwordDir);
	else
#endif
		buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
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

	_set_dialog_global_option(b, "Strong's Numbers",
				 ops->strongs);
	_set_dialog_global_option(b, "Morphological Tags",
				 ops->morphs);
	_set_dialog_global_option(b, "Footnotes",
				 ops->footnotes);
	_set_dialog_global_option(b, "Greek Accents",
				 ops->greekaccents);
	_set_dialog_global_option(b, "Lemmas",
				 ops->lemmas);
	_set_dialog_global_option(b, "Cross-references",
				 ops->scripturerefs);
	_set_dialog_global_option(b, "Hebrew Vowel Points",
				 ops->hebrewpoints);
	_set_dialog_global_option(b, "Hebrew Cantillation",
				 ops->hebrewcant);
	// always turn on headings in the engine (and later cache them).
	// whether we display them or not is another matter (display.cc).
	_set_dialog_global_option(b, "Headings", 1);
	_set_dialog_global_option(b, "Words of Christ in Red",
				 ops->words_in_red);

	_set_dialog_global_textual(b, "Transliteration",
				  (ops->transliteration
				   ? "Latin" : "Off"));

	if (ops->variants_primary)
		_set_dialog_global_textual(b, "Textual Variants",
					  "Primary Reading");
	else if (ops->variants_secondary)
		_set_dialog_global_textual(b, "Textual Variants",
					  "Secondary Reading");
	else if (ops->variants_all)
		_set_dialog_global_textual(b, "Textual Variants",
					  "All Readings");
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
	_set_global_option(ops->module_type, "Strong's Numbers",
			  ops->strongs);
	_set_global_option(ops->module_type, "Morphological Tags",
			  ops->morphs);
	_set_global_option(ops->module_type, "Footnotes",
			  ops->footnotes);
	_set_global_option(ops->module_type, "Greek Accents",
			  ops->greekaccents);
	_set_global_option(ops->module_type, "Lemmas",
			  ops->lemmas);
	_set_global_option(ops->module_type, "Cross-references",
			  ops->scripturerefs);
	_set_global_option(ops->module_type, "Hebrew Vowel Points",
			  ops->hebrewpoints);
	_set_global_option(ops->module_type, "Hebrew Cantillation",
			  ops->hebrewcant);
	// always turn on headings in the engine (and later cache them).
	// whether we display them or not is another matter (display.cc).
	_set_global_option(ops->module_type, "Headings", 1);
	_set_global_option(ops->module_type, "Words of Christ in Red",
			  ops->words_in_red);

	_set_global_textual(ops->module_type, "Transliteration",
			   (ops->transliteration
			    ? "Latin" : "Off"));

	if (ops->variants_primary)
		_set_global_textual(ops->module_type, "Textual Variants",
				   "Primary Reading");
	else if (ops->variants_secondary)
		_set_global_textual(ops->module_type, "Textual Variants",
				   "Secondary Reading");
	else if (ops->variants_all)
		_set_global_textual(ops->module_type, "Textual Variants",
				   "All Readings");
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

GLOBAL_OPS *main_new_globals(gchar * mod_name, int dialog)
{
	GLOBAL_OPS *ops;
	gchar *buf = NULL;

#if 0
	if (dialog)
		buf = g_strdup_printf("%s/modops-dialog.conf", settings.gSwordDir);
    	else
#endif
		buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);

	module_options.Load();
	g_free(buf);

	ops = g_new0(GLOBAL_OPS, 1);
	ops->module_type = 0;
	ops->dialog = dialog;
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
	ops->transliteration =
	    gui_of2tf(module_options[mod_name]["Transliteration"].c_str());

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
	}
	ops->verse_per_line = 
	    (*(module_options[mod_name]["style"].c_str()) == 'v')
	    ? 1	
	    : 0;//  otherwise, paragraph style.
	
	ops->image_content =
	    (*(module_options[mod_name]["Image Content"].c_str()) == '\0')
	    ? -1	// "unknown"; otherwise, it's like the others.
	    : gui_of2tf(module_options[mod_name]["Image Content"].c_str());

	ops->respect_font_faces =
	    (*(module_options[mod_name]["Respect Font Faces"].c_str()) == '\0')
	    ? -1	// "unknown"; otherwise, it's like the others.
	    : gui_of2tf(module_options[mod_name]["Respect Font Faces"].c_str());

	ops->commentary_by_chapter =
	    gui_of2tf(module_options[mod_name]["Commentary by Chapter"].c_str());

	ops->doublespace =
	    gui_of2tf(module_options[mod_name]["Doublespace"].c_str());

	return ops;
}
