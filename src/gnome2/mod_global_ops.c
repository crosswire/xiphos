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


#include "gui/mod_global_ops.h"



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

void gui_set_global_options(GLOBAL_OPS * ops)
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
		if (ops->variants_primary)
			set_text_global_option("Textual Variants",
					       "Primary Reading");
		else if (ops->variants_secondary)
			set_text_global_option("Textual Variants",
					       "Secondary Reading");
		else
			set_text_global_option("Textual Variants",
					       "All Readings");
	}
}


/******************************************************************************
 * Name
 *  global_option_red_words
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_red_words(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_red_words(GtkMenuItem * menuitem,
				    GLOBAL_OPS * ops)
{
	ops->words_in_red = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Words of Christ in Red",
					   ops->words_in_red);
		break;
	case 3:
		gui_update_gbs_global_ops("Words of Christ in Red",
					  ops->words_in_red);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_strongs
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_strongs(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_strongs(GtkMenuItem * menuitem,
				  GLOBAL_OPS * ops)
{
	ops->strongs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Strong's Numbers",
					   ops->strongs);
		break;
	case 3:
		gui_update_gbs_global_ops("Strong's Numbers",
					  ops->strongs);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_morph
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_morph(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_morph(GtkMenuItem * menuitem,
				GLOBAL_OPS * ops)
{
	ops->morphs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Morphological Tags",
					   ops->morphs);
		break;
	case 3:
		gui_update_gbs_global_ops("Morphological Tags",
					  ops->morphs);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_footnotes
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_footnotes(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_footnotes(GtkMenuItem * menuitem,
				    GLOBAL_OPS * ops)
{
	ops->footnotes = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Footnotes", ops->footnotes);
		break;
	case 3:
		gui_update_gbs_global_ops("Footnotes", ops->footnotes);
		break;
	}
}

/******************************************************************************
 * Name
 *  global_option_greekaccents
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_greekaccents(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_greekaccents(GtkMenuItem * menuitem,
				       GLOBAL_OPS * ops)
{
	ops->greekaccents = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Greek Accents",
					   ops->greekaccents);
		break;
	case 3:
		gui_update_gbs_global_ops("Greek Accents",
					  ops->greekaccents);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_lemmas
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_lemmas(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_lemmas(GtkMenuItem * menuitem,
				 GLOBAL_OPS * ops)
{
	ops->lemmas = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Lemmas", ops->lemmas);
		break;
	case 3:
		gui_update_gbs_global_ops("Lemmas", ops->lemmas);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_scripturerefs
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_scripturerefs(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_scripturerefs(GtkMenuItem * menuitem,
					GLOBAL_OPS * ops)
{
	ops->scripturerefs = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Cross-references",
					   ops->scripturerefs);
		break;
	case 3:
		gui_update_gbs_global_ops("Cross-references",
					  ops->scripturerefs);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_hebrewpoints
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_hebrewpoints(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_hebrewpoints(GtkMenuItem * menuitem,
				       GLOBAL_OPS * ops)
{
	ops->hebrewpoints = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Hebrew Vowel Points",
					   ops->hebrewpoints);
		break;
	case 3:
		gui_update_gbs_global_ops("Hebrew Vowel Points",
					  ops->hebrewpoints);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_hebrewcant
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_hebrewcant(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_hebrewcant(GtkMenuItem * menuitem,
				     GLOBAL_OPS * ops)
{
	ops->hebrewcant = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Hebrew Cantillation",
					   ops->hebrewcant);
		break;
	case 3:
		gui_update_gbs_global_ops("Hebrew Cantillation",
					  ops->hebrewcant);
		break;
	}
}


/******************************************************************************
 * Name
 *  global_option_headings
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_headings(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_headings(GtkMenuItem * menuitem,
				   GLOBAL_OPS * ops)
{
	ops->headings = GTK_CHECK_MENU_ITEM(menuitem)->active;
	switch (ops->module_type) {
	case 0:
		gui_update_text_global_ops("Headings", ops->headings);
		break;
	case 3:
		gui_update_gbs_global_ops("Headings", ops->headings);
		break;
	}
}



/******************************************************************************
 * Name
 *  global_option_variants_all
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_variants_all(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_all(GtkMenuItem * menuitem,
				       GLOBAL_OPS * ops)
{
	ops->variants_all = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (ops->variants_all) {

	}
}


/******************************************************************************
 * Name
 *  global_option_variants_primary
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_variants_primary(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_primary(GtkMenuItem * menuitem,
					   GLOBAL_OPS * ops)
{
	//gchar *text_str;

	ops->variants_primary = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (ops->variants_primary) {
		/*
		   text_str = get_commentary_text(c->mod_name, c->key);
		   entry_display(c->html, c->mod_name, text_str, c->key, TRUE);
		   free(text_str);
		 */
	}
}


/******************************************************************************
 * Name
 *    global_option_variants_scondary
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   void global_option_variants_scondary(GtkMenuItem * menuitem,
				      GBS_DATA * g)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

static void global_option_variants_scondary(GtkMenuItem * menuitem,
					    GLOBAL_OPS * ops)
{
	ops->variants_secondary = GTK_CHECK_MENU_ITEM(menuitem)->active;
	if (ops->variants_secondary) {

	}
}


/******************************************************************************
 * Name
 *    
 *
 * Synopsis
 *   #include "gui/.h"
 *
 *   
 *
 * Description
 *   add global module options to popup menus
 *
 * Return value
 *   void
 */

void gui_add_global_option_items(gchar * mod_name,
				 GtkWidget * module_options_menu,
				 gint type_module, GLOBAL_OPS * ops)
{
	GtkWidget *item;
	GtkWidget *menu;
	GSList *group = NULL;


	if ((check_for_global_option(mod_name, "GBFRedLetterWords"))
	    ||
	    (check_for_global_option(mod_name, "OSISRedLetterWords"))) {
		if (!ops->module_type)
			ops->words_in_red =
			    load_module_options(mod_name,
						"Words of Christ in Red");


		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Words of Christ in Red"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->words_in_red;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_red_words), ops);
	}

	if ((check_for_global_option(mod_name, "GBFStrongs")) ||
	    (check_for_global_option(mod_name, "ThMLStrongs")) ||
	    (check_for_global_option(mod_name, "OSISStrongs"))) {
		if (!ops->module_type)
			ops->strongs =
			    load_module_options(mod_name,
						"Strong's Numbers");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Strong's Numbers"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->strongs;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_strongs), ops);
	}

	if ((check_for_global_option(mod_name, "GBFMorph")) ||
	    (check_for_global_option(mod_name, "ThMLMorph")) ||
	    (check_for_global_option(mod_name, "OSISMorph"))) {
		if (!ops->module_type)
			ops->morphs =
			    load_module_options(mod_name,
						"Morphological Tags");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Morphological Tags"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->morphs;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK(global_option_morph), ops);
	}

	if ((check_for_global_option(mod_name, "GBFFootnotes")) ||
	    (check_for_global_option(mod_name, "ThMLFootnotes")) ||
	    (check_for_global_option(mod_name, "OSISFootnotes"))) {
		if (!ops->module_type)
			ops->footnotes =
			    load_module_options(mod_name, "Footnotes");

		item =
		    gtk_check_menu_item_new_with_label(_("Footnotes"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->footnotes;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_footnotes), ops);
	}

	if (check_for_global_option(mod_name, "UTF8GreekAccents")) {
		if (!ops->module_type)
			ops->greekaccents =
			    load_module_options(mod_name,
						"Greek Accents");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Greek Accents"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->greekaccents;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_greekaccents), ops);
	}

	if (check_for_global_option(mod_name, "ThMLLemma")) {
		if (!ops->module_type)
			ops->lemmas =
			    load_module_options(mod_name, "Lemmas");

		item = gtk_check_menu_item_new_with_label(_("Lemmas"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->lemmas;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK(global_option_lemmas), ops);
	}

	if (check_for_global_option(mod_name, "ThMLScripref") ||
	    (check_for_global_option(mod_name, "OSISScripref"))) {
		if (!ops->module_type)
			ops->scripturerefs =
			    load_module_options(mod_name,
						"Scripture Cross-references");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Scripture Cross-references"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->scripturerefs;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_scripturerefs), ops);
	}

	if (check_for_global_option(mod_name, "UTF8HebrewPoints")) {
		if (!ops->module_type)
			ops->hebrewpoints =
			    load_module_options(mod_name,
						"Hebrew Vowel Points");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Hebrew Vowel Points"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->hebrewpoints;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_hebrewpoints), ops);
	}

	if (check_for_global_option(mod_name, "UTF8Cantillation")) {
		if (!ops->module_type)
			ops->hebrewcant =
			    load_module_options(mod_name,
						"Hebrew Cantillation");

		item =
		    gtk_check_menu_item_new_with_label(_
						       ("Hebrew Cantillation"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->hebrewcant;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_hebrewcant), ops);
	}

	if (check_for_global_option(mod_name, "ThMLHeadings") ||
	    (check_for_global_option(mod_name, "OSISHeadings"))) {
		if (!ops->module_type)
			ops->headings =
			    load_module_options(mod_name, "Headings");

		item =
		    gtk_check_menu_item_new_with_label(_("Headings"));
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);

		GTK_CHECK_MENU_ITEM(item)->active = ops->headings;
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_headings), ops);
	}

	if (check_for_global_option(mod_name, "ThMLVariants")) {
		if (!ops->module_type) {
			ops->variants_all =
			    load_module_options(mod_name,
						"All Readings");
			ops->variants_primary =
			    load_module_options(mod_name,
						"Primary Reading");
			ops->variants_secondary =
			    load_module_options(mod_name,
						"Secondary Reading");
		}
		item = gtk_menu_item_new_with_label(_("Variants"));
		gtk_container_add(GTK_CONTAINER(module_options_menu),
				  item);
		gtk_widget_show(item);

		menu = gtk_menu_new();
		gtk_menu_item_set_submenu(GTK_MENU_ITEM(item), menu);

		item =
		    gtk_radio_menu_item_new_with_label(group,
						       _
						       ("All Readings"));
/*		group =
		    gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM
					      (item));*/
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		if (ops->variants_all)
			gtk_check_menu_item_set_active
			    (GTK_CHECK_MENU_ITEM(item),
			     ops->variants_all);
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_variants_all), ops);

		item =
		    gtk_radio_menu_item_new_with_label(group,
						       _
						       ("Primary Readings"));
		/*group =
		   gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM
		   (item)); */
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		if (ops->variants_primary)
			gtk_check_menu_item_set_active
			    (GTK_CHECK_MENU_ITEM(item),
			     ops->variants_primary);
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_variants_primary), ops);

		item =
		    gtk_radio_menu_item_new_with_label(group,
						       _
						       ("Secondary Readings"));
		/*group =
		   gtk_radio_menu_item_group(GTK_RADIO_MENU_ITEM
		   (item)); */
		gtk_widget_show(item);
		gtk_container_add(GTK_CONTAINER(menu), item);
		g_signal_connect(GTK_OBJECT(item), "toggled",
				 G_CALLBACK
				 (global_option_variants_scondary),
				 ops);
		if (ops->variants_secondary)
			gtk_check_menu_item_set_active
			    (GTK_CHECK_MENU_ITEM(item),
			     ops->variants_secondary);
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

GLOBAL_OPS *gui_new_globals(void)
{
	GLOBAL_OPS *ops;
	ops = g_new0(GLOBAL_OPS, 1);
	ops->module_type = 0;
	ops->words_in_red = FALSE;
	ops->strongs = FALSE;
	ops->morphs = FALSE;
	ops->footnotes = FALSE;
	ops->greekaccents = FALSE;
	ops->lemmas = FALSE;
	ops->scripturerefs = FALSE;
	ops->hebrewpoints = FALSE;
	ops->hebrewcant = FALSE;
	ops->headings = FALSE;
	ops->variants_all = FALSE;
	ops->variants_primary = FALSE;
	ops->variants_secondary = FALSE;
	return ops;
}
