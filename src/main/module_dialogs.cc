/*
 * GnomeSword Bible Study Tool
 * module_dialogs.cc - view module in a dialog
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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
#include <swmgr.h>
#include <swmodule.h>
#include <gtkhtml/gtkhtml.h>
#include <gal/widgets/e-unicode.h>
#include <string.h>


#include "gui/gtkhtml_display.h"
#include "gui/bibletext_dialog.h"
#include "gui/display_info.h"
#include "gui/font_dialog.h"
#include "gui/sidebar.h"
#include "gui/html.h"
#include "gui/main_window.h"
#include "gui/gnomesword.h"
#include "gui/utilities.h"
#include "gui/widgets.h"

#include "main/bibletext.h"
#include "main/module_dialogs.h"
#include "main/sword.h"
#include "main/settings.h"
#include "main/lists.h"
#include "main/key.h"
#include "main/xml.h"
#include "main/display.hh"



#include "backend/dialogs.hh"


gboolean bible_freed;
gboolean bible_apply_change;

/******************************************************************************
 * static - global to this file only
 */
static TEXT_DATA *dlg_bible;
static GList *bible_list;
static gboolean bible_in_url;

/******************************************************************************
 * externs
 */
extern gboolean gsI_isrunning;	/* information dialog */



/******************************************************************************
 * Name
 *   update_controls
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void update_controls(TEXT_DATA * vt)	
 *
 * Description
 *   update the book, chapter and verse contorls
 *
 * Return value
 *   void
 */

void main_dialog_update_controls(TEXT_DATA * vt)
{
	gchar *val_key;
	gint cur_chapter, cur_verse;

	dlg_bible = vt;
	bible_apply_change = FALSE;
	val_key = get_valid_key(vt->key);
	cur_chapter = get_chapter_from_key(val_key);
	cur_verse = get_verse_from_key(val_key);
	/* 
	 *  set book, chapter,verse and freeform lookup entries
	 *  to new verse - settings.bible_apply_change is set to false so we don't
	 *  start a loop
	 */
	gtk_entry_set_text(GTK_ENTRY(vt->cbe_book),
			   get_book_from_key(val_key));
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (vt->spb_chapter), cur_chapter);
	gtk_spin_button_set_value(GTK_SPIN_BUTTON
				  (vt->spb_verse), cur_verse);
	gtk_entry_set_text(GTK_ENTRY(vt->freeform_lookup), val_key);
	g_free(val_key);

	bible_apply_change = TRUE;
	//return val_key;
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void (TEXT_DATA * vt)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_bible_dialog_passage_changed(TEXT_DATA * t, gchar * passage)
{	
	ModuleDialogs *be = (ModuleDialogs*)t->backend;
	gchar buf[256];
	gchar *val_key = NULL;
	if (bible_apply_change) {
		if (*passage) {
			val_key = be->get_valid_key(passage);
			if(t->key)
				g_free(t->key);
			t->key = g_strdup(val_key );
			main_dialog_update_controls(t);	
			be->set_key(t->key);
			be->mod->Display();
			g_free(val_key);
		}
	}
}



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void (TEXT_DATA * vt)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_bible_dialog_display(TEXT_DATA * t)
{	
	ModuleDialogs *be = (ModuleDialogs*)t->backend;
	be->set_module_key(t->mod_name,t->key);
	be->mod->Display();	
}

/******************************************************************************
 * Name
 *   free_on_destroy
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void free_on_destroy(TEXT_DATA * vt)
 *
 * Description
 *   removes dialog from dialog_list when dialog is destroyed other than
 *   program shut down
 *
 * Return value
 *   void
 */

void main_free_on_destroy(TEXT_DATA * t)
{
	GList *tmp = NULL;
	bible_list = g_list_remove(bible_list, (TEXT_DATA*) t);
	g_free(t->ops); 
	g_free(t->key);
	if((ModuleDialogs*)t->backend) {
		ModuleDialogs* be = (ModuleDialogs*)t->backend;
		delete be;
	}
	g_free(t);
}

/******************************************************************************
 * Name
 *   main_open_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void main_open_bibletext_dialog(gchar * mod_name)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_open_bibletext_dialog(gchar * mod_name)
{	
	GtkWidget *popupmenu;
	ModuleDialogs *be;
	TEXT_DATA *t = NULL;
	gchar *val_key = NULL;
	
	t = g_new0(TEXT_DATA, 1);
	t->backend = (ModuleDialogs*) new ModuleDialogs();
	be = (ModuleDialogs*)t->backend;
	t->ops = main_new_globals(mod_name);
	t->mod_num = get_module_number(mod_name, TEXT_MODS);
	t->search_string = NULL;
	t->dialog = NULL;
	t->key = NULL;
	t->is_dialog = TRUE;
	t->mod_name = g_strdup(mod_name);
	t->is_rtol = is_module_rtl(t->mod_name);
	gui_create_bibletext_dialog(t);
	
	be->chapDisplay = new GTKChapDisp(t->html); 
	be->init_SWORD(t->mod_name);
	
	if (has_cipher_tag(t->mod_name)) {
		t->is_locked = module_is_locked(t->mod_name);
		t->cipher_old = get_cipher_key(t->mod_name);
	}

	else {
		t->is_locked = 0;
		t->cipher_old = NULL;
	}
	gtk_widget_show(t->dialog);
	bible_list = g_list_append(bible_list, (TEXT_DATA *) t);
	dlg_bible = t;
	t->sync = FALSE;
	t->key = g_strdup(settings.currentverse);
	main_dialog_update_controls(t);
	be->set_key(t->key);
	be->mod->Display();
	bible_apply_change = TRUE;
	g_free(val_key);
	//main_sync_bibletext_dialog_with_main(t->dlg);
}


/******************************************************************************
 * Name
 *   gui_bibletext_dialog_goto_bookmark
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_bibletext_dialog_goto_bookmark(gchar * mod_name, gchar * key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_bibletext_dialog_goto_bookmark(gchar * mod_name, gchar * key)
{
	GList *tmp = NULL;
	tmp = g_list_first(bible_list);
	while (tmp != NULL) {
		TEXT_DATA *vt = (TEXT_DATA *) tmp->data;
		if(!strcmp(vt->mod_name, mod_name)) {
			vt->key = key;
			//display(vt, vt->key, TRUE);
//			update_controls(vt);
			gdk_window_raise(vt->dialog->window);
			return;
		}		
		tmp = g_list_next(tmp);
	}
	//gui_open_bibletext_dialog(mod_name);
//	strcpy(cur_vt->key, key);
//	display(cur_vt, cur_vt->key, TRUE);
//	update_controls(cur_vt);
}


/******************************************************************************
 * Name
 *   gui_setup_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *   void gui_setup_bibletext_dialog(GList *mods)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_setup_bibletext_dialog(GList * mods)
{
	bible_list = NULL;
	bible_freed = FALSE;
	bible_apply_change = FALSE;
}


/******************************************************************************
 * Name
 *   gui_sync_bibletext_dialog_with_main
 *
 * Synopsis
 *   #include "gui/bibletext_dialog.h"
 *
 *   gui_sync_bibletext_dialog_with_main(TEXT_DATA * vt)	
 *
 * Description
 *   set bibletext dialog to main window current verse
 *
 * Return value
 *   void
 */

void main_sync_bibletext_dialog_with_main(TEXT_DATA * vt)
{ 
	//t->sword->set_module_key(vt->mod_name, settings.currentverse);
	//t->sword->mod->Display();
}


/******************************************************************************
 * Name
 *   gui_keep_bibletext_dialog_in_sync
 *
 * Synopsis
 *   #include "commentary_dialog.h"
 *
 *   void gui_keep_bibletext_dialog_in_sync(gchar * key)	
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void main_keep_bibletext_dialog_in_sync(gchar * key)
{
	GList *tmp = NULL;
	tmp = g_list_first(bible_list);
	while (tmp != NULL) {
		TEXT_DATA * t = (TEXT_DATA*) tmp->data;
		if(t->sync) {
			ModuleDialogs * be = (ModuleDialogs *) t->backend; 
			be->set_module_key(t->mod_name, 
					settings.currentverse);
			be->mod->Display();;
		}
		tmp = g_list_next(tmp);
	}
}


/******************************************************************************
 * Name
 *  gui_shutdown_bibletext_dialog
 *
 * Synopsis
 *   #include "bibletext_dialog.h"
 *
 *  void gui_shutdown_bibletext_dialog(void)	
 *
 * Description
 *   shut down bibletext dialog support clean mem
 *
 * Return value
 *   void
 */

void main_shutdown_bibletext_dialog(void)
{
	GList *tmp = bible_list;
	//g_warning("items = %d",g_list_length(tmp));
	while (tmp != NULL) {
		TEXT_DATA *t = (TEXT_DATA*) tmp->data;
		bible_freed = TRUE;
		/* 
		 *  destroy any dialogs created 
		 */
		if (t->dialog)
			gtk_widget_destroy(t->dialog);
		/* 
		 * free each TEXT_DATA item created 
		 */
		if((ModuleDialogs*)t->backend) {
			ModuleDialogs* be = (ModuleDialogs*)t->backend;
			delete be;
		}
		g_free(t->mod_name);
		g_free(t->ops);
		g_free(t->key);
		g_free(t);
		tmp = g_list_next(tmp);
	}
	g_list_free(bible_list);
}

/******   end of file   ******/
