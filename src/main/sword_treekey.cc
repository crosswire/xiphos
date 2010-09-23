/*
 * Xiphos Bible Study Tool
 * sword_treekey.cc - treekey stuff for book editor
 *
 * Copyright (C) 2008-2010 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */


#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gstdio.h>
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
	if (atol(buf) == 0)
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
void setEntryText (RawGenBook * book, const gchar * text)
{
	TreeKeyIdx *treeKey = (TreeKeyIdx *) (SWKey *) (*book);
	if (treeKey->getOffset ()) {

		(*book) << text;
	}
}

void appendSibbling (TreeKeyIdx * treeKey, const gchar * name)
{
	if (treeKey->getOffset ()) {
		treeKey->append ();
		treeKey->setLocalName (name);
		treeKey->save ();
	}
}

void appendChild (TreeKeyIdx * treeKey, const gchar * name)
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
	SWMgr *mgr = backend->get_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!g_ascii_isdigit(offset[0])) return 0; /* make sure offset is a number */

	if (!mod)
		return 0;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	treekey->setOffset (atol(offset));
        mod->SetKey(treekey);
	mod->KeyText();      //snap to entry
	mod->deleteEntry();
	treekey->remove();
	return treekey->getOffset();
}

unsigned long main_treekey_append_sibling (char *book, char *name, char * offset)
{
	SWMgr *mgr = backend->get_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!g_ascii_isdigit(offset[0])) return 0; /* make sure offset is a number */

	if (!mod)
		return 0;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	treekey->setOffset (atol(offset));
	GS_message (("offset1: %ld",  treekey->getOffset()));
	treekey->append ();
	treekey->setLocalName (name);
	treekey->save ();

        mod->SetKey(treekey);
	mod->KeyText();      //snap to entry
	(*mod) << name;

	GS_message (("offset2: %ld",  treekey->getOffset()));
	return treekey->getOffset();
}


unsigned long main_treekey_append_child (char *book, char *name, char * offset)
{
	SWMgr *mgr = backend->get_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!g_ascii_isdigit(offset[0])) return 0; /* make sure offset is a number */

	if (!mod)
		return 0;

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
	SWMgr *mgr = backend->get_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!g_ascii_isdigit(offset[0])) return; /* make sure offset is a number */

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
	SWMgr *mgr = backend->get_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!g_ascii_isdigit(offset[0])) return NULL; /* make sure offset is a number */

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
	SWMgr *mgr = backend->get_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!offset || !g_ascii_isdigit(offset[0])) return; /* make sure offset is a number */

	if (!mod || (atol(offset) == 0))
		return;

	TreeKeyIdx *treekey = (TreeKeyIdx *) mod->CreateKey ();
	TreeKeyIdx treenode = *treekey;
        treenode.setOffset(atol(offset));
	mod->SetKey(treenode);
        mod->KeyText();      //snap to entry
	(*mod) << text;
	if (settings.book_mod && book
		   	&& !strcmp(settings.book_mod,book)){
		GS_message(("main_treekey_save_book_text"));
		main_display_book(book, offset);
	}
}

/*********************************    *************************************/


void main_load_book_tree_in_editor (GtkTreeView * treeview, char *book)
{
	GtkTreeIter parent;
	GtkTreeStore *store;
	SWMgr *mgr = backend->get_mgr ();
	SWModule *mod = mgr->Modules[book];

	if (!mod)
		return;
	tmpbuf = book;
	mod_name = book;
	TreeKeyIdx root = *((TreeKeyIdx *) mod->CreateKey ());
	root.root ();

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
