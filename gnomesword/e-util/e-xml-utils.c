/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * e-xml-utils.c
 * Copyright (C) 2000  Helix Code, Inc.
 * Author: Chris Lahey <clahey@helixcode.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <locale.h>
#include "e-xml-utils.h"
#include <gnome-xml/parser.h>
#include <gnome-xml/xmlmemory.h>


xmlNode *e_xml_get_child_by_name(xmlNode *parent, const xmlChar *child_name)
{
	xmlNode *child;

	g_return_val_if_fail(parent != NULL, NULL);
	g_return_val_if_fail(child_name != NULL, NULL);
	
	for (child = parent->childs; child; child = child->next) {
		if ( !xmlStrcmp( child->name, child_name ) ) {
			return child;
		}
	}
	return NULL;
}

/* Returns the first child with the name child_name and the "lang"
 * attribute that matches the current LC_MESSAGES, or else, the first
 * child with the name child_name and no "lang" attribute.
 */
xmlNode *
e_xml_get_child_by_name_by_lang(xmlNode *parent, const xmlChar *child_name, const char *lang)
{
	xmlNode *child;
	/* This is the default version of the string. */
	xmlNode *C = NULL;

	g_return_val_if_fail(parent != NULL, NULL);
	g_return_val_if_fail(child_name != NULL, NULL);

	if (lang == NULL)
		lang = setlocale(LC_MESSAGES, NULL);

	for (child = parent->childs; child; child = child->next) {
		if ( !xmlStrcmp( child->name, child_name ) ) {
			char *this_lang = xmlGetProp(child, "lang");
			if ( this_lang == NULL ) {
				C = child;
			}
			else if (!strcmp(this_lang, "lang"))
				return child;
		}
	}
	return C;
}

int
e_xml_get_integer_prop_by_name(xmlNode *parent, const xmlChar *prop_name)
{
	xmlChar *prop;
	int ret_val = 0;

	g_return_val_if_fail (parent != NULL, 0);
	g_return_val_if_fail (prop_name != NULL, 0);

	prop = xmlGetProp(parent, prop_name);
	if (prop) {
		ret_val = atoi(prop);
		xmlFree(prop);
	}
	return ret_val;
}

void
e_xml_set_integer_prop_by_name(xmlNode *parent, const xmlChar *prop_name, int value)
{
	xmlChar *valuestr;

	g_return_if_fail (parent != NULL);
	g_return_if_fail (prop_name != NULL);

	valuestr = g_strdup_printf("%d", value);
	xmlSetProp(parent, prop_name, valuestr);
	g_free (valuestr);
}


