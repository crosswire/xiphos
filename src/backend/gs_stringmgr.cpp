/******************************************************************************
 *  gs_stringmgr.cpp - GS implementation of class StringMgr
 *                     StringMgr is a way to provide UTf8 handling 
 *                     by the Sword frontend
 *
 *  gs_stringmgr was mostly inspired by BTSringMgr from BibleTime
 *  Credits to the BibleTime Team (http://www.bibletime.info)
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 */

#include "gs_stringmgr.h"

//System includes
#include <glib.h>

SWORD_NAMESPACE_START

char* GS_StringMgr::upperUTF8(char* text, unsigned int maxlen) const {
	if ((text == NULL) || (*text == '\0')) {
		return g_strdup("");
	}
	char *upper_str = g_utf8_strup(text, -1);
	int newlen = strlen(upper_str);
	if (newlen >= maxlen) newlen = maxlen - 1;
	memcpy(text, upper_str, newlen);
	text[newlen] = 0;	// be sure we're null terminated.
	g_free(upper_str);
	return text;
}

bool GS_StringMgr::supportsUnicode() const {
	return true;
}

SWORD_NAMESPACE_END
