/*
 * Xiphos Bible Study Tool
 * display.cc -
 *
 * Copyright (C) 2000-2010 Xiphos Developer Team
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

#include <glib.h>

#include <osishtmlhref.h>
#include <osisvariants.h>
#include <thmlvariants.h>
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

#include <regex.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#ifdef USE_GTKMOZEMBED
# ifdef PACKAGE_BUGREPORT
#  undef PACKAGE_BUGREPORT
# endif
# ifdef PACKAGE_NAME
#  undef PACKAGE_NAME
# endif
# ifdef PACKAGE_STRING
#  undef PACKAGE_STRING
# endif
# ifdef PACKAGE_TARNAME
#  undef PACKAGE_TARNAME
# endif
# ifdef PACKAGE_VERSION
#  undef PACKAGE_VERSION
# endif
#endif /* USE_GTKMOZEMBED */

#include "main/display.hh"
#include "main/settings.h"
#include "main/global_ops.hh"
#include "main/sword.h"
#include "main/modulecache.hh"
#include "main/xml.h"

#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gui/dialog.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

// for one-time content rendering.
extern ModuleCache::CacheMap ModuleMap;

// for tracking personal annotations.
typedef struct {
    gchar   *module;
    gchar   *book;
    int     chapter;
    int     verse;
    GString *annotation;
} marked_element;

typedef std::map <int, marked_element *> MC;
MC marked_cache;

gchar *marked_cache_modname = NULL, *marked_cache_book = NULL;
int marked_cache_chapter = -1;

int footnote, xref;

#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><STYLE type=\"text/css\"><!-- A { text-decoration:none } %s --></STYLE></head>"

// CSS style blocks to control blocked strongs+morph output
// BOTH is when the user wants to see both types of markup.
// ONE is when he wants one or the other, so we can use a single
// specification which overlays both on top of one another.
#define CSS_BLOCK_BOTH \
" *        { line-height: 3.8em; }" \
" .word    { position: relative; top:  0.0em; left: 0 }" \
" .strongs { position: absolute; top:  0.3em; left: 0; z-index: 2; height: 1em }" \
" .morph   { position: absolute; top:  1.2em; left: 0; z-index: 1 }"
#define CSS_BLOCK_ONE \
" *        { line-height: 2.7em; }" \
" .word    { position: relative; top:  0.0em; left: 0 }" \
" .strongs { position: absolute; top:  0.8em; left: 0 }" \
" .morph   { position: absolute; top:  0.8em; left: 0 }"

#define DOUBLE_SPACE " * { line-height: 2em ! important; }"

using namespace sword;
using namespace std;

//
// user annotation cache filling.
//

#define	NUM_REPLACE	4

struct replace {
    gchar c;
    gchar *s;
} replacement[NUM_REPLACE] = {
    // < and > must be first.
    { '<',  (gchar *)"&lt;"   },
    { '>',  (gchar *)"&gt;"   },
    { '\n', (gchar *)"<br />" },
#ifdef USE_GTKMOZEMBED
    { '"',  (gchar *)"&quot;" },
#else
    { '"',  (gchar *)"'" },
#endif /* !USE_GTKMOZEMBED */
};

void
marked_cache_fill(gchar *modname, gchar *key)
{
	gchar *s, *t, *err, *mhold;
	char *key_book = g_strdup(main_get_osisref_from_key((const char *)modname,
							    (const char *)key));
	int key_chapter; //, key_verse;

	// free the old cache.  first free contents, then the map itself.
	MC::iterator it;
	for (it = marked_cache.begin();
	     it != marked_cache.end();
	     ++it) {
		marked_element *e = (*it).second;
		g_free(e->module);
		g_string_free(e->annotation, TRUE);
		delete e;
	}
	marked_cache.clear();

	// tear apart the key (e.g. "Gen.1.1").
	*(s = strrchr(key_book, '.')) = '\0';
	*(t = strrchr(key_book, '.')) = '\0';
	key_chapter = atoi(t+1);
	// key_verse   = atoi(s+1);

	// remember exactly what chapter this cache is for
	g_free(marked_cache_modname);
	g_free(marked_cache_book);
	marked_cache_modname = g_strdup(modname);
	marked_cache_book    = g_strdup(key_book);
	marked_cache_chapter = key_chapter;

	// load up the annotation content
	marked_element *e = new marked_element;
    	if (xml_set_section_ptr("osisrefmarkedverses") && xml_get_label()) {
		do {
			e->module = xml_get_label();
			s = xml_get_list();
			e->annotation = g_string_new(s);
			g_free(s);

			gchar *m = e->module;
			mhold = g_strdup(m);

			// tear apart "NASB Gen.1.1"
			if ((s = strrchr(m, '.')) == NULL)	// rightmost dot
				goto fail;
			*s = '\0';
			if ((t = strrchr(m, '.')) == NULL)	// chapter:verse
				goto fail;
			*t = '\0';
			e->chapter = atoi(t+1);
			e->verse   = atoi(s+1);
			if ((s = strchr(m, ' ')) == NULL)	// leftmost space
				goto fail;
			*(s++) = '\0';
			e->book = s;
			// now properly delimited: module & book, plus numeric c:v.

			g_free(mhold);

			// for fast reference: is this annotation relevant?
			if ((key_chapter != e->chapter) ||
			    (*m && (strcasecmp(m, modname) != 0)) ||
			    (strcasecmp(e->book, key_book) != 0)) {
				// junk: re-use same element in next loop.
				g_free(e->module);
				g_string_free(e->annotation, TRUE);
			} else {
				// replace embedded badness characters.
				for (int i = 0; i < NUM_REPLACE; ++i) {
					for (s = strchr(e->annotation->str, replacement[i].c);
					     s;
					     s = strchr(e->annotation->str, replacement[i].c)) {
						(void) g_string_erase(e->annotation,
								      s - (e->annotation->str), 1);
						(void) g_string_insert(e->annotation,
								       s - (e->annotation->str),
								       replacement[i].s);
					}
				}
				// valid: insert + get fresh one to work with.
				marked_cache[e->verse] = e;
				e = new marked_element;
			}
		} while (xml_next_item() && xml_get_label());
		// remove extra element that we necessarily have at loop's end.
		delete e;
	}
	g_free(key_book);
	return;

fail:
	err = g_strdup_printf(_("Improperly encoded personal annotation label:\n'%s'"),
			      mhold);
	gui_generic_warning(err);
	g_free(err);
	g_free(key_book);
	return;
}

//
// user annotation cache checking: is "this verse" annotated?
//
marked_element *
marked_cache_check(int thisVerse)
{
	MC::iterator it = marked_cache.find(thisVerse);
	if (it != marked_cache.end())
		return (*it).second;
	return NULL;
}

//
// utility function to blank `<img src="foo.jpg" />' content from text.
//
void
ClearImages(gchar *text)
{
	gchar *s, *t;

	for (s = strstr(text, "<img "); s; s = strstr(s, "<img ")) {
		if ((t = strchr(s+4, '>'))) {
			while (s <= t)
				*(s++) = ' ';
		} else {
			GS_message(("ClearImages: no img end: %s\n", s));
			return;
		}
	}
}

//
// utility function to blank `<font face="..." />' content from text.
//
void
ClearFontFaces(gchar *text)
{
	gchar *s, *t;

	// huge assumption: no nested <font> specs: <font face="">
	// is never followed by another <font anything> before </font>.
	for (s = strstr(text, "<font face=\"Galax"); s; s = strstr(s, "<font face=\"Galax")) {
		if ((t = strchr(s+15, '>'))) {
			while (s <= t)
				*(s++) = ' ';
			s = strstr(s, "</font>");
			t = s + 6;
			while (s <= t)
				*(s++) = ' ';
		} else {
			GS_message(("ClearFontFaces: no font end: %s\n", s));
			return;
		}
	}
}

//
// utility function for block_render() below.
// having a word + annotation in hand, stuff them into the buffer.
// span class names are from CSS_BLOCK macros.
// we garbage-collect here so block_render doesn't have to.
//

#ifdef USE_GTKMOZEMBED
#define	ALIGN_WORD	"&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"
#else
#define	ALIGN_WORD	" "	// gtkhtml3 has no alignment need.
#endif /* USE_GTKMOZEMBED */

void
block_dump(SWBuf& rendered,
	   const char **word,
	   const char **strongs,
	   const char **morph)
{
#ifdef USE_GTKMOZEMBED
	int wlen;
#endif
	int slen, mlen, min_length;
	char *s, *s0, *t;

	// unannotated words need no help.
	if (*word && (*strongs == NULL) && (*morph == NULL)) {
                rendered += "<span class=\"word\">";
		rendered += *word;
                rendered += "</span>";
		g_free((char *)*word);
		*word = NULL;
		rendered += " ";
		return;
	}

	if (*word == NULL) {
		// we need to cobble up something to take the place of
		// a word, in order that the strongs/morph not overlay.
		*word = g_strdup(ALIGN_WORD);
	}

	rendered += "<span class=\"word\">";
	if (*strongs) {
		s = g_strrstr(*strongs, "</a>");
		*s = '\0';
		t = (char*)strrchr(*strongs, '>') + 1;
		// correct weird NASB lexicon references.
		if ((s0 = (char*)strchr(*strongs, '!'))) {
			do {
				*s0 = *(s0+1);
				++s0;
			} while (*s0 != '"');
			*s0 = ' ';
		}
		*s = '<';
		slen = s - t;
#ifdef USE_GTKMOZEMBED
		s = (char*)strstr(*strongs, "&lt;");
		*s = *(s+1) = *(s+2) = *(s+3) = ' ';
		s = strstr(s, "&gt;");
		*s = *(s+1) = *(s+2) = *(s+3) = ' ';
#endif /* USE_GTKMOZEMBED */

		// gross hack needed to handle new class="..." in sword -r2512.
		if ((s = (char*)strstr(*strongs, " class=\"strongs\">"))) {
			memcpy(s, ">                ", 17);
			if ((s = (char*)strstr(s, " class=\"strongs\">")))
				memcpy(s, ">                ", 17);
		}
	} else
		slen = 0;
	if (*morph) {
		s = s0 = (char*)g_strrstr(*morph, "\">") + 2;
		t = strchr(s, '<');
		for (/* */; s < t; ++s)
			if (isupper(*s))
				*s = tolower(*s);
		for (s = strchr(s0, ' '); s && (s < t); s = strchr(s, ' '))
			*s = '-';
		s = g_strrstr(*morph, "</a>");
		*s = '\0';
		t = (char*)strrchr(*morph, '>') + 1;
		*s = '<';
		mlen = s - t;
#ifdef USE_GTKMOZEMBED
		s = (char*)strchr(*morph, '(');
		*s = ' ';
		s = strrchr(s, ')');
		*s = ' ';
#endif /* USE_GTKMOZEMBED */

		// gross hack needed to handle new class="..." in sword -r2512.
		if ((s = (char*)strstr(*morph, " class=\"morph\">"))) {
			memcpy(s, ">              ", 15);
			if ((s = (char*)strstr(s, " class=\"morph\">")))
				memcpy(s, ">              ", 15);
		}
	} else
		mlen = 0;
	min_length = 2 + max(slen, mlen);

	rendered += *word;
#ifdef USE_GTKMOZEMBED
	for (wlen = strlen(*word); wlen <= min_length; ++wlen)
		rendered += "&nbsp;";
#endif /* USE_GTKMOZEMBED */

	g_free((char *)*word);
	*word = NULL;

	rendered += "<span class=\"strongs\">";
	rendered += (*strongs ? *strongs : "&nbsp;");
	rendered += "</span>";
	if (*strongs) g_free((char *)*strongs);
	*strongs = NULL;

	rendered += "<span class=\"morph\">";
	rendered += (*morph ? *morph : "&nbsp;");
	rendered += "</span>";
	if (*morph) g_free((char *)*morph);
	*morph = NULL;

	rendered += "</span> <span class=\"word\">&nbsp;</span>";
}

//
// re-process a block of text so as to envelope its strong's and morph
// references in <span> blocks which will be interpreted by the CSS
// directives to put each ref immediately below the word it annotates.
// this keeps the text linearly readable while providing annotation.
//
// secondary interface.
// text destination is provided, ready to go.
// this means we are able to recurse when needed.

#ifdef USE_GTKMOZEMBED
#define EMPTY_WORD	""
#else
#define EMPTY_WORD	"&nbsp;"
#endif /* USE_GTKMOZEMBED */

void
block_render_secondary(const char *text,
		       SWBuf& rendered)
{
	const char *word    = NULL,
		   *strongs = NULL,
		   *morph   = NULL;
	int bracket;
	const char *s;
	char *t, *u;

	for (s = text; *s; ++s) {
		switch (*s) {
		case ' ':
		case '\t':
			break;

		case '<':
			// <token> causes a lot of grief, because we must
			// keep it bound with its </token> terminator,
			// esp. because anchors contain <small></small>. *sigh*
			// i believe we see only anchors + font switches here.
			if ((((*(s+1) == 'a') || (*(s+1) == 'A')) && (*(s+2) == ' ')) ||
			    (((*(s+1) == 'b') || (*(s+1) == 'B') ||
			      (*(s+1) == 'i') || (*(s+1) == 'I') ||
			      (*(s+1) == 'u') || (*(s+1) == 'U')) && (*(s+2) == '>'))) {
				if (word)
					block_dump(rendered, &word, &strongs, &morph);

				static char end[5] = "</X>";
				end[2] = *(s+1);
			again:
				if ((t = strstr((char *)s, end)) == NULL) {
					GS_warning(("No %s in %s\n", end, s));
					break;
                                }

				// yet another nightmare:
				// if the markup results in e.g. doubled <i>
				// (bogus "<hi><hi>word</hi></hi>"),
				// then we will mis-assess termination.
				// so we search for the same markup embedded within.
				// if we find an internal set, we just wipe it out.
				static char embedded[4] = "<X>";
				embedded[1] = *(s+1);
				if ((u = g_strstr_len(s+3, t-(s+3), embedded))) {
					*u = *(u+1) = *(u+2) =
					    *t = *(t+1) = *(t+2) = *(t+3) = ' ';
					goto again;	// yuck, yes, i know...
				}

				// nasb eph 5:31: whole verse is an italicized
				// quotation of gen 2:24...containing strongs.
				// in event of font switch, output bare switch
				// controls but recurse on content within.
				if ((*(s+1) == 'a') || (*(s+1) == 'A')) {
					// <a href> anchor -- uninteresting.
					t += 4;
					word = g_strndup(s, t-s);
					s = t-1;
				} else {
					// font switch nightmare.
					word = g_strndup(s, 3);
					rendered += word;
					g_free((char *)word);
					word = g_strndup(s+3, t-(s+3));
					block_render_secondary(word, rendered);
					g_free((char *)word);
					word = NULL;
					rendered += end;
					s = t+3;
				}
				break;
			} else if (!strncmp(s+1, "small>", 6)) {
				// strongs and morph are bounded by "<small>".
				if ((t = strstr((char *)s, "</small>")) == NULL) {
					GS_warning(("No </small> in %s\n", s));
					break;
				}
				t += 8;
				// this is very dicey -- phenomenological/
				// observable about Sword filters' provision.
				// strongs: "<em>&lt;...&gt;</em>"
				// morph:   "<em>(...)</em>"
				// if Sword ever changes this, we're dead.
				const char *u = s+11;
				while ((*u != '(') && (*u != '&'))
					++u;	// it has to be one or the other.
				if (*u == '(') {
					if (morph) {
						block_dump(rendered, &word, &strongs, &morph);
						word = g_strdup(EMPTY_WORD);
					}
					morph   = g_strndup(s, t-s);
				} else {
					if (strongs) {
						block_dump(rendered, &word, &strongs, &morph);
						word = g_strdup(EMPTY_WORD);
					}
					strongs = g_strndup(s, t-s);
				}
				s = t-1;
				break;
			}
			// ...fall through to ordinary text...
			// (includes other "<>"-bounded markup.)

		default:
			if (word)
				block_dump(rendered, &word, &strongs, &morph);

			// here's an unfortunate problem.  consider:
			// L<font size="-1">ORD</font> followed by strongs.
			// the SPC breaks it into 2 "words", very badly.
			// we need to track <> use to get it as *1* word,
			// before we capture the strongs, or just the latter
			// half of it ("size=...") goes inside the <span>.
			bracket = 0;
			word = s;
			do {
				while ((*s == ' ') || (*s == '\t'))
					s++;
				for ( /* */ ;
				     *s && (*s != ' ') && (*s != '\t');
				     ++s) {
					if (*s == '<') {
						if (!strncmp(s+1, "small>", 6) ||
						    ((*(s+1) == 'a') &&
						     (*(s+2) == ' '))) {
							// "break 2;"
							goto outword;
						}
						bracket++;
					}
					else if (*s == '>')
						bracket--;
					assert(bracket >= 0);
				}
			} while (bracket != 0);
		    outword:
			word = g_strndup(word, s-word);
			s--;
		}
	}
	if (word || strongs || morph)
		block_dump(rendered, &word, &strongs, &morph);
}

// entry interface.
// initializes for secondary interface.
const char *
block_render(const char *text)
{
	static SWBuf rendered;

	rendered = "";
	block_render_secondary(text, rendered);
	return rendered.c_str();
}

//
// in-place removal of inconvenient-to-the-user content, and note/xref marking.
//
GString *
CleanupContent(GString *text,
	       GLOBAL_OPS *ops,
	       char *name,
	       bool reset = true)
{
	if (ops->image_content == 0)
		ClearImages((gchar *)text->str);
	else if ((ops->image_content == -1) &&	// "unknown"
		 (strcasestr(text->str, "<img ") != NULL)) {
		ops->image_content = 1;		// now known.
		main_save_module_options(name, "Image Content", 1, ops->dialog);
	}
	if (ops->respect_font_faces == 0)
		ClearFontFaces((gchar *)text->str);
	else if ((ops->respect_font_faces == -1) &&	// "unknown"
		 (strcasestr(text->str, "<font face=\"Galax") != NULL)) {
		ops->respect_font_faces = 1;	// now known.
		main_save_module_options(name, "Respect Font Faces", 1, ops->dialog);
	}

	gint pos;
	gchar value[50], *reported, *notetype, *s = text->str;

	// test for any 'n="X"' content.  if so, use it directly.
	if ((reported = backend->get_entry_attribute("Footnote", "1", "n"))) {
		g_free(reported);		// dispose of test junk.

		// operate on notes+xrefs together: both are "Footnote".
		while ((s = strchr(s, '*'))) {
			if ((*(s+1) != 'n') && (*(s+1) != 'x')) {
				s++;		// that's not our marker.
				continue;
			}

		again:
			g_snprintf(value, 5, "%d", ++footnote);
			if ((reported = backend->get_entry_attribute("Footnote", value, "n"))) {

				notetype = backend->get_entry_attribute("Footnote", value, "type");
				if (notetype &&
				    (((*(s+1) == 'n') && !strcmp(notetype, "crossReference")) ||
				     ((*(s+1) == 'x') && strcmp(notetype, "crossReference")))) {
					// only 1 of notes & xrefs is active.
					g_free(notetype);
					g_free(reported);
					goto again;
				}

				g_free(notetype);
				pos = s-(text->str)+2;
				text = g_string_insert(text, pos, reported);
				g_free(reported);
				s = text->str + pos + 1;
			}
		}

		// naïveté: if any verse uses 'n=', all do: reset for next verse.
		if (reset)
			footnote = 0;
	}

	// otherwise we simply count notes & xrefs through the verse.
	else {
		while ((s = strstr(s, "*n"))) {
			g_snprintf(value, 5, "%d", ++footnote);
			pos = s-(text->str)+2;
			text = g_string_insert(text, pos, value);
			s = text->str + pos + 1;
		}

		s = text->str;
		while ((s = strstr(s, "*x"))) {
			g_snprintf(value, 5, "%d", ++xref);
			pos = s-(text->str)+2;
			text = g_string_insert(text, pos, value);
			s = text->str + pos + 1;
		}
	}

	return text;
}

//
// utility function to fill headers from verses.
//
void
CacheHeader(ModuleCache::CacheVerse& cVerse,
	    SWModule& mod,
	    GLOBAL_OPS *ops, BackEnd *be)
{
	int x = 0;
	gchar heading[8];
	const gchar *preverse, *preverse2;
	GString *text = g_string_new("");

	cVerse.SetHeader("");

	sprintf(heading, "%d", x);
	while ((preverse = be->get_entry_attribute("Heading", "Preverse",
							heading)) != NULL) {
		preverse2 = mod.RenderText(preverse);
		g_string_printf(text,
				"<br/><b>%s</b><br/><br/>",
				(((ops->strongs || ops->lemmas) ||
				  ops->morphs)
				 ? block_render(preverse2)
				 : preverse2));
		text = CleanupContent(text, ops, mod.Name(), false);

		cVerse.AppendHeader(text->str);
		// g_free((gchar *)preverse2);
		g_free((gchar *)preverse);
		++x;
		sprintf(heading, "%d", x);
	}
	g_string_free(text, TRUE);
}

void
set_morph_order(SWModule& imodule)
{
	for (FilterList::const_iterator it =
		 imodule.getRenderFilters().begin();
	     it != imodule.getRenderFilters().end();
	     it++) {
		OSISHTMLHREF *f = dynamic_cast<OSISHTMLHREF *>(*it);
		if (f)
			f->setMorphFirst();
	}
}

//
// display of commentary by chapter.
//
char
GTKEntryDisp::DisplayByChapter(SWModule &imodule)
{
	imodule.setSkipConsecutiveLinks(true);

	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	gchar *buf, *vbuf, *num;
	char *ModuleName = imodule.Name();
	GString *rework;			// for image size analysis rework.
	footnote = xref = 0;

	// we come into this routine with swbuf init'd with
	// boilerplate html startup, plus ops and mf ready.

	cache_flags = ConstructFlags(ops);
	is_rtol = main_is_mod_rtol(ModuleName);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
			     ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);

	// open the table.
	if (settings.showversenum) {
		buf = g_strdup_printf("<font face=\"%s\"><table border=\"0\""
				      " cellpadding=\"5\" cellspacing=\"0\">",
				      ((mf->old_font) ? mf->old_font : ""));
		swbuf.append(buf);
		g_free(buf);
	}

	for (key->Verse(1);
	     (key->Book()    == curBook)    &&
	     (key->Chapter() == curChapter) &&
	     !imodule.Error();
	     imodule++) {

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		// use the module cache rather than re-accessing Sword.
		// but editable personal commentaries don't use the cache.
		if (!cVerse.CacheIsValid(cache_flags) && 
		    (backend->module_type(imodule.Name()) != PERCOM_TYPE)) {
			rework = g_string_new(strongs_or_morph
					      ? block_render(imodule.RenderText())
					      : imodule.RenderText());
			rework = CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework->str, cache_flags);
		} else {
			if (backend->module_type(imodule.Name()) == PERCOM_TYPE)
				rework = g_string_new(strongs_or_morph
						      ? block_render(imodule.getRawEntry())
						      : imodule.getRawEntry());
			else
				rework = g_string_new(cVerse.GetText());
		}
		swbuf.append("<tr>");

		// insert verse numbers
		num = main_format_number(key->Verse());
		vbuf = g_strdup_printf((settings.showversenum
					? "<td valign=\"top\" align=\"right\">"
					"<a name=\"%d\" href=\"sword:///%s\">"
					"<font size=\"%+d\" color=\"%s\">%s</font></a></td>"
					: "<p/><a name=\"%d\"> </a>"),
				       key->Verse(),
				       (char*)key->getText(),
				       settings.verse_num_font_size + settings.base_font_size,
				       settings.bible_verse_num_color,
				       num);
		g_free(num);
		if (!is_rtol)
			swbuf.append(vbuf);

		if (settings.showversenum) {
			buf = g_strdup_printf("<td><font size=\"%+d\">",
					      ((mf->old_font_size)
					       ? atoi(mf->old_font_size) + settings.base_font_size
					       : settings.base_font_size));
			swbuf.append(buf);
			g_free(buf);
		}
		swbuf.append(settings.imageresize
			     ? AnalyzeForImageSize(rework->str,
						   GDK_WINDOW(GTK_WINDOW(gtkText)))
			     : rework->str /* left as-is */);
		if (settings.showversenum)
			swbuf.append("</font></td>");

		if (is_rtol)
			swbuf.append(vbuf);
		g_free(vbuf);

		swbuf.append("</tr>");
	}

	// close the table.
	if (settings.showversenum)
		swbuf.append("</table></font>");
	swbuf.append("</div></font></body></html>");

	buf = g_strdup_printf("%d", curVerse);
	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, buf);
	g_free(buf);

	free_font(mf);
	mf = NULL;
	g_free(ops);
	ops = NULL;
	return 0;
}

//
// general display of entries: commentary, genbook, lexdict
//
char
GTKEntryDisp::Display(SWModule &imodule)
{
#ifdef USE_GTKMOZEMBED
	if (!gtk_widget_get_realized(GTK_WIDGET(gtkText)))
		gtk_widget_realize(gtkText);
#endif

	gchar *buf;
	mf = get_font(imodule.Name());
	swbuf = "";
	footnote = xref = 0;

	ops = main_new_globals(imodule.Name(),0);

	GString *rework;			// for image size analysis rework.

	(const char *)imodule;	// snap to entry
	main_set_global_options(ops);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
			     ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);

	buf = g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">"
			      "[<a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\">"
			      "<font color=\"%s\">"
			      "*%s*</font></a>]<br/>",
			      (strongs_and_morph		// both
			       ? CSS_BLOCK_BOTH
			       : (strongs_or_morph		// either
				  ? CSS_BLOCK_ONE
				  : (ops->doublespace		// neither
				     ? DOUBLE_SPACE
				     : ""))),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      ((mf->old_font) ? mf->old_font : ""),
			      ((mf->old_font_size)
			       ? atoi(mf->old_font_size) + settings.base_font_size
			       : settings.base_font_size),
			      imodule.Description(),
			      imodule.Name(),
			      settings.bible_verse_num_color,
			      imodule.Name());
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
			       ? "rtl"
			       : "ltr"));
	//
	// the rest of this routine is irrelevant if we are
	// instead heading off to show a whole chapter
	// (this option can be enabled only in commentaries.)
	//
	if (ops->commentary_by_chapter)
		return DisplayByChapter(imodule);

	// we will use the module cache for regular commentaries,
	// which navigate/change a lot, whereas pers.comms, lexdicts,
	// and genbooks still do fresh access every time -- the nature
	// of those modules' use won't buy much with a module cache.

	// there is some unfortunate but unavoidable code duplication
	// for handling potential clearing of images, due to the
	// difference in how modules are being accessed.

	if (backend->module_type(imodule.Name()) == COMMENTARY_TYPE) {
		VerseKey *key = (VerseKey *)(SWKey *)imodule;
		cache_flags = ConstructFlags(ops);
		const char *ModuleName = imodule.Name();

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
					      ? block_render(imodule.RenderText())
					      : imodule.RenderText());
			rework = CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());

	} else {

		if ((backend->module_type(imodule.Name()) == PERCOM_TYPE) ||
		    (backend->module_type(imodule.Name()) == PRAYERLIST_TYPE))
			rework = g_string_new(strongs_or_morph
					      ? block_render(imodule.getRawEntry())
					      : imodule.getRawEntry());
		else
			rework = g_string_new(strongs_or_morph
					      ? block_render(imodule.RenderText())
					      : imodule.RenderText());
		rework = CleanupContent(rework, ops, imodule.Name());
	}

	swbuf.append(settings.imageresize
		     ? AnalyzeForImageSize(rework->str,
					   GDK_WINDOW(GTK_WINDOW(gtkText)))
		     : rework->str /* left as-is */);

	swbuf.append("</div></font></body></html>");

	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);

	free_font(mf);
	mf = NULL;
	g_free(ops);
	ops = NULL;
	return 0;
}

void
GTKChapDisp::getVerseBefore(SWModule &imodule)
{
	gchar *buf;
	char *num;

	sword::VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int chapter = key->Chapter();
	int curBook = key->Book();
	int curTest = key->Testament();
#if 0
	const char *ModuleName = imodule.Name();
#endif

	key->Verse(1);
	imodule--;

	if (imodule.Error()) {
		num = main_format_number(chapter);
		buf=g_strdup_printf("<div style=\"text-align: center\">"
				    "<p><b><font size=\"%+d\">%s</font></b></p>"
				    "<b>%s %s</b></div>",
				    1 + ((mf->old_font_size)
					 ? atoi(mf->old_font_size) +
					 settings.base_font_size
					 : settings.base_font_size),
				    imodule.Description(),
				    _("Chapter"),
				    num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);
	} else {

		if (strongs_and_morph)
			set_morph_order(imodule);

#if 0		// with footnote counting, we no longer cache before/after verses.
		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render((const char *)imodule)
					: (const char *)imodule),
				       cache_flags);
#else
		const gchar *text = (strongs_or_morph
				     ? block_render((const char *)imodule)
				     : (const char *)imodule);
#endif /* !0 */

		swbuf.appendFormatted("<div dir=%s>",
				      ((is_rtol && !ops->transliteration)
				       ? "rtl"
				       : "ltr"));

		num = main_format_number(key->Verse());
		buf=g_strdup_printf(settings.showversenum
				? "&nbsp; <a name=\"%d\" href=\"sword:///%s\">"
				  "<font size=\"%+d\" color=\"%s\">%s</font></a> "
				: "&nbsp; <a name=\"%d\"> </a>",
				0,
				(char*)key->getText(),
				((settings.versestyle)
				 ? settings.verse_num_font_size + settings.base_font_size
				 : settings.base_font_size - 2),
				settings.bible_verse_num_color,
				num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		num = main_format_number(chapter);
		buf=g_strdup_printf(
				"%s%s<br/><hr/><div style=\"text-align: center\"><b>%s %s</b></div>",
#if 0
				cVerse.GetText(),
#else
				text,
#endif
				// extra break when excess strongs/morph space.
				(strongs_or_morph ? "<br/>" : ""),
				_("Chapter"), num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		swbuf.append("</div>");

	}

	imodule++;

	if (!ops->headings)
		return;

	//
	// Display content at 0:0 and n:0.
	//
	char oldAutoNorm = key->AutoNormalize();
	key->AutoNormalize(0);

	for (int i = 0; i < 2; ++i) {
		// Get chapter 0 iff we're in chapter 1.
		if ((i == 0) && (chapter != 1))
			continue;

		key->Chapter(i*chapter);
		key->Verse(0);

#if 0		// with footnote counting, we no longer cache before/after verses.
		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render((const char *)imodule)
					: (const char *)imodule),
				       cache_flags);
		buf=g_strdup_printf("%s<br />", cVerse.GetText());
#else
		buf=g_strdup_printf("%s<br />", (strongs_or_morph
						 ? block_render((const char *)imodule)
						 : (const char *)imodule));
#endif /* !0 */
		swbuf.append(buf);
		g_free(buf);
	}

	key->AutoNormalize(oldAutoNorm);

	key->Book(curBook);
	key->Chapter(chapter);
	key->Verse(curVerse);
	key->Testament(curTest);
}

void
GTKChapDisp::getVerseAfter(SWModule &imodule)
{
	gchar *buf;
#if 0
	const char *ModuleName = imodule.Name();
#endif
	sword::VerseKey *key = (VerseKey *)(SWKey *)imodule;

	imodule++;
	if (imodule.Error()) {
		buf=g_strdup_printf(
			"%s<hr/><div style=\"text-align: center\"><p><b>%s</b></p></div>",
			// extra break when excess strongs/morph space.
			(strongs_or_morph ? "<br/><br/>" : ""),
			imodule.Description());
		swbuf.append(buf);
		g_free(buf);
	} else {
		imodule--;
		char *num = main_format_number(key->Chapter());

		buf=g_strdup_printf(
			"%s<hr/><div style=\"text-align: center\"><b>%s %s</b></div>",
			(strongs_or_morph ? "<br/><br/>" : ""),
			_("Chapter"), num);
		swbuf.append(buf);
		g_free(buf);
		g_free(num);

		swbuf.appendFormatted("<div dir=%s>",
				      ((is_rtol && !ops->transliteration)
				       ? "rtl"
				       : "ltr"));

		num = main_format_number(key->Verse());
		buf=g_strdup_printf(settings.showversenum
				? "&nbsp; <a name=\"%d\" href=\"sword:///%s\">"
				  "<font size=\"%+d\" color=\"%s\">%s</font></a> "
				: "&nbsp; <a name=\"%d\"> </a>",
				0,
				(char*)key->getText(),
				((settings.versestyle)
				 ? settings.verse_num_font_size + settings.base_font_size
				 : settings.base_font_size - 2),
				settings.bible_verse_num_color,
				num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		if (strongs_and_morph)
			set_morph_order(imodule);

#if 0		// with footnote counting, we no longer cache before/after verses.
		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render((const char *)imodule)
					: (const char *)imodule),
				       cache_flags);
		swbuf.append(cVerse.GetText());
#else
		swbuf.append((strongs_or_morph
			      ? block_render((const char *)imodule)
			      : (const char *)imodule));
#endif /* !0 */
		swbuf.append("</div>");
	}
}

char
GTKChapDisp::Display(SWModule &imodule)
{
        // following line ensures linked verses work correctly
        // it does not solve the problem of marking groups of verses (1-4), etc
        imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	gchar *buf;
	char *num;
	const gchar *paragraphMark = NULL;
	gboolean newparagraph = FALSE;
	GString *rework;			// for image size analysis rework.
	char *ModuleName = imodule.Name();
	ops = main_new_globals(ModuleName, 0);
	cache_flags = ConstructFlags(ops);
	marked_element *e = NULL;

	is_rtol = main_is_mod_rtol(ModuleName);
	mf = get_font(ModuleName);

	if (!gtk_widget_get_realized (GTK_WIDGET(gtkText))) return 0;

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
			     ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);

	// when strongs/morph are on, the anchor boundary must be smaller.
	// or if main window is too small to keep curverse in-pane.
	gint display_boundary = ((settings.gs_height < 500) ? 0 : (strongs_or_morph ? 1 : 2));

	// if we are no longer where annotations were current, re-load.
	if (strcasecmp(ModuleName,
		       (marked_cache_modname ? marked_cache_modname : "")) ||
	    strcasecmp(key->getBookAbbrev(), marked_cache_book) ||
	    (curChapter != marked_cache_chapter))
		marked_cache_fill(ModuleName, settings.currentverse);

	if (!strcmp(ModuleName, "KJV"))
		paragraphMark = "&para;&nbsp;";
	else
		paragraphMark = "";

	swbuf = "";
	footnote = xref = 0;

	buf=g_strdup_printf(HTML_START
			    "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			    "<font face=\"%s\" size=\"%+d\">",
			    // strongs & morph specs win over dblspc.
			    (strongs_and_morph		// both
			     ? CSS_BLOCK_BOTH
			     : (strongs_or_morph	// either
				? CSS_BLOCK_ONE
				: (ops->doublespace	// neither
				   ? DOUBLE_SPACE
				   : ""))),
			    settings.bible_bg_color,
			    settings.bible_text_color,
			    settings.link_color,
			    ((mf->old_font) ? mf->old_font : ""),
			    ((mf->old_font_size)
			     ? atoi(mf->old_font_size) + settings.base_font_size
			     : settings.base_font_size));
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
			       ? "rtl"
			       : "ltr"));

	main_set_global_options(ops);
	getVerseBefore(imodule);

	for (key->Verse(1);
	     (key->Book()    == curBook)    &&
	     (key->Chapter() == curChapter) &&
	     !imodule.Error();
	     imodule++) {

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.HeaderIsValid())
			CacheHeader(cVerse, imodule, ops, be);

		if (cache_flags & ModuleCache::Headings) {
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(cVerse.GetHeader(),
							   GDK_WINDOW(GTK_WINDOW(gtkText)))
				     : cVerse.GetHeader() /* left as-is */);
		} else
			cVerse.InvalidateHeader();

		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
					      ? block_render(imodule.RenderText())
					      : imodule.RenderText());
			rework = CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());

		// special contrasty highlighting
		if (((e = marked_cache_check(key->Verse())) &&
		     settings.annotate_highlight) ||
		    ((key->Verse() == curVerse) &&
		     settings.versehighlight)) {
			buf = g_strdup_printf(
#ifdef USE_GTKMOZEMBED
			    "<span style=\"background-color: %s\">"
#else
			    "<table bgcolor=\"%s\"><tr><td>"
#endif
			    "<font face=\"%s\" size=\"%+d\">",
			    ((settings.annotate_highlight && e)
			     ? settings.highlight_fg
			     : settings.highlight_bg),
			    ((mf->old_font) ? mf->old_font : ""),
			    ((mf->old_font_size)
			     ? atoi(mf->old_font_size) + settings.base_font_size
			     : settings.base_font_size));
			swbuf.append(buf);
			g_free(buf);
		}

		num = main_format_number(key->Verse());
		buf = g_strdup_printf(settings.showversenum
			? "&nbsp; <span class=\"word\"><a name=\"%d\" href=\"sword:///%s\">"
			  "<font size=\"%+d\" color=\"%s\">%s</font></a></span> "
			: "&nbsp; <a name=\"%d\"> </a>",
			key->Verse(),
			(char*)key->getText(),
			settings.verse_num_font_size + settings.base_font_size,
			(((settings.versehighlight && (key->Verse() == curVerse)) ||
			  (e && settings.annotate_highlight))
			 ? ((e && settings.annotate_highlight)
			    ? settings.highlight_bg
			    : settings.highlight_fg)
			 : settings.bible_verse_num_color),
			num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		// insert the userfootnote reference
		if (e) {
			buf = g_strdup_printf("<span class=\"word\">"
					      "<a href=\"passagestudy.jsp?action=showUserNote&"
					      "module=%s&passage=%s&value=%s\"><small>"
					      "<sup>*u</sup></small></a></span> ",
					      settings.MainWindowModule,
			                      (char*)key->getShortText(),
					      e->annotation->str);
			swbuf.append(buf);
			g_free(buf);
		}

		if ((key->Verse() == curVerse) || (e && settings.annotate_highlight)) {
			buf = g_strdup_printf("<font color=\"%s\">",
					      ((settings.versehighlight ||
						(e && settings.annotate_highlight))
					       ? ((e && settings.annotate_highlight)
						  ? settings.highlight_bg
						  : settings.highlight_fg)
					       : settings.currentverse_color));
			swbuf.append(buf);
			g_free(buf);
		}

		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			swbuf.append(paragraphMark);;
		}

#ifndef USE_GTKMOZEMBED
		// correct a highlight glitch: in poetry verses which end in
		// a forced line break, we must remove the break to prevent
		// the enclosing <table> from producing a double break.
		if ((settings.versehighlight ||
		     (e && settings.annotate_highlight)) &&	// doing <table> h/l.
		    !settings.versestyle &&			// paragraph format.
		    (key->Verse() == curVerse)) {
			GString *text = g_string_new(NULL);

			g_string_printf(text, "%s", rework->str);
			if (!strcmp(text->str + text->len - 6, "<br />")) {
				text->len -= 6;
				*(text->str + text->len) = '\0';
			}
			else if (!strcmp(text->str + text->len - 4, "<br/>")) {
				text->len -= 4;
				*(text->str + text->len) = '\0';
			}
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(text->str,
							   GDK_WINDOW(gtkText->window))
				     : text->str /* left as-is */);
			g_string_free(text, TRUE);
		} else
#endif /* USE_GTKMOZEMBED */
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(rework->str,
							   GDK_WINDOW(GTK_WINDOW(gtkText)))
				     : rework->str /* left as-is */);

		if (key->Verse() == curVerse) {
			swbuf.append("</font>");
			ReadAloud(curVerse, rework->str);
		}

		if (settings.versestyle) {
			if (strstr(rework->str, "<!p>")) {
				newparagraph = TRUE;
			} else {
				newparagraph = FALSE;
			}
			if ((key->Verse() != curVerse) ||
			    (!settings.versehighlight &&
			     (!e || !settings.annotate_highlight)))
				swbuf.append("<br/>");
		}

		// special contrasty highlighting
		if (((key->Verse() == curVerse) && settings.versehighlight) ||
		    (e && settings.annotate_highlight))
#ifdef USE_GTKMOZEMBED
			swbuf.append("</font></span>");
#else
			swbuf.append("</font></td></tr></table>");
#endif
	}

	getVerseAfter(imodule);

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);

	swbuf.append("</div></font></body></html>");

#ifdef USE_GTKMOZEMBED
	if (strongs_and_morph)
		buf = g_strdup_printf("%d", curVerse);
	else	/* this is not dangling: connects to following "if" */
#endif /* USE_GTKMOZEMBED */
	if (curVerse > display_boundary)
		buf = g_strdup_printf("%d", curVerse - display_boundary);
	else
		buf = NULL;
	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, buf);
	if (buf)
		g_free(buf);

	free_font(mf);
	mf = NULL;
	g_free(ops);
	ops = NULL;
	return 0;
}


//
// display of commentary by chapter.
//
char
DialogEntryDisp::DisplayByChapter(SWModule &imodule)
{
       imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	gchar *buf;
	char *ModuleName = imodule.Name();
	GString *rework;			// for image size analysis rework.
	footnote = xref = 0;

	// we come into this routine with swbuf init'd with
	// boilerplate html startup, plus ops and mf ready.

	cache_flags = ConstructFlags(ops);
	is_rtol = main_is_mod_rtol(ModuleName);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
			     ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
			       ? "rtl"
			       : "ltr"));

	for (key->Verse(1);
	     (key->Book()    == curBook)    &&
	     (key->Chapter() == curChapter) &&
	     !imodule.Error();
	     imodule++) {

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
					      ? block_render(imodule.RenderText())
					      : imodule.RenderText());
			rework = CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());
		buf = g_strdup_printf("<p /><a name=\"%d\"> </a>",
				      key->Verse());
		swbuf.append(buf);
		g_free(buf);
		swbuf.append(settings.imageresize
			     ? AnalyzeForImageSize(rework->str,
						   GDK_WINDOW(GTK_WINDOW(gtkText)))
			     : rework->str /* left as-is */);
	}

	swbuf.append("</div></font></body></html>");

	buf = g_strdup_printf("%d", curVerse);
	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, buf);
	g_free(buf);

	free_font(mf);
	mf = NULL;
	g_free(ops);
	ops = NULL;
	return 0;
}

char
DialogEntryDisp::Display(SWModule &imodule)
{
	swbuf = "";
	char *buf;
	mf = get_font(imodule.Name());
	ops = main_new_globals(imodule.Name(),1);
	main_dialog_set_global_options((BackEnd*)be, ops);
	GString *rework;			// for image size analysis rework.
	footnote = xref = 0;

	(const char *)imodule;	// snap to entry
	//main_set_global_options(ops);

	buf = g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">"
			      "<font color=\"%s\">"
			      "<a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font><br/>",
			      (ops->doublespace ? DOUBLE_SPACE : ""),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      ((mf->old_font) ? mf->old_font : ""),
			      ((mf->old_font_size)
			       ? atoi(mf->old_font_size) + settings.base_font_size
			       : settings.base_font_size),
			      settings.bible_verse_num_color,
			      imodule.Description(),
			      imodule.Name(),
			      imodule.Name());
	swbuf.append(buf);
	g_free(buf);

	//
	// the rest of this routine is irrelevant if we are
	// instead heading off to show a whole chapter
	//
	if (ops->commentary_by_chapter)
		return DisplayByChapter(imodule);

	if (be->module_type(imodule.Name()) == COMMENTARY_TYPE) {
		VerseKey *key = (VerseKey *)(SWKey *)imodule;
		cache_flags = ConstructFlags(ops);
		const char *ModuleName = imodule.Name();

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new((const char *)imodule);
			rework = CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());

	} else {

		if ((be->module_type(imodule.Name()) == PERCOM_TYPE) ||
		    (be->module_type(imodule.Name()) == PRAYERLIST_TYPE))
			rework = g_string_new(imodule.getRawEntry());
		else
			rework = g_string_new((const char *)imodule);
		rework = CleanupContent(rework, ops, imodule.Name());
	}

	swbuf.append(settings.imageresize
		     ? AnalyzeForImageSize(rework->str,
					   GDK_WINDOW(GTK_WINDOW(gtkText)))
		     : rework->str /* left as-is */);

	swbuf.append("</font></body></html>");

	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);

	free_font(mf);
	mf = NULL;
	g_free(ops);
	ops = NULL;
	return 0;
}


char
DialogChapDisp::Display(SWModule &imodule)
{
	imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	gchar *buf;
	char *num;
	const gchar *paragraphMark = NULL;
	gboolean newparagraph = FALSE;
	GString *rework;			// for image size analysis rework.
	marked_element *e = NULL;

	char *ModuleName = imodule.Name();
	ops = main_new_globals(ModuleName, 1);
	cache_flags = ConstructFlags(ops);

	is_rtol = main_is_mod_rtol(ModuleName);
	mf = get_font(ModuleName);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
			     ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);

	// when strongs/morph are on, the anchor boundary must be smaller.
	gint display_boundary = (strongs_or_morph ? 1 : 2);

	// if we are no longer where annotations were current, re-load.
	if (strcasecmp(ModuleName,
		       (marked_cache_modname ? marked_cache_modname : "")) ||
	    strcasecmp(key->getBookName(), marked_cache_book) ||
	    (curChapter != marked_cache_chapter))
		marked_cache_fill(ModuleName, (gchar *)key->getShortText());

	if (!strcmp(ModuleName, "KJV"))
		paragraphMark = "&para;&nbsp;";
	else
		paragraphMark = "";

	gint versestyle;
	gchar *file = NULL, *style = NULL;

	file = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	style = get_conf_file_item(file, ModuleName, "style");
	if ((style) && !strcmp(style, "verse"))
		versestyle = TRUE;
	else
		versestyle = FALSE;
	g_free(style);
	g_free(file);

	main_dialog_set_global_options((BackEnd*)be, ops);

	swbuf = "";
	footnote = xref = 0;
	buf = g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">",
			      (strongs_and_morph
			       ? CSS_BLOCK_BOTH
			       : (strongs_or_morph
				  ? CSS_BLOCK_ONE
				  : (ops->doublespace
				     ? DOUBLE_SPACE
				     : ""))),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      ((mf->old_font) ? mf->old_font : ""),
			      ((mf->old_font_size)
			       ? atoi(mf->old_font_size) + settings.base_font_size
			       : settings.base_font_size));
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
			       ? "rtl"
			       : "ltr"));

	for (key->Verse(1);
	     (key->Book() == curBook) &&
	     (key->Chapter() == curChapter) &&
	     !imodule.Error();
	     imodule++) {

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
					      ? block_render(imodule.RenderText())
					      : imodule.RenderText());
			rework = CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());

		if (!cVerse.HeaderIsValid())
			CacheHeader(cVerse, imodule, ops, be);

		if (cache_flags & ModuleCache::Headings)
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(cVerse.GetHeader(),
							   GDK_WINDOW(GTK_WINDOW(gtkText)))
				     : cVerse.GetHeader() /* left as-is */);
		else
			cVerse.InvalidateHeader();

		// special contrasty highlighting
		if (((e = marked_cache_check(key->Verse())) &&
		     settings.annotate_highlight) ||
		    ((key->Verse() == curVerse) && settings.versehighlight)) {
			buf = g_strdup_printf(
#ifdef USE_GTKMOZEMBED
			    "<span style=\"background-color: %s\">"
#else
			    "<table bgcolor=\"%s\"><tr><td>"
#endif
			    "<font face=\"%s\" size=\"%+d\">",
			    ((settings.annotate_highlight && e)
			     ? settings.highlight_fg
			     : settings.highlight_bg),
			    ((mf->old_font) ? mf->old_font : ""),
			    ((mf->old_font_size)
			     ? atoi(mf->old_font_size) + settings.base_font_size
			     : settings.base_font_size));
			swbuf.append(buf);
			g_free(buf);
		}

		num = main_format_number(key->Verse());
		buf = g_strdup_printf(settings.showversenum
			? "&nbsp; <a name=\"%d\" href=\"sword:///%s\">"
			  "<font size=\"%+d\" color=\"%s\">%s</font></a> "
			: "&nbsp; <a name=\"%d\"> </a>",
			key->Verse(),
			(char*)key->getText(),
			settings.verse_num_font_size + settings.base_font_size,
			(((settings.versehighlight && (key->Verse() == curVerse)) ||
			  (e && settings.annotate_highlight))
			 ? ((e && settings.annotate_highlight)
			    ? settings.highlight_bg
			    : settings.highlight_fg)
			 : settings.bible_verse_num_color),
			num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		// insert the userfootnote reference
		if (e) {
			buf = g_strdup_printf("<span class=\"word\">"
					      "<a href=\"passagestudy.jsp?action=showUserNote&"
					      "module=%s&passage=%s&value=%s\">"
					      "<small><sup>*u</sup></small></a></span> ",
/*xxx*/					      settings.MainWindowModule,
			                      (char*)key->getShortText(),
					      e->annotation->str);
			swbuf.append(buf);
			g_free(buf);
		}

		if ((key->Verse() == curVerse) || (e && settings.annotate_highlight)) {
			buf = g_strdup_printf("<font color=\"%s\">",
					      ((settings.versehighlight ||
						(e && settings.annotate_highlight))
					       ? ((e && settings.annotate_highlight)
						  ? settings.highlight_bg
						  : settings.highlight_fg)
					       : settings.currentverse_color));
			swbuf.append(buf);
			g_free(buf);
		}

		if (newparagraph && versestyle) {
			newparagraph = FALSE;
			swbuf.append(paragraphMark);;
		}

#ifndef USE_GTKMOZEMBED
		// same forced line break glitch in highlighted current verse.
		if ((settings.versehighlight ||
		     (e && settings.annotate_highlight)) &&	// doing <table> h/l.
		    !versestyle &&				// paragraph format.
		    (key->Verse() == curVerse)) {
			GString *text = g_string_new(NULL);

			g_string_printf(text, "%s", rework->str);
			if (!strcmp(text->str + text->len - 6, "<br />")) {
				text->len -= 6;
				*(text->str + text->len) = '\0';
			}
			else if (!strcmp(text->str + text->len - 4, "<br/>")) {
				text->len -= 4;
				*(text->str + text->len) = '\0';
			}
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(text->str,
							   GDK_WINDOW(gtkText->window))
				     : text->str /* left as-is */);
			g_string_free(text, TRUE);
		} else
#endif /* USE_GTKMOZEMBED */
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(rework->str,
							   GDK_WINDOW(GTK_WINDOW(gtkText)))
				     : rework->str /* left as-is */);

		if (key->Verse() == curVerse) {
			swbuf.append("</font>");
			ReadAloud(curVerse, cVerse.GetText());
		}

		if (versestyle) {
			if (strstr(cVerse.GetText(), "<!p>")) {
				newparagraph = TRUE;
			} else {
				newparagraph = FALSE;
			}
			if ((key->Verse() != curVerse) ||
			    (!settings.versehighlight &&
			     (!e || !settings.annotate_highlight)))
				swbuf.append("<br/>");
		}

		// special contrasty highlighting
		if (((key->Verse() == curVerse) && settings.versehighlight) ||
		    (e && settings.annotate_highlight))
#ifdef USE_GTKMOZEMBED
			swbuf.append("</font></span>");
#else
			swbuf.append("</font></td></tr></table>");
#endif
	}

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);

	swbuf.append("</div></font></body></html>");

#ifdef USE_GTKMOZEMBED
	if (strongs_and_morph)
		buf = g_strdup_printf("%d", curVerse);
	else	/* this is not dangling: connects to following "if" */
#endif /* USE_GTKMOZEMBED */
	if (curVerse > display_boundary)
		buf = g_strdup_printf("%d", curVerse - display_boundary);
	else
		buf = NULL;
	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, buf);
	if (buf)
		g_free(buf);

	free_font(mf);
	mf = NULL;
	g_free(ops);
    	ops = NULL;
	return 0;
}


char
GTKPrintEntryDisp::Display(SWModule &imodule)
{
#ifdef USE_GTKMOZEMBED
	gchar *keytext = NULL;
	gchar *buf;
	SWBuf swbuf = "";
	gint mod_type;
	MOD_FONT *mf = get_font(imodule.Name());

	GLOBAL_OPS * ops = main_new_globals(imodule.Name(),0);

	(const char *)imodule;	// snap to entry
	GS_message(("%s",(const char *)imodule.getRawEntry()));
	main_set_global_options(ops);
	mod_type = backend->module_type(imodule.Name());

	if (mod_type == BOOK_TYPE)
		keytext = strdup(backend->treekey_get_local_name(
				settings.book_offset));
	else if (mod_type == DICTIONARY_TYPE)
		keytext = g_strdup((char*)imodule.KeyText());
	else
		keytext = strdup((char*)imodule.KeyText());

	buf=g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">"
			      "<font color=\"%s\">"
			      "<a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font>[%s]<br/>",
			      (ops->doublespace ? DOUBLE_SPACE : ""),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      ((mf->old_font) ? mf->old_font : ""),
			      ((mf->old_font_size)
			       ? atoi(mf->old_font_size) + settings.base_font_size
			       : settings.base_font_size),
			      settings.bible_verse_num_color,
			      imodule.Description(),
			      imodule.Name(),
			      imodule.Name(),
			      (gchar*)keytext );
	swbuf.append(buf);
	g_free(buf);

	swbuf.append((const char *)imodule);
	swbuf.append("</font></body></html>");

	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);
	free_font(mf);
	g_free(ops);
	if (keytext)
		g_free(keytext);
#endif
	return 0;
}

char
GTKPrintChapDisp::Display(SWModule &imodule)
{
#ifdef USE_GTKMOZEMBED
       imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	gchar *buf;
	gchar *preverse = NULL;
	const gchar *paragraphMark = NULL;
	gchar heading[32];
	SWBuf swbuf;
	char *num;

	GLOBAL_OPS * ops = main_new_globals(imodule.Name(),0);
	gboolean is_rtol = main_is_mod_rtol(imodule.Name());
	gboolean newparagraph = FALSE;
	mf = get_font(imodule.Name());

	if (!strcmp(imodule.Name(), "KJV"))
		paragraphMark = "&para;&nbsp;";
	else
		paragraphMark = "";

	swbuf = "";

	buf=g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">",
			      (ops->doublespace ? DOUBLE_SPACE : ""),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      ((mf->old_font) ? mf->old_font : ""),
			      ((mf->old_font_size)
			       ? atoi(mf->old_font_size) + settings.base_font_size
			       : settings.base_font_size));
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
			       ? "rtl"
			       : "ltr"));

	main_set_global_options(ops);

	for (key->Verse(1);
	     key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error();
	     imodule++) {
		int x = 0;
		sprintf(heading, "%d", x);
		while ((preverse
			= backend->get_entry_attribute("Heading", "Preverse",
						       heading)) != NULL) {
			const char *preverse2 = imodule.RenderText(preverse);
			buf=g_strdup_printf("<br/><b>%s</b><br/><br/>", preverse2);
			swbuf.append(buf);
			g_free(buf);
			g_free(preverse);
			++x;
			sprintf(heading, "%d", x);
		}

		num = main_format_number(key->Verse());
		buf=g_strdup_printf(settings.showversenum
			? "&nbsp; <a name=\"%d\" href=\"sword:///%s\">"
			  "<font size=\"%+d\" color=\"%s\">%s</font></a> "
			: "&nbsp; <a name=\"%d\"> </a>",
			key->Verse(),
			(char*)key->getText(),
			settings.verse_num_font_size + settings.base_font_size,
			settings.bible_verse_num_color,
			num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			swbuf.append(paragraphMark);
		}

		swbuf.append((const char *)imodule);

		buf = g_strdup_printf("%s", (const char *)imodule);

		if (settings.versestyle) {
			if (g_strstr_len(buf, strlen(buf), "<!p>")) {
				newparagraph = TRUE;
			} else {
				newparagraph = FALSE;
			}
			swbuf.append("<br/>");
		}

		g_free(buf);
	}

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);

	swbuf.append("</div></font></body></html>");

	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);

	free_font(mf);
	g_free(ops);
#endif
	return 0;
}
