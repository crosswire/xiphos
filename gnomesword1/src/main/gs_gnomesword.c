/*
 * GnomeSword Bible Study Tool
 * gnomesword.c - glue
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <ctype.h>
#include <time.h>

#include "gui/main_window.h"
#include "gui/main_menu.h"
#include "gui/about_modules.h"
#include "gui/shortcutbar_main.h"
#include "gui/shortcutbar_dialog.h"
#include "gui/shortcutbar_viewer.h"
#include "gui/studypad.h"
#include "gui/info_box.h"
#include "gui/gbs.h"
#include "gui/bibletext.h"
#include "gui/bibletext_dialog.h"
#include "gui/commentary.h"
#include "gui/commentary_dialog.h"
#include "gui/dictlex.h"
#include "gui/dictlex_dialog.h"
#include "gui/percomm.h"
#include "gui/interlinear.h"
#include "gui/bookmarks.h"
#include "gui/history.h"
#include "gui/toolbar_nav.h"
#include "gui/utilities.h"
#include "gui/html.h"
#include "gui/interlinear.h"

#include "main/gs_gnomesword.h"
#include "main/settings.h"
#include "main/lists.h"
 
#include "backend/sword.h"



/******************************************************************************
 * Name
 *  init_gnomesword
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void init_gnomesword(void)	
 *
 * Description
 *    sets up the interface
 *
 * Return value
 *   void
 */ 
 
 
void init_gnomesword(void)
{	
	
	
	settings.displaySearchResults = FALSE;
	settings.havethayer = backend_check_for_module("Thayer");
	settings.havebdb = backend_check_for_module("BDB");
	/*
	 *  setup shortcut bar 
	 */
	gui_setup_shortcut_bar();

	/*
	 *  interlinear stuff 
	 */
	gui_set_interlinear_options_at_start();
	gui_create_interlinear_popup(get_list(TEXT_DESC_LIST));
	/*
	 *  setup Bible text gui 
	 */
	if (settings.havebible) {
		gui_setup_text(get_list(TEXT_LIST));
		gui_setup_bibletext_dialog(get_list(TEXT_LIST));
	}
	
	/*
	 *  setup commentary gui support 
	 */
	if (settings.havecomm) {
		gui_setup_commentary(get_list(COMM_LIST));	
		gui_setup_commentary_dialog(get_list(COMM_LIST));
	}
	
	/*
	 *  setup personal comments gui support 
	 */
	if (settings.havepercomm) {
		gui_setup_percomm(get_list(PERCOMM_LIST));
	}

	/*
	 *  setup general book gui support 
	 */
	if (settings.havebook) {
		gui_setup_gbs(get_list(GBS_LIST));
	}

	/*
	 *  setup Dict/Lex gui support 
	 */
	if (settings.havedict) {
		gui_setup_dictlex(get_list(DICT_LIST));
		gui_setup_dictlex_dialog(get_list(DICT_LIST));
	}
	
	g_print("%s\n", "Initiating GnomeSWORD\n");

	gui_set_shortcutbar_porgram_start();
	
	gui_show_main_window();
	
	settings.addhistoryitem = FALSE;
	
	gui_change_verse(settings.currentverse);
	
	/* showing the devotional must come after the the app is shown or
	 *  it will mess up the shortcut bar display 
	 */
	/* 
	 * FIXME: maybe we need to move the devotional ? 
	 */
	if (settings.showdevotional) {
		display_devotional();
	}
	
	g_print("done\n");
}


/******************************************************************************
 * Name
 *  gnomesword_shutdown
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void gnomesword_shutdown(void)
 *
 * Description
 *    do an orderly shutdown
 *
 * Return value
 *   void
 */ 

void gnomesword_shutdown(void)
{
	gui_save_bookmarks(NULL,NULL);
	
	/* if study pad file has changed since last save */
	gui_studypad_can_close();
		
	/* shutdown the sword stuff */
	backend_shutdown();
	
	shutdown_list();
	
	/* free dir and file stuff */
	g_free(settings.gSwordDir);
	g_free(settings.shortcutbarDir);
	g_free(settings.fnconfigure);
	g_free(settings.swbmDir);
	
	if(settings.havebible){
		gui_shutdown_text();
		gui_shutdown_bibletext_dialog();
	}
	if(settings.havebook)
		gui_shutdown_gbs();
	if(settings.havecomm){
		gui_shutdown_commentary();
		gui_shutdown_commentary_dialog();
	}
	if(settings.havedict){
		gui_shutdown_dictlex();
		gui_shutdown_dictlex_dialog();
	}
	if(settings.havepercomm)
		gui_shutdown_percomm();
	
	g_print("\nGnomeSWORD is shutdown\n");
}


/******************************************************************************
 * Name
 *  string_is_color
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   	gint string_is_color(gchar * color)
 *
 * Description
 *    this code is from bluefish-0.6
 *
 * Return value
 *   gint
 */


gint string_is_color(gchar * color)
{
	gint i;

	if (!color) {
		g_warning("string_is_color, pointer NULL\n");
		return 0;
	}
	if (strlen(color) != 7) {
		g_warning("string_is_color, strlen(%s) != 7\n", color);
		return 0;
	}
	if (color[0] != '#') {
		g_warning("string_is_color, 0 in %s is not #\n", color);
		return 0;
	}
	for (i = 1; i < 7; i++) {
		if ((color[i] > 102)
		    || (color[i] < 48)
		    || ((color[i] > 57) && (color[i] < 65))
		    || ((color[i] > 70) && (color[i] < 97))) {
			g_warning
			    ("string_is_color, %d in %s is not from a color, it is %d\n",
			     i, color, color[i]);
			return 0;
		}
	}
	//g_warning("string_is_color, %s is color\n", color);
	return 1;

}


/******************************************************************************
 * Name
 *  gdouble_arr_to_hex
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gchar *gdouble_arr_to_hex(gdouble * color, gint websafe)	
 *
 * Description
 *    this code is from bluefish-0.6
 *
 * Return value
 *   gchar *
 */ 

gchar *gdouble_arr_to_hex(gdouble * color, gint websafe)
{
	gchar *tmpstr;
	unsigned int red_int;
	unsigned int green_int;
	unsigned int blue_int;
	gdouble red;
	gdouble green;
	gdouble blue;

	red = color[0];
	green = color[1];
	blue = color[2];

	if (websafe) {
		red_int = 0x33 * ((unsigned int) (red * 255 / 0x33));
		green_int =
		    0x33 * ((unsigned int) (green * 255 / 0x33));
		blue_int = 0x33 * ((unsigned int) (blue * 255 / 0x33));
	} else {
		red_int = (unsigned int) (red * 255);
		green_int = (unsigned int) (green * 255);
		blue_int = (unsigned int) (blue * 255);
	}
	tmpstr = g_malloc(8 * sizeof(char));
	g_snprintf(tmpstr, 8, "#%.2X%.2X%.2X", red_int, green_int,
		   blue_int);
	return tmpstr;
}


/******************************************************************************
 * Name
 *  hex_to_gdouble_arr
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   gdouble *hex_to_gdouble_arr(gchar * color)	
 *
 * Description
 *    hex_to_gdouble_arr -- this code is from bluefish-0.6
 *
 * Return value
 *   gdouble *
 */ 

gdouble *hex_to_gdouble_arr(gchar * color)
{
	static gdouble tmpcol[4];
	gchar tmpstr[8];
	long tmpl;


	strncpy(tmpstr, &color[1], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[0] = (gdouble) tmpl;

	strncpy(tmpstr, &color[3], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[1] = (gdouble) tmpl;

	strncpy(tmpstr, &color[5], 2);
	tmpl = strtol(tmpstr, NULL, 16);
	tmpcol[2] = (gdouble) tmpl;

	tmpcol[3] = 0;

	return tmpcol;
}


/******************************************************************************
 * Name
 *  display_devotional
 *
 * Synopsis
 *   #include "gnomesword.h"
 *
 *   void display_devotional(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void display_devotional(void)
{
	gchar buf[80];
	time_t curtime;
	struct tm *loctime;

	/* 
	 * Get the current time. 
	 */
	curtime = time(NULL);

	/* 
	 * Convert it to local time representation. 
	 */
	loctime = localtime(&curtime);

	/* 
	 * Print it out in a nice format. 
	 */
	strftime(buf, 80, "%m.%d", loctime);

	gui_display_dictlex_in_viewer(settings.devotionalmod, buf);
	set_sb_for_daily_devotion();
}
