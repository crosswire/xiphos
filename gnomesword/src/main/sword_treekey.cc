/*
 * GnomeSword Bible Study Tool
 * sword_treekey.cc - treekey stuff for book editor
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

static char *mod_name;
static gchar buf[256];
static gchar *tmpbuf;






static void add_leaf_node_to_treeview (GtkTreeStore * model,
				       GtkTreeIter * iter)
{				/* has no children */
	gtk_tree_store_set (GTK_TREE_STORE (model),
			    iter,
			    COL_OPEN_PIXBUF, pixbufs->pixbuf_helpdoc,
			    COL_CLOSED_PIXBUF, NULL,
			    COL_CAPTION, (gchar *) tmpbuf,
			    COL_MODULE, (gchar *) mod_name,
			    COL_OFFSET, (gchar *) buf, -1);
}

static void add_parent_to_treeview (GtkTreeStore * model, GtkTreeIter * iter)
{
	gtk_tree_store_set (GTK_TREE_STORE (model),
			    iter,
			    COL_OPEN_PIXBUF, pixbufs->pixbuf_closed,
			    COL_CLOSED_PIXBUF, pixbufs->pixbuf_closed,
			    COL_CAPTION, (gchar *) tmpbuf,
			    COL_MODULE, (gchar *) mod_name,
			    COL_OFFSET, (gchar *) buf, -1);
}

static void load_treeview (GtkTreeStore * model, GtkTreeIter * parent,
			   TreeKeyIdx treeKey, int level = 1)
{
	GtkTreeIter iter;

	/*if (!target)
		target = &treeKey;*/

	sprintf (buf, "%lu", treeKey.getOffset ());
	tmpbuf = (char *) treeKey.getLocalName ();
	if(atol(buf) == 0) 
		tmpbuf = mod_name;
	//GS_message(("\nlocal_name: %d\noffset: %s",strlen(tmpbuf),buf));
	if (treeKey.hasChildren ()) {
		add_parent_to_treeview (model, parent);
	} else {
		add_leaf_node_to_treeview (model, parent);
	}

	if (treeKey.firstChild ()) {
		gtk_tree_store_append (GTK_TREE_STORE (model), &iter, parent);
		load_treeview (model, &iter, treeKey, level + 1);
		treeKey.parent ();
	}
	if (treeKey.nextSibling ()) {
		gtk_tree_store_insert_after (model, &iter, NULL, parent);
		load_treeview (model, &iter, treeKey, level);
	}

}

/*********************************    *************************************/
void setEntryText (RawGenBook * book, gchar * text)
{
	TreeKeyIdx *treeKey = (TreeKeyIdx *) (SWKey *) (*book);
	if (treeKey->getOffset ()) {

		(*book) << text;
	}
}

void appendSibbling (TreeKeyIdx * treeKey, gchar * name)
{
	if (treeKey->getOffset ()) {
		treeKey->append ();
		treeKey->setLocalName (name);
		treeKey->save ();
	}
}

void appendChild (TreeKeyIdx * treeKey, gchar * name)
{
	treeKey->appendChild ();
	treeKey->setLocalName (name);
	treeKey->save ();
	GS_message (("name: %s\nlocalName: %s", name,
		     treeKey->getLocalName ()));
}


void setLocalName (TreeKeyIdx * treeKey, char *new_name)
{

	treeKey->setLocalName (buf);
	treeKey->save ();
}

unsigned long main_treekey_remove (char *book, char *name, char * offset)
{
	SWMgr *mgr = backend->get_display_mgr ();
	SWModule *mod = mgr->Modules[book];
	
	if(!g_ascii_isdigit(offset[0])) return -1; /* make sure offset is a number */

	if (!mod)
		return -1;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	treekey->setOffset (atol(offset));
        mod->SetKey(treekey);
	mod->KeyText();      //snap to entry
	mod->deleteEntry();
	treekey->remove();
	treekey->getText();   //snap to entry
	return treekey->getOffset();
}

unsigned long main_treekey_append_sibling (char *book, char *name, char * offset)
{
	SWMgr *mgr = backend->get_display_mgr ();
	SWModule *mod = mgr->Modules[book];
	
	if(!g_ascii_isdigit(offset[0])) return -1; /* make sure offset is a number */

	if (!mod)
		return -1;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	treekey->setOffset (atol(offset));
	GS_message (("offset1: %d",  treekey->getOffset()));
	treekey->append ();
	treekey->setLocalName (name);
	treekey->save ();
	
        mod->SetKey(treekey);
	mod->KeyText();      //snap to entry
	(*mod) << name;

	GS_message (("offset2: %d",  treekey->getOffset()));
	return treekey->getOffset();
}


unsigned long main_treekey_append_child (char *book, char *name, char * offset)
{
	SWMgr *mgr = backend->get_display_mgr ();
	SWModule *mod = mgr->Modules[book];
	
	if(!g_ascii_isdigit(offset[0])) return -1; /* make sure offset is a number */

	if (!mod)
		return -1;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	treekey->setOffset (atol(offset));
	treekey->appendChild ();
	treekey->setLocalName (name);
	treekey->save ();
	
        mod->SetKey(treekey);
	mod->KeyText();      //snap to entry
	(*mod) << name;
	GS_message (("book: %s\nlocalName: %s\noffset :%s", book, name,
		     offset));
	return treekey->getOffset();
}

void main_treekey_set_local_name (char *book, char *name, char * offset)
{
	SWMgr *mgr = backend->get_display_mgr ();
	SWModule *mod = mgr->Modules[book];
	
	if(!g_ascii_isdigit(offset[0])) return; /* make sure offset is a number */
	
	if (!mod)
		return;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	treekey->setOffset (atol(offset));
	treekey->setLocalName (name);
	treekey->save ();

	GS_message (("book: %s\nlocalName: %s\noffset :%s", book, name,
		     offset));

}

char *main_get_book_raw_text (char *book, char * offset)
{
	SWMgr *mgr = backend->get_display_mgr ();
	SWModule *mod = mgr->Modules[book];
	
	if(!g_ascii_isdigit(offset[0])) return NULL; /* make sure offset is a number */

	if (!mod)
		return NULL;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	TreeKeyIdx treenode = *treekey;
        treenode.setOffset(atol(offset));
	mod->SetKey(treenode);
        mod->KeyText();      //snap to entry
	
	return strdup(mod->getRawEntry());
}

void main_treekey_save_book_text (char *book, char * offset, char * text)
{
	SWMgr *mgr = backend->get_display_mgr ();
	SWModule *mod = mgr->Modules[book];
	
	if(!g_ascii_isdigit(offset[0])) return; /* make sure offset is a number */
	
	if (!mod || (atol(offset) == 0))
		return;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	TreeKeyIdx treenode = *treekey;
        treenode.setOffset(atol(offset));
	mod->SetKey(treenode);
        mod->KeyText();      //snap to entry
	(*mod) << text;
}

/*********************************    *************************************/


void main_load_book_tree_in_editor (GtkTreeView * treeview, char *book)
{
	GtkTreeIter parent;
	GtkTreeStore *store;
	TreeKeyIdx *target = 0;
	int level = 1;
	SWMgr *mgr = backend->get_display_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!mod)
		return;
	tmpbuf = book;
	mod_name = book;
	TreeKeyIdx root = *((TreeKeyIdx *) mod->CreateKey ());
	root.root ();
	TreeKeyIdx *treeKey = (TreeKeyIdx *) mod->CreateKey ();

	store = gtk_tree_store_new (N_COLUMNS,
				    GDK_TYPE_PIXBUF,
				    GDK_TYPE_PIXBUF,
				    G_TYPE_STRING,
				    G_TYPE_STRING, G_TYPE_STRING);
	gtk_tree_view_set_model (GTK_TREE_VIEW (treeview),
				 GTK_TREE_MODEL (store));
	gtk_tree_store_clear (store);
	gtk_tree_store_append (GTK_TREE_STORE (store), &parent, NULL);
	load_treeview (store, &parent, root);
}



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

gint main_prayer_list_new (gchar * list_name)
{
	RawGenBook *book = NULL;
	gchar *path;
	TreeKeyIdx *treeKey;
	gint test;
	GS_DIALOG *info;

	if (list_name == NULL) {
		info = gui_new_dialog ();
		info->stock_icon = GTK_STOCK_DIALOG_QUESTION;
		info->title = _("Prayer List");
		info->label_top = _("Name for new prayer list");
		info->label1 = _("Name: ");
		info->text1 = g_strdup (_("MyPrayerList"));
		info->ok = TRUE;
		info->cancel = TRUE;
	}
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
	setEntryText (book, "<b>Salvation</b><br>");
	//add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "Spiritual Growth");
	setEntryText (book, "<b>Spiritual Growth</b><br>");
	//add_prayer_list_sections (book, treeKey);
	appendSibbling (treeKey, "Health");
	setEntryText (book, "<b>Health</b><br>");
	//add_prayer_list_sections (book, treeKey);

	/*
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
	 */
	delete treeKey;
	g_free (info->text1);
	g_free (info);
	main_update_module_lists ();
	main_load_module_tree (sidebar.module_list);
}
