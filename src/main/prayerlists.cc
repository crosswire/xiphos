/*
 * GnomeSword Bible Study Tool
 * prayerlists.cc - code to create several different prayer lists
 *
 * Copyright (C) 2008 GnomeSword Developer Team
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <swmodule.h>
#include <swmgr.h>
#include <treekeyidx.h>
#include <rawgenbook.h>
#include <swconfig.h>
#include <iostream>
#include <string>
#include <stdio.h>


#include "main/mod_mgr.h"
#include "main/prayerlists.h"
#include "main/sword_treekey.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"

#include "gui/dialog.h"
#include "gui/sidebar.h"

#include "backend/sword_main.hh"

#ifndef NO_SWORD_NAMESPACE
using sword::TreeKeyIdx;
using sword::RawGenBook;
using sword::SWKey;
using sword::SWConfig;
using sword::SWModule;
#endif


enum
{
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_CAPTION,
	COL_MODULE,
	COL_OFFSET,
	N_COLUMNS
};






/*********************************    *************************************/

static
void setEntryText (RawGenBook * book, gchar * text)
{
	TreeKeyIdx *treeKey = (TreeKeyIdx *) (SWKey *) (*book);
	if (treeKey->getOffset ()) {

		(*book) << text;
	}
}


/*
void setLocalName (TreeKeyIdx * treeKey, char *new_name)
{

	treeKey->setLocalName (buf);
	treeKey->save ();
}

*/
static
void appendChild (TreeKeyIdx * treeKey, gchar * name)
{
	treeKey->appendChild ();
	treeKey->setLocalName (name);
	treeKey->save ();
	GS_message (("name: %s\nlocalName: %s", name,
		     treeKey->getLocalName ()));
}


static
void appendSibbling (TreeKeyIdx * treeKey, gchar * name)
{
	if (treeKey->getOffset ()) {
		treeKey->append ();
		treeKey->setLocalName (name);
		treeKey->save ();
	}
}

static
void add_prayer_list_sections (RawGenBook * book, TreeKeyIdx * treeKey)
{
	appendChild (treeKey, "Growth");
	setEntryText (book, "<b>For Growth</b><br>");
	appendSibbling (treeKey, "Salvation");
	setEntryText (book, "<b>For Salvation</b><br>");
	appendSibbling (treeKey, "Health");
	setEntryText (book, "<b>For Health</b><br>");
	appendSibbling (treeKey, "Misc");
	setEntryText (book, "<b>Miscellaneous</b><br>");

	treeKey->parent ();
}



/******************************************************************************
 * Name
 *   main_prayer_list_new
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *
 *   gint main_prayer_list_new(gchar * list_name)
 *
 * Description
 *   create a new prayer list module and add it to the sidebar module tree
 *
 * Return value
 *   int
 */
int main_prayerlist_basic_create(void)
{	
	RawGenBook *book = NULL;
	gchar *path;
	TreeKeyIdx *treeKey;
	gint test;
	GS_DIALOG *info;
	char *list_name = NULL;

	//if (list_name == NULL) {
		info = gui_new_dialog ();
		info->stock_icon = GTK_STOCK_DIALOG_QUESTION;
		info->title = _("Prayer List");
		info->label_top = _("Name for new prayer list");
		info->label1 = _("Name: ");
		info->text1 = g_strdup (_("MyPrayerList"));
		info->ok = TRUE;
		info->cancel = TRUE;
	//}
	test = gui_gs_dialog (info);
	//list_name = "prayer";

	if (test == GS_OK) {
		list_name = info->text1;
	} else {
		g_free (info->text1);
		g_free (info);
		return 0;
	}

	path = g_strdup_printf ("%s/.sword/modules/genbook/rawgenbook/%s",
				settings.homedir, list_name);
	if (access (path, F_OK) == -1) {
		if ((mkdir (path, S_IRWXU)) != 0) {
			printf (_("can not create path\n"));
		}
	} else {
		printf (_("Prayer list already exist\n"));
		g_free (path);
		return 0;
	}
	g_free (path);
	path = g_strdup_printf ("./modules/genbook/rawgenbook/%s/%s",
				list_name, list_name);
	gchar *conf_path = g_strdup_printf ("%s/.sword/mods.d/%s.conf",
					    settings.homedir,
					    list_name);
	SWConfig config (conf_path);
	config[list_name]["DataPath"] = path;
	config[list_name]["ModDrv"] = "RawGenBook";
	config[list_name]["GSType"] = "PrayerList";
	config[list_name]["Encoding"] = "UTF-8";
	config[list_name]["Lang"] = "en";	/* fix me */
	config[list_name]["Version"] = "0.1";
	config[list_name]["MinimumVersion"] = "1.5.10";
	config[list_name]["DisplayLevel"] = "2";
	config[list_name]["Description"] = "Prayer List";
	config[list_name]["About"] =
		  "\\par\\par My prayer list \\par\\par Module created in GnomeSword";
	config.Save ();

	g_free (path);
	g_free (conf_path);
	path = g_strdup_printf ("%s/.sword/modules/genbook/rawgenbook/%s/%s",
				settings.homedir, list_name, list_name);

	RawGenBook::createModule (path);
	book = new RawGenBook (path);

	TreeKeyIdx root = *((TreeKeyIdx *) ((SWKey *) (*book)));
	treeKey = (TreeKeyIdx *) (SWKey *) (*book);

	appendChild (treeKey, "MyPrayerList");
	setEntryText (book, "<b>People:</b><br>Bob<br>Sam<br>Sue<br><br><b>Church:</b><br>pews<br>fellowship<br>Bibles for missionarys<br><br><br>");
	
	delete treeKey;
	g_free (info->text1);
	g_free (info);
	main_update_module_lists ();
	main_load_module_tree (sidebar.module_list);
	
}
 

int main_prayerlist_not_so_basic_create(void)
{
	
	RawGenBook *book = NULL;
	gchar *path;
	TreeKeyIdx *treeKey;
	gint test;
	GS_DIALOG *info;
	char *list_name = NULL;

	
	info = gui_new_dialog ();
	info->stock_icon = GTK_STOCK_DIALOG_QUESTION;
	info->title = _("Prayer List");
	info->label_top = _("Name for new prayer list");
	info->label1 = _("Name: ");
	info->text1 = g_strdup (_("MyPrayerList"));
	info->ok = TRUE;
	info->cancel = TRUE;
	
	test = gui_gs_dialog (info);
	//list_name = "prayer";

	if (test == GS_OK) {
		list_name = info->text1;
	} else {
		g_free (info->text1);
		g_free (info);
		return 0;
	}

	path = g_strdup_printf ("%s/.sword/modules/genbook/rawgenbook/%s",
				settings.homedir, list_name);
	if (access (path, F_OK) == -1) {
		if ((mkdir (path, S_IRWXU)) != 0) {
			printf (_("can not create path\n"));
		}
	} else {
		printf (_("Prayer list already exist\n"));
		g_free (path);
		return 0;
	}
	g_free (path);
	path = g_strdup_printf ("./modules/genbook/rawgenbook/%s/%s",
				list_name, list_name);
	gchar *conf_path = g_strdup_printf ("%s/.sword/mods.d/%s.conf",
					    settings.homedir,
					    list_name);
	SWConfig config (conf_path);
	config[list_name]["DataPath"] = path;
	config[list_name]["ModDrv"] = "RawGenBook";
	config[list_name]["GSType"] = "PrayerList";
	config[list_name]["Encoding"] = "UTF-8";
	config[list_name]["Lang"] = "en";	/* fix me */
	config[list_name]["Version"] = "0.1";
	config[list_name]["MinimumVersion"] = "1.5.10";
	config[list_name]["DisplayLevel"] = "2";
	config[list_name]["Description"] = "Prayer List";
	config[list_name]["About"] =
		  "\\par\\par My prayer list \\par\\par Module created in GnomeSword";
	config.Save ();

	g_free (path);
	g_free (conf_path);
	path = g_strdup_printf ("%s/.sword/modules/genbook/rawgenbook/%s/%s",
				settings.homedir, list_name, list_name);

	RawGenBook::createModule (path);
	book = new RawGenBook (path);

	TreeKeyIdx root = *((TreeKeyIdx *) ((SWKey *) (*book)));
	treeKey = (TreeKeyIdx *) (SWKey *) (*book);

	appendChild (treeKey, "Salvation");
	setEntryText (book, "Bob<br>Sam<br>Sue<br>John<br>");
	//add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "Spiritual Growth");
	setEntryText (book, "Mike<br>Steve<br>");
	//add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "Health");
	setEntryText (book, "Sue<br>John<br>");
	
	delete treeKey;
	g_free (info->text1);
	g_free (info);
	main_update_module_lists ();
	main_load_module_tree (sidebar.module_list);
	
	
}


int main_prayerlist_wild_create(void)
{
	RawGenBook *book = NULL;
	gchar *path;
	TreeKeyIdx *treeKey;
	gint test;
	GS_DIALOG *info;
	char *list_name = NULL;
	
	info = gui_new_dialog ();
	info->stock_icon = GTK_STOCK_DIALOG_QUESTION;
	info->title = _("Prayer List");
	info->label_top = _("Name for new prayer list");
	info->label1 = _("Name: ");
	info->text1 = g_strdup (_("MyPrayerList"));
	info->ok = TRUE;
	info->cancel = TRUE;
	
	test = gui_gs_dialog (info);
	//list_name = "prayer";

	if (test == GS_OK) {
		list_name = info->text1;
	} else {
		g_free (info->text1);
		g_free (info);
		return 0;
	}

	path = g_strdup_printf ("%s/.sword/modules/genbook/rawgenbook/%s",
				settings.homedir, list_name);
	if (access (path, F_OK) == -1) {
		if ((mkdir (path, S_IRWXU)) != 0) {
			printf (_("can not create path\n"));
		}
	} else {
		printf (_("Prayer list already exist\n"));
		g_free (path);
		return 0;
	}
	g_free (path);
	path = g_strdup_printf ("./modules/genbook/rawgenbook/%s/%s",
				list_name, list_name);
	gchar *conf_path = g_strdup_printf ("%s/.sword/mods.d/%s.conf",
					    settings.homedir,
					    list_name);
	SWConfig config (conf_path);
	config[list_name]["DataPath"] = path;
	config[list_name]["ModDrv"] = "RawGenBook";
	config[list_name]["GSType"] = "PrayerList";
	config[list_name]["Encoding"] = "UTF-8";
	config[list_name]["Lang"] = "en";	/* fix me */
	config[list_name]["Version"] = "0.1";
	config[list_name]["MinimumVersion"] = "1.5.10";
	config[list_name]["DisplayLevel"] = "2";
	config[list_name]["Description"] = "Prayer List";
	config[list_name]["About"] =
		  "\\par\\par My prayer list \\par\\par Module created in GnomeSword";
	config.Save ();

	g_free (path);
	g_free (conf_path);
	path = g_strdup_printf ("%s/.sword/modules/genbook/rawgenbook/%s/%s",
				settings.homedir, list_name, list_name);

	RawGenBook::createModule (path);
	book = new RawGenBook (path);

	TreeKeyIdx root = *((TreeKeyIdx *) ((SWKey *) (*book)));
	treeKey = (TreeKeyIdx *) (SWKey *) (*book);

	appendChild (treeKey, "01");
	setEntryText (book, "<b>January</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "02");
	setEntryText (book, "<b>Febuary</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "03");
	setEntryText (book, "<b>March</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "04");
	setEntryText (book, "<b>April</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "05");
	setEntryText (book, "<b>May</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "06");
	setEntryText (book, "<b>June</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "07");
	setEntryText (book, "<b>July</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "08");
	setEntryText (book, "<b>August</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "09");
	setEntryText (book, "<b>September</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "10");
	setEntryText (book, "<b>October</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "11");
	setEntryText (book, "<b>November</b><br>");
	add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "12");
	setEntryText (book, "<b>December</b><br>");
	add_prayer_list_sections (book, treeKey);

	delete treeKey;
	g_free (info->text1);
	g_free (info);
	main_update_module_lists ();
	main_load_module_tree (sidebar.module_list);
	
	
	
}
