/*
 * GnomeSword Bible Study Tool
 * modulecache.hh - 
 *
 * Copyright (C) 2007-2008 GnomeSword Developer Team
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

//
// Class definition (w/inline implementation) for the module cache.
// => Provides a display-ready cache of HTML text, post-Sword hackery.
// Just wrap head/tail stanzas and per-current-verse colorization around it.
//
// Goal: Reach out to Sword with all its heavyweight display-related
// processing only when actually needed.  If the user is not changing his
// display choices, then don't re-process verses over and over.
// When display choices change, invalidate (or destroy) and start over.
// Result: We spend CPU when we need it, and not gratuitously.
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gnome.h>
#include <glib.h>
#include <assert.h>
#include "main/global_ops.hh"

typedef unsigned short int uint16_t;

#ifdef __cplusplus

#include <map>

extern "C" {

namespace ModuleCache {

	// bitwise cache type indicators:
	// if flags change (=> new display choice), text becomes invalid.
	static const int Footnotes          = 0x001;
	static const int GreekAccents       = 0x002;
	static const int Headings           = 0x004;
	static const int HebrewCantillation = 0x008;
	static const int HebrewVowels       = 0x010;
	static const int Lemmas             = 0x020;
	static const int Morphology         = 0x040;
	static const int RedWordsOfChrist   = 0x080;
	static const int StrongsNumbers     = 0x100;
	static const int Xrefs              = 0x200;
	static const int Images             = 0x400;

	class CacheVerse {
	public:
		CacheVerse();
		CacheVerse(uint16_t flags,
			   const char *text,
			   const char *header = NULL);
		~CacheVerse();

		bool		TextIsValid();
		bool		HeaderIsValid();
		bool		CacheIsValid(uint16_t flags);

		uint16_t	GetFlags();
		void		SetFlags(uint16_t flags = 0);
		void		AddFlag(uint16_t flag);
		void		DelFlag(uint16_t flag);

		const char *	GetText();
		void		SetText(const char *text, uint16_t flags);
		void		AppendText(const char *text, uint16_t flags);

		const char *	GetHeader();
		void		SetHeader(const char *text);
		void		AppendHeader(const char *text);

		void		Invalidate();
		void		InvalidateHeader();

	private:
		char *		_text;
		char *		_header;
		uint16_t	_flags;		// bitmask
	};

	// In precision order.
	typedef std::map < int, CacheVerse   > CacheChapter;
	typedef std::map < int, CacheChapter > CacheBook;
	typedef std::map < int, CacheBook    > CacheModule;

	// Index by module name => Cache for that module.
	typedef std::map < const char *, CacheModule > CacheMap;

	// Textually:
	// CacheMap contains modules of books of chapters of verses,
	// subscriptable at any stage to get the subordinate content.

	// Similar concept for genbooks and lexdicts.
	typedef std::map < const char *, CacheVerse > BookCacheMap;
	// a "verse" is just a section entry, textually keyed.
}

// Lifecycle.

inline
ModuleCache::CacheVerse::
CacheVerse()
    : _text(NULL),
      _header(NULL),
      _flags(0)
{
	// just initializers preceding
}

inline
ModuleCache::CacheVerse::
CacheVerse(uint16_t flags, const char *text, const char *header)
    : _text(text     ? g_strdup(text)   : NULL),
      _header(header ? g_strdup(header) : NULL),
      _flags(flags)
{
	// just initializers preceding
}

inline
ModuleCache::CacheVerse::
~CacheVerse()
{
	if (_text)
		g_free(_text);
	if (_header)
		g_free(_header);
}

// Access.

inline
bool ModuleCache::CacheVerse::
TextIsValid()
{
	return _text != NULL;
}

inline
bool ModuleCache::CacheVerse::
HeaderIsValid()
{
	return _header != NULL;
}

inline
bool ModuleCache::CacheVerse::
CacheIsValid(uint16_t flags)
{
	return TextIsValid() && (flags == GetFlags());
}

inline
uint16_t ModuleCache::CacheVerse::
GetFlags()
{
	return _flags;
}

inline
void ModuleCache::CacheVerse::
SetFlags(uint16_t flags)
{
	_flags = flags;
}

inline
void ModuleCache::CacheVerse::
AddFlag(uint16_t flag)
{
	_flags |= flag;
}

inline
void ModuleCache::CacheVerse::
DelFlag(uint16_t flag)
{
	_flags &= ~flag;
}

inline
const char *ModuleCache::CacheVerse::
GetText()
{
	assert(_text);
	return _text;
}

inline
void ModuleCache::CacheVerse::
SetText(const char *text, uint16_t flags)
{
	if (_text)
		g_free(_text);
	_text  = g_strdup(text);
	_flags = flags;
	// we are setting from scratch: neutralize header.
	if (_header) {
		g_free(_header);
		_header = NULL;
	}
}

inline
void ModuleCache::CacheVerse::
AppendText(const char *text, uint16_t flags)
{
	if (_text) {
		char *new_text = g_strconcat(_text, text, NULL);
		g_free(_text);
		_text = new_text;
	} else
		_text  = g_strdup(text);
	_flags = flags;
}

// large, but seemingly valid, assumption...
// the almost-equivalent header routines ignore flags entirely:
// we depend on the idea that any time one assigns a header, within
// a few lines of straight line code we will also assign text+flags.
// also, a null header is legit.

inline
const char *ModuleCache::CacheVerse::
GetHeader()
{
	return (_header ? _header : "");
}

inline
void ModuleCache::CacheVerse::
SetHeader(const char *text)
{
	if (_header)
		g_free(_header);
	_header  = g_strdup(text);
}

inline
void ModuleCache::CacheVerse::
AppendHeader(const char *text)
{
	if (_header) {
		char *new_text = g_strconcat(_header, text, NULL);
		g_free(_header);
		_header = new_text;
	} else
		_header  = g_strdup(text);
}

inline
void ModuleCache::CacheVerse::
Invalidate()
{
	_flags = 0;
	if (_text)
		g_free(_text);
	if (_header)
		g_free(_header);
	_text = _header = NULL;
}

inline
void ModuleCache::CacheVerse::
InvalidateHeader()
{
	if (_header)
		g_free(_header);
	_header = NULL;
}

#endif	/* __cplusplus */

// namespace-free routines.

// utility function to give our bitset.
uint16_t ConstructFlags(GLOBAL_OPS * ops);

// access from plain C to eliminate a module's entire cache (mod_mgr.c).
void ModuleCacheErase(const char *modname);
void BookModuleCacheErase(const char *modname);

#ifdef __cplusplus
}
#endif
