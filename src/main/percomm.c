/*
 * GnomeSword Bible Study Tool
 * percomm.c - support for personal commentary modules
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
#  include <config.h>
#endif

#include <gnome.h>

#include "gui/percomm.h"
#include "gui/_editor.h"
#include "backend/module_options.h"
#include "gui/info_box.h"

#include "main/percomm.h"
#include "main/gs_gnomesword.h"
#include "main/gs_html.h"
#include "main/support.h"

#include "backend/percomm_.h"
#include "backend/sword.h"


/******************************************************************************
 * Name
 *   get_percomm_key
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   char *get_percomm_key(void)
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_percomm_key(void)
{
	backend_get_percomm_key();
}

/******************************************************************************
 * Name
 *   get_percomm_text
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   char *get_percomm_text(char * key)
 *
 * Description
 *    
 *
 * Return value
 *   char *
 */ 

char *get_percomm_text(char * key)
{
	return backend_get_percomm_text(key);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void set_percomm_key(char * key)
{
	backend_set_percomm_key(key);
}

/******************************************************************************
 * Name
 *   change_percomm_module
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void change_percomm_module(char * mod_name)
 *
 * Description
 *    
 *
 * Return value
 *   void
 */ 

void change_percomm_module(char * mod_name)
{
	backend_change_percomm_module(mod_name);
}

/******************************************************************************
 * Name
 *  save_percomm_note
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void save_percomm_note(gchar *note)	
 *
 * Description
 *    save note to current personal commentary (cur_p)
 *    by calling backend_save_personal_comment()
 *
 * Return value
 *   void
 */ 

void save_percomm_note(gchar *note)
{
	backend_save_personal_comment(note);
}

/******************************************************************************
 * Name
 *  delete_percomm_note
 *
 * Synopsis
 *   #include "percomm.h"
 *
 *   void delete_percomm_note(void)	
 *
 * Description
 *    opens info box to make sure user want to delete note if so
 *    calls backend_delete_personal_comment() to delete note
 *
 * Return value
 *   void
 */ 

void delete_percomm_note(void)
{		
	backend_delete_personal_comment();
}

