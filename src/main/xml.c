/*
 * Xiphos Bible Study Tool
 * xml.c -
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <libxml/parser.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "main/lists.h"
#include "main/settings.h"
#include "main/sword.h"
#include "main/xml.h"
#include "gui/dialog.h"

struct _bookmark_data
{
	gchar *caption;
	gchar *key;
	gchar *module;
	gboolean is_leaf;
};
typedef struct _bookmark_data BOOKMARK_DATA;

static xmlDocPtr xml_settings_doc;
static xmlDocPtr xml_export_doc;
static xmlNodePtr section_ptr;
static xmlDocPtr bookmark_doc;
static xmlNodePtr xml_find_prop(xmlDocPtr doc, const char *type_doc,
				const char *section, const char *prop);

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
	const char *xml_filename;
	gchar buf[256];

	sprintf(buf, "%s/%s/bookmarks.xml", settings.gSwordDir,
		"bookmarks");
	xml_filename = buf;
	xml_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (xml_doc == NULL) {
		fprintf(stderr,
			_("Document not created successfully. \n"));
		return;
	}

	xml_node = xmlNewNode(NULL, (const xmlChar *)"SwordBookmarks");
	(void)xmlNewProp(xml_node, (const xmlChar *)"syntaxVersion",
			 (const xmlChar *)"1.0");
	xmlDocSetRootElement(xml_doc, xml_node);

	xml_root = xml_add_folder_to_parent(xml_node, _("Personal"));
	xml_folder =
	    xml_add_folder_to_parent(xml_root,
				     _("What must I do to be saved?"));
	xml_add_bookmark_to_parent(xml_folder, _("Acts 16:31"),
				   _("Acts 16:31"), NULL, NULL);
	xml_add_bookmark_to_parent(xml_folder, _("Eph 2:8,9"),
				   _("Eph 2:8,9"), NULL, NULL);
	xml_add_bookmark_to_parent(xml_folder, _("Romans 1:16"),
				   _("Romans 1:16"), NULL, NULL);
	xml_folder =
	    xml_add_folder_to_parent(xml_root, _("What is the Gospel?"));
	xml_add_bookmark_to_parent(xml_folder, _("1 Cor 15:1-4"),
				   _("1 Cor 15:1-4"), NULL, NULL);

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

xmlNodePtr xml_add_folder_to_parent(xmlNodePtr parent, gchar *caption)
{
	xmlNodePtr cur_node;

	cur_node = xmlNewChild(parent,
			       NULL, (const xmlChar *)"Folder", NULL);
	(void)xmlNewProp(cur_node,
			 (const xmlChar *)"caption",
			 (const xmlChar *)caption);
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

void xml_add_bookmark_to_parent(xmlNodePtr parent,
				gchar *caption,
				gchar *key,
				gchar *module, const gchar *mod_desc)
{
	xmlNodePtr xml_node;
	//gchar *mod_desc = NULL;

	/*if (module) {
	   if (strlen(module) > 2)
	   mod_desc = get_module_description(module);
	   } */

	if (mod_desc == NULL)
		mod_desc = " ";

	xml_node = xmlNewChild(parent,
			       NULL, (const xmlChar *)"Bookmark", NULL);
	(void)xmlNewProp(xml_node,
			 (const xmlChar *)"modulename",
			 (const xmlChar *)module);
	(void)xmlNewProp(xml_node, (const xmlChar *)"key",
			 (const xmlChar *)key);
	(void)xmlNewProp(xml_node, (const xmlChar *)"moduledescription",
			 (const xmlChar *)mod_desc);
	(void)xmlNewProp(xml_node, (const xmlChar *)"description",
			 (const xmlChar *)caption);
	/*xml_attr =
	   xmlNewProp(xml_node, "caption",
	   (const xmlChar *) caption); */
}

#if 0
// unneeded at this time.  disabled to silence cppcheck.
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

void xml_write_bookmark_doc(const xmlChar *xml_filename)
{
	XI_print(("\nsaving = %s\n", xml_filename));
	xmlSaveFormatFile((const char *)xml_filename, bookmark_doc, 1);
}
#endif

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void
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

xmlNodePtr xml_load_bookmark_file(const xmlChar *bookmark_file)
{

	xmlNodePtr cur = NULL;

	bookmark_doc = xmlParseFile((const char *)bookmark_file);

	if (bookmark_doc == NULL) {
		fprintf(stderr, _("Document not parsed successfully. \n"));
		return NULL;
	}

	cur = xmlDocGetRootElement(bookmark_doc);
	if (cur == NULL) {
		fprintf(stderr, _("empty document \n"));
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)"SwordBookmarks")) {
		fprintf(stderr,
			_("wrong type, root node != SwordBookmarks\n"));
		xmlFreeDoc(bookmark_doc);
		return NULL;
	}

	cur = cur->xmlChildrenNode;
	return cur;
}

/******************************************************************************
 * Name
 *   xml_save_export_doc
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_save_export_doc(char *file_name)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void xml_save_export_doc(char *name)
{

	xmlSaveFormatFile(name, xml_export_doc, 1);
}

/******************************************************************************
 * Name
 *   xml_get_copy_export_value
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   char *xml_get_copy_export_value(char *section, char *item)
 *
 * Description
 *
 *
 * Return value
 *   char *
 */

char *xml_get_copy_export_value(const char *section, const char *item)
{
	xmlNodePtr cur = NULL;
	if ((cur =
		 xml_find_prop(xml_export_doc, "Copy_Export", section,
			       item)) != NULL) {
		xmlChar *results =
		    xmlNodeListGetString(xml_export_doc,
					 cur->xmlChildrenNode, 1);
		if (results)
			return (char *)results;
	}
	return NULL;
}

/******************************************************************************
 * Name
 *   xml_export_set_value
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_export_set_value(char *type_doc, char *section, char *item,
 *		   char *value)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void xml_export_set_value(const char *type_doc, const char *section,
			  const char *item, const char *value)
{
	xmlNodePtr cur = NULL;
	if ((cur =
		 xml_find_prop(xml_export_doc, type_doc, section,
			       item)) != NULL) {
		xmlNodeSetContent(cur, (const xmlChar *)value);
	}
}

/******************************************************************************
 * Name
 *   xml_free_export_doc
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_free_export_doc(void)
 *
 * Description
 *
 *
 * Return value
 *   void
 */

void xml_free_export_doc(void)
{
	xmlFreeDoc(xml_export_doc);
}

/******************************************************************************
 * Name
 *   xml_load_copy_export_file
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   xmlNodePtr xml_load_copy_export_file(char * bookmark_file)
 *
 * Description
 *
 *
 * Return value
 *   xmlNodePtr
 */

int xml_load_copy_export_file(const xmlChar *file)
{
	xmlNodePtr cur = NULL;

	xml_export_doc = xmlParseFile((const char *)file);

	if (xml_export_doc == NULL) {
		fprintf(stderr, _("Document not parsed successfully. \n"));
		return 0;
	}

	cur = xmlDocGetRootElement(xml_export_doc);
	if (cur == NULL) {
		fprintf(stderr, _("empty document \n"));
		return 0;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)"Copy_Export")) {
		fprintf(stderr,
			_("wrong type, root node != Copy_Export\n"));
		xmlFreeDoc(xml_export_doc);
		return 0;
	}
	//cur = cur->xmlChildrenNode;
	return 1;
}

void xml_add_new_section_to_doc(xmlDocPtr doc, char *section)
{
	xmlNodePtr root_node = NULL;

	root_node = xmlDocGetRootElement(doc);
	if (root_node == NULL) {
		fprintf(stderr, _("empty document \n"));
		return;
	}
	(void)xmlNewChild(root_node, NULL,
			  (const xmlChar *)section, NULL);
}

/******************************************************************************
 * Name
 *   xml_create_copy_export_file
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   int xml_create_copy_export_file(char * path)
 *
 * Description
 *   create new copy/export file
 *
 * Return value
 *   int
 */

int xml_create_copy_export_file(char *path)
{
	xmlNodePtr root_node;
	xmlNodePtr section_node;
	xmlDocPtr xml_doc;

	xml_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (xml_doc == NULL) {
		fprintf(stderr,
			_("Document not created successfully. \n"));
		return 0;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *)"Copy_Export");
	(void)xmlNewProp(root_node, (const xmlChar *)"Version",
			 (const xmlChar *)"0");
	xmlDocSetRootElement(xml_doc, root_node);

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"dialog", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"plaintext",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"HTML",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"verse_numbers",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"reference_last",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"version",
			(const xmlChar *)"1");

	/* book format */

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"book", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"header",
			(const xmlChar *)
			_("%s<center>%s<br>Chapter %d</center><br>"));
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"chapterheader",
			(const xmlChar *)
			_("<br><center>Chapter %d</center><br>"));
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"plain_header",
			(const xmlChar *)_("%s\n\nChapter %d\n\n"));
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"plain_chapterheader",
			(const xmlChar *)_("\n\nChapter %d\n\n"));

	/* chapter format */

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"chapter",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"header",
			(const xmlChar *)
			_("%s%s: %s Chapter %d<br><br>"));
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"plain_header",
			(const xmlChar *)_("%s: %s Chapter %d\n\n"));

	/* versenumber format */

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"versenumber",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"format",
			(const xmlChar *)_("&nbsp;&nbsp;[%d]"));
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"plain_format",
			(const xmlChar *)_(" [%d]"));

	/* single verse format */

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"singleverse",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"last",
			(const xmlChar *)
			_("%s%s (%s %d:%d%s)</body></html>"));
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"first",
			(const xmlChar *)
			_("%s(%s %d:%d%s)%s </body></html>"));
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"plain_last",
			(const xmlChar *)_("%s (%s %d:%d%s)"));
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"plain_first",
			(const xmlChar *)_("(%s %d:%d%s) %s"));

	/* verse range format */

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"verserange",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"verse",
			(const xmlChar *)_(" %s%s"));
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"last",
			(const xmlChar *)_("%s(%s %d:%d-%d%s)"));
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"first",
			(const xmlChar *)_("(%s %d:%d-%d%s)<br>"));
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"plain_last",
			(const xmlChar *)_("%s(%s %d:%d-%d%s)"));
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"plain_first",
			(const xmlChar *)_("(%s %d:%d-%d%s)\n"));

	xmlSaveFormatFile(path, xml_doc, 1);
	xmlFreeDoc(xml_doc);
	return 1;
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
	GList *tmp = NULL;

	xml_settings_doc = xmlNewDoc((const xmlChar *)"1.0");

	if (xml_settings_doc == NULL) {
		fprintf(stderr,
			_("Document not created successfully. \n"));
		return 0;
	}

	root_node = xmlNewNode(NULL, (const xmlChar *)"Xiphos");
	(void)xmlNewProp(root_node, (const xmlChar *)"Version",
			 (const xmlChar *)VERSION);
	xmlDocSetRootElement(xml_settings_doc, root_node);

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"ranges", NULL);
	cur_node =
	    xmlNewChild(section_node, NULL, (const xmlChar *)"range",
			NULL);
	(void)xmlNewProp(cur_node, (const xmlChar *)"label",
			 (const xmlChar *)_("Old Testament"));
	(void)xmlNewProp(cur_node, (const xmlChar *)"list",
			 (const xmlChar *)_("Gen - Mal"));

	cur_node =
	    xmlNewChild(section_node, NULL, (const xmlChar *)"range",
			NULL);
	(void)xmlNewProp(cur_node, (const xmlChar *)"label",
			 (const xmlChar *)_("New Testament"));
	(void)xmlNewProp(cur_node, (const xmlChar *)"list",
			 (const xmlChar *)_("Mat - Rev"));

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"modlists",
			NULL);
	cur_node =
	    xmlNewChild(section_node, NULL, (const xmlChar *)"modlist",
			NULL);
	(void)xmlNewProp(cur_node, (const xmlChar *)"label",
			 (const xmlChar *)_("Sample Module List"));
	tmp = get_list(TEXT_LIST);
	(void)xmlNewProp(cur_node,
			 (const xmlChar *)"list",
			 (tmp ? (const xmlChar *)(gchar *)(tmp->data)
			      : (const xmlChar *)(gchar *) "-ReplaceMe-"));

	section_node = xmlNewChild(root_node, NULL,
				   (const xmlChar *)"editor", NULL);
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"spell_language",
			(const xmlChar *)"unknown");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"locale", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"special",
			(const xmlChar *)"");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"modmgr", NULL);
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"mod_mgr_source",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"mod_mgr_destination",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"mod_mgr_local_source_index",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"mod_mgr_remote_source_index",
			(const xmlChar *)"0");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"fontsize",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"versenum",
			(const xmlChar *)"+0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"basefontsize",
			(const xmlChar *)"+0");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"HTMLcolors",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"background", (const xmlChar *)"#FFFFFF");   /* white */
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"text_fg", (const xmlChar *)"#000000");      /* black */
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"versenum", (const xmlChar *)"#0000CF");     /* blue */
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"currentverse", (const xmlChar *)"#009000"); /* green */
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"link", (const xmlChar *)"#878787");	 /* grey */
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"found", (const xmlChar *)"#D02898");	/* ? */
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"highlight_fg", (const xmlChar *)"#FFFF00"); /* yellow */
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"highlight_bg", (const xmlChar *)"#060680"); /* navy */

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"parallel",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"parallel",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"cantillation",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"points",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"footnotes",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"morphs",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"strongs",
			(const xmlChar *)"0");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"book", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"module",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"key", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"offset",
			(const xmlChar *)"0");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"keys", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"book",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"dictionary",
			(const xmlChar *)"Grace");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"verse",
			(const xmlChar *)"Romans 8:28");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"layout", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"maximized",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"height",
			(const xmlChar *)"600");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"width",
			(const xmlChar *)"800");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"app_x",
			(const xmlChar *)"40");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"app_y",
			(const xmlChar *)"40");

	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"parallelopen",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"parallel_height",
			(const xmlChar *)"361");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"parallel_width",
			(const xmlChar *)"657");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"parallel_x",
			(const xmlChar *)"40");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"parallel_y",
			(const xmlChar *)"40");

	xmlNewTextChild(section_node, NULL, (const xmlChar *)"modmgropen",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"modmgr_height",
			(const xmlChar *)"450");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"modmgr_width",
			(const xmlChar *)"710");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"modmgr_x",
			(const xmlChar *)"40");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"modmgr_y",
			(const xmlChar *)"40");

	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"advsearchopen",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"advsearch_height",
			(const xmlChar *)"465");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"advsearch_width",
			(const xmlChar *)"665");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"advsearch_x",
			(const xmlChar *)"40");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"advsearch_y",
			(const xmlChar *)"40");

	xmlNewTextChild(section_node, NULL, (const xmlChar *)"prefsopen",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"prefs_height",
			(const xmlChar *)"300");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"prefs_width",
			(const xmlChar *)"590");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"prefs_x",
			(const xmlChar *)"40");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"prefs_y",
			(const xmlChar *)"40");

	xmlNewTextChild(section_node, NULL, (const xmlChar *)"textpane",
			(const xmlChar *)"368");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"shortcutbar",
			(const xmlChar *)"200");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"uperpane",
			(const xmlChar *)"210");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"vltoppaneheight",
			(const xmlChar *)"210");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"sidebar_notebook_height",
			(const xmlChar *)"350");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"bibleheight",
			(const xmlChar *)"250");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"commentaryheight",
			(const xmlChar *)"240");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"lexicons",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"indictpane",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"inviewer",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"usedefaultdict",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"defaultdictionary", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"greek",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"hebrew",
			NULL);

	/* cppcheck dislikes assigning this to section_node  */
	/* because it is then immediately re-assigned after. */
	(void) xmlNewChild(root_node, NULL,
			   (const xmlChar *)"osisrefmarkedverses",
			   NULL);

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"misc", NULL);
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"dailydevotional",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"splash",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"usedefault",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"formatpercom",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"showcomms",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"showdicts",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"showtexts",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"versestyle",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"pinnedtabs",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"showversenum",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"imageresize",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"verse_num_bold",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"verse_num_bracket",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"verse_num_superscript",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"readaloud",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"versehighlight",
			(const xmlChar *)"0");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"annotatehighlight",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"xrefsinverselist",
			(const xmlChar *)"1");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"modules",
			NULL);
	tmp = get_list(GBS_LIST);
	if (tmp)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"book",
				(const xmlChar *)tmp->data);
	else
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"book", NULL);
	tmp = get_list(COMM_LIST);
	if (tmp)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"comm",
				(const xmlChar *)(gchar *) tmp->data);
	else
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"comm", NULL);
	tmp = get_list(DEVOTION_LIST);
	if (tmp)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"devotional",
				(const xmlChar *)(gchar *) tmp->data);
	else
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"devotional", NULL);
	tmp = get_list(DICT_LIST);
	if (tmp)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"dict",
				(const xmlChar *)(gchar *) tmp->data);
	else
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"dict", NULL);

	tmp = get_list(TEXT_LIST);
	if (tmp) /* default parallels list set to 1 bible. */
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"parallels",
				(const xmlChar *)(gchar *) tmp->data);
	else
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"parallels", NULL);

	if (tmp)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"bible",
				(const xmlChar *)(gchar *) tmp->data);
	else
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"bible", NULL);

	tmp = get_list(PERCOMM_LIST);
	if (tmp)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"percomm",
				(const xmlChar *)(gchar *) tmp->data);
	else
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)"percomm", NULL);

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"shortcutbar",
			NULL);
	xmlNewTextChild(section_node, NULL,
			(const xmlChar *)"shortcutbar",
			(const xmlChar *)"1");
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"docked",
			(const xmlChar *)"1");

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"studypad",
			NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"directory",
			(const xmlChar *)settings.homedir);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"lastfile",
			NULL);

	section_node =
	    xmlNewChild(root_node, NULL, (const xmlChar *)"tabs", NULL);
	xmlNewTextChild(section_node, NULL, (const xmlChar *)"browsing",
			(const xmlChar *)"1");
	return 1;
}

/******************************************************************************
 * Name
 *   xml_convert_to_osisref
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   void xml_convert_to_osisref()
 *
 * Description
 *   convert "Gen 1:1" format to OSISRef "Gen.1.1" format.
 *   backward compatibility support from 3.1.1's first (bad) cut that
 *   failed to localize.  for shame, the pain i cause myself...
 *
 * Return value
 *   void
 */

void xml_convert_to_osisref(void)
{
	gchar reference[100];
	GList *deletable = NULL, *hold;

	/* pull entries from old "markedverses" set */
	/* re-insert into new "osisrefmarkedverses" */
	if (xml_set_section_ptr("markedverses") && xml_get_label()) {
		do {
			gchar *label, *content, *s, *t;

			label = xml_get_label();
			content = g_strdup(xml_get_list());

			/* detect "NASB Revelation of John 1:1" */
			if (((s = strrchr(label, ' ')) != NULL) &&      /* rightmost space */
			    ((s = strchr((t = s + 1), ':')) != NULL) && /* chapter:verse */
			    ((t = strchr(label, ' ')) < s)) {		/* leftmost space */

				/* having gotten this far, we successfully */
				/* detected a wrongly-formatted label.  convert. */

				/* mark old bogus version for removal. */
				deletable =
				    g_list_prepend(deletable,
						   g_strdup(label));

				/* cons up a new version. */
				*t = '\0';
				g_snprintf(reference, 100, "%s %s", label
					   /* this is just the module name now */
					   ,
					   main_get_osisref_from_key((const char *)
								     label,
								     (const char *)t + 1));
				xml_set_list_item("osisrefmarkedverses",
						  "markedverse", reference,
						  content);
			}
			g_free(content);
		} while (xml_next_item() && xml_get_label());
	}

	hold = deletable;
	while (deletable) {
		xml_remove_node("markedverses", "markedverse",
				deletable->data);
		g_free(deletable->data);
		deletable = g_list_next(deletable);
	}
	xml_remove_section("markedverses");
	g_list_free(hold);
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

static xmlNodePtr xml_find_section(xmlDocPtr doc, const char *type_doc,
				   const char *section)
{
	xmlNodePtr cur = NULL;

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, _("empty document \n"));
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)type_doc)) {
		fprintf(stderr,
			_("wrong type, root node != %s\n"), type_doc);
		return NULL;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)section)) {
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

static xmlNodePtr xml_find_item(xmlNodePtr parent, const char *item,
				const char *label)
{
	xmlNodePtr cur = NULL;
	xmlChar *prop_label = NULL;

	cur = parent->xmlChildrenNode;

	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)item)) {
			/* no label means any (i.e. first) element matches */
			if (!label)
				return cur;

			/* otherwise we must find the label match. */
			prop_label =
			    xmlGetProp(cur, (const xmlChar *)"label");
			if (!xmlStrcmp(prop_label, (const xmlChar *)label)) {
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

char *xml_get_list_from_label(const char *section, const char *item,
			      const char *label)
{
	xmlNodePtr cur = NULL;

	if ((cur =
		 xml_find_section(xml_settings_doc, "Xiphos",
				  section)) != NULL) {

		cur = cur->xmlChildrenNode;

		while (cur != NULL) {

			//g_warning("cur->name = %s", cur->name);
			if (!xmlStrcmp(cur->name, (const xmlChar *)item)) {
				xmlChar *prop_label =
				    xmlGetProp(cur,
					       (const xmlChar *)"label");
				XI_message(("\nprop_label: %s\nlabel: %s",
					    prop_label, label));
				if (!xmlStrcmp(prop_label,
					       (const xmlChar *)label)) {
					return (char *)xmlGetProp(cur,
								  (const xmlChar *)"list");
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

void xml_set_list_item(const char *section, const char *item,
		       const char *label, const char *value)
{
	xmlNodePtr cur = NULL;
	xmlNodePtr cur_item = NULL;
	xmlNodePtr new = NULL;

	if ((cur =
		 xml_find_section(xml_settings_doc, "Xiphos",
				  section)) == NULL) {
		xmlNodePtr root_node =
		    xmlDocGetRootElement(xml_settings_doc);
		cur =
		    xmlNewChild(root_node, NULL, (const xmlChar *)section,
				NULL);
	}

	if (cur) {
		if ((cur_item = xml_find_item(cur, item, label))) {
			xmlSetProp(cur_item, (const xmlChar *)"list",
				   (const xmlChar *)value);
		} else {
			new =
			    xmlNewChild(cur, NULL, (const xmlChar *)item,
					NULL);
			xmlNewProp(new, (const xmlChar *)"label",
				   (const xmlChar *)label);
			xmlNewProp(new, (const xmlChar *)"list",
				   (const xmlChar *)value);
		}
	}
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

void xml_set_new_element(const char *section, const char *item,
			 const char *content)
{
	xmlNodePtr cur = NULL;

	if ((cur =
		 xml_find_section(xml_settings_doc, "Xiphos",
				  section)) == NULL) {
		xmlNodePtr root_node =
		    xmlDocGetRootElement(xml_settings_doc);
		cur =
		    xmlNewChild(root_node, NULL, (const xmlChar *)section,
				NULL);
	}

	if (cur)
		(void)xmlNewChild(cur, NULL, (const xmlChar *)item,
				  (const xmlChar *)content);
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

int xml_set_section_ptr(const char *section)
{
	xmlNodePtr cur = NULL;

	if ((cur =
		 xml_find_section(xml_settings_doc, "Xiphos",
				  section)) != NULL) {
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
	return (char *)xmlGetProp(section_ptr, (const xmlChar *)"label");
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
	return (char *)xmlGetProp(section_ptr, (const xmlChar *)"list");
}

/******************************************************************************
 * Name
 *   xml_find_prop
 *
 * Synopsis
 *   #include "main/xml.h"
 *
 *   xmlNodePtr xml_find_prop(xmlDocPtr doc, char *type_doc, char *section,
				char *prop)
 *
 * Description
 *   "Xiphos"
 *
 * Return value
 *   xmlNodePtr
 */

static xmlNodePtr xml_find_prop(xmlDocPtr doc, const char *type_doc,
				const char *section, const char *prop)
{
	xmlNodePtr cur = NULL;
	xmlNodePtr item_cur = NULL;

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		fprintf(stderr, _("empty document \n"));
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *)type_doc)) {
		fprintf(stderr,
			_("wrong type, root node != %s\n"), type_doc);
		return NULL;
	}

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if (!xmlStrcmp(cur->name, (const xmlChar *)section)) {
			item_cur = cur->xmlChildrenNode;
			while (item_cur != NULL) {
				if (!xmlStrcmp(item_cur->name,
					       (const xmlChar *)prop)) {
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

char *xml_get_value(const char *section, const char *item)
{
	xmlNodePtr cur = NULL;
	if ((cur =
		 xml_find_prop(xml_settings_doc, "Xiphos", section,
			       item)) != NULL) {
		xmlChar *results =
		    xmlNodeListGetString(xml_settings_doc,
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

void xml_set_value(const char *type_doc, const char *section,
		   const char *item, const char *value)
{
	xmlNodePtr cur = NULL;
	if ((cur =
		 xml_find_prop(xml_settings_doc, type_doc, section,
			       item)) != NULL) {
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

void xml_save_settings_doc(char *name)
{
#ifndef WIN32
	/* win32 doesn't do the backup right because files are open */
	int retval;
	struct stat buf;
	char *backup_name = g_strdup_printf("%s.SAVE", name);

	/* preserve a good backup if possible. */
	g_unlink(backup_name);
	g_rename(name, backup_name);
	g_free(backup_name);
#endif

	/* save the new one. */
	xmlSaveFormatFile(name, xml_settings_doc, 1);

#ifndef WIN32
	/* did it save properly? */
	if (((retval = stat(name, &buf)) < 0) ||
	    ((retval == 0) && (buf.st_size == 0))) {
		char *msg =
		    g_strdup_printf(_("Save of settings failed! stat %d, size %d\n%s"),
				    retval, (int)buf.st_size,
				    _("Attempting to revert to previous save."));
		g_unlink(name);
		g_rename(backup_name, name);
		gui_generic_warning_modal(msg);
		g_free(msg);
	}
#endif
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

void xml_add_new_section_to_settings_doc(char *section)
{
	xmlNodePtr root_node = NULL;

	root_node = xmlDocGetRootElement(xml_settings_doc);
	if (root_node == NULL) {
		fprintf(stderr, _("empty document \n"));
		return;
	}
	(void)xmlNewChild(root_node, NULL,
			  (const xmlChar *)section, NULL);
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

void xml_add_new_item_to_export_doc_section(char *section, char *item_name,
					    char *value)
{
	xmlNodePtr section_node = NULL;

	section_node =
	    xml_find_section(xml_export_doc, "Copy_Export", section);

	if (section_node)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)item_name,
				(const xmlChar *)value);
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

void xml_add_new_item_to_section(char *section, char *item_name,
				 char *value)
{
	xmlNodePtr section_node = NULL;

	section_node =
	    xml_find_section(xml_settings_doc, "Xiphos", section);

	if (section_node)
		xmlNewTextChild(section_node, NULL,
				(const xmlChar *)item_name,
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

void xml_remove_node(const char *section, const char *item,
		     const char *label)
{
	xmlNodePtr cur = NULL;
	xmlNodePtr cur_item = NULL;
	if ((cur =
		 xml_find_section(xml_settings_doc, "Xiphos",
				  section)) != NULL) {
		if ((cur_item = xml_find_item(cur, item, label)) != NULL) {
			xmlUnlinkNode(cur_item);
			xmlFreeNode(cur_item);
		}
	}
}

/******************************************************************************
 * Name
 *   xml_remove_section
 *
 * Synopsis
 *   #include "gui/bookmarks.h"
 *
 *   void xml_remove_section(char *section)
 *
 * Description
 *   annihilates an entire section from the document.
 *
 * Return value
 *   void
 */

void xml_remove_section(const char *section)
{
	xmlNodePtr cur = NULL;

	if ((cur =
		 xml_find_section(xml_settings_doc, "Xiphos",
				  section)) != NULL) {
		xmlUnlinkNode(cur);
		xmlFreeNode(cur);
	}
}
