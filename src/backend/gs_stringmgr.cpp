/******************************************************************************
 *  gs_stringmgr.cpp - GS implementation of class StringMgr
 *                     StringMgr is a way to provide UTf8 handling
 *                     by the Sword frontend
 *
 *  gs_stringmgr was mostly inspired by BTSringMgr from BibleTime
 *  Credits to the BibleTime Team (http://www.bibletime.info)
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, U.
 */

#include "gs_stringmgr.h"

//System includes
#include <glib.h>

SWORD_NAMESPACE_START

char *GS_StringMgr::upperUTF8(char *text, unsigned int maxlen) const
{
	if ((text == NULL) || (*text == '\0')) {
		return g_strdup("");
	}
	if (maxlen == 0)
		maxlen = strlen(text);
	char *upper_str = g_utf8_strup(text, -1);
	unsigned int newlen = strlen(upper_str);
	if (newlen >= maxlen)
		newlen = maxlen;
	memcpy(text, upper_str, newlen);
	//text[newlen] = 0;	// be sure we're null terminated.
	g_free(upper_str);
	return text;
}

bool GS_StringMgr::supportsUnicode() const
{
	return true;
}

SWORD_NAMESPACE_END
