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

xmlDocPtr xml_settings_doc;

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

int create_xml_file(gchar * filename, gchar * name)
{
/*	xmlDocPtr xml_doc;
	xmlNodePtr xml_node;
	xmlAttrPtr xml_attr;
	const xmlChar *xml_filename;
	//gchar buf[256];

	g_warning(filename);
	//sprintf(buf, "%s/%s", settings.gSwordDir, filename);
	xml_filename = (const xmlChar *) filename;
	xml_settings_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_settings_doc == NULL) {
		fprintf(stderr,
			"Document not created successfully. \n");
		return 0;
	}

	xml_node = xmlNewNode(NULL, (const xmlChar *) "GnomeSword");
	xml_attr = xmlNewProp(xml_node, "syntaxVersion",
			      (const xmlChar *) "1.0");
	xmlDocSetRootElement(xml_settings_doc, xml_node);

	xml_node = xmlNewChild(xml_node, NULL,
			       (const xmlChar *) "Section", NULL);
	xml_attr = xmlNewProp(xml_node, "name", (const xmlChar *) name);

	xmlSaveFile(xml_filename, xml_settings_doc);
	xmlFreeDoc(xml_settings_doc);*/
	return 1;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

int xml_create_settings_file(char * path)
{
	//xmlDocPtr xml_settings_doc;
	xmlNodePtr root_node;
	xmlNodePtr section_node;
	xmlAttrPtr xml_attr;
	const xmlChar *xml_filename;
	char buf[256];

	//sprintf(buf, "%s/settings.xml", path);
	xml_filename = (const xmlChar *) path;
	xml_settings_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_settings_doc == NULL) {
		fprintf(stderr,
			"Document not created successfully. \n");
		return 0;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *) "GnomeSword");
	xml_attr = xmlNewProp(root_node, "Version","0.9.dev3");
	xmlDocSetRootElement(xml_settings_doc, root_node);

	section_node = xmlNewChild(root_node, NULL,
			       (const xmlChar *)"editor" , NULL);
	xmlNewTextChild(section_node,NULL,"UsePercommDialog", "1");
	xmlNewTextChild(section_node,NULL,"UseStudyPad", "1");
	xmlNewTextChild(section_node,NULL,"UseStudypadDialog", "1");
	
		
	section_node = xmlNewChild(root_node, NULL, "fontsize", NULL);
	xmlNewTextChild(section_node,NULL,"versenum", "+0");
	
		
	section_node = xmlNewChild(root_node, NULL, "HTMLcolors", NULL);
	xmlNewTextChild(section_node,NULL,"background", "#FFFFFF");
	xmlNewTextChild(section_node,NULL,"text", "#000000");
	xmlNewTextChild(section_node,NULL,"versenum", "#0000CF");
	xmlNewTextChild(section_node,NULL,"currentverse", "#339766");
	xmlNewTextChild(section_node,NULL,"link", "#878787");
	xmlNewTextChild(section_node,NULL,"found", "#D02898");
	
	
	section_node = xmlNewChild(root_node, NULL, "interlinear", NULL);
	xmlNewTextChild(section_node,NULL,"interlinear", "1");	
	xmlNewTextChild(section_node,NULL,"cantillation", "0");
	xmlNewTextChild(section_node,NULL,"points", "0");
	xmlNewTextChild(section_node,NULL,"footnotes", "0");
	xmlNewTextChild(section_node,NULL,"morphs", "0");
	xmlNewTextChild(section_node,NULL,"strongs", "0");
	
	
	section_node = xmlNewChild(root_node, NULL, "keys", NULL);
	xmlNewTextChild(section_node,NULL,"book", "");	
	xmlNewTextChild(section_node,NULL,"dictionary", "Grace");
	xmlNewTextChild(section_node,NULL,"verse", "Romans 8:28");
	
	
	section_node = xmlNewChild(root_node, NULL, "layout", NULL);
	xmlNewTextChild(section_node,NULL,"hight", "480");
	xmlNewTextChild(section_node,NULL,"width", "640");
	xmlNewTextChild(section_node,NULL,"textpane", "240");
	xmlNewTextChild(section_node,NULL,"shortcutbar", "178");
	xmlNewTextChild(section_node,NULL,"uperpane", "210");	
	
	
	section_node = xmlNewChild(root_node, NULL,"lexicons", NULL);
	xmlNewTextChild(section_node,NULL,"indictpane", "1");
	xmlNewTextChild(section_node,NULL,"inviewer", "1");
	xmlNewTextChild(section_node,NULL,"usedefaultdict", "1");
	xmlNewTextChild(section_node,NULL,"defaultdictionary", NULL);
	xmlNewTextChild(section_node,NULL,"greek", "StrongsGreek");
	xmlNewTextChild(section_node,NULL,"greekviewer", "StrongsGreek");
	xmlNewTextChild(section_node,NULL,"hebrew", "StrongsHebrew");
	xmlNewTextChild(section_node,NULL,"hebrewviewer", "StrongsHebrew");

	
	section_node = xmlNewChild(root_node, NULL, "misc", NULL);
	xmlNewTextChild(section_node,NULL,"dailydevotional", "0");
	xmlNewTextChild(section_node,NULL,"splash", "1");
	xmlNewTextChild(section_node,NULL,"usedefault", "1");
	xmlNewTextChild(section_node,NULL,"formatpercom", "1");
	xmlNewTextChild(section_node,NULL,"showcomms", "1");
	xmlNewTextChild(section_node,NULL,"showdicts", "1");	
	xmlNewTextChild(section_node,NULL,"showtexts", "1");
	xmlNewTextChild(section_node,NULL,"versestyle", "1");
	
	
	section_node = xmlNewChild(root_node, NULL, "modules", NULL);
	xmlNewTextChild(section_node,NULL,"book", "");
	xmlNewTextChild(section_node,NULL,"comm", "MHC");
	xmlNewTextChild(section_node,NULL,"devotional", "");
	xmlNewTextChild(section_node,NULL,"dict", "BDB");
	xmlNewTextChild(section_node,NULL,"int1", "");
	xmlNewTextChild(section_node,NULL,"int2", "");	
	xmlNewTextChild(section_node,NULL,"int3", "");
	xmlNewTextChild(section_node,NULL,"int4", "");
	xmlNewTextChild(section_node,NULL,"int5", "");
	xmlNewTextChild(section_node,NULL,"text", "KJV");
	xmlNewTextChild(section_node,NULL,"percomm", "Personal");
	
	
	section_node = xmlNewChild(root_node, NULL, "shortcutbar", NULL);	
	xmlNewTextChild(section_node,NULL,"shortcutbar", "1");
	xmlNewTextChild(section_node,NULL,"docked", "1");
	xmlNewTextChild(section_node,NULL,"book", "0");
	xmlNewTextChild(section_node,NULL,"bookmark", "0");
	xmlNewTextChild(section_node,NULL,"comm", "0");
	xmlNewTextChild(section_node,NULL,"dict", "0");
	xmlNewTextChild(section_node,NULL,"favo", "1");
	xmlNewTextChild(section_node,NULL,"history", "0");
	xmlNewTextChild(section_node,NULL,"text", "0");
	
		
	section_node = xmlNewChild(root_node, NULL, "studypad", NULL);
	xmlNewTextChild(section_node,NULL,"directory", "~");
	xmlNewTextChild(section_node,NULL,"lastfile", "");
	xmlNewTextChild(section_node,NULL,"editbar", "1");
	xmlNewTextChild(section_node,NULL,"stylebar", "1");
	
		
	section_node = xmlNewChild(root_node, NULL, "tabs", NULL);
	xmlNewTextChild(section_node,NULL,"text", "1");
	xmlNewTextChild(section_node,NULL,"comm", "1");
	xmlNewTextChild(section_node,NULL,"dict", "1");
	xmlNewTextChild(section_node,NULL,"book", "1");			
	xmlNewTextChild(section_node,NULL,"percomm", "1");
	
	//xmlSaveFile(xml_filename, xml_settings_doc);
	//xmlFreeDoc(xml_settings_doc);
	return 1;
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
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
				if(!xmlStrcmp
					    (item_cur->name, (const xmlChar *) prop)) {
						return item_cur;
				}
				if (item_cur->next)
					item_cur =
					    item_cur->next;
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
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

char *xml_get_value(char *section, char *item)
{
	xmlNodePtr cur = NULL;
	xmlChar * results = NULL;
	if ((cur =
	     xml_find_prop("GnomeSword",
			   (xmlChar *) section,
			   (xmlChar *) item)) != NULL) {
		results = xmlNodeListGetString(xml_settings_doc,cur->xmlChildrenNode,1);
		g_warning("%s = %s", item, results);
		if(results)
			return results;
	}
	return NULL;
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

void xml_set_value(char *type_doc, char *section, char *item,
		   char *value)
{
	xmlNodePtr cur = NULL;
	if ((cur =
	     xml_find_prop((xmlChar *) type_doc,
			   (xmlChar *) section,
			   (xmlChar *) item)) != NULL) {
		xmlNodeSetContent(cur,value);
		g_warning("setting %s to %s",cur->name,value);
		/*xmlSetProp(cur, (const xmlChar *) "value",
			   (const xmlChar *) value);*/
	}
}


/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

int xml_parse_settings_file(char *file_name)
{
	const xmlChar *file;
	char buf[256];
g_warning(file_name);
	//sprintf(buf, "%s/settings.xml", file_name);
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
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */


void xml_save_settings_doc(char *file_name)
{
	const xmlChar *file;
	char buf[256];

	//sprintf(buf, "%s/settings.xml", file_name);
	file = (const xmlChar *) file_name;
	xmlSaveFile(file, xml_settings_doc);
}



/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   
 *
 * Description
 *   
 *
 * Return value
 *   
 */

void xml_free_settings_doc(void)
{
	xmlFreeDoc(xml_settings_doc);
}
