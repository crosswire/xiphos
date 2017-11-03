/*
 * Xiphos Bible Study Tool
 * mod_global_ops.c - setup for SWORD global options and
 *                    a few of our own in the gui
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
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
#include "main/sword.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

static void _set_global_option(const char *option,
			       gboolean choice)
{
	SWMgr *mgr = backend->get_mgr();
	char *on_off;

	on_off = gui_tf2of(choice);
	mgr->setGlobalOption(option, on_off);
}

void _set_global_textual(const char *option,
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
			     int choice)
{
	gchar *buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);

	module_options.load();
	g_free(buf);

	module_options[mod_name][option] = (choice ? "On" : "Off");

	module_options.save();
	return true;
}

/******************************************************************************
 * Name
 *  main_get_option
 *
 * Synopsis
 *   #include "main/mod_global_ops.h"
 *
 *   void main_get_one_option(const char *mod_name, const char *op)
 *
 * Description
 *   obtain a single option
 *
 * Return value
 *   int
 */

int
main_get_one_option(const char *mod_name, const char *op)
{
	gchar *buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);

	module_options.load();
	g_free(buf);

	return gui_of2tf(module_options[mod_name][op].c_str());
}

/******************************************************************************
 * Name
 *  main_set_global_options
 *
 * Synopsis
 *   #include "main/global_ops.h"
 *
 *   void main_set_global_options(GLOBAL_OPS * ops)
 *
 * Description
 *   set module global options
 *
 * Return value
 *   void
 */

void main_set_global_options(GLOBAL_OPS *ops)
{
	_set_global_option("Strong's Numbers", ops->strongs);
	_set_global_option("Morphological Tags", ops->morphs);
	_set_global_option("Footnotes", ops->footnotes);
	_set_global_option("Greek Accents", ops->greekaccents);
	_set_global_option("Lemmas", ops->lemmas);
	_set_global_option("Cross-references", ops->scripturerefs);
	_set_global_option("Hebrew Vowel Points", ops->hebrewpoints);
	_set_global_option("Hebrew Cantillation", ops->hebrewcant);

	// always turn on headings in the engine (and later cache them).
	// whether we display them or not is another matter (display.cc).
	_set_global_option("Headings", 1);

	_set_global_option("Words of Christ in Red", ops->words_in_red);
	_set_global_option("Transliterated Forms", ops->xlit);
	_set_global_option("Enumerations", ops->enumerated);
	_set_global_option("Glosses", ops->glosses);
	_set_global_option("Morpheme Segmentation", ops->morphseg);

	_set_global_textual("Transliteration", (ops->transliteration
						    ? "Latin"
						    : "Off"));

	if (ops->variants_primary)
		_set_global_textual("Textual Variants", "Primary Reading");
	else if (ops->variants_secondary)
		_set_global_textual("Textual Variants", "Secondary Reading");
	else if (ops->variants_all)
		_set_global_textual("Textual Variants", "All Readings");
}

/******************************************************************************
 * Name
 *    main_new_globals
 *
 * Synopsis
 *   #include "main/global_ops.h"
 *
 *   GLOBAL_OPS *main_new_globals(gchar *mod_name)
 *
 * Description
 *   obtain all options for the module.
 *
 * Return value
 *   GLOBAL_OPS *
 */

GLOBAL_OPS *main_new_globals(const gchar *mod_name)
{
	GLOBAL_OPS *ops;
	gchar *buf = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	SWConfig module_options(buf);

	module_options.load();
	g_free(buf);

	ops = g_new0(GLOBAL_OPS, 1);
	ops->strongs =
	    gui_of2tf(module_options[mod_name]["Strong's Numbers"].c_str());
	ops->morphs =
	    gui_of2tf(module_options[mod_name]["Morphological Tags"].c_str());
	ops->lemmas =
	    gui_of2tf(module_options[mod_name]["Lemmas"].c_str());
	ops->transliteration =
	    gui_of2tf(module_options[mod_name]["Transliteration"].c_str());
	ops->xlit =
	    gui_of2tf(module_options[mod_name]["Transliterated Forms"].c_str());
	ops->enumerated =
	    gui_of2tf(module_options[mod_name]["Enumerations"].c_str());
	ops->morphseg =
	    gui_of2tf(module_options[mod_name]["Morpheme Segmentation"].c_str());

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

	// special case options, default on:
	ops->words_in_red =
	    (module_options[mod_name]["Words of Christ in Red"] != "Off");
	ops->headings =
	    (module_options[mod_name]["Headings"] != "Off");
	ops->footnotes =
	    (module_options[mod_name]["Footnotes"] != "Off");
	ops->scripturerefs =
	    (module_options[mod_name]["Cross-references"] != "Off");
	ops->xrefnotenumbers =
	    (module_options[mod_name]["XrefNoteNumbers"] != "Off");

	// more special case default on: heb/grk support.
	ops->greekaccents =
	    (module_options[mod_name]["Greek Accents"] != "Off");
	ops->hebrewpoints =
	    (module_options[mod_name]["Hebrew Vowel Points"] != "Off");
	ops->hebrewcant =
	    (module_options[mod_name]["Hebrew Cantillation"] != "Off");

	// more special case default on: japanese glosses.
	ops->glosses =
	    (module_options[mod_name]["Glosses"] != "Off");

	// we prefer and assume paragraph layout.
	// we take user preference, and alter our default for
	// those modules showing proper configuration for it.
	ops->verse_per_line =
	    ((*(module_options[mod_name]["style"].c_str()) == 'v')
		 ? 1
		 : ((*(module_options[mod_name]["style"].c_str()) == 'p')
			? 0
			: (main_check_for_option(mod_name, "Feature", "NoParagraphs")
			       ? 1
			       : 0))); // default to paragraph style.

	// options not from configuration: xiphos-specific capability.
	ops->image_content =
	    (*(module_options[mod_name]["Image Content"].c_str()) == '\0')
		? -1 // "unknown"; otherwise, it's like the others.
		: gui_of2tf(module_options[mod_name]["Image Content"].c_str());

	ops->respect_font_faces =
	    (*(module_options[mod_name]["Respect Font Faces"].c_str()) == '\0')
		? -1 // "unknown"; otherwise, it's like the others.
		: gui_of2tf(module_options[mod_name]["Respect Font Faces"].c_str());

	ops->commentary_by_chapter =
	    gui_of2tf(module_options[mod_name]["Commentary by Chapter"].c_str());

	ops->doublespace =
	    gui_of2tf(module_options[mod_name]["Doublespace"].c_str());

	return ops;
}
