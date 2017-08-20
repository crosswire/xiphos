/******************************************************************************
 *	gs_stringmgr.h - A class which provides string handling functions
 *                   reimplemented by Xiphos
 *
 *  gs_stringmgr was mostly inspired by BTSringMgr from BibleTime
 *  Credits to the BibleTime Team (http://www.bibletime.info)
 *
 * Copyright (C) 2000-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation version 2.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.	See the GNU
 * General Public License for more details.
 */

#ifndef GS_STRINGMGR_H
#define GS_STRINGMGR_H

//Sword includes
#include <stringmgr.h>

SWORD_NAMESPACE_START
/* StringMgr is a way to provide UTf8 handling by the Sword frontend
 * Each platform, if it's up-to-date, should provide functions to handle
 * unicode and utf8. This class makes it possible to implement Unicode
 * support on the user-side and not in Sword itself.
 */
class GS_StringMgr : public StringMgr
{

      public:
	/* Converts the param to an upper case Utf8 string
	 *  @param text The text encoded in utf8 which should
	 *  be turned into an upper case string
	 *  @param max Only change max chars
	 */
	virtual char *upperUTF8(char *text, unsigned int max = 0) const;

      protected:
	/* Checks whether Utf8 support is available.
	 *  Override the function supportsUnicode() to tell whether
	 *  your implementation has utf8 support.
	 *  @return True if this implementation provides support for
	 *  Utf8 handling or false if just latin1 handling is available
	 */
	virtual bool supportsUnicode() const;
};

SWORD_NAMESPACE_END
#endif
