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

#include <glib-1.2/glib.h>
#include <libxml/parser.h>


#include "main/xml.h"
#include "main/settings.h"

static xmlDocPtr xml_settings_doc;
static xmlNodePtr section_ptr;


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
	xmlNewTextChild(section_node, NULL, "greek", "StrongsGreek");
	xmlNewTextChild(section_node, NULL, "greekviewer",
			"StrongsGreek");
	xmlNewTextChild(section_node, NULL, "hebrew", "StrongsHebrew");
	xmlNewTextChild(section_node, NULL, "hebrewviewer",
			"StrongsHebrew");


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
	xmlNewTextChild(section_node, NULL, "book", "NeeNormal");
	xmlNewTextChild(section_node, NULL, "comm", "MHC");
	xmlNewTextChild(section_node, NULL, "devotional", "SME");
	xmlNewTextChild(section_node, NULL, "dict", "BDB");
	xmlNewTextChild(section_node, NULL, "int1", "KJV");
	xmlNewTextChild(section_node, NULL, "int2", "KJV");
	xmlNewTextChild(section_node, NULL, "int3", "KJV");
	xmlNewTextChild(section_node, NULL, "int4", "KJV");
	xmlNewTextChild(section_node, NULL, "int5", "KJV");
	xmlNewTextChild(section_node, NULL, "bible", "KJV");
	xmlNewTextChild(section_node, NULL, "percomm", "Personal");


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
