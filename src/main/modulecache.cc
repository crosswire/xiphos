/*
 * Xiphos Bible Study Tool
 * modulecache.cc -
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
// Class implementation for the module cache.
//

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include "modulecache.hh"

using namespace ModuleCache;

ModuleCache::CacheMap ModuleMap;	 // exactly one, always in existence.
ModuleCache::BookCacheMap BookModuleMap; // (we may destroy modules within.)

// The entire class is implemented inline in .hh.
// Needed here: Just the namespace-less functions.

// utility function to get a simple bitset.
int
ConstructFlags(GLOBAL_OPS *ops)
{
	int flags = 0;
	flags |= (ops->footnotes ? Footnotes : 0);
	flags |= (ops->greekaccents ? GreekAccents : 0);
	flags |= (ops->headings ? Headings : 0);
	flags |= (ops->hebrewcant ? HebrewCantillation : 0);
	flags |= (ops->hebrewpoints ? HebrewVowels : 0);
	flags |= (ops->lemmas ? Lemmas : 0);
	flags |= (ops->morphs ? Morphology : 0);
	flags |= (ops->words_in_red ? RedWordsOfChrist : 0);
	flags |= (ops->strongs ? StrongsNumbers : 0);
	flags |= (ops->scripturerefs ? Xrefs : 0);
	flags |= (ops->image_content ? Images : 0);
	flags |= (ops->respect_font_faces ? RespectFontFaces : 0);
	flags |= (ops->variants_all ? VariantsAll : 0);
	flags |= (ops->variants_primary ? VariantsPrimary : 0);
	flags |= (ops->variants_secondary ? VariantsSecondary : 0);
	flags |= (ops->transliteration ? Transliteration : 0);
	flags |= (ops->xlit ? Xlit : 0);
	flags |= (ops->enumerated ? Enumerated : 0);
	flags |= (ops->glosses ? Glosses : 0);
	flags |= (ops->morphseg ? MorphSegmentation : 0);

	flags |= (ops->commentary_by_chapter ? CommentaryChapter : 0);
	flags |= (ops->doublespace ? Doublespace : 0);
	flags |= (ops->xrefnotenumbers ? XrefNoteNumbers : 0);

	return flags;
}

// when de-installing, destroy module's cache.
void
ModuleCacheErase(const char *modname)
{
	CacheMap::iterator itr = ModuleMap.find(modname);

	if (itr != ModuleMap.end())
		ModuleMap.erase(itr);
	// or just ModuleMap.erase(modname), i guess.
}

#if 0
// unused at this time -- we don't cache book content.
void
BookModuleCacheErase(const char *modname)
{
	BookCacheMap::iterator itr = BookModuleMap.find(modname);

	if (itr != BookModuleMap.end())
		BookModuleMap.erase(itr);
}
#endif
