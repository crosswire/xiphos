/*
 * GnomeSword Bible Study Tool
 * xml.c - 
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


//#include <glib-2.0/glib.h>
#include <libxml/parser.h>


#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"

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
// void xmlFreeNode                     (xmlNodePtr cur);
// void        xmlUnlinkNode                   (xmlNodePtr cur);



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
	const char *xml_filename;
	gchar buf[256];
	
	sprintf(buf, "%s/%s/bookmarks.xml", settings.gSwordDir,
		"bookmarks");
	xml_filename = buf;
	xml_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_doc == NULL) {
		fprintf(stderr,
			_("Document not created successfully. \n"));
		return;
	}

	xml_node = xmlNewNode(NULL, (const xmlChar *) "SwordBookmarks");
	xml_attr = xmlNewProp(xml_node, (const xmlChar *) "syntaxVersion",
			      (const xmlChar *) "1.0");
	xmlDocSetRootElement(xml_doc, xml_node);

	xml_root = xml_add_folder_to_parent(xml_node, _("Personal"));
	xml_folder = xml_add_folder_to_parent(xml_root, _("What must I do to be saved?"));
	xml_add_bookmark_to_parent(xml_folder, 
					_("Acts 16:31"),
					_("Acts 16:31"),
					NULL,
					NULL);
	xml_add_bookmark_to_parent(xml_folder, 
					_("Eph 2:8"),
					_("Eph 2:8"),
					NULL,
					NULL);
	xml_add_bookmark_to_parent(xml_folder, 
					_("Romans 1:16"),
					_("Romans 1:16"),
					NULL,
					NULL);
	xml_folder = xml_add_folder_to_parent(xml_root, _("What is the Gospel?"));
	xml_add_bookmark_to_parent(xml_folder, 
					_("1 Cor 15:1-4"),
					_("1 Cor 15:1"),
					NULL,
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
			      (const xmlChar *) "caption", (const xmlChar *) caption);
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
			gchar * key, gchar * module, gchar * mod_desc)
{
	xmlNodePtr xml_node;
	xmlAttrPtr xml_attr;
	//gchar *mod_desc = NULL;

	/*if (module) {
		if (strlen(module) > 2)
			mod_desc = get_module_description(module);
	} */

	if (mod_desc == NULL)
		mod_desc = " ";

	xml_node = xmlNewChild(parent,
			       NULL,
			       (const xmlChar *) "Bookmark", NULL);
	xml_attr = xmlNewProp(xml_node,
			      (const xmlChar *) "modulename",
			      (const xmlChar *) module);
	xml_attr =
	    xmlNewProp(xml_node, (const xmlChar *) "key", (const xmlChar *) key);
	xml_attr =
	    xmlNewProp(xml_node, (const xmlChar *) "moduledescription",
		       (const xmlChar *) mod_desc);
	xml_attr =
	    xmlNewProp(xml_node, (const xmlChar *) "description",
		       (const xmlChar *) caption);
	/*xml_attr =
	    xmlNewProp(xml_node, "caption",
		       (const xmlChar *) caption);*/
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
	gchar *mod_desc = NULL;

	for (work = node->children; work; work = work->next) {
		get_node_data(work);
		if (!es->is_leaf) {
			cur_node = xml_add_folder_to_parent(parent, es->caption);
		} else {
			if (es->module) {
				if (strlen(es->module) > 2)
					mod_desc = main_get_module_description(es->module);
			}
			xml_add_bookmark_to_parent(parent, es->caption,
						es->key, es->module, mod_desc);
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
	const char *xml_filename;
	gchar *mod_desc = NULL;
	
	if (!gnode)
		return;
	xml_filename = file_buf;
	root_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (root_doc != NULL) {
		root_node = xmlNewNode(NULL, (const xmlChar *)
				       "SwordBookmarks");
		root_attr =
		    xmlNewProp(root_node, (const xmlChar *) "syntaxVersion",
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
		if (es->module) {
			if (strlen(es->module) > 2)
				mod_desc = main_get_module_description(es->module);
		}
		xml_add_bookmark_to_parent(root_node, 
						es->caption,
						es->key,
						es->module,
						mod_desc);
	}

	while ((gnode = g_node_next_sibling(gnode)) != NULL) {
		get_node_data(gnode);
		if (!es->is_leaf) {
			cur_node =
			    xml_add_folder_to_parent(root_node,
						     es->caption);
			parse_gnode_tree(gnode, cur_node);
		} else {
			if (es->module) {
				if (strlen(es->module) > 2)
					mod_desc = main_get_module_description(es->module);
			}
			if (root_doc != NULL) {
				xml_add_bookmark_to_parent
				    (root_node, es->caption,
						es->key,
						es->module,
						mod_desc);
			}
		}
	}
	GS_print(("\nsaving = %s\n", xml_filename));
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
	GS_print(("\nsaving = %s\n", xml_filename));
	xmlSaveFormatFile((const char *) xml_filename, bookmark_doc,1);
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

	bookmark_doc = xmlParseFile((const char *) bookmark_file);

	if (bookmark_doc == NULL) {
		fprintf(stderr, _("Document not parsed successfully. \n"));
		return;
	}

	cur = xmlDocGetRootElement(bookmark_doc);
	if (cur == NULL) {
		fprintf(stderr, _("empty document \n"));
		return;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "SwordBookmarks")) {
		fprintf(stderr,
			_("wrong type, root node != SwordBookmarks\n"));
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
	GList *tmp = NULL;

	xml_filename = (const xmlChar *) path;
	xml_settings_doc = xmlNewDoc((const xmlChar *) "1.0");

	if (xml_settings_doc == NULL) {
		fprintf(stderr,
			_("Document not created successfully. \n"));
		return 0;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *) "GnomeSword");
	xml_attr = xmlNewProp(root_node, (const xmlChar *) "Version", (const xmlChar *) VERSION);
	xmlDocSetRootElement(xml_settings_doc, root_node);

	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) "ranges", NULL);
	cur_node = xmlNewChild(section_node,
			       NULL, (const xmlChar *) "range", NULL);
	xml_attr = xmlNewProp(cur_node,
			      (const xmlChar *) "label",
			      (const xmlChar *) _("Old Testament"));
	xml_attr =
	    xmlNewProp(cur_node, (const xmlChar *) "list", (const xmlChar *) _("Gen - Mal"));
	cur_node =
	    xmlNewChild(section_node, NULL, (const xmlChar *) "range",
			NULL);
	xml_attr =
	    xmlNewProp(cur_node, (const xmlChar *) "label",
		       (const xmlChar *) _("New Testament"));
	xml_attr =
	    xmlNewProp(cur_node, (const xmlChar *) "list", (const xmlChar *) _("Mat - Rev"));

	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) "modlists", NULL);
	cur_node = xmlNewChild(section_node,
			       NULL, (const xmlChar *) "modlist", NULL);
	xml_attr = xmlNewProp(cur_node,
			      (const xmlChar *) "label",
			      (const xmlChar *) _("Sample Module List"));
	xml_attr =
	    xmlNewProp(cur_node, (const xmlChar *) "list", (const xmlChar *) _("Select the modules you want"));


	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) "editor", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "UsePercommDialog", (const xmlChar *) "1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "UseStudyPad", (const xmlChar *) "1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "UseStudypadDialog", (const xmlChar *) "1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "spell_language", (const xmlChar *) "unknown");


	section_node = xmlNewChild(root_node, NULL, (const xmlChar *) "fontsize", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "versenum", (const xmlChar *) "+0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "basefontsize", (const xmlChar *) "+0");


	section_node = xmlNewChild(root_node, NULL, (const xmlChar *) "HTMLcolors", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "background", (const xmlChar *) "#FFFFFF");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "text_fg", (const xmlChar *) "#000000");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "versenum",(const xmlChar *)  "#0000CF");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "currentverse", (const xmlChar *) "#339766");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "link", (const xmlChar *) "#878787");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "found", (const xmlChar *) "#D02898");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "highlight_fg", (const xmlChar *)"#FFFF00");
	xmlNewTextChild(section_node, NULL, (const xmlChar *) "highlight_bg", (const xmlChar *)"#060680");


	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"parallel", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"parallel", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"cantillation", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"points", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"footnotes", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,(const xmlChar *) "morphs", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"strongs", (const xmlChar *)"0");

	section_node = xmlNewChild(root_node, NULL, (const xmlChar *)"book", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"module", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"key", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"offset", (const xmlChar *)"0");

	section_node = xmlNewChild(root_node, NULL, (const xmlChar *)"keys", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"book", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"dictionary", (const xmlChar *)"Grace");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"verse", (const xmlChar *)"Romans 8:28");


	section_node = xmlNewChild(root_node, NULL, (const xmlChar *)"layout", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"hight", (const xmlChar *)"480");
	xmlNewTextChild(section_node, NULL,(const xmlChar *) "width", (const xmlChar *)"640");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"app_x", (const xmlChar *)"40");
	xmlNewTextChild(section_node, NULL,(const xmlChar *) "app_y", (const xmlChar *)"40");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"textpane", (const xmlChar *)"240");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"shortcutbar", (const xmlChar *)"158");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"uperpane", (const xmlChar *)"210");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"vltoppaneheight", (const xmlChar *)"210");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"sidebar_notebook_hight", (const xmlChar *)"250");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"biblehight", (const xmlChar *)"250");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"commentaryhight", (const xmlChar *)"240");


	section_node = xmlNewChild(root_node, NULL, (const xmlChar *)"lexicons", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"indictpane", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"inviewer", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"usedefaultdict", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"defaultdictionary", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"greek", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"greekviewer", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"hebrew", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"hebrewviewer", NULL);


	section_node = xmlNewChild(root_node, NULL, (const xmlChar *)"misc", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"dailydevotional", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"splash", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"usedefault", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"formatpercom", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL,(const xmlChar *) "showcomms", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"showdicts", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"showtexts", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"versestyle", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"pinnedtabs", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"showversenum", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"readaloud", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"versehighlight", (const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"doublespace", (const xmlChar *)"0");

	section_node = xmlNewChild(root_node, NULL, (const xmlChar *)"modules", NULL);
	tmp = get_list(GBS_LIST);
	if(tmp)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"book", (const xmlChar *)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"book", NULL);
	tmp = get_list(COMM_LIST);
	if(tmp)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"comm", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL,(const xmlChar *) "comm", NULL);
	tmp = get_list(DEVOTION_LIST);
	if(tmp)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"devotional", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"devotional", NULL);
	tmp = get_list(DICT_LIST);
	if(tmp)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"dict", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"dict", NULL);
	tmp = get_list(TEXT_LIST);
	if(tmp)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int1", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int1", NULL);
	if((tmp = g_list_next(tmp)) != NULL)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int2",(const xmlChar *)(gchar*)tmp->data);		
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int2", NULL);
	if((tmp = g_list_next(tmp)) != NULL)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int3", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int3", NULL);
	if((tmp = g_list_next(tmp)) != NULL)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int4", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int4", NULL);
	if((tmp = g_list_next(tmp)) != NULL)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int5", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"int5", NULL);
	tmp = get_list(TEXT_LIST);
	if(tmp)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"bible", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"bible", NULL);
	tmp = get_list(PERCOMM_LIST);
	if(tmp)
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"percomm", (const xmlChar *)(gchar*)tmp->data);
	else
		xmlNewTextChild(section_node, NULL, (const xmlChar *)"percomm", NULL);


	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"shortcutbar", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"shortcutbar", (const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"docked", (const xmlChar *)"1");


	section_node = xmlNewChild(root_node, NULL, (const xmlChar *)"studypad", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"directory", (const xmlChar *)settings.homedir);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"lastfile", NULL);


	section_node = xmlNewChild(root_node, NULL,(const xmlChar *) "tabs", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"browsing",(const xmlChar *) "1");
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
		fprintf(stderr, _("empty document \n"));
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) type_doc)) {
		fprintf(stderr,
			_("wrong type, root node != %s\n"), type_doc);
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
			prop_label = xmlGetProp(cur, (const xmlChar *)"label");
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


	if ((cur = xml_find_section("GnomeSword", section)) != NULL) {

		cur = cur->xmlChildrenNode;

		while (cur != NULL) {

			//g_warning("cur->name = %s", cur->name);
			if (!xmlStrcmp
			    (cur->name, (const xmlChar *) item)) {
				prop_label = xmlGetProp(cur, (const xmlChar *) "label");
				//g_warning(prop_label);
				if (!xmlStrcmp
				    (prop_label,
				     (const xmlChar *) label)) {
					return (char *)xmlGetProp(cur, (const xmlChar *) "list");
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
	     xml_find_section("GnomeSword", section)) != NULL) {
		if ((cur_item = xml_find_item(cur, item, label)) !=
		    NULL) {
			xmlSetProp(cur_item, (const xmlChar *) "list", (const xmlChar *) value);
		} else {
			new = xmlNewChild(cur,
					  NULL, (const xmlChar *) item,
					  NULL);
			xmlNewProp(new,(const xmlChar *) "label",
				   (const xmlChar *) label);
			xmlNewProp(new, (const xmlChar *)"list",
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

	if ((cur = xml_find_section("GnomeSword", section)) != NULL) {
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
	return (char *) xmlGetProp(section_ptr, (const xmlChar *) "label");
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
	return (char *) xmlGetProp(section_ptr, (const xmlChar *) "list");
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
		fprintf(stderr, _("empty document \n"));
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) type_doc)) {
		fprintf(stderr,
			_("wrong type, root node != %s\n"), type_doc);
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
	     xml_find_prop("GnomeSword", section, (char *)item)) != NULL) {
		results = xmlNodeListGetString(xml_settings_doc,
					       cur->xmlChildrenNode, 1);
		if (results)
			return (char *)results;
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
	     xml_find_prop(type_doc, section, (char *)item)) != NULL) {
		xmlNodeSetContent(cur, (const xmlChar *)value);
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
	//const xmlChar *file;

	//file = (const xmlChar *) file_name;
	xml_settings_doc = xmlParseFile(file_name);

	if (xml_settings_doc == NULL) {
		fprintf(stderr, _("Document not parsed successfully. \n"));
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
	//const xmlChar *file;

	//file = (const xmlChar *) file_name;
	xmlSaveFormatFile(file_name, xml_settings_doc,1);
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

void xml_add_new_section_to_settings_doc(char * section)
{
	xmlNodePtr root_node = NULL;
	xmlNodePtr section_node = NULL;
	
	root_node = xmlDocGetRootElement(xml_settings_doc);
	if (root_node == NULL) {
		fprintf(stderr, _("empty document \n"));
		return NULL;
	}	
	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *) section, NULL);
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

void xml_add_new_item_to_section(char * section, char * item_name, char * value)
{
	xmlNodePtr section_node = NULL;
	
	section_node = xml_find_section("GnomeSword", section);
	
	if(section_node) 		
		xmlNewTextChild(section_node, NULL, (const xmlChar *)item_name, 
						    (const xmlChar *)value);
}

/******************************************************************************
 * Name
 *   
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   	
 *
 * Description
 *    
 *
 * Return value
 *   void
 */

void xml_remove_node(char *section, char *item, char *label)
{
	xmlNodePtr cur = NULL;
	xmlNodePtr cur_item = NULL;
	xmlNodePtr new = NULL;
	if ((cur =
	     xml_find_section( "GnomeSword", section)) != NULL) {
		if ((cur_item = xml_find_item(cur, item, label)) != NULL) {			
			xmlUnlinkNode(cur_item);
			xmlFreeNode(cur_item);      
		} 	  
	}	
	
}
