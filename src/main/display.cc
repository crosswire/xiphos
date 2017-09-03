/*
 * Xiphos Bible Study Tool
 * display.cc -
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

#include <glib.h>

#include <osisxhtml.h>
#include <thmlxhtml.h>
#include <gbfxhtml.h>
#include <teixhtml.h>

#include <osisvariants.h>
#include <thmlvariants.h>
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>

#include <regex.h>
#include <string.h>
#include <ctype.h>
#include <assert.h>

#include "xiphos_html/xiphos_html.h"

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

#include <glib/gstdio.h>

// for one-time content rendering.
extern ModuleCache::CacheMap ModuleMap;

// for tracking personal annotations.
typedef struct
{
	gchar *module;
	gchar *book;
	int chapter;
	int verse;
	GString *annotation;
} marked_element;

typedef std::map<int, marked_element *> MC;
MC marked_cache;

gchar *marked_cache_modname = NULL, *marked_cache_book = NULL;
int marked_cache_chapter = -1;

int footnote, xref;

// flag for having discovered (in)valid bible keys.
// used for e.g. navigating from a bible w/apocrypha
// to a tab with a non-apocrypha bible.
gboolean valid_scripture_key = TRUE;
const gchar *no_content =
    N_("<br/><br/><center><i>This module has no content at this point.</i></center>");

#define HTML_START \
	"<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"> \
<style type=\"text/css\"><!-- \
A { text-decoration:none } \
*[dir=rtl] { text-align: right; } \
body {background-color:%s;color:%s;} \
a:link{color:%s} %s%s -->\
</style>%s</head><body>"
// 6 interpolable strings: bg/txt/link colors, block, renderHeader, external css.

// CSS style blocks to control blocked strongs+morph output
// BOTH is when the user wants to see both types of markup.
// ONE is when he wants one or the other, so we can use a single
// specification which overlays both on top of one another.
#define CSS_BLOCK_BOTH                                                                            \
	" *        { line-height: 3.8em; }"                                                       \
	" .word    { position: relative; top:  0.0em; left: 0; }"                                 \
	" .strongs { position: absolute; top:  0.3em; left: 0; white-space: nowrap; z-index: 2 }" \
	" .morph   { position: absolute; top:  1.2em; left: 0; white-space: nowrap; z-index: 1 }"
#define CSS_BLOCK_ONE                                                                  \
	" *        { line-height: 2.7em; }"                                            \
	" .word    { position: relative; top:  0.0em; left: 0; }"                      \
	" .strongs { position: absolute; top:  0.8em; left: 0; white-space: nowrap; }" \
	" .morph   { position: absolute; top:  0.8em; left: 0; white-space: nowrap; }"

#define DOUBLE_SPACE " * { line-height: 2em ! important; }"

using namespace sword;
using namespace std;

//
// user annotation cache filling.
//

#define NUM_REPLACE 4

struct replace
{
	gchar c;
	gchar *s;
} replacement[NUM_REPLACE] = {
      // < and > must be first.
      {'<', (gchar *)"&lt;"},
      {'>', (gchar *)"&gt;"},
      {'\n', (gchar *)"<br />"},
      {'"', (gchar *)"&quot;"},
};

// a macro to substitute the visually ugly presentation below.
// this puts all the decoration possible around a verse number.
#define PRETTYPRINT(v)                                             \
	(settings.verse_num_superscript ? superscript_start : ""), \
	    (settings.verse_num_bracket ? "[" : ""),               \
	    (settings.verse_num_bold ? bold_start : ""),           \
	    (v),                                                   \
	    (settings.verse_num_bold ? bold_end : ""),             \
	    (settings.verse_num_bracket ? "]" : ""),               \
	    (settings.verse_num_superscript ? superscript_end : "")

const char *bold_start = "<b>",
	   *bold_end = "</b>",
	   *superscript_start = "<sup>",
	   *superscript_end = "</sup>";

void
marked_cache_fill(const gchar *modname, gchar *key)
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
	key_chapter = atoi(t + 1);
	// key_verse   = atoi(s+1);

	// remember exactly what chapter this cache is for
	g_free(marked_cache_modname);
	g_free(marked_cache_book);
	marked_cache_modname = g_strdup(modname);
	marked_cache_book = g_strdup(key_book);
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
			if ((s = strrchr(m, '.')) == NULL) // rightmost dot
				goto fail;
			*s = '\0';
			if ((t = strrchr(m, '.')) == NULL) // chapter:verse
				goto fail;
			*t = '\0';
			e->chapter = atoi(t + 1);
			e->verse = atoi(s + 1);
			if ((s = strchr(m, ' ')) == NULL) // leftmost space
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
						(void)g_string_erase(e->annotation,
								     s - (e->annotation->str), 1);
						(void)g_string_insert(e->annotation,
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
	gchar *s;

	for (s = strstr(text, "<img "); s; s = strstr(s, "<img ")) {
		gchar *t;
		if ((t = strchr(s + 4, '>'))) {
			while (s <= t)
				*(s++) = ' ';
		} else {
			XI_message(("ClearImages: no img end: %s\n", s));
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
	gchar *s;

	// huge assumption: no nested <font> specs: <font face="">
	// is never followed by another <font anything> before </font>.
	for (s = strstr(text, "<font face=\"Galax"); s; s = strstr(s, "<font face=\"Galax")) {
		gchar *t;
		if ((t = strchr(s + 15, '>'))) {
			while (s <= t)
				*(s++) = ' ';
			s = strstr(s, "</font>");
			t = s + 6;
			while (s <= t)
				*(s++) = ' ';
		} else {
			XI_message(("ClearFontFaces: no font end: %s\n", s));
			return;
		}
	}
}

//
// retrieve the content of the module's personal css
//
static const char *stylefile =
    "style.css"; // default name, per module.
static const char *default_stylefile =
    "default-style.css"; // default name, overall.

const gchar *get_css_references(SWModule &module)
{
	static string css; // static -> safe to return it

	// assume nothing will be available.
	css = "";

	// non-specific CSS for all module displays.
	char *css_file = g_build_filename(settings.gSwordDir,
					  default_stylefile, NULL);

	if (g_file_test(css_file, G_FILE_TEST_EXISTS)) {
		css += (string) "<link rel=\"stylesheet\" type=\"text/css\" href=\""
#ifdef WIN32
		       + "http://127.0.0.1:7878/" // see main.c (sob)
#else
		       + "file:"
#endif
		       + css_file + "\" />";
	}
	g_free(css_file);

	// construct path to module's CSS.
	char *datapath = main_get_mod_config_entry(module.getName(), "AbsoluteDataPath");
	char *prefcss = main_get_mod_config_entry(module.getName(), "PreferredCSSXHTML");

	// module-specific CSS.
	css_file = g_build_filename(datapath, (prefcss ? prefcss : stylefile), NULL);

	if (g_file_test(css_file, G_FILE_TEST_EXISTS)) {
		css += (string) "<link rel=\"stylesheet\" type=\"text/css\" href=\""
#ifdef WIN32
		       + "http://127.0.0.1:7878/" // see main.c (sob)
#else
		       + "file:"
#endif
		       + css_file + "\" />";
	}
	g_free(css_file); // get rid of old one first.

	return css.c_str();
}

//
// utility function for block_render() below.
// having a word + annotation in hand, stuff them into the buffer.
// span class names are from CSS_BLOCK macros.
// we garbage-collect here so block_render doesn't have to.
//

#define ALIGN_WORD "&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;"

void
block_dump(SWBuf &rendered,
	   const char **word,
	   const char **strongs,
	   const char **morph)
{
	int wlen, min_length, slen, mlen;
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
		t = (char *)strrchr(*strongs, '>') + 1;
		// correct weird NASB lexicon references.
		if ((s0 = (char *)strchr(*strongs, '!'))) {
			do {
				*s0 = *(s0 + 1);
				++s0;
			} while (*s0 != '"');
			*s0 = ' ';
		}
		*s = '<';
		slen = s - t;
		s = (char *)strstr(*strongs, "&lt;");
		*s = *(s + 1) = *(s + 2) = *(s + 3) = ' ';
		s = strstr(s, "&gt;");
		*s = *(s + 1) = *(s + 2) = *(s + 3) = ' ';

		// gross hack needed to handle new class="..." in sword -r2512.
		if ((s = (char *)strstr(*strongs, " class=\"strongs\">"))) {
			memcpy(s, ">                ", 17);
			if ((s = (char *)strstr(s, " class=\"strongs\">")))
				memcpy(s, ">                ", 17);
		}
	} else
		slen = 0;

	if (*morph) {
		s = s0 = (char *)g_strrstr(*morph, "\">") + 2;
		t = strchr(s, '<');
		for (/* */; s < t; ++s)
			if (isupper(*s))
				*s = tolower(*s);
		for (s = strchr(s0, ' '); s && (s < t); s = strchr(s, ' '))
			*s = '-';
		s = g_strrstr(*morph, "</a>");
		*s = '\0';
		t = (char *)strrchr(*morph, '>') + 1;
		*s = '<';
		mlen = s - t;
		s = (char *)strchr(*morph, '(');
		*s = ' ';
		s = strrchr(s, ')');
		*s = ' ';

		// gross hack needed to handle new class="..." in sword -r2512.
		if ((s = (char *)strstr(*morph, " class=\"morph\">"))) {
			memcpy(s, ">              ", 15);
			if ((s = (char *)strstr(s, " class=\"morph\">")))
				memcpy(s, ">              ", 15);
		}
	} else
		mlen = 0;

	min_length = 2 + max(slen, mlen);

	rendered += *word;

	for (wlen = strlen(*word); wlen <= min_length; ++wlen)
		rendered += "&nbsp;";

	g_free((char *)*word);
	*word = NULL;

	rendered += "<span class=\"strongs\"><sup>";
	rendered += (*strongs ? *strongs : "&nbsp;");
	rendered += "</sup></span>";
	if (*strongs)
		g_free((char *)*strongs);
	*strongs = NULL;

	rendered += "<span class=\"morph\"><sup>";
	rendered += (*morph ? *morph : "&nbsp;");
	rendered += "</sup></span>";
	if (*morph)
		g_free((char *)*morph);
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

#define EMPTY_WORD ""

void
block_render_secondary(const char *text,
		       SWBuf &rendered)
{
	const char *word = NULL,
		   *strongs = NULL,
		   *morph = NULL;
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
			if ((((*(s + 1) == 'a') || (*(s + 1) == 'A')) && (*(s + 2) == ' ')) ||
			    (((*(s + 1) == 'b') || (*(s + 1) == 'B') ||
			      (*(s + 1) == 'i') || (*(s + 1) == 'I') ||
			      (*(s + 1) == 'u') || (*(s + 1) == 'U')) &&
			     (*(s + 2) == '>'))) {
				if (word)
					block_dump(rendered, &word, &strongs, &morph);

				static char end[5] = "</X>";
				end[2] = *(s + 1);
			again:
				if ((t = strstr((char *)s, end)) == NULL) {
					XI_warning(("No %s in %s\n", end, s));
					break;
				}

				// yet another nightmare:
				// if the markup results in e.g. doubled <i>
				// (bogus "<hi><hi>word</hi></hi>"),
				// then we will mis-assess termination.
				// so we search for the same markup embedded within.
				// if we find an internal set, we just wipe it out.
				static char embedded[4] = "<X>";
				embedded[1] = *(s + 1);
				if ((u = g_strstr_len(s + 3, t - (s + 3), embedded))) {
					*u = *(u + 1) = *(u + 2) =
					    *t = *(t + 1) = *(t + 2) = *(t + 3) = ' ';
					goto again; // yuck, yes, i know...
				}

				// nasb eph 5:31: whole verse is an italicized
				// quotation of gen 2:24...containing strongs.
				// in event of font switch, output bare switch
				// controls but recurse on content within.
				if ((*(s + 1) == 'a') || (*(s + 1) == 'A')) {
					// <a href> anchor -- uninteresting.
					t += 4;
					word = g_strndup(s, t - s);
					s = t - 1;
				} else {
					// font switch nightmare.
					word = g_strndup(s, 3);
					rendered += word;
					g_free((char *)word);
					word = g_strndup(s + 3, t - (s + 3));
					block_render_secondary(word, rendered);
					g_free((char *)word);
					word = NULL;
					rendered += end;
					s = t + 3;
				}
				break;
			} else if (!strncmp(s + 1, "small>", 6)) {
				// strongs and morph are bounded by "<small>".
				if ((t = strstr((char *)s, "</small>")) == NULL) {
					XI_warning(("No </small> in %s\n", s));
					break;
				}
				t += 8;
				// this is very dicey -- phenomenological/
				// observable about Sword filters' provision.
				// strongs: "<em>&lt;...&gt;</em>"
				// morph:   "<em>(...)</em>"
				// if Sword ever changes this, we're dead.
				const char *u = s + 11;
				while ((*u != '(') && (*u != '&'))
					++u; // it has to be one or the other.
				if (*u == '(') {
					if (morph) {
						block_dump(rendered, &word, &strongs, &morph);
						word = g_strdup(EMPTY_WORD);
					}
					morph = g_strndup(s, t - s);
				} else {
					if (strongs) {
						block_dump(rendered, &word, &strongs, &morph);
						word = g_strdup(EMPTY_WORD);
					}
					strongs = g_strndup(s, t - s);
				}
				s = t - 1;
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
				for (/* */;
				     *s && (*s != ' ') && (*s != '\t');
				     ++s) {
					if (*s == '<') {
						if (!strncmp(s + 1, "small>", 6) ||
						    ((*(s + 1) == 'a') &&
						     (*(s + 2) == ' '))) {
							// "break 2;"
							goto outword;
						}
						bracket++;
					} else if (*s == '>')
						bracket--;
					assert(bracket >= 0);
				}
			} while (bracket != 0);
		outword:
			word = g_strndup(word, s - word);
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
	       const char *name,
	       bool reset = true)
{
	if (ops->image_content == 0)
		ClearImages((gchar *)text->str);
	else if ((ops->image_content == -1) && // "unknown"
		 (strcasestr(text->str, "<img ") != NULL)) {
		ops->image_content = 1; // now known.
		main_save_module_options(name, "Image Content", 1);
	}
	if (ops->respect_font_faces == 0)
		ClearFontFaces((gchar *)text->str);
	else if ((ops->respect_font_faces == -1) && // "unknown"
		 (strcasestr(text->str, "<font face=\"Galax") != NULL)) {
		ops->respect_font_faces = 1; // now known.
		main_save_module_options(name, "Respect Font Faces", 1);
	}

	gint pos;
	gchar value[50], *reported, *s = text->str;

	// test for any 'n="X"' content.  if so, use it directly.
	if ((reported = backend->get_entry_attribute("Footnote", "1", "n", false))) {
		g_free(reported); // dispose of test junk.
	}
	// otherwise we simply count notes & xrefs through the verse.
	else if (ops->xrefnotenumbers) {
		while ((s = strstr(s, "*n"))) {
			g_snprintf(value, 5, "%d", ++footnote);
			pos = s - (text->str) + 2;
			text = g_string_insert(text, pos, value);
			s = text->str + pos + 1;
		}

		s = text->str;
		while ((s = strstr(s, "*x"))) {
			g_snprintf(value, 5, "%d", ++xref);
			pos = s - (text->str) + 2;
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
CacheHeader(ModuleCache::CacheVerse &cVerse,
	    SWModule &mod,
	    GLOBAL_OPS *ops, BackEnd *be)
{
	int x = 0;
	gchar heading[8];
	const gchar *preverse;
	SWBuf preverse2;
	GString *text = g_string_new("");

	cVerse.SetHeader("");

	sprintf(heading, "%d", x);
	while ((preverse = be->get_entry_attribute("Heading", "Preverse",
						   heading)) != NULL) {
		preverse2 = mod.renderText(preverse);
		g_string_printf(text,
				"%s",
				(((ops->strongs || ops->lemmas) ||
				  ops->morphs)
				     ? block_render(preverse2.c_str())
				     : preverse2.c_str()));
		text = CleanupContent(text, ops, mod.getName(), false);

		cVerse.AppendHeader(text->str);
		g_free((gchar *)preverse);
		++x;
		sprintf(heading, "%d", x);
	}
	g_string_free(text, TRUE);
}

void
set_morph_order(SWModule &imodule)
{
	for (FilterList::const_iterator it =
		 imodule.getRenderFilters().begin();
	     it != imodule.getRenderFilters().end();
	     ++it) {
		OSISXHTML *f = dynamic_cast<OSISXHTML *>(*it);
		if (f)
			f->setMorphFirst();
	}
}

void
set_render_numbers(SWModule &imodule, GLOBAL_OPS *ops)
{
	// if we have not yet determined options, don't bother.
	if (!ops)
		return;

	for (FilterList::const_iterator it =
		 imodule.getRenderFilters().begin();
	     it != imodule.getRenderFilters().end();
	     ++it) {
		OSISXHTML *f1 = dynamic_cast<OSISXHTML *>(*it);
		if (f1)
			f1->setRenderNoteNumbers((ops->xrefnotenumbers != 0));
		ThMLXHTML *f2 = dynamic_cast<ThMLXHTML *>(*it);
		if (f2)
			f2->setRenderNoteNumbers((ops->xrefnotenumbers != 0));
		GBFXHTML *f3 = dynamic_cast<GBFXHTML *>(*it);
		if (f3)
			f3->setRenderNoteNumbers((ops->xrefnotenumbers != 0));
		TEIXHTML *f4 = dynamic_cast<TEIXHTML *>(*it);
		if (f4)
			f4->setRenderNoteNumbers((ops->xrefnotenumbers != 0));
	}
}

//
// display of commentary by chapter.
//
char
GTKEntryDisp::displayByChapter(SWModule &imodule)
{
	imodule.setSkipConsecutiveLinks(true);

	VerseKey *key = (VerseKey *)(SWKey *) imodule;
	bool before_curVerse(true);
	int curVerse = key->getVerse();
	int curChapter = key->getChapter();
	int curBook = key->getBook();
	gchar *buf, *vbuf;
	const char *ModuleName = imodule.getName();
	GString *rework; // for image size analysis rework.
	footnote = xref = 0;

	// we come into this routine with swbuf init'd with
	// boilerplate html startup, plus ops and mf ready.

	cache_flags = ConstructFlags(ops);
	is_rtol = main_is_mod_rtol(ModuleName);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph = ((ops->strongs || ops->lemmas) ||
			    ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);
	set_render_numbers(imodule, ops);

	// open the table.
	if (settings.showversenum) {
		buf = g_strdup_printf("<font face=\"%s\"><table border=\"0\""
				      " cellpadding=\"5\" cellspacing=\"0\">",
				      ((mf->old_font) ? mf->old_font : ""));
		swbuf.append(buf);
		g_free(buf);
	}

	for (key->setVerse(1);
	     (key->getBook() == curBook) && (key->getChapter() == curChapter) && !imodule.popError();
	     imodule++) {

		ModuleCache::CacheVerse &cVerse = ModuleMap
		    [ModuleName]
		    [key->getTestament()]
		    [key->getBook()]
		    [key->getChapter()]
		    [key->getVerse()];

		// use the module cache rather than re-accessing Sword.
		// but editable personal commentaries don't use the cache.
		if (!cVerse.CacheIsValid(cache_flags) &&
		    (backend->module_type(imodule.getName()) != PERCOM_TYPE)) {
			rework = g_string_new(strongs_or_morph
						  ? block_render(imodule.renderText().c_str())
						  : imodule.renderText().c_str());
			rework = CleanupContent(rework, ops, imodule.getName());
			cVerse.SetText(rework->str, cache_flags);
		} else {
			if (backend->module_type(imodule.getName()) == PERCOM_TYPE)
				rework = g_string_new(strongs_or_morph
							  ? block_render(imodule.getRawEntry())
							  : imodule.getRawEntry());
			else
				rework = g_string_new(cVerse.GetText());
		}

		// add an anchor for where in the chapter we are.
		// (commentaries can have big sections on 1 verse [<hr>],
		// and many missing verses [<a name>].)
		if ((curVerse != 1) && // not at top of pane
		    ((curVerse == key->getVerse()) ||
		     (before_curVerse && (key->getVerse() > curVerse)))) {
			buf = NULL;
			vbuf = g_strdup_printf("<tr><td>%s<hr/></td><td><hr/></td></tr>",
					       // repeated conditional check here
					       ((before_curVerse &&
						 (key->getVerse() > curVerse))
						    ? (buf = g_strdup_printf(
							   "<a name=\"%d\"> </a>", curVerse))
						    : ""));
			g_free(buf);
			swbuf.append(vbuf);
			g_free(vbuf);
		}

		swbuf.append("<tr>");

		// insert verse numbers
		char *num = main_format_number(key->getVerse());
		vbuf = g_strdup_printf((settings.showversenum
					    ? "<td valign=\"top\" align=\"right\">"
					      "<a name=\"%d\" href=\"sword:///%s\">"
					      "<font size=\"%+d\" color=\"%s\">%s%s%s%s%s%s%s</font></a></td>"
					    : "<p/><a name=\"%d\"> </a>"),
				       key->getVerse(),
				       (char *)key->getText(),
				       settings.verse_num_font_size + settings.base_font_size,
				       settings.bible_verse_num_color,
				       PRETTYPRINT(num));
		g_free(num);

		swbuf.append(vbuf);

		if (settings.showversenum) {
			buf = g_strdup_printf("<td><font size=\"%+d\">",
					      mf->old_font_size_value);
			swbuf.append(buf);
			g_free(buf);
		}
		swbuf.append(settings.imageresize
				 ? AnalyzeForImageSize(rework->str,
						       GDK_WINDOW(gtk_widget_get_window(gtkText)))
				 : rework->str /* left as-is */);
		if (settings.showversenum)
			swbuf.append("</font></td>");

		g_free(vbuf);

		swbuf.append("</tr>");
		before_curVerse = (key->getVerse() < curVerse);
	}

	// if we haven't gotten around to placing the anchor, do so now.
	if (before_curVerse) {
		buf = g_strdup_printf("<tr><td><a name=\"%d\"> </a><hr/></td><td><hr/></td></tr>",
				      curVerse);
		swbuf.append(buf);
		g_free(buf);
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
GTKEntryDisp::display(SWModule &imodule)
{
	if (!gtk_widget_get_realized(GTK_WIDGET(gtkText)))
		gtk_widget_realize(gtkText);

	const char *abbreviation = main_get_abbreviation(imodule.getName());
	gchar *buf;
	mf = get_font(imodule.getName());
	swbuf = "";
	footnote = xref = 0;

	ops = main_new_globals(imodule.getName());

	GString *rework; // for image size analysis rework.

	imodule.getRawEntry(); // snap to entry
	main_set_global_options(ops);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph = ((ops->strongs || ops->lemmas) ||
			    ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);
	set_render_numbers(imodule, ops);

	buf = g_strdup_printf(HTML_START // //bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">"
			      "[<a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\">"
			      "<font color=\"%s\">*%s*</font></a>]<br/>",
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      (strongs_and_morph // both
				   ? CSS_BLOCK_BOTH
				   : (strongs_or_morph // either
					  ? CSS_BLOCK_ONE
					  : (ops->doublespace // neither
						 ? DOUBLE_SPACE
						 : ""))),
			      imodule.getRenderHeader(),
			      get_css_references(imodule),
			      ((mf->old_font) ? mf->old_font : ""),
			      mf->old_font_size_value,
			      imodule.getDescription(),
			      imodule.getName(),
			      settings.bible_verse_num_color,
			      (abbreviation ? abbreviation : imodule.getName()));
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
				   ? "rtl"
				   : "ltr"));

	if (!valid_scripture_key) {
		swbuf.append(no_content);
		swbuf.append("</div></font></body></html>");
		HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);
		free_font(mf);
		mf = NULL;
		g_free(ops);
		ops = NULL;
		return 0;
	}

	//
	// the rest of this routine is irrelevant if we are
	// instead heading off to show a whole chapter
	// (this option can be enabled only in commentaries.)
	//
	if (ops->commentary_by_chapter)
		return displayByChapter(imodule);

	// we will use the module cache for regular commentaries,
	// which navigate/change a lot, whereas pers.comms, lexdicts,
	// and genbooks still do fresh access every time -- the nature
	// of those modules' use won't buy much with a module cache.

	// there is some unfortunate but unavoidable code duplication
	// for handling potential clearing of images, due to the
	// difference in how modules are being accessed.

	int modtype = backend->module_type(imodule.getName());
	if (modtype == COMMENTARY_TYPE) {
		VerseKey *key = (VerseKey *)(SWKey *) imodule;
		cache_flags = ConstructFlags(ops);
		const char *ModuleName = imodule.getName();

		ModuleCache::CacheVerse &cVerse = ModuleMap
		    [ModuleName]
		    [key->getTestament()]
		    [key->getBook()]
		    [key->getChapter()]
		    [key->getVerse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
						  ? block_render(imodule.renderText().c_str())
						  : imodule.renderText().c_str());
			rework = CleanupContent(rework, ops, imodule.getName());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());
	} else {
		if ((modtype == PERCOM_TYPE) ||
		    (modtype == PRAYERLIST_TYPE))
			rework = g_string_new(strongs_or_morph
						  ? block_render(imodule.getRawEntry())
						  : imodule.getRawEntry());
		else {
			rework = g_string_new(strongs_or_morph
						  ? block_render(imodule.renderText().c_str())
						  : imodule.renderText().c_str());
			if (modtype == DICTIONARY_TYPE) {
				char *f = (char *)imodule.getConfigEntry("Feature");
				if (f && !strcmp(f, "DailyDevotion")) {
					char *pretty;
					char *month = backend->get_module_key();
					char *day = strchr(month, '.');
					if (day)
						*(day++) = '\0';
					else
						day = (char *)"XX";
					int idx_month = atoi(month) - 1;
					pretty = g_strdup_printf("<b>%s %s</b><br/>",
								 month_names[idx_month],
								 day);
					swbuf.append(pretty);
					g_free(pretty);
				}
			}
		}
		rework = CleanupContent(rework, ops, imodule.getName());
	}

	swbuf.append(settings.imageresize
			 ? AnalyzeForImageSize(rework->str,
					       GDK_WINDOW(gtk_widget_get_window(gtkText)))
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

	sword::VerseKey *key = (VerseKey *)(SWKey *) imodule;
	int curVerse = key->getVerse();
	int chapter = key->getChapter();
	int curBook = key->getBook();
	int curTest = key->getTestament();
#if 0
	const char *ModuleName = imodule.getName();
#endif

	key->setVerse(1);
	imodule--;

	if (imodule.popError()) {
		num = main_format_number(chapter);
		buf = g_strdup_printf("<a name=\"TOP\"></a><div style=\"text-align: center\">"
				      "<p><b><font size=\"%+d\">%s</font></b></p>"
				      "<b>%s %s</b></div>",
				      1 + mf->old_font_size_value,
				      imodule.getDescription(),
				      _("Chapter"),
				      num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);
	} else {

		if (strongs_and_morph)
			set_morph_order(imodule);
		set_render_numbers(imodule, ops);

		swbuf.appendFormatted("<div dir=%s>",
				      ((is_rtol && !ops->transliteration)
					   ? "rtl"
					   : "ltr"));

		num = main_format_number(key->getVerse());
		buf = g_strdup_printf(
		    (settings.showversenum
			 ? "&nbsp; <a name=\"0\" href=\"sword:///%s\">"
			   "<font size=\"%+d\" color=\"%s\">%s%s%s%s%s%s%s</font>&nbsp;"
			 : "&nbsp; <a name=\"0\"> </a>"),
		    (char *)key->getText(),
		    (settings.versestyle
			 ? settings.verse_num_font_size + settings.base_font_size
			 : settings.base_font_size - 2),
		    settings.bible_verse_num_color,
		    PRETTYPRINT(num));
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		num = main_format_number(chapter);
		buf = g_strdup_printf(
		    "<font color=\"%s\">%s</font>%s%s<br/><a name=\"TOP\"></a><hr/><div style=\"text-align: center\"><b>%s %s</b></div>",
		    settings.bible_text_color,
		    (strongs_or_morph
			 ? block_render(imodule.renderText().c_str())
			 : imodule.renderText().c_str()),
		    (settings.showversenum ? "</a>" : ""),
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
	char oldAutoNorm = key->isAutoNormalize();
	key->setAutoNormalize(0);

	for (int i = 0; i < 2; ++i) {
		// Get chapter 0 iff we're in chapter 1.
		if ((i == 0) && (chapter != 1))
			continue;

		key->setChapter(i * chapter);
		key->setVerse(0);

#if 0 // with footnote counting, we no longer cache before/after verses.
		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [key->Testament()]
		    [key->Book()]
		    [key->Chapter()]
		    [key->Verse()];
	
		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render(imodule.renderText().c_str())
					: imodule.renderText().c_str()),
				       cache_flags);
		buf = g_strdup_printf("%s<br />", cVerse.GetText());
#else
		buf = g_strdup_printf("%s<br />",
				      (strongs_or_morph
					   ? block_render(imodule.renderText().c_str())
					   : imodule.renderText().c_str()));
#endif /* !0 */
		swbuf.append(buf);
		g_free(buf);
	}

	key->setAutoNormalize(oldAutoNorm);

	key->setTestament(curTest);
	key->setBook(curBook);
	key->setChapter(chapter);
	key->setVerse(curVerse);
}

void
GTKChapDisp::getVerseAfter(SWModule &imodule)
{
	gchar *buf;
#if 0
	const char *ModuleName = imodule.Name();
#endif
	sword::VerseKey *key = (VerseKey *)(SWKey *) imodule;

	imodule++;
	if (imodule.popError()) {
		buf = g_strdup_printf(
		    "%s<hr/><div style=\"text-align: center\"><p><b>%s</b></p></div>",
		    // extra break when excess strongs/morph space.
		    (strongs_or_morph ? "<br/><br/>" : ""),
		    imodule.getDescription());
		swbuf.append(buf);
		g_free(buf);
	} else {
		imodule--;
		char *num = main_format_number(key->getChapter());

		buf = g_strdup_printf(
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

		num = main_format_number(key->getVerse());
		buf = g_strdup_printf(
		    (settings.showversenum
			 ? "&nbsp; <a name=\"0\" href=\"sword:///%s\">"
			   "<font size=\"%+d\" color=\"%s\">%s%s%s%s%s%s%s</font>&nbsp;"
			 : "&nbsp; <a name=\"0\"> </a>"),
		    (char *)key->getText(),
		    (settings.versestyle
			 ? settings.verse_num_font_size + settings.base_font_size
			 : settings.base_font_size - 2),
		    settings.bible_verse_num_color,
		    PRETTYPRINT(num));
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		if (strongs_and_morph)
			set_morph_order(imodule);
		set_render_numbers(imodule, ops);

		buf = g_strdup_printf("<font color=\"%s\">%s</font>%s</div>",
				      settings.bible_text_color,
				      (strongs_or_morph
				       ? block_render(imodule.renderText().c_str())
				       : imodule.renderText().c_str()),
				      (settings.showversenum ? "</a>" : ""));
		swbuf.append(buf);
		g_free(buf);
	}
}

char
GTKChapDisp::display(SWModule &imodule)
{
	// following line ensures linked verses work correctly
	// it does not solve the problem of marking groups of verses (1-4), etc
	imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *) imodule;
	int curVerse = key->getVerse();
	int curChapter = key->getChapter();
	int curBook = key->getBook();
	gchar *buf;
	GString *rework; // for image size analysis rework.
	const char *ModuleName = imodule.getName();
	ops = main_new_globals(ModuleName);
	cache_flags = ConstructFlags(ops);

	is_rtol = main_is_mod_rtol(ModuleName);
	mf = get_font(ModuleName);

	if (!gtk_widget_get_realized(GTK_WIDGET(gtkText)))
		return 0;

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph = ((ops->strongs || ops->lemmas) ||
			    ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);
	set_render_numbers(imodule, ops);

	settings.versestyle = ops->verse_per_line;

	// when strongs/morph are on, the anchor boundary must be smaller.
	// or if main window is too small to keep curverse in-pane,
	// of it the user wants really big fonts.
	gint display_boundary = (((settings.gs_height < 500) ||
				  (mf->old_font_size_value > 2))
				     ? 0
				     : (strongs_or_morph ? 1 : 2));

	// if we are no longer where annotations were current, re-load.
	if (strcasecmp(ModuleName,
		       (marked_cache_modname ? marked_cache_modname : "")) ||
	    strcasecmp(key->getBookAbbrev(), marked_cache_book) ||
	    (curChapter != marked_cache_chapter))
		marked_cache_fill(ModuleName, settings.currentverse);

	swbuf = "";
	footnote = xref = 0;

	buf = g_strdup_printf(HTML_START // "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">",
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      // strongs & morph specs win over dblspc.
			      (strongs_and_morph // both
				   ? CSS_BLOCK_BOTH
				   : (strongs_or_morph // either
					  ? CSS_BLOCK_ONE
					  : (ops->doublespace // neither
						 ? DOUBLE_SPACE
						 : ""))),
			      imodule.getRenderHeader(),
			      get_css_references(imodule),
			      ((mf->old_font) ? mf->old_font : ""),
			      mf->old_font_size_value);
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
				   ? "rtl"
				   : "ltr"));

	main_set_global_options(ops);

	if (!valid_scripture_key) {
		swbuf.append(no_content);
		swbuf.append("</div></font></body></html>");
		HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);
		free_font(mf);
		mf = NULL;
		g_free(ops);
		ops = NULL;
		return 0;
	}

	getVerseBefore(imodule);

	for (key->setVerse(1);
	     (key->getBook() == curBook) && (key->getChapter() == curChapter) && !imodule.popError();
	     imodule++) {

		ModuleCache::CacheVerse &cVerse = ModuleMap
		    [ModuleName]
		    [key->getTestament()]
		    [key->getBook()]
		    [key->getChapter()]
		    [key->getVerse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.HeaderIsValid())
			CacheHeader(cVerse, imodule, ops, be);

		if (cache_flags & ModuleCache::Headings) {
			swbuf.append(settings.imageresize
					 ? AnalyzeForImageSize(cVerse.GetHeader(),
							       GDK_WINDOW(gtk_widget_get_window(gtkText)))
					 : cVerse.GetHeader() /* left as-is */);
		} else
			cVerse.InvalidateHeader();

		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
						  ? block_render(imodule.renderText().c_str())
						  : imodule.renderText().c_str());
			rework = CleanupContent(rework, ops, imodule.getName());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());

		// special contrasty highlighting
		marked_element *e;
		if (((e = marked_cache_check(key->getVerse())) &&
		     settings.annotate_highlight) ||
		    ((key->getVerse() == curVerse) &&
		     settings.versehighlight)) {
			buf = g_strdup_printf(
			    "<span style=\"background-color: %s\">"
			    "<font face=\"%s\" size=\"%+d\">",
			    ((settings.annotate_highlight && e)
				 ? settings.highlight_fg
				 : settings.highlight_bg),
			    ((mf->old_font) ? mf->old_font : ""),
			    mf->old_font_size_value);
			swbuf.append(buf);
			g_free(buf);
		}

		gchar *num = main_format_number(key->getVerse());
		buf = g_strdup_printf(settings.showversenum
					  ? "&nbsp; <span class=\"word\"><a name=\"%d\" href=\"sword:///%s\">"
					    "<font size=\"%+d\" color=\"%s\">%s%s%s%s%s%s%s</font></a></span>&nbsp;"
					  : "&nbsp; <a name=\"%d\"> </a>",
				      key->getVerse(),
				      (char *)key->getText(),
				      settings.verse_num_font_size + settings.base_font_size,
				      (((settings.versehighlight && (key->getVerse() == curVerse)) ||
					(e && settings.annotate_highlight))
					   ? ((e && settings.annotate_highlight)
						  ? settings.highlight_bg
						  : settings.highlight_fg)
					   : settings.bible_verse_num_color),
				      PRETTYPRINT(num));
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		// insert the userfootnote reference
		if (e) {
			buf = g_strdup_printf("<span class=\"word\">"
					      "<a href=\"passagestudy.jsp?action=showUserNote&"
					      "module=%s&passage=%s&value=%s\"><small>"
					      "<sup>*u</sup></small></a></span>&nbsp;",
					      settings.MainWindowModule,
					      (char *)key->getShortText(),
					      e->annotation->str);
			swbuf.append(buf);
			g_free(buf);
		}

		if ((key->getVerse() == curVerse) || (e && settings.annotate_highlight)) {
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

		swbuf.append(settings.imageresize
				 ? AnalyzeForImageSize(rework->str,
						       GDK_WINDOW(gtk_widget_get_window(gtkText)))
				 : rework->str /* left as-is */);

		if (key->getVerse() == curVerse) {
			swbuf.append("</font>");
			ReadAloud(curVerse, rework->str);
		}

		if (settings.versestyle) {
			if ((key->getVerse() != curVerse) ||
			    (!settings.versehighlight &&
			     (!e || !settings.annotate_highlight)))
				swbuf.append("<br/>");
			else if (key->getVerse() == curVerse)
				swbuf.append("<br/>");
		}

		// special contrasty highlighting
		if (((key->getVerse() == curVerse) && settings.versehighlight) ||
		    (e && settings.annotate_highlight))
			swbuf.append("</font></span>");
	}

	getVerseAfter(imodule);

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->setBook(curBook);
	key->setChapter(curChapter);
	key->setVerse(curVerse);

	swbuf.append("</div></font></body></html>");

	if (strongs_and_morph && (curVerse != 1))
		buf = g_strdup_printf("%d", curVerse);
	else if ((curVerse == 1) || (display_boundary >= curVerse))
		buf = g_strdup("TOP");
	else if (curVerse > display_boundary)
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
DialogEntryDisp::displayByChapter(SWModule &imodule)
{
	imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *) imodule;
	int curVerse = key->getVerse();
	int curChapter = key->getChapter();
	int curBook = key->getBook();
	gchar *buf;
	const char *ModuleName = imodule.getName();
	GString *rework; // for image size analysis rework.
	footnote = xref = 0;

	// we come into this routine with swbuf init'd with
	// boilerplate html startup, plus ops and mf ready.

	cache_flags = ConstructFlags(ops);
	is_rtol = main_is_mod_rtol(ModuleName);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph = ((ops->strongs || ops->lemmas) ||
			    ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);
	set_render_numbers(imodule, ops);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
				   ? "rtl"
				   : "ltr"));

	for (key->setVerse(1);
	     (key->getBook() == curBook) && (key->getChapter() == curChapter) && !imodule.popError();
	     imodule++) {

		ModuleCache::CacheVerse &cVerse = ModuleMap
		    [ModuleName]
		    [key->getTestament()]
		    [key->getBook()]
		    [key->getChapter()]
		    [key->getVerse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
						  ? block_render(imodule.renderText().c_str())
						  : imodule.renderText().c_str());
			rework = CleanupContent(rework, ops, imodule.getName());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());
		buf = g_strdup_printf("<p /><a name=\"%d\"> </a>",
				      key->getVerse());
		swbuf.append(buf);
		g_free(buf);
		swbuf.append(settings.imageresize
				 ? AnalyzeForImageSize(rework->str,
						       GDK_WINDOW(gtk_widget_get_window(gtkText)))
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
DialogEntryDisp::display(SWModule &imodule)
{
	swbuf = "";
	char *buf;
	mf = get_font(imodule.getName());
	ops = main_new_globals(imodule.getName());
	main_set_global_options(ops);
	GString *rework; // for image size analysis rework.
	footnote = xref = 0;

	imodule.getRawEntry(); // snap to entry

	buf = g_strdup_printf(HTML_START
			      "<font face=\"%s\" size=\"%+d\">"
			      "<font color=\"%s\">"
			      "<a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font><br/>",
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      (ops->doublespace ? DOUBLE_SPACE : ""),
			      imodule.getRenderHeader(),
			      get_css_references(imodule),
			      ((mf->old_font) ? mf->old_font : ""),
			      mf->old_font_size_value,
			      settings.bible_verse_num_color,
			      imodule.getDescription(),
			      imodule.getName(),
			      imodule.getName());
	swbuf.append(buf);
	g_free(buf);

	if (!valid_scripture_key) {
		swbuf.append(no_content);
		swbuf.append("</div></font></body></html>");
		HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);
		free_font(mf);
		mf = NULL;
		g_free(ops);
		ops = NULL;
		return 0;
	}

	//
	// the rest of this routine is irrelevant if we are
	// instead heading off to show a whole chapter
	//
	if (ops->commentary_by_chapter)
		return displayByChapter(imodule);

	if (be->module_type(imodule.getName()) == COMMENTARY_TYPE) {
		VerseKey *key = (VerseKey *)(SWKey *) imodule;
		cache_flags = ConstructFlags(ops);
		const char *ModuleName = imodule.getName();

		ModuleCache::CacheVerse &cVerse = ModuleMap
		    [ModuleName]
		    [key->getTestament()]
		    [key->getBook()]
		    [key->getChapter()]
		    [key->getVerse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(imodule.renderText().c_str());
			rework = CleanupContent(rework, ops, imodule.getName());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());

	} else {
		if ((be->module_type(imodule.getName()) == PERCOM_TYPE) ||
		    (be->module_type(imodule.getName()) == PRAYERLIST_TYPE))
			rework = g_string_new(imodule.getRawEntry());
		else
			rework = g_string_new(imodule.renderText().c_str());
		rework = CleanupContent(rework, ops, imodule.getName());
	}

	swbuf.append(settings.imageresize
			 ? AnalyzeForImageSize(rework->str,
					       GDK_WINDOW(gtk_widget_get_window(gtkText)))
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
DialogChapDisp::display(SWModule &imodule)
{
	imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *) imodule;
	int curVerse = key->getVerse();
	int curChapter = key->getChapter();
	int curBook = key->getBook();
	gchar *buf;
	GString *rework; // for image size analysis rework.

	const char *ModuleName = imodule.getName();
	ops = main_new_globals(ModuleName);
	cache_flags = ConstructFlags(ops);

	is_rtol = main_is_mod_rtol(ModuleName);
	mf = get_font(ModuleName);

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph = ((ops->strongs || ops->lemmas) ||
			    ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);
	set_render_numbers(imodule, ops);

	// when strongs/morph are on, the anchor boundary must be smaller.
	// or if main window is too small to keep curverse in-pane,
	// of it the user wants really big fonts.
	gint display_boundary = (((settings.gs_height < 500) ||
				  (mf->old_font_size_value > 2))
				     ? 0
				     : (strongs_or_morph ? 1 : 2));

	// if we are no longer where annotations were current, re-load.
	if (strcasecmp(ModuleName,
		       (marked_cache_modname ? marked_cache_modname : "")) ||
	    strcasecmp(key->getBookName(), marked_cache_book) ||
	    (curChapter != marked_cache_chapter))
		marked_cache_fill(ModuleName, (gchar *)key->getShortText());

	gint versestyle = ops->verse_per_line;

	main_set_global_options(ops);

	swbuf = "";
	footnote = xref = 0;
	buf = g_strdup_printf(HTML_START
			      "<font face=\"%s\" size=\"%+d\">",
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      (strongs_and_morph
				   ? CSS_BLOCK_BOTH
				   : (strongs_or_morph
					  ? CSS_BLOCK_ONE
					  : (ops->doublespace
						 ? DOUBLE_SPACE
						 : ""))),
			      imodule.getRenderHeader(),
			      get_css_references(imodule),
			      ((mf->old_font) ? mf->old_font : ""),
			      mf->old_font_size_value);
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
				   ? "rtl"
				   : "ltr"));

	if (!valid_scripture_key) {
		swbuf.append(no_content);
		swbuf.append("</div></font></body></html>");
		HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);
		free_font(mf);
		mf = NULL;
		g_free(ops);
		ops = NULL;
		return 0;
	}

	for (key->setVerse(1);
	     (key->getBook() == curBook) && (key->getChapter() == curChapter) && !imodule.popError();
	     imodule++) {

		ModuleCache::CacheVerse &cVerse = ModuleMap
		    [ModuleName]
		    [key->getTestament()]
		    [key->getBook()]
		    [key->getChapter()]
		    [key->getVerse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = g_string_new(strongs_or_morph
						  ? block_render(imodule.renderText().c_str())
						  : imodule.renderText().c_str());
			rework = CleanupContent(rework, ops, imodule.getName());
			cVerse.SetText(rework->str, cache_flags);
		} else
			rework = g_string_new(cVerse.GetText());

		if (!cVerse.HeaderIsValid())
			CacheHeader(cVerse, imodule, ops, be);

		if (cache_flags & ModuleCache::Headings)
			swbuf.append(settings.imageresize
					 ? AnalyzeForImageSize(cVerse.GetHeader(),
							       GDK_WINDOW(gtk_widget_get_window(gtkText)))
					 : cVerse.GetHeader() /* left as-is */);
		else
			cVerse.InvalidateHeader();

		// special contrasty highlighting
		marked_element *e;
		if (((e = marked_cache_check(key->getVerse())) &&
		     settings.annotate_highlight) ||
		    ((key->getVerse() == curVerse) && settings.versehighlight)) {
			buf = g_strdup_printf(
			    "<span style=\"background-color: %s\">"
			    "<font face=\"%s\" size=\"%+d\">",
			    ((settings.annotate_highlight && e)
				 ? settings.highlight_fg
				 : settings.highlight_bg),
			    ((mf->old_font) ? mf->old_font : ""),
			    mf->old_font_size_value);
			swbuf.append(buf);
			g_free(buf);
		}

		gchar *num = main_format_number(key->getVerse());
		buf = g_strdup_printf(settings.showversenum
					  ? "&nbsp; <a name=\"%d\" href=\"sword:///%s\">"
					    "<font size=\"%+d\" color=\"%s\">%s%s%s%s%s%s%s</font></a>&nbsp;"
					  : "&nbsp; <a name=\"%d\"> </a>",
				      key->getVerse(),
				      (char *)key->getText(),
				      settings.verse_num_font_size + settings.base_font_size,
				      (((settings.versehighlight && (key->getVerse() == curVerse)) ||
					(e && settings.annotate_highlight))
					   ? ((e && settings.annotate_highlight)
						  ? settings.highlight_bg
						  : settings.highlight_fg)
					   : settings.bible_verse_num_color),
				      PRETTYPRINT(num));
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		// insert the userfootnote reference
		if (e) {
			buf = g_strdup_printf("<span class=\"word\">"
					      "<a href=\"passagestudy.jsp?action=showUserNote&"
					      "module=%s&passage=%s&value=%s\">"
					      "<small><sup>*u</sup></small></a></span> ",
					      /*xxx*/ settings.MainWindowModule,
					      (char *)key->getShortText(),
					      e->annotation->str);
			swbuf.append(buf);
			g_free(buf);
		}

		if ((key->getVerse() == curVerse) || (e && settings.annotate_highlight)) {
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

		swbuf.append(settings.imageresize
				 ? AnalyzeForImageSize(rework->str,
						       GDK_WINDOW(gtk_widget_get_window(gtkText)))
				 : rework->str /* left as-is */);

		if (key->getVerse() == curVerse) {
			swbuf.append("</font>");
			ReadAloud(curVerse, cVerse.GetText());
		}

		if (versestyle) {
			if ((key->getVerse() != curVerse) ||
			    (!settings.versehighlight &&
			     (!e || !settings.annotate_highlight)))
				swbuf.append("<br/>");
			else if (key->getVerse() == curVerse)
				swbuf.append("<br/>");
		}

		// special contrasty highlighting
		if (((key->getVerse() == curVerse) && settings.versehighlight) ||
		    (e && settings.annotate_highlight))
			swbuf.append("</font></span>");
	}

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->setBook(curBook);
	key->setChapter(curChapter);
	key->setVerse(curVerse);

	swbuf.append("</div></font></body></html>");

	if (strongs_and_morph && (curVerse != 1))
		buf = g_strdup_printf("%d", curVerse);
	else if ((curVerse == 1) || (display_boundary >= curVerse))
		buf = g_strdup("TOP");
	else if (curVerse > display_boundary)
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
GTKPrintEntryDisp::display(SWModule &imodule)
{
	gchar *keytext = NULL;
	gchar *buf;
	SWBuf swbuf = "";
	gint mod_type;
	MOD_FONT *mf = get_font(imodule.getName());

	GLOBAL_OPS *ops = main_new_globals(imodule.getName());

	imodule.getRawEntry(); // snap to entry
	XI_message(("%s", (const char *)imodule.getRawEntry()));
	main_set_global_options(ops);
	mod_type = backend->module_type(imodule.getName());

	if (mod_type == BOOK_TYPE)
		keytext = strdup(backend->treekey_get_local_name(
		    settings.book_offset));
	else if (mod_type == DICTIONARY_TYPE)
		keytext = g_strdup((char *)imodule.getKeyText());
	else
		keytext = strdup((char *)imodule.getKeyText());

	buf = g_strdup_printf(HTML_START
			      "<font face=\"%s\" size=\"%+d\">"
			      "<font color=\"%s\">"
			      "<a href=\"passagestudy.jsp?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font>[%s]<br/>",
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      (ops->doublespace ? DOUBLE_SPACE : ""),
			      imodule.getRenderHeader(),
			      get_css_references(imodule),
			      ((mf->old_font) ? mf->old_font : ""),
			      mf->old_font_size_value,
			      settings.bible_verse_num_color,
			      imodule.getDescription(),
			      imodule.getName(),
			      imodule.getName(),
			      (gchar *)keytext);
	swbuf.append(buf);
	g_free(buf);

	swbuf.append(imodule.renderText());
	swbuf.append("</font></body></html>");

	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);
	free_font(mf);
	g_free(ops);
	if (keytext)
		g_free(keytext);

	return 0;
}

char
GTKPrintChapDisp::display(SWModule &imodule)
{
	imodule.setSkipConsecutiveLinks(true);
	VerseKey *key = (VerseKey *)(SWKey *) imodule;
	int curVerse = key->getVerse();
	int curChapter = key->getChapter();
	int curBook = key->getBook();
	gchar *buf;
	gchar heading[32];
	SWBuf swbuf;

	GLOBAL_OPS *ops = main_new_globals(imodule.getName());
	gboolean is_rtol = main_is_mod_rtol(imodule.getName());
	mf = get_font(imodule.getName());

	swbuf = "";

	buf = g_strdup_printf(HTML_START
			      "<font face=\"%s\" size=\"%+d\">",
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color,
			      (ops->doublespace ? DOUBLE_SPACE : ""),
			      imodule.getRenderHeader(),
			      get_css_references(imodule),
			      ((mf->old_font) ? mf->old_font : ""),
			      mf->old_font_size_value);
	swbuf.append(buf);
	g_free(buf);

	swbuf.appendFormatted("<div dir=%s>",
			      ((is_rtol && !ops->transliteration)
				   ? "rtl"
				   : "ltr"));

	main_set_global_options(ops);

	for (key->setVerse(1);
	     (key->getBook() == curBook) && (key->getChapter() == curChapter) && !imodule.popError();
	     imodule++) {
		int x = 0;
		gchar *preverse;
		sprintf(heading, "%d", x);

		while ((preverse = backend->get_entry_attribute("Heading", "Preverse",
								heading)) != NULL) {
			SWBuf preverse2 = imodule.renderText(preverse);
			buf = g_strdup_printf("%s", preverse2.c_str());
			swbuf.append(buf);
			g_free(buf);
			g_free(preverse);
			++x;
			sprintf(heading, "%d", x);
		}

		gchar *num = main_format_number(key->getVerse());
		buf = g_strdup_printf(settings.showversenum
					  ? "&nbsp; <a name=\"%d\" href=\"sword:///%s\">"
					    "<font size=\"%+d\" color=\"%s\">%s%s%s%s%s%s%s</font></a>&nbsp;"
					  : "&nbsp; <a name=\"%d\"> </a>",
				      key->getVerse(),
				      (char *)key->getText(),
				      settings.verse_num_font_size + settings.base_font_size,
				      settings.bible_verse_num_color,
				      PRETTYPRINT(num));
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		swbuf.append(imodule.renderText());

		buf = g_strdup_printf("%s", imodule.renderText().c_str());

		if (settings.versestyle) {
			swbuf.append("<br/>");
		}

		g_free(buf);
	}

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->setBook(curBook);
	key->setChapter(curChapter);
	key->setVerse(curVerse);

	swbuf.append("</div></font></body></html>");

	HtmlOutput((char *)swbuf.c_str(), gtkText, mf, NULL);

	free_font(mf);
	g_free(ops);

	return 0;
}
