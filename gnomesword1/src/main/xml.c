/*
 * GnomeSword Bible Study Tool
 * configs.c - 
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

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <libxml/parser.h>


#include "main/xml.h"
#include "main/module.h"
#include "main/settings.h"

struct _bookmark_data {
	gchar *caption;
	gchar *key;
	gchar *module;
	gboolean is_leaf;
};
typedef struct _bookmark_data BOOKMARK_DATA;
	
static BOOKMARK_DATA *es;
static xmlDocPtr xml_settings_doc;
static xmlNodePtr section_ptr;
static xmlDocPtr bookmark_doc;

/******************************************************************************
 * Name
 *   xml_new_bookmark_file
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void xml_new_bookmark_file(void)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void xml_new_bookmark_file(void)
{
	xmlDocPtr xml_doc;
	xmlNodePtr xml_root;
	xmlNodePtr xml_node;
	xmlNodePtr xml_folder;
	xmlAttrPtr xml_attr;
	const xmlChar *xml_filename;
	gchar buf[256];
	
	sprintf(buf, "%s/%s/bookmarks.xml", settings.gSwordDir,
		"bookmarks");
	xml_filename = (const xmlChar *) buf;
	xml_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_doc == NULL) {
		fprintf(stderr,
			"Document not created successfully. \n");
		return;
	}

	xml_node = xmlNewNode(NULL, (const xmlChar *) "SwordBookmarks");
	xml_attr = xmlNewProp(xml_node, "syntaxVersion",
			      (const xmlChar *) "1.0");
	xmlDocSetRootElement(xml_doc, xml_node);

	xml_root = xml_add_folder_to_parent(xml_node, "Personal");
	xml_folder = xml_add_folder_to_parent(xml_root, "What must I do to be saved?");
	xml_add_bookmark_to_parent(xml_folder, 
					"Acts 16:31",
					"Acts 16:31",
					NULL);
	xml_add_bookmark_to_parent(xml_folder, 
					"Eph 2:8",
					"Eph 2:8",
					NULL);
	xml_add_bookmark_to_parent(xml_folder, 
					"Romans 1:16",
					"Romans 1:16",
					NULL);
	xml_folder = xml_add_folder_to_parent(xml_root, "What is the Gospel?");
	xml_add_bookmark_to_parent(xml_folder, 
					"1 Cor 15:1-4",
					"1 Cor 15:1",
					NULL);

	xmlSaveFile(xml_filename, xml_doc);
	xmlFreeDoc(xml_doc);
}

/******************************************************************************
 * Name
 *  xml_add_folder_to_parent
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   xmlNodePtr xml_add_folder_to_parent(xmlNodePtr parent, 
 *						BOOKMARK_DATA * es)
 *
 * Description
 *    
 *
 * Return value
 *   xmlNodePtr
 */

xmlNodePtr xml_add_folder_to_parent(xmlNodePtr parent, gchar * caption)
{ 
	xmlNodePtr cur_node;
	xmlAttrPtr xml_attr;

	cur_node = xmlNewChild(parent,
			       NULL, (const xmlChar *) "Folder", NULL);
	xml_attr = xmlNewProp(cur_node,
			      "caption", (const xmlChar *) caption);
	return cur_node;
}


/******************************************************************************
 * Name
 *  xml_add_bookmark_to_parent
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_add_bookmark_to_parent(xmlNodePtr parent, 
 *						BOOKMARK_DATA * es)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void xml_add_bookmark_to_parent(xmlNodePtr parent, gchar * caption, 
					gchar * key, gchar * module)
{
	xmlNodePtr xml_node;
	xmlAttrPtr xml_attr;
	gchar *mod_desc = NULL;

	if (module) {
		if (strlen(module) > 2)
			mod_desc = get_module_description(module);
	} 

	if (mod_desc == NULL)
		mod_desc = " ";

	xml_node = xmlNewChild(parent,
			       NULL,
			       (const xmlChar *) "Bookmark", NULL);
	xml_attr = xmlNewProp(xml_node,
			      "modulename",
			      (const xmlChar *) module);
	xml_attr =
	    xmlNewProp(xml_node, "key", (const xmlChar *) key);
	xml_attr =
	    xmlNewProp(xml_node, "moduledescription",
		       (const xmlChar *) mod_desc);
	xml_attr =
	    xmlNewProp(xml_node, "description",
		       (const xmlChar *) caption);
}

/******************************************************************************
 * Name
 *  get_node_data
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void get_node_data(GNode * node)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void get_node_data(GNode * node)
{
	es = g_new(BOOKMARK_DATA, 1);
	es = (BOOKMARK_DATA *) node->data;
}

/******************************************************************************
 * Name
 *  parse_gnode_tree
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void parse_gnode_tree(GNode * node, xmlNodePtr parent)	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

static void parse_gnode_tree(GNode * node, xmlNodePtr parent)
{
	static xmlNodePtr cur_node;
	GNode *work;

	for (work = node->children; work; work = work->next) {
		get_node_data(work);
		if (!es->is_leaf) {
			cur_node = xml_add_folder_to_parent(parent, es->caption);
		} else {
			xml_add_bookmark_to_parent(parent, es->caption,
						es->key, es->module);
		}
		g_free(es);
		parse_gnode_tree(work, cur_node);
	}
}


/******************************************************************************
 * Name
 *  xml_save_gnode_to_bookmarks
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_save_gnode_to_bookmarks(GNode * node, gchar *file_buf)	
 *
 * Description
 *    for compatability with old bookmarks
 *
 * Return value
 *   void
 */

void xml_save_gnode_to_bookmarks(GNode * gnode, gchar * file_buf)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr cur_node = NULL;
	xmlDocPtr root_doc;
	xmlAttrPtr root_attr;
	const xmlChar *xml_filename;
	
	if (!gnode)
		return;
	xml_filename = (const xmlChar *) file_buf;
	root_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)
				       "SwordBookmarks");
		root_attr =
		    xmlNewProp(root_node, "syntaxVersion",
			       (const xmlChar *) "1.0");
		xmlDocSetRootElement(root_doc, root_node);
	}

	es = (BOOKMARK_DATA *) gnode->data;

	if ((!g_strcasecmp(es->module, "ROOT")) ||
	    (!g_strcasecmp(es->module, "GROUP"))) {
		cur_node =
		    xml_add_folder_to_parent(root_node, es->caption);
		parse_gnode_tree(gnode, cur_node);
	} else {
		xml_add_bookmark_to_parent(root_node, 
						es->caption,
						es->key,
						es->module);
	}

	while ((gnode = g_node_next_sibling(gnode)) != NULL) {
		get_node_data(gnode);
		if (!es->is_leaf) {
			cur_node =
			    xml_add_folder_to_parent(root_node,
						     es->caption);
			parse_gnode_tree(gnode, cur_node);
		} else {
			if (root_doc != NULL) {
				xml_add_bookmark_to_parent
				    (root_node, es->caption,
						es->key,
						es->module);
			}
		}
	}
	g_print("\nsaving = %s\n", xml_filename);
	xmlSaveFile(xml_filename, root_doc);
	xmlFreeDoc(root_doc);
	g_free(file_buf);
}

/******************************************************************************
 * Name
 *   xml_write_bookmark_doc
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_write_bookmark_doc(const xmlChar * xml_filename)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void xml_write_bookmark_doc(const xmlChar * xml_filename)
{	
	g_print("\nsaving = %s\n", xml_filename);
#ifdef USE_GNOME2
	xmlSaveFormatFile(xml_filename, bookmark_doc,1);
#else
	xmlSaveFile(xml_filename, bookmark_doc);
#endif
}

/******************************************************************************
 * Name
 *   xml_load_bookmark_file
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_load_bookmark_file(char * bookmark_file)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void xml_free_bookmark_doc(void)
{
	xmlFreeDoc(bookmark_doc);
}


/******************************************************************************
 * Name
 *   xml_load_bookmark_file
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   xmlNodePtr xml_load_bookmark_file(char * bookmark_file)
 *
 * Description
 *   
 *
 * Return value
 *   xmlNodePtr
 */

xmlNodePtr xml_load_bookmark_file(const xmlChar * bookmark_file) 
{
	
	xmlNodePtr cur = NULL;

	bookmark_doc = xmlParseFile(bookmark_file);

	if (bookmark_doc == NULL) {
		fprintf(stderr, "Document not parsed successfully. \n");
		return;
	}

	cur = xmlDocGetRootElement(bookmark_doc);
	if (cur == NULL) {
		fprintf(stderr, "empty document \n");
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "SwordBookmarks")) {
		fprintf(stderr,
			"wrong type, root node != SwordBookmarks\n");
		xmlFreeDoc(bookmark_doc);
		return;
	}

	cur = cur->xmlChildrenNode;
	return cur;
}


/******************************************************************************
 * Name
 *   xml_create_settings_file
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   int xml_create_settings_file(char * path)
 *
 * Description
 *   create new settings file
 *
 * Return value
 *   int
 */

int xml_create_settings_file(char *path)
{
	xmlNodePtr root_node;
	xmlNodePtr cur_node;
	xmlNodePtr section_node;
	xmlAttrPtr xml_attr;
	const xmlChar *xml_filename;

	xml_filename = (const xmlChar *) path;
	xml_settings_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_settings_doc == NULL) {
		fprintf(stderr,
			"Document not created successfully. \n");
		return 0;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *) "GnomeSword");
	xml_attr = xmlNewProp(root_node, "Version", VERSION);
	xmlDocSetRootElement(xml_settings_doc, root_node);

	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) "ranges", NULL);
	cur_node = xmlNewChild(section_node,
			       NULL, (const xmlChar *) "range", NULL);
	xml_attr = xmlNewProp(cur_node,
			      "label",
			      (const xmlChar *) "Old Testament");
	xml_attr =
	    xmlNewProp(cur_node, "list", (const xmlChar *) "Gen - Mal");
	cur_node =
	    xmlNewChild(section_node, NULL, (const xmlChar *) "range",
			NULL);
	xml_attr =
	    xmlNewProp(cur_node, "label",
		       (const xmlChar *) "New Testament");
	xml_attr =
	    xmlNewProp(cur_node, "list", (const xmlChar *) "Mat - Rev");

	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) "modlists", NULL);


	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) "editor", NULL);
	xmlNewTextChild(section_node, NULL, "UsePercommDialog", "1");
	xmlNewTextChild(section_node, NULL, "UseStudyPad", "1");
	xmlNewTextChild(section_node, NULL, "UseStudypadDialog", "1");


	section_node = xmlNewChild(root_node, NULL, "fontsize", NULL);
	xmlNewTextChild(section_node, NULL, "versenum", "+0");


	section_node = xmlNewChild(root_node, NULL, "HTMLcolors", NULL);
	xmlNewTextChild(section_node, NULL, "background", "#FFFFFF");
	xmlNewTextChild(section_node, NULL, "text_fg", "#000000");
	xmlNewTextChild(section_node, NULL, "versenum", "#0000CF");
	xmlNewTextChild(section_node, NULL, "currentverse", "#339766");
	xmlNewTextChild(section_node, NULL, "link", "#878787");
	xmlNewTextChild(section_node, NULL, "found", "#D02898");


	section_node =
	    xmlNewChild(root_node, NULL, "parallel", NULL);
	xmlNewTextChild(section_node, NULL, "parallel", "1");
	xmlNewTextChild(section_node, NULL, "cantillation", "0");
	xmlNewTextChild(section_node, NULL, "points", "0");
	xmlNewTextChild(section_node, NULL, "footnotes", "0");
	xmlNewTextChild(section_node, NULL, "morphs", "0");
	xmlNewTextChild(section_node, NULL, "strongs", "0");


	section_node = xmlNewChild(root_node, NULL, "keys", NULL);
	xmlNewTextChild(section_node, NULL, "book", "");
	xmlNewTextChild(section_node, NULL, "dictionary", "Grace");
	xmlNewTextChild(section_node, NULL, "verse", "Romans 8:28");


	section_node = xmlNewChild(root_node, NULL, "layout", NULL);
	xmlNewTextChild(section_node, NULL, "hight", "480");
	xmlNewTextChild(section_node, NULL, "width", "640");
	xmlNewTextChild(section_node, NULL, "textpane", "240");
	xmlNewTextChild(section_node, NULL, "shortcutbar", "178");
	xmlNewTextChild(section_node, NULL, "uperpane", "210");


	section_node = xmlNewChild(root_node, NULL, "lexicons", NULL);
	xmlNewTextChild(section_node, NULL, "indictpane", "0");
	xmlNewTextChild(section_node, NULL, "inviewer", "0");
	xmlNewTextChild(section_node, NULL, "usedefaultdict", "0");
	xmlNewTextChild(section_node, NULL, "defaultdictionary", NULL);
	xmlNewTextChild(section_node, NULL, "greek", NULL);
	xmlNewTextChild(section_node, NULL, "greekviewer", NULL);
	xmlNewTextChild(section_node, NULL, "hebrew", NULL);
	xmlNewTextChild(section_node, NULL, "hebrewviewer", NULL);


	section_node = xmlNewChild(root_node, NULL, "misc", NULL);
	xmlNewTextChild(section_node, NULL, "dailydevotional", "0");
	xmlNewTextChild(section_node, NULL, "splash", "0");
	xmlNewTextChild(section_node, NULL, "usedefault", "1");
	xmlNewTextChild(section_node, NULL, "formatpercom", "1");
	xmlNewTextChild(section_node, NULL, "showcomms", "1");
	xmlNewTextChild(section_node, NULL, "showdicts", "1");
	xmlNewTextChild(section_node, NULL, "showtexts", "1");
	xmlNewTextChild(section_node, NULL, "versestyle", "1");


	section_node = xmlNewChild(root_node, NULL, "modules", NULL);
	xmlNewTextChild(section_node, NULL, "book", NULL);
	xmlNewTextChild(section_node, NULL, "comm", NULL);
	xmlNewTextChild(section_node, NULL, "devotional", NULL);
	xmlNewTextChild(section_node, NULL, "dict", NULL);
	xmlNewTextChild(section_node, NULL, "int1", NULL);
	xmlNewTextChild(section_node, NULL, "int2", NULL);
	xmlNewTextChild(section_node, NULL, "int3", NULL);
	xmlNewTextChild(section_node, NULL, "int4", NULL);
	xmlNewTextChild(section_node, NULL, "int5", NULL);
	xmlNewTextChild(section_node, NULL, "bible", NULL);
	xmlNewTextChild(section_node, NULL, "percomm", NULL);


	section_node =
	    xmlNewChild(root_node, NULL, "shortcutbar", NULL);
	xmlNewTextChild(section_node, NULL, "shortcutbar", "1");
	xmlNewTextChild(section_node, NULL, "docked", "1");
	xmlNewTextChild(section_node, NULL, "book", "0");
	xmlNewTextChild(section_node, NULL, "bookmark", "1");
	xmlNewTextChild(section_node, NULL, "comm", "0");
	xmlNewTextChild(section_node, NULL, "dict", "0");
	xmlNewTextChild(section_node, NULL, "favo", "0");
	xmlNewTextChild(section_node, NULL, "history", "0");
	xmlNewTextChild(section_node, NULL, "bible", "0");


	section_node = xmlNewChild(root_node, NULL, "studypad", NULL);
	xmlNewTextChild(section_node, NULL, "directory",
			settings.homedir);
	xmlNewTextChild(section_node, NULL, "lastfile", "");
	xmlNewTextChild(section_node, NULL, "editbar", "1");
	xmlNewTextChild(section_node, NULL, "stylebar", "1");


	section_node = xmlNewChild(root_node, NULL, "tabs", NULL);
	xmlNewTextChild(section_node, NULL, "bible", "1");
	xmlNewTextChild(section_node, NULL, "comm", "1");
	xmlNewTextChild(section_node, NULL, "dict", "1");
	xmlNewTextChild(section_node, NULL, "book", "1");
	xmlNewTextChild(section_node, NULL, "percomm", "1");
	return 1;
}





/******************************************************************************
 * Name
 *   xml_find_section
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   xmlNodePtr xml_find_section(char *type_doc, char *section)
 *
 * Description
 *   
 *
 * Return value
 *   xmlNodePtr
 */

static xmlNodePtr xml_find_section(char *type_doc, char *section)
{
	xmlNodePtr cur = NULL;

	cur = xmlDocGetRootElement(xml_settings_doc);
	if (cur == NULL) {
		fprintf(stderr, "empty document \n");
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) type_doc)) {
		fprintf(stderr,
			"wrong type, root node != %s\n", type_doc);
		return NULL;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *) section)) {
			return cur;
		}
		if (cur->next)
			cur = cur->next;
		else
			break;
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   xml_find_item
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   xmlNodePtr xml_find_item(xmlNodePtr parent, char * item, 
 *							char * label)
 *
 * Description
 *   
 *
 * Return value
 *   xmlNodePtr
 */

static xmlNodePtr xml_find_item(xmlNodePtr parent, char *item,
				char *label)
{
	xmlNodePtr cur = NULL;
	xmlChar *prop_label = NULL;

	cur = parent->xmlChildrenNode;

	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *) item)) {
			prop_label = xmlGetProp(cur, "label");
			if (!xmlStrcmp
			    (prop_label, (const xmlChar *) label)) {
				return cur;
			}
		}
		if (cur->next)
			cur = cur->next;
		else
			break;
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   xml_get_list_from_label
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   char *xml_get_list_from_label(char * section, char * item, char * label)
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *xml_get_list_from_label(char *section, char *item, const char *label)
{
	xmlNodePtr cur = NULL;
	xmlChar *prop_label = NULL;


	if ((cur = xml_find_section((xmlChar *) "GnomeSword",
				    (xmlChar *) section)) != NULL) {

		cur = cur->xmlChildrenNode;

		while (cur != NULL) {

			//g_warning("cur->name = %s", cur->name);
			if (!xmlStrcmp
			    (cur->name, (const xmlChar *) item)) {
				prop_label = xmlGetProp(cur, "label");
				//g_warning(prop_label);
				if (!xmlStrcmp
				    (prop_label,
				     (const xmlChar *) label)) {
					return xmlGetProp(cur, "list");
				}
			}
			if (cur->next)
				cur = cur->next;
			else
				break;
		}
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   xml_set_list_item
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_set_list_item(char * section, char * item, char * label, 
 *				char * value)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void xml_set_list_item(char *section, char *item, char *label,
		       char *value)
{
	xmlNodePtr cur = NULL;
	xmlNodePtr cur_item = NULL;
	xmlNodePtr new = NULL;
	if ((cur =
	     xml_find_section((xmlChar *) "GnomeSword",
			      (xmlChar *) section)) != NULL) {
		if ((cur_item = xml_find_item(cur,
					      (xmlChar *) item,
					      (xmlChar *) label)) !=
		    NULL) {
			xmlSetProp(cur_item, "list", value);
		} else {
			new = xmlNewChild(cur,
					  NULL, (const xmlChar *) item,
					  NULL);
			xmlNewProp(new, "label",
				   (const xmlChar *) label);
			xmlNewProp(new, "list",
				   (const xmlChar *) value);
		}
	}
}

/******************************************************************************
 * Name
 *   xml_set_section_ptr
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   int xml_set_section_ptr(char * section)
 *
 * Description
 *   set section pointer to first item of section
 *
 * Return value
 *   int
 */

int xml_set_section_ptr(char *section)
{
	xmlNodePtr cur = NULL;

	if ((cur = xml_find_section((xmlChar *) "GnomeSword",
				    (xmlChar *) section)) != NULL) {
		section_ptr = cur->xmlChildrenNode;
		if (section_ptr)
			return 1;
	}
	return 0;
}

/******************************************************************************
 * Name
 *   xml_next_item
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   int xml_next_item(void)
 *
 * Description
 *   sets section pointer to next item - returns 1 if pointer is not null
 *
 * Return value
 *   int
 */

int xml_next_item(void)
{
	while (section_ptr != NULL) {
		section_ptr = section_ptr->next;
		return 1;
	}
	return 0;
}

/******************************************************************************
 * Name
 *   xml_get_label
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   char *xml_get_label(void)
 *
 * Description
 *   returns a label for ranges or module lists for search dialog
 *
 * Return value
 *   char *
 */

char *xml_get_label(void)
{
	return (char *) xmlGetProp(section_ptr, "label");
}


/******************************************************************************
 * Name
 *   xml_get_list
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   char *xml_get_list(void)
 *
 * Description
 *   gets a list of ranges or modules for search dialog
 *
 * Return value
 *   char *
 */

char *xml_get_list(void)
{
	return (char *) xmlGetProp(section_ptr, "list");
}


/******************************************************************************
 * Name
 *   xml_find_prop
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   xmlNodePtr xml_find_prop(char *type_doc, char *section,
				char *prop)
 *
 * Description
 *   "GnomeSword"
 *
 * Return value
 *   xmlNodePtr
 */

static xmlNodePtr xml_find_prop(char *type_doc, char *section,
				char *prop)
{
	xmlNodePtr cur = NULL;
	xmlNodePtr item_cur = NULL;

	cur = xmlDocGetRootElement(xml_settings_doc);
	if (cur == NULL) {
		fprintf(stderr, "empty document \n");
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) type_doc)) {
		fprintf(stderr,
			"wrong type, root node != %s\n", type_doc);
		return NULL;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *) section)) {
			item_cur = cur->xmlChildrenNode;
			while (item_cur != NULL) {
				if (!xmlStrcmp
				    (item_cur->name,
				     (const xmlChar *) prop)) {
					return item_cur;
				}
				if (item_cur->next)
					item_cur = item_cur->next;
				else
					break;
			}
		}
		if (cur->next)
			cur = cur->next;
		else
			break;
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   xml_get_value
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   char *xml_get_value(char *section, char *item)
 *
 * Description
 *   
 *
 * Return value
 *   char *
 */

char *xml_get_value(char *section, const char *item)
{
	xmlNodePtr cur = NULL;
	xmlChar *results = NULL;
	if ((cur =
	     xml_find_prop("GnomeSword",
			   (xmlChar *) section,
			   (xmlChar *) item)) != NULL) {
		results = xmlNodeListGetString(xml_settings_doc,
					       cur->xmlChildrenNode, 1);
		if (results)
			return results;
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   xml_set_value
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_set_value(char *type_doc, char *section, char *item,
 *		   char *value)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void xml_set_value(char *type_doc, char *section, const char *item,
		   const char *value)
{
	xmlNodePtr cur = NULL;
	if ((cur =
	     xml_find_prop((xmlChar *) type_doc,
			   (xmlChar *) section,
			   (xmlChar *) item)) != NULL) {
		xmlNodeSetContent(cur, value);
	}
}


/******************************************************************************
 * Name
 *   xml_parse_settings_file
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   int xml_parse_settings_file(char *file_name)
 *
 * Description
 *   load settings.xml at program start
 *
 * Return value
 *   int
 */

int xml_parse_settings_file(char *file_name)
{
	const xmlChar *file;

	file = (const xmlChar *) file_name;
	xml_settings_doc = xmlParseFile(file);

	if (xml_settings_doc == NULL) {
		fprintf(stderr, "Document not parsed successfully. \n");
		return FALSE;
	}
	return TRUE;
}


/******************************************************************************
 * Name
 *   xml_save_settings_doc
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_save_settings_doc(char *file_name)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void xml_save_settings_doc(char *file_name)
{
	const xmlChar *file;

	file = (const xmlChar *) file_name;
	xmlSaveFile(file, xml_settings_doc);
}


/******************************************************************************
 * Name
 *   xml_free_settings_doc
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_free_settings_doc(void)
 *
 * Description
 *   
 *
 * Return value
 *   void
 */

void xml_free_settings_doc(void)
{
	xmlFreeDoc(xml_settings_doc);
}
