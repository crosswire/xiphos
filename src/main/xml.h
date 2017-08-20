/*
 * Xiphos Bible Study Tool
 * xml.h -
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

#ifndef __XML_H__
#define __XML_H__

#include <libxml/parser.h>

#ifdef __cplusplus
extern "C" {
#endif

void xml_add_new_section_to_settings_doc(char *section);
void xml_new_bookmark_file(void);
xmlNodePtr xml_add_folder_to_parent(xmlNodePtr parent,
				    gchar *caption);
void xml_add_bookmark_to_parent(xmlNodePtr parent, gchar *caption,
				gchar *key, gchar *module,
				const gchar *mod_desc);
void xml_write_bookmark_doc(const xmlChar *xml_filename);

void xml_free_bookmark_doc(void);
xmlNodePtr xml_load_bookmark_file(const xmlChar *bookmark_file);
char *xml_get_list_from_label(const char *section,
			      const char *item, const char *label);
int xml_set_section_ptr(const char *section);
void xml_set_list_item(const char *section, const char *item,
		       const char *label, const char *value);
void xml_set_new_element(const char *section, const char *item,
			 const char *content);
int xml_next_item(void);
char *xml_get_label(void);
char *xml_get_list(void);
void xml_free_export_doc(void);
void xml_export_set_value(const char *type_doc,
			  const char *section, const char *item,
			  const char *value);
void xml_save_export_doc(char *name);
void xml_add_new_section_to_doc(xmlDocPtr doc, char *section);
int xml_create_copy_export_file(char *path);
int xml_load_copy_export_file(const xmlChar *file);
char *xml_get_copy_export_value(const char *section,
				const char *item);
int xml_create_settings_file(char *file_name);
int xml_parse_settings_file(char *file_name);
char *xml_get_value(const char *section, const char *item);
void xml_set_value(const char *type_doc, const char *section,
		   const char *item, const char *value);
void xml_save_settings_doc(char *file_name);
void xml_free_settings_doc(void);
void xml_add_new_item_to_export_doc_section(char *section,
					    char *item_name,
					    char *value);
void xml_add_new_item_to_section(char *section, char *item_name,
				 char *value);
void xml_remove_node(const char *section, const char *item,
		     const char *label);
void xml_remove_section(const char *section);
void xml_convert_to_osisref(void);

#ifdef __cplusplus
}
#endif
#endif
