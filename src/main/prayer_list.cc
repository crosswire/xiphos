/*
 * GnomeSword Bible Study Tool
 * prayer_list.cc - 
 *
 * Copyright (C) 2007 GnomeSword Developer Team
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
#include <treekeyidx.h>
#include <rawgenbook.h>
#include <swconfig.h>
#include <iostream>
#include <string>
#include <stdio.h>

#include "main/prayer_list.h"
#include "main/settings.h"
#include "main/sword.h"


#include "backend/sword_main.hh"

#ifndef NO_SWORD_NAMESPACE
using sword::TreeKeyIdx;
using sword::RawGenBook;
using sword::SWKey;
using sword::SWConfig;
#endif




void printTree(TreeKeyIdx treeKey, TreeKeyIdx *target = 0, int level = 1) {
	if (!target)
		target = &treeKey;
	unsigned long currentOffset = target->getOffset();
	std::cout << ((currentOffset == treeKey.getOffset()) ? "==>" : "");
	for (int i = 0; i < level; i++) std::cout << "\t";
	std::cout << treeKey.getLocalName() << "/\n";
	if (treeKey.firstChild()) {
		printTree(treeKey, target, level+1);
		treeKey.parent();
	}
	if (treeKey.nextSibling())
		printTree(treeKey, target, level);

}
/*
void main_fill_prayer_list_tree(gchar * mod_name)
{
	//backend->set_module(mod_name);
	//SWModule *book = backend->display_mod;
	gchar *path2book = g_strdup_printf("%s/.sword/modules/genbook/rawgenbook/%s/%s", 
				settings.homedir,mod_name, 
				mod_name);
	RawGenBook *book =  new RawGenBook(path2book);
	g_message(book->Name());
	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);
	root.root(); printTree(root, treeKey);
	
}
*/

void setEntryText(RawGenBook *book, gchar * text) 
{
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);
	if (treeKey->getOffset()) {

		(*book) << text;
	}
}

void appendSibbling(TreeKeyIdx *treeKey, gchar * name) 
{
	if (treeKey->getOffset()) {
		treeKey->append();
		treeKey->setLocalName(name);
		treeKey->save();
	}
}

void appendChild(TreeKeyIdx *treeKey, gchar * name)
{	
	treeKey->appendChild();
	treeKey->setLocalName(name);
	treeKey->save();
	GS_message(("name: %s\nlocalName: %s",name,treeKey->getLocalName()));
}

void add_prayer_list_sections(RawGenBook * book, TreeKeyIdx * treeKey)
{
	appendChild(treeKey, "Growth");
	setEntryText(book, "<b>For Growth</b><br>");
	appendSibbling(treeKey, "Salvation");
	setEntryText(book, "<b>For Salvation</b><br>");
	appendSibbling(treeKey, "Health");
	setEntryText(book, "<b>For Health</b><br>");
	appendSibbling(treeKey, "Misc");
	setEntryText(book, "<b>Miscellaneous</b><br>");
	
	treeKey->parent();
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

gint main_prayer_list_new(gchar * list_name)
{
  	RawGenBook *book = NULL;
	gchar *path;	
	TreeKeyIdx * treeKey;
	
	if(list_name == NULL)
		list_name = "prayer";
	
	path = g_strdup_printf("%s/.sword/modules/genbook/rawgenbook/%s", 
				settings.homedir, 
				list_name);
	if (access(path, F_OK) == -1) {
		if ((mkdir(path, S_IRWXU)) != 0) {
			printf(_("can not create path\n"));
		} 
	} else {
		printf(_("Prayer list already exist\n"));
		g_free(path);
		return 0;
	}
	g_free(path);
	path = g_strdup_printf("./modules/genbook/rawgenbook/%s/%s", 
				list_name, 
				list_name);
	gchar *conf_path = g_strdup_printf("%s/.sword/mods.d/%s.conf", 
				settings.homedir, 
				list_name);
	SWConfig config(conf_path);	
	config[list_name]["DataPath"] = path;
	config[list_name]["ModDrv"] = "RawGenBook";
	config[list_name]["GSType"] = "PrayerList";
	config[list_name]["Encoding"] = "UTF-8";
	config[list_name]["Lang"] = "en"; /* fix me */
	config[list_name]["Version"] = "0.1";
	config[list_name]["MinimumVersion"] = "1.5.9";
	config[list_name]["DisplayLevel"] = "2";
	config[list_name]["Description"] = "Prayer List";
	config[list_name]["About"] = "\\par\\par My prayer list \\par\\par Module created in GnomeSword";
	config.Save();
	
	g_free(path);
	g_free(conf_path);
	path = g_strdup_printf("%s/.sword/modules/genbook/rawgenbook/%s/%s", 
				settings.homedir, 
				list_name,
				list_name);
	
    	RawGenBook::createModule(path);
   	book = new RawGenBook(path);
	
	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	treeKey = (TreeKeyIdx *)(SWKey *)(*book);
	appendChild(treeKey, "01");
	setEntryText(book, "<b>January</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "02");
	setEntryText(book, "<b>Febuary</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "03");
	setEntryText(book, "<b>March</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "04");
	setEntryText(book, "<b>April</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "05");
	setEntryText(book, "<b>May</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "06");
	setEntryText(book, "<b>June</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "07");
	setEntryText(book, "<b>July</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "08");
	setEntryText(book, "<b>August</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "09");
	setEntryText(book, "<b>September</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "10");
	setEntryText(book, "<b>October</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "11");
	setEntryText(book, "<b>November</b><br>");
	add_prayer_list_sections(book, treeKey);
	appendSibbling(treeKey, "12");
	setEntryText(book, "<b>December</b><br>");
	add_prayer_list_sections(book, treeKey);
	
	delete treeKey;
}

void main_prayer_list_add_child(gchar * mod_name)
{
	backend->set_module(mod_name);
	SWModule *book = backend->display_mod;
	
	GS_message((book->Name()));
	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);
	root.root(); printTree(root, treeKey);
	
}
