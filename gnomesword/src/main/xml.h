/*
 * GnomeSword Bible Study Tool
 * xml.h - 
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

#ifndef __XML_H__
#define __XML_H__

#ifdef __cplusplus
extern "C" {
#endif
#include <libxml/parser.h>

void xml_new_bookmark_file(void);	
xmlNodePtr xml_add_folder_to_parent(xmlNodePtr parent, gchar * caption);	
void xml_add_bookmark_to_parent(xmlNodePtr parent, gchar * caption, 
			gchar * key, gchar * module, gchar * mod_desc);
void xml_save_gnode_to_bookmarks(GNode * gnode, gchar * file_buf);
void xml_write_bookmark_doc(const xmlChar * xml_filename);

void xml_free_bookmark_doc(void);
xmlNodePtr xml_load_bookmark_file(const xmlChar * bookmark_file);
char *xml_get_list_from_label(char * section, char * item, const char * label);
int xml_set_section_ptr(char * section);
void xml_set_list_item(char * section, char * item, char * label, 
				char * value);
int xml_next_item(void);
char *xml_get_label(void);
char *xml_get_list(void);
int xml_create_settings_file(char * file_name);
int xml_parse_settings_file(char *file_name);
char *xml_get_value(char *section, const char *item);
void xml_set_value(char *type_doc, char *section, const char *item,
		  const char *value);
void xml_save_settings_doc(char *file_name);
void xml_free_settings_doc(void);
void xml_add_new_item_to_section(char * section, char * item_name, char * value);
void xml_remove_node(char *section, char *item, char *label);

#ifdef __cplusplus
}
#endif
#endif
