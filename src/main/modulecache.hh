/*
 * Xiphos Bible Study Tool
 * modulecache.hh -
 *
 * Copyright (C) 2007-2017 Xiphos Developer Team
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

#include <gtk/gtk.h>
#include <glib.h>
#include <assert.h>
#include "main/global_ops.hh"

#ifdef __cplusplus

#include <map>
#include <string>

namespace ModuleCache
{

// bitwise cache type indicators:
// if flags change (=> new display choice), text becomes invalid.

// options known to Sword.
static const int Footnotes = (1 << 0);
static const int GreekAccents = (1 << 1);
static const int Headings = (1 << 2);
static const int HebrewCantillation = (1 << 3);
static const int HebrewVowels = (1 << 4);
static const int Lemmas = (1 << 5);
static const int Morphology = (1 << 6);
static const int RedWordsOfChrist = (1 << 7);
static const int StrongsNumbers = (1 << 8);
static const int Xrefs = (1 << 9);
static const int Images = (1 << 10);
static const int RespectFontFaces = (1 << 11);
static const int VariantsAll = (1 << 12);
static const int VariantsPrimary = (1 << 13);
static const int VariantsSecondary = (1 << 14);
static const int Transliteration = (1 << 15);
static const int Xlit = (1 << 16);
static const int Enumerated = (1 << 17);
static const int Glosses = (1 << 18);
static const int MorphSegmentation = (1 << 19);
// 20-23 yet to be assigned.

// options known only within Xiphos.
static const int CommentaryChapter = (1 << 24);
static const int Doublespace = (1 << 25);
static const int XrefNoteNumbers = (1 << 26);

class CacheVerse
{
      public:
	CacheVerse();
	CacheVerse(int flags,
		   const char *text,
		   const char *header = NULL);
	~CacheVerse();

	bool TextIsValid();
	bool HeaderIsValid();
	bool CacheIsValid(int flags);

	int GetFlags();
	void SetFlags(int flags = 0);
	void AddFlag(int flag);
	void DelFlag(int flag);

	const char *GetText();
	void SetText(const char *text, int flags);
	void AppendText(const char *text, int flags);

	const char *GetHeader();
	void SetHeader(const char *text);
	void AppendHeader(const char *text);

	void Invalidate();
	void InvalidateHeader();

      private:
	char *_text;
	char *_header;
	int _flags; // bitmask
};

// In precision order.
typedef std::map<int, CacheVerse> CacheChapter;
typedef std::map<int, CacheChapter> CacheBook;
typedef std::map<int, CacheBook> CacheTestament;
typedef std::map<int, CacheTestament> CacheModule;

// Index by module name => Cache for that module.
typedef std::map<const std::string, CacheModule> CacheMap;

// Textually:
// CacheMap contains modules of testaments of books of chapters of
// verses, subscriptable at any stage to get the subordinate content.

// Similar concept for genbooks and lexdicts.
typedef std::map<const char *, CacheVerse> BookCacheMap;
// a "verse" is just a section entry, textually keyed.
}

// Lifecycle.

inline ModuleCache::CacheVerse::
    CacheVerse()
    : _text(NULL),
      _header(NULL),
      _flags(0)
{
	// just initializers preceding
}

inline ModuleCache::CacheVerse::
    CacheVerse(int flags, const char *text, const char *header)
    : _text(text ? g_strdup(text) : NULL),
      _header(header ? g_strdup(header) : NULL),
      _flags(flags)
{
	// just initializers preceding
}

inline ModuleCache::CacheVerse::
    ~CacheVerse()
{
	if (_text)
		g_free(_text);
	if (_header)
		g_free(_header);
}

// Access.

inline bool ModuleCache::CacheVerse::
    TextIsValid()
{
	return _text != NULL;
}

inline bool ModuleCache::CacheVerse::
    HeaderIsValid()
{
	return _header != NULL;
}

inline bool ModuleCache::CacheVerse::
    CacheIsValid(int flags)
{
	return TextIsValid() && (flags == GetFlags());
}

inline int ModuleCache::CacheVerse::
    GetFlags()
{
	return _flags;
}

inline void ModuleCache::CacheVerse::
    SetFlags(int flags)
{
	_flags = flags;
}

inline void ModuleCache::CacheVerse::
    AddFlag(int flag)
{
	_flags |= flag;
}

inline void ModuleCache::CacheVerse::
    DelFlag(int flag)
{
	_flags &= ~flag;
}

inline const char *ModuleCache::CacheVerse::
    GetText()
{
	assert(_text);
	return _text;
}

inline void ModuleCache::CacheVerse::
    SetText(const char *text, int flags)
{
	if (_text)
		g_free(_text);
	_text = g_strdup(text);
	_flags = flags;
	// we are setting from scratch: neutralize header.
	if (_header) {
		g_free(_header);
		_header = NULL;
	}
}

inline void ModuleCache::CacheVerse::
    AppendText(const char *text, int flags)
{
	if (_text) {
		char *new_text = g_strconcat(_text, text, NULL);
		g_free(_text);
		_text = new_text;
	} else
		_text = g_strdup(text);
	_flags = flags;
}

// large, but seemingly valid, assumption...
// the almost-equivalent header routines ignore flags entirely:
// we depend on the idea that any time one assigns a header, within
// a few lines of straight line code we will also assign text+flags.
// also, a null header is legit.

inline const char *ModuleCache::CacheVerse::
    GetHeader()
{
	return (_header ? _header : "");
}

inline void ModuleCache::CacheVerse::
    SetHeader(const char *text)
{
	if (_header)
		g_free(_header);
	_header = g_strdup(text);
}

inline void ModuleCache::CacheVerse::
    AppendHeader(const char *text)
{
	if (_header) {
		char *new_text = g_strconcat(_header, text, NULL);
		g_free(_header);
		_header = new_text;
	} else
		_header = g_strdup(text);
}

inline void ModuleCache::CacheVerse::
    Invalidate()
{
	_flags = 0;
	if (_text)
		g_free(_text);
	if (_header)
		g_free(_header);
	_text = _header = NULL;
}

inline void ModuleCache::CacheVerse::
    InvalidateHeader()
{
	if (_header)
		g_free(_header);
	_header = NULL;
}

extern "C" {
#endif /* __cplusplus */

// namespace-free routines.

// utility function to give our bitset.
int ConstructFlags(GLOBAL_OPS *ops);

// access from plain C to eliminate a module's entire cache (mod_mgr.c).
void ModuleCacheErase(const char *modname);
void BookModuleCacheErase(const char *modname);

#ifdef __cplusplus
}
#endif
