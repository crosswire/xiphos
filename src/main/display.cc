/*
 * Xiphos Bible Study Tool
 * display.cc -
 *
 * Copyright (C) 2000-2009 Xiphos Developer Team
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
#include <gdk-pixbuf/gdk-pixbuf.h>

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

#ifndef WIN32
#include <sys/socket.h>
#include <netinet/in.h>
#else
#include <winsock2.h>
#endif /* !WIN32 */
#include <errno.h>

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
#ifdef WIN32
#include "geckowin/gecko-html.h"
#else
#include "gecko/gecko-html.h"
#endif /* WIN32 */
#else
#ifdef __cplusplus
extern "C" {
#include <gtkhtml/gtkhtml.h>
#include <gtkhtml/htmltypes.h>
}
#endif /* __cplusplus */
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

#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><STYLE type=\"text/css\"><!-- A { text-decoration:none } %s --></STYLE></head>"

// CSS style blocks to control blocked strongs+morph output
// BOTH is when the user wants to see both types of markup.
// ONE is when he wants one or the other, so we can use a single
// specification which overlays both on top of one another.
#define CSS_BLOCK_BOTH \
" *        { line-height: 3.8em; }" \
" .word    { position: relative; top: -1.1em; left: 0 }" \
" .strongs { position: absolute; top:  0.1em; left: 0; z-index: 2; height: 1em }" \
" .morph   { position: absolute; top:  1.1em; left: 0; z-index: 1 }"
#define CSS_BLOCK_ONE \
" *        { line-height: 2.7em; }" \
" .word    { position: relative; top: -0.7em; left: 0 }" \
" .strongs { position: absolute; top:  0.9em; left: 0 }" \
" .morph   { position: absolute; top:  0.9em; left: 0 }"

#define DOUBLE_SPACE " * { line-height: 2em ! important; }"

using namespace sword;
using namespace std;

int
ImageDimensions(const char *path, int *x, int *y)
{

	if (gdk_pixbuf_get_file_info (path, x, y))
		return 0;
	else
		return -1;
}

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
	int key_chapter, key_verse;

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
	key_verse   = atoi(s+1);

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

#ifndef HAVE_STRCASESTR
/*
 * strcasestr() turns out to be nonstandard extension, but we need it.
 */
char *
strcasestr(const char *haystack, const char *needle)
{
	char *lower_haystack = g_strdup(haystack);
	char *lower_needle = g_strdup(needle);
	char *s;

	for (s = lower_haystack; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);
	for (s = lower_needle; *s; ++s)
		if (isupper(*s))
			*s = tolower(*s);

	s = strstr(lower_haystack, lower_needle);
	if (s)
		s = (char *)haystack + (s - lower_haystack);

	g_free(lower_haystack);
	g_free(lower_needle);
	return s;
}
#endif /* !HAVE_STRCASESTR */

#define	IMGSRC_LENGTH	10	// strlen('<img src="')

const char *
AnalyzeForImageSize(const char *origtext,
		    GdkWindow *window)
{
	static SWBuf resized;
	SWBuf text;

	const char *trail;	// "trail" trails behind ...
	char *path;		// ... the current "path".
	char *end, save;	// "end" is the path's end.
	char buf[32];		// for preparing new width+height spec.
	gint image_x, image_y, window_x, window_y = -999;
	int image_retval;
	bool no_warning_yet = true;

	// performance tweak:
	// image content is by no means common. therefore, spend an extra
	// search call to determine whether any of the rest is needed,
	// most especially to stop copying large blocks of text w/no images.
	if (strcasestr(origtext, "<img src=\"") == NULL)
		return origtext;

	text = origtext;
	resized = "";
	trail = text;

	for (path = (char*)strcasestr(text, "<img src=\"");
	     path;
	     path = strcasestr(path, "<img src=\"")) {

		if (window_y == -999) {
			/* we have images, but we don't know bounds yet */

			gdk_drawable_get_size(window, &window_x, &window_y);
			if ((window_x > 200) || (window_y > 200)) {
				window_x -= 23;
				window_y -= 23;
			} else {
				window_x = (window_x * 93)/100;
				window_y = (window_y * 93)/100;
			}
		}

		path += IMGSRC_LENGTH;
		save = *path;
		*path = '\0';
                resized += trail;
		*path = save;
		trail = path;

		// some play fast-n-loose with proper file spec.
		if (strncmp(path, "file://", 7) == 0) {
			path += 7;
			resized += "file://";
		} else if (strncmp(path, "file:", 5) == 0) {
			path += 5;
			resized += "file:";
		} else
			continue;	// no file spec there -- odd.

		// getting this far means we have a valid img src and file.
		// find closing '"' to determine pathname end.
		if ((end = strchr(path, '"')) == 0)
			continue;

		*end = '\0';
		resized += path;
		image_retval = ImageDimensions(path, &image_x, &image_y);

		*end = '"';
		resized += "\"";
		path = end+1;
		trail = path;

		if (image_retval != 0) {
			if (no_warning_yet) {
				gui_generic_warning(
				    _("An image file's size could not be determined.\nIs ImageMagick's 'identify' not installed?\n(Or possibly the image is of type other than bmp/gif/jpg/png.)\nXiphos cannot resize images to fit window."));
				// settings.imageresize = 0;
				no_warning_yet = false;
			}
			continue;
		}

		// knowing image size & window size, adjust to fit.
		if (image_x > window_x) {
			float proportion = (float)window_x / (float)image_x;
			image_x = window_x;
			image_y = (int)((float)image_y * proportion);
		}
		if (image_y > window_y) {
			float proportion = (float)window_y / (float)image_y;
			image_y = window_y;
			image_x = (int)((float)image_x * proportion);
		}
		sprintf(buf, " WIDTH=%d HEIGHT=%d", image_x, image_y);
		resized += buf;
        }

	resized += trail;	// remainder of text appended.
	return resized.c_str();
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
	} else
		slen = 0;
	if (*morph) {
		s = s0 = (char*)strstr(*morph, "\">") + 2;
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
				if (*(s+11) == '(') {
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
// in-place removal of inconvenient-to-the-user content.
//
void
CleanupContent(const char *text,
	       GLOBAL_OPS *ops,
	       char *name)
{
	if (ops->image_content == 0)
		ClearImages((gchar *)text);
	else if ((ops->image_content == -1) &&	// "unknown"
		 (strcasestr(text, "<img ") != NULL)) {
		ops->image_content = 1;		// now known.
		main_save_module_options(name, "Image Content", 1, ops->dialog);
	}
	if (ops->respect_font_faces == 0)
		ClearFontFaces((gchar *)text);
	else if ((ops->respect_font_faces == -1) &&	// "unknown"
		 (strcasestr(text, "<font face=\"Galax") != NULL)) {
		ops->respect_font_faces = 1;	// now known.
		main_save_module_options(name, "Respect Font Faces", 1, ops->dialog);
	}
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
	gchar *text;

	cVerse.SetHeader("");

	sprintf(heading, "%d", x);
	while ((preverse = be->get_entry_attribute("Heading", "Preverse",
							heading)) != NULL) {
		preverse2 = mod.RenderText(preverse);
		text = g_strdup_printf("<br><b>%s</b><br><br>",
				       (((ops->strongs || ops->lemmas) ||
					ops->morphs)
					? block_render(preverse2)
					: preverse2));
		CleanupContent(text, ops, mod.Name());

		cVerse.AppendHeader(text);
		g_free((gchar *)text);
		// g_free(preverse2); // does RenderText's result need free()?
		g_free((gchar *)preverse);
		++x;
		sprintf(heading, "%d", x);
	}
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
	gchar *buf;
	char *ModuleName = imodule.Name();
	const char *rework;	// for image size analysis rework.

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
			rework = (strongs_or_morph
				  ? block_render(imodule.RenderText())
				  : imodule.RenderText());
			CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework, cache_flags);
		} else
			rework = cVerse.GetText();
		buf = g_strdup_printf("<p /><a name=\"%d\"> </a>",
				      key->Verse());
		swbuf.append(buf);
		g_free(buf);
		swbuf.append(settings.imageresize
			     ? AnalyzeForImageSize(rework,
						   GDK_WINDOW(gtkText->window))
			     : rework /* left as-is */);
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

//
// general display of entries: commentary, genbook, lexdict
//
char
GTKEntryDisp::Display(SWModule &imodule)
{
#ifdef USE_GTKMOZEMBED
	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText)))
		gtk_widget_realize(gtkText);
#endif

	gchar *buf;
	mf = get_font(imodule.Name());
	swbuf = "";

	ops = main_new_globals(imodule.Name(),0);

	const char *rework;	// for image size analysis rework.

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
			      "<font color=\"%s\">"
			      "<a href=\"xiphos.url?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font><br>",
			      (strongs_and_morph		// both
			       ? CSS_BLOCK_BOTH
			       : (strongs_or_morph		// either
				  ? CSS_BLOCK_ONE
				  : (settings.doublespace	// neither
				     ? DOUBLE_SPACE
				     : ""))),
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
			rework = (strongs_or_morph
				  ? block_render(imodule.RenderText())
				  : imodule.RenderText());
			CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework, cache_flags);
		} else
			rework = cVerse.GetText();

	} else {

		if ((backend->module_type(imodule.Name()) == PERCOM_TYPE) ||
		    (backend->module_type(imodule.Name()) == PRAYERLIST_TYPE))
			rework = (strongs_or_morph
				  ? block_render(imodule.getRawEntry())
				  : imodule.getRawEntry());
		else
			rework = (strongs_or_morph
				  ? block_render(imodule.RenderText())
				  : imodule.RenderText());
		CleanupContent(rework, ops, imodule.Name());
	}

	swbuf.append(settings.imageresize
		     ? AnalyzeForImageSize(rework,
					   GDK_WINDOW(gtkText->window))
		     : rework /* left as-is */);

	swbuf.append("</font></body></html>");

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
	gchar *utf8_key;
	gchar *buf;
	char *num;

	sword::VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int chapter = key->Chapter();
	int curBook = key->Book();
	int curTest = key->Testament();
	const char *ModuleName = imodule.Name();

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

		utf8_key = strdup((char*)key->getText());

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
				utf8_key,
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
				"%s%s<br><hr><div style=\"text-align: center\"><b>%s %s</b></div>",
				cVerse.GetText(),
				// extra break when excess strongs/morph space.
				(strongs_or_morph ? "<br>" : ""),
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
	gchar *utf8_key;
	gchar *buf;
	const char *ModuleName = imodule.Name();
	sword::VerseKey *key = (VerseKey *)(SWKey *)imodule;

	imodule++;
	if (imodule.Error()) {
		buf=g_strdup_printf(
			"%s<hr><div style=\"text-align: center\"><p><b>%s</b></p></div>",
			// extra break when excess strongs/morph space.
			(strongs_or_morph ? "<br>" : ""),
			imodule.Description());
		swbuf.append(buf);
		g_free(buf);
	} else {
		imodule--;
		char *num = main_format_number(key->Chapter());

		buf=g_strdup_printf(
			"%s<hr><div style=\"text-align: center\"><b>%s %s</b></div>",
			(strongs_or_morph ? "<br>" : ""),
			_("Chapter"), num);
		swbuf.append(buf);
		g_free(buf);
		g_free(num);

		utf8_key = strdup((char*)key->getText());

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
				utf8_key,
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
		swbuf.append("</div>");
	}
}

//
// Read aloud some text, i.e. the current verse.
// Text is cleaned of '"', <tokens>, "&symbols;", and *n/*x strings,
// then scribbled out the local static socket with (SayText "...").
// Non-zero verse param is prefixed onto supplied text.
//

#ifndef INVALID_SOCKET
# define INVALID_SOCKET -1
#endif

void
ReadAloud(unsigned int verse, const char *suppliedtext)
{
	static int tts_socket = INVALID_SOCKET;	// no initial connection.
	static int use_counter = -2;	// to shortcircuit early uses.

	if (settings.readaloud ||       // read anything, or
	    (verse == 0)) {		// read what's handed us.
		gchar *s, *t;

		// setup for communication.
		if (tts_socket < 0) {
			struct sockaddr_in service;

			if ((tts_socket = socket (AF_INET, SOCK_STREAM, 0)) < 0) {
				char msg[256];
				sprintf(msg, "ReadAloud disabled:\nsocket failed, %s",
					strerror(errno));
				settings.readaloud = 0;
				gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
							       (widgets.readaloud_item),
							       settings.readaloud);
				gui_generic_warning(msg);
				return;
			}

			// festival's port (1314) on localhost (127.0.0.1).
			memset(&service, 0, sizeof service);
			service.sin_family = AF_INET;
			service.sin_port = htons(1314);
			service.sin_addr.s_addr = htonl(0x7f000001);
			if (connect(tts_socket, (const sockaddr *)&service,
				    sizeof(service)) != 0) {
				StartFestival();
#ifdef WIN32
				Sleep(2); // give festival a moment to init.
#else
				sleep(2); // give festival a moment to init.
#endif
				if (connect(tts_socket, (const sockaddr *)&service,
					    sizeof(service)) != 0) {
					// it still didn't work -- missing.
					char msg[256];
					sprintf(msg, "%s\n%s, %s",
						"TTS \"festival\" not started -- perhaps not installed",
						"TTS connect failed", strerror(errno));
					StopFestival(&tts_socket);
					settings.readaloud = 0;
					gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
								       (widgets.readaloud_item),
								       settings.readaloud);
					gui_generic_warning(msg);
					return;
				}
			}
		}

		// avoid speaking the first *2* times.
		// (2 Display() calls are made during startup.)
		// though speaking may be intended, startup speech is annoying.
		if (verse && (++use_counter < 1))
			return;

		GString *text = g_string_new(NULL);
		if (verse != 0)
			g_string_printf(text, "%d. ...  %s", verse, suppliedtext);
			// use of ". ..." is to induce proper pauses.
		else
			g_string_printf(text, "%s", suppliedtext);
		GS_message(("ReadAloud: dirty: %s\n", text->str));

		// clean: no <span> surrounding strongs/morph.
		// i wish there was a regexp form of strstr().
		for (s = strstr(text->str, "<span class=\"strongs\">");
		     s;
		     s = strstr(s, "<span class=\"strongs\">")) {
			if ((t = strstr(s, "</span>"))) {
				t += 6;
				while (s <= t)
					*(s++) = ' ';
			} else {
				GS_message(("ReadAloud: Unmatched <span strong></span> in %s\n", s));
				goto out;
			}
		}
		for (s = strstr(text->str, "<span class=\"morph\">");
		     s;
		     s = strstr(s, "<span class=\"morph\">")) {
			if ((t = strstr(s, "</span>"))) {
				t += 6;
				while (s <= t)
					*(s++) = ' ';
			} else {
				GS_message(("ReadAloud: Unmatched <span morph></span> in %s\n", s));
				goto out;
			}
		}

		// clean: no quotes (conflict w/festival syntax).
		for (s = strchr(text->str, '"'); s; s = strchr(s, '"'))
			*s = ' ';

		// clean: no <tokens>.
		for (s = strchr(text->str, '<'); s; s = strchr(s, '<')) {
			if ((t = strchr(s, '>'))) {
				while (s <= t)
					*(s++) = ' ';
			} else {
				GS_message(("ReadAloud: Unmatched <> in %s\n", s));
				goto out;
			}
		}

		// clean: no &lt;...&gt; sequences.  (Strong's ref, "<1234>".)
		for (s = strstr(text->str, "&lt;"); s; s = strstr(s, "&lt;")) {
			if ((t = strstr(s, "&gt;"))) {
				t += 3;
				while (s <= t)
					*(s++) = ' ';
			} else {
				GS_message(("ReadAloud: Unmatched &lt;&gt; in %s\n", s));
				goto out;
			}
		}

		// clean: no other &symbols;.
		for (s = strchr(text->str, '&'); s; s = strchr(s, '&')) {
			if ((t = strchr(s, ';'))) {
				while (s <= t)
					*(s++) = ' ';
			} else {
				GS_message(("ReadAloud: Incomplete &xxx; in %s\n", s));
				goto out;
			}
		}

		// clean: no note/xref strings.
		for (s = strstr(text->str, "*n"); s; s = strstr(s, "*n")) {
			*(s++) = ' ';
			*(s++) = ' ';
		}
		for (s = strstr(text->str, "*u"); s; s = strstr(s, "*u")) {
			*(s++) = ' ';
			*(s++) = ' ';
		}
		for (s = strstr(text->str, "*x"); s; s = strstr(s, "*x")) {
			*(s++) = ' ';
			*(s++) = ' ';
		}

		// festival *pronounces* brackets and asterisks -- idiots.
		for (s = strchr(text->str, '['); s; s = strchr(s, '['))
			*s = ' ';
		for (s = strchr(text->str, ']'); s; s = strchr(s, ']'))
			*s = ' ';
		for (s = strchr(text->str, '*'); s; s = strchr(s, '*'))
			*s = ' ';
		// in case it isn't obvious, i'd really like a  standard
		// function that walks a string for multiple individual chars.
		GS_message(("ReadAloud: clean: %s\n", text->str));
		// scribble clean text to the socket.
		if (FestivalSpeak(text->str, strlen(text->str), tts_socket) == false) 
		{
			char msg[256];
			sprintf(msg, "TTS disappeared?\nTTS write failed: %s",
				strerror(errno));
			StopFestival(&tts_socket);
			settings.readaloud = 0;
			gtk_check_menu_item_set_active(GTK_CHECK_MENU_ITEM
						       (widgets.readaloud_item),
						       settings.readaloud);
			gui_generic_warning(msg);
		}

	out:
		g_string_free(text, TRUE);
		return;

	} else {

		// Reading aloud is disabled.
		// If we had been reading, shut it down.
		if (tts_socket >= 0) {
			StopFestival(&tts_socket);
		}
		use_counter++;
		return;
	}
}

//
// starts festival in a async process
//
void
StartFestival()
{
#ifdef WIN32
	//on windows, we will ship festival directly under Xiphos
	gchar *festival_args[5];
        gchar *festival_com = g_win32_get_package_installation_directory_of_module(NULL);
	festival_com = g_strconcat(festival_com, "\0", NULL);
	gchar *festival_lib = g_build_filename(festival_com, "festival\\lib\0");
	festival_com = g_build_filename(festival_com, "festival\\festival.exe\0");
	festival_args[0] = festival_com;
	festival_args[1] = g_strdup("--libdir");
	festival_args[2] = festival_lib;
	festival_args[3] = g_strdup("--server");
	festival_args[4] = NULL;
#else
	gchar *festival_args[3];
	festival_args[0] = g_strdup("festival");
	festival_args[1] = g_strdup("--server");
	festival_args[2] = NULL;
#endif
	g_spawn_async ( NULL,
			festival_args,
			NULL,
			G_SPAWN_SEARCH_PATH,
			NULL,
			NULL,
			NULL,
			NULL);
}

//
// shuts down Festival
//
void
StopFestival(int *tts_socket)
{
#ifdef WIN32
	closesocket(*tts_socket);
#else
	shutdown(*tts_socket, SHUT_RDWR);
	close(*tts_socket);
#endif
	*tts_socket = INVALID_SOCKET;
}

//
// tells Festival to say the given text
//
gboolean
FestivalSpeak(gchar *text, int length, int tts_socket)
{
#ifdef WIN32
	if ((send(tts_socket, "(SayText \"", 10, MSG_DONTROUTE) == INVALID_SOCKET)  ||
	    (send(tts_socket, text, length, MSG_DONTROUTE) == INVALID_SOCKET) ||
	    (send(tts_socket, "\")\r\n", 4, MSG_DONTROUTE) == INVALID_SOCKET ))
		return false;
#else
	if ((write(tts_socket, "(SayText \"", 10) < 0)  ||
	    (write(tts_socket, text, length ) < 0) ||
	    (write(tts_socket, "\")\r\n", 4) < 0))
		return false;
#endif
	return true;
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
	const char *rework;	// for image size analysis rework.
	marked_element *e = NULL;

	char *ModuleName = imodule.Name();
	ops = main_new_globals(ModuleName, 0);
	cache_flags = ConstructFlags(ops);

	is_rtol = main_is_mod_rtol(ModuleName);
	mf = get_font(ModuleName);

	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;

	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
			     ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);

	// when strongs/morph are on, the anchor boundary must be smaller.
	// or if main window is too small to keep curverse in-pane.
	gint display_boundary = ((settings.gs_hight < 500) ? 0 : (strongs_or_morph ? 1 : 2));

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

	buf=g_strdup_printf(HTML_START
			    "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			    "<font face=\"%s\" size=\"%+d\">",
			    // strongs & morph specs win over dblspc.
			    (strongs_and_morph		// both
			     ? CSS_BLOCK_BOTH
			     : (strongs_or_morph	// either
				? CSS_BLOCK_ONE
				: (settings.doublespace	// neither
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
		if (!cVerse.CacheIsValid(cache_flags)) {
			rework = (strongs_or_morph
				  ? block_render(imodule.RenderText())
				  : imodule.RenderText());
			CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework, cache_flags);
		}
		rework = cVerse.GetText();

		if (!cVerse.HeaderIsValid())
			CacheHeader(cVerse, imodule, ops, be);

		if (cache_flags & ModuleCache::Headings)
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(cVerse.GetHeader(),
							   GDK_WINDOW(gtkText->window))
				     : cVerse.GetHeader() /* left as-is */);
		else
			cVerse.InvalidateHeader();

		// special contrasty highlighting
		if (((e = marked_cache_check(key->Verse()))) ||
		    ((key->Verse() == curVerse) && settings.versehighlight)) {
			buf = g_strdup_printf(
			    "<table bgcolor=\"%s\"><tr><td>"
			    "<font face=\"%s\" size=\"%+d\">",
			    (e ? settings.highlight_fg : settings.highlight_bg),
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
			(((settings.versehighlight && (key->Verse() == curVerse)) || e)
			 ? (e ? settings.highlight_bg : settings.highlight_fg)
			 : settings.bible_verse_num_color),
			num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		// insert the userfootnote reference
		if (e) {
			buf = g_strdup_printf("<span class=\"word\">"
					      "<a href=\"xiphos.url?action=showUserNote&"
					      "module=%s&passage=%s&value=%s\"><small><sup>*u</sup>"
					      "</small></a></span> ",
					      settings.MainWindowModule,
			                      (char*)key->getShortText(),
					      e->annotation->str);
			swbuf.append(buf);
			g_free(buf);
		}

		if ((key->Verse() == curVerse) || e) {
			buf = g_strdup_printf("<font color=\"%s\">",
					      ((settings.versehighlight || e)
					       ? (e ? settings.highlight_bg : settings.highlight_fg)
					       : settings.currentverse_color));
			swbuf.append(buf);
			g_free(buf);
		}

		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			swbuf.append(paragraphMark);;
		}

		// correct a highlight glitch: in poetry verses which end in
		// a forced line break, we must remove the break to prevent
		// the enclosing <table> from producing a double break.
		if (settings.versehighlight &&		// doing <table> h/l.
		    !settings.versestyle &&		// paragraph format.
		    (key->Verse() == curVerse)) {
			GString *text = g_string_new(NULL);

			g_string_printf(text, "%s", rework);
			if (!strcmp(text->str + text->len - 6, "<br />")) {
				text->len -= 6;
				*(text->str + text->len) = '\0';
			}
			else if (!strcmp(text->str + text->len - 4, "<br>")) {
				text->len -= 4;
				*(text->str + text->len) = '\0';
			}
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(text->str,
							   GDK_WINDOW(gtkText->window))
				     : text->str /* left as-is */);
			g_string_free(text, TRUE);
		} else
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(rework,
							   GDK_WINDOW(gtkText->window))
				     : rework /* left as-is */);

		if (key->Verse() == curVerse) {
			swbuf.append("</font>");
			ReadAloud(curVerse, rework);
		}

		if (settings.versestyle) {
			if (strstr(rework, "<!p>")) {
				newparagraph = TRUE;
			} else {
				newparagraph = FALSE;
			}
			if ((key->Verse() != curVerse) ||
			    !settings.versehighlight)
				swbuf.append("<br>");
		}

		// special contrasty highlighting
		if (((key->Verse() == curVerse) && settings.versehighlight) ||
		    e)
			swbuf.append("</font></td></tr></table>");
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
	const char *rework;	// for image size analysis rework.

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
			rework = (strongs_or_morph
				  ? block_render(imodule.RenderText())
				  : imodule.RenderText());
			CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework, cache_flags);
		} else
			rework = cVerse.GetText();
		buf = g_strdup_printf("<p /><a name=\"%d\"> </a>",
				      key->Verse());
		swbuf.append(buf);
		g_free(buf);
		swbuf.append(settings.imageresize
			     ? AnalyzeForImageSize(rework,
						   GDK_WINDOW(gtkText->window))
			     : rework /* left as-is */);
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
	const char *rework;	// for image size analysis rework.

	(const char *)imodule;	// snap to entry
	//main_set_global_options(ops);

	buf = g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">"
			      "<font color=\"%s\">"
			      "<a href=\"xiphos.url?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font><br>",
			      (settings.doublespace ? DOUBLE_SPACE : ""),
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
			rework = (const char *)imodule;
			CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework, cache_flags);
		} else
			rework = cVerse.GetText();

	} else {

		if ((be->module_type(imodule.Name()) == PERCOM_TYPE) ||
		    (be->module_type(imodule.Name()) == PRAYERLIST_TYPE))
			rework = imodule.getRawEntry();
		else
			rework = (const char *)imodule;
		CleanupContent(rework, ops, imodule.Name());
	}

	swbuf.append(settings.imageresize
		     ? AnalyzeForImageSize(rework,
					   GDK_WINDOW(gtkText->window))
		     : rework /* left as-is */);

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
	const char *rework;	// for image size analysis rework.
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
	if ((style) && strcmp(style, "verse"))
		versestyle = FALSE;
	else
		versestyle = TRUE;
	g_free(style);
	g_free(file);

	main_dialog_set_global_options((BackEnd*)be, ops);

	swbuf = "";
	buf = g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">",
			      (strongs_and_morph
			       ? CSS_BLOCK_BOTH
			       : (strongs_or_morph
				  ? CSS_BLOCK_ONE
				  : (settings.doublespace
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
			rework = (strongs_or_morph
				  ? block_render(imodule.RenderText())
				  : imodule.RenderText());
			CleanupContent(rework, ops, imodule.Name());
			cVerse.SetText(rework, cache_flags);
		}
		rework = cVerse.GetText();

		if (!cVerse.HeaderIsValid())
			CacheHeader(cVerse, imodule, ops, be);

		if (cache_flags & ModuleCache::Headings)
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(cVerse.GetHeader(),
							   GDK_WINDOW(gtkText->window))
				     : cVerse.GetHeader() /* left as-is */);
		else
			cVerse.InvalidateHeader();

		// special contrasty highlighting
		if (((e = marked_cache_check(key->Verse()))) ||
		    ((key->Verse() == curVerse) && settings.versehighlight)) {
			buf = g_strdup_printf(
			    "<table bgcolor=\"%s\"><tr><td>"
			    "<font face=\"%s\" size=\"%+d\">",
			    (e ? settings.highlight_fg : settings.highlight_bg),
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
			(((settings.versehighlight && (key->Verse() == curVerse)) || e)
			 ? (e ? settings.highlight_bg : settings.highlight_fg)
			 : settings.bible_verse_num_color),
			num);
		g_free(num);
		swbuf.append(buf);
		g_free(buf);

		// insert the userfootnote reference
		if (e) {
			buf = g_strdup_printf("<span class=\"word\">"
					      "<a href=\"xiphos.url?action=showUserNote&"
					      "module=%s&value=%s\"><small><sup>*u</sup>"
					      "</small></a></span> ",
					      settings.MainWindowModule,
					      e->annotation->str);
			swbuf.append(buf);
			g_free(buf);
		}

		if ((key->Verse() == curVerse) || e) {
			buf = g_strdup_printf("<font color=\"%s\">",
					      ((settings.versehighlight || e)
					       ? (e ? settings.highlight_bg : settings.highlight_fg)
					       : settings.currentverse_color));
			swbuf.append(buf);
			g_free(buf);
		}

		if (newparagraph && versestyle) {
			newparagraph = FALSE;
			swbuf.append(paragraphMark);;
		}

		// same forced line break glitch in highlighted current verse.
		if (settings.versehighlight &&		// doing <table> h/l.
		    !versestyle &&			// paragraph format.
		    (key->Verse() == curVerse)) {
			GString *text = g_string_new(NULL);

			g_string_printf(text, "%s", rework);
			if (!strcmp(text->str + text->len - 6, "<br />")) {
				text->len -= 6;
				*(text->str + text->len) = '\0';
			}
			else if (!strcmp(text->str + text->len - 4, "<br>")) {
				text->len -= 4;
				*(text->str + text->len) = '\0';
			}
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(text->str,
							   GDK_WINDOW(gtkText->window))
				     : text->str /* left as-is */);
			g_string_free(text, TRUE);
		} else
			swbuf.append(settings.imageresize
				     ? AnalyzeForImageSize(rework,
							   GDK_WINDOW(gtkText->window))
				     : rework /* left as-is */);

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
			    !settings.versehighlight)
				swbuf.append("<br>");
		}

		// special contrasty highlighting
		if (((key->Verse() == curVerse) && settings.versehighlight) ||
		    e)
			swbuf.append("</font></td></tr></table>");
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

	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");

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
			      "<a href=\"xiphos.url?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font>[%s]<br>",
			      (settings.doublespace ? DOUBLE_SPACE : ""),
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

	if (swbuf.length())
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
	gecko_html_close(html);
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
	gchar *utf8_key;
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

	if (!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");

	if (!strcmp(imodule.Name(), "KJV"))
		paragraphMark = "&para;&nbsp;";
	else
		paragraphMark = "";

	swbuf = "";

	buf=g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">",
			      (settings.doublespace ? DOUBLE_SPACE : ""),
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

	gecko_html_write(html,swbuf.c_str(),swbuf.length());

	swbuf = "";
	main_set_global_options(ops);

	gecko_html_write(html,swbuf.c_str(),swbuf.length());

	swbuf = "";

	for (key->Verse(1);
	     key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error();
	     imodule++) {
		int x = 0;
		sprintf(heading, "%d", x);
		while ((preverse
			= backend->get_entry_attribute("Heading", "Preverse",
						       heading)) != NULL) {
			const char *preverse2 = imodule.RenderText(preverse);
			buf=g_strdup_printf("<br><b>%s</b><br><br>", preverse2);
			swbuf.append(buf);
			g_free(buf);
			g_free(preverse);
			++x;
			sprintf(heading, "%d", x);
		}

		utf8_key = strdup((char*)key->getText());

		num = main_format_number(key->Verse());
		buf=g_strdup_printf(settings.showversenum
			? "&nbsp; <a name=\"%d\" href=\"sword:///%s\">"
			  "<font size=\"%+d\" color=\"%s\">%s</font></a> "
			: "&nbsp; <a name=\"%d\"> </a>",
			key->Verse(),
			utf8_key,
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
			swbuf.append("<br>");
		}

		g_free(buf);

		gecko_html_write(html,swbuf.c_str(),swbuf.length());

		swbuf = "";
	}
	swbuf = "";

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);

	swbuf.append("</div></font></body></html>");

	if (swbuf.length())
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
	gecko_html_close(html);
	if (curVerse > 2) {
		buf = g_strdup_printf("%d", curVerse - 2);
		gecko_html_jump_to_anchor (html,buf);
		g_free(buf);
	}

	free_font(mf);
	g_free(ops);
#endif
	return 0;
}
