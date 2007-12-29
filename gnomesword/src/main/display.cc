/*
 * GnomeSword Bible Study Tool
 * display.cc -
 *
 * Copyright (C) 2000,2001,2002,2003,2004 GnomeSword Developer Team
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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <osishtmlhref.h>
#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <gbfosis.h>
#include <thmlosis.h>

#include <regex.h>
#include <string.h>
#include <assert.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>

#ifdef USE_GTKMOZEMBED
#include "gecko/gecko-html.h"
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

#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gui/dialog.h"

#include "backend/sword_main.hh"
//#include "backend/gs_osishtmlhref.h"

extern ModuleCache::CacheMap ModuleMap;

#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><STYLE type=\"text/css\"><!-- A { text-decoration:none } %s --></STYLE></head>"

// CSS style blocks to control blocked strongs+morph output
// BOTH is when the user wants to see both types of markup.
// ONE is when he wants one or the other, so we can use a single
// specification which overlays both on top of one another.
#define CSS_BLOCK_BOTH \
" *        { line-height: 3.5em; }" \
" .word    { position: relative; }" \
" .strongs { position: absolute; top: 0.3em; left: 0 }" \
" .morph   { position: absolute; top: 1.3em; left: 0 }"
#define CSS_BLOCK_ONE \
" *        { line-height: 2.7em; }" \
" .word    { position: relative; }" \
" .strongs { position: absolute; top:  0.6em; left: 0 }" \
" .morph   { position: absolute; top:  0.6em; left: 0 }"

#define DOUBLE_SPACE " * { line-height: 2em ! important; }"

using namespace sword;
using namespace std;

int strongs_on;
//T<font size=\"small\" >EST</font>  /* small caps */
//static gchar *f_message = "main/display.cc line #%d \"%s\" = %s";

int mod_use_counter[] = { -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1 };
		// indexed by module type e.g. COMMENTARY_TYPE.
		// used to avoid calling _get_size before these windows exist.

// shell command to obtain size spec: prints exactly "123x456".
#define	IDENTIFY	"identify \"%s\" 2>&1 | head -1 | sed -e 's/^.*\\(BMP\\|GIF\\|JPEG\\|PNG\\) //' -e 's/ .*$//'"

int
ImageDimensions(const char *path, int *x, int *y)
{
	char buf[350];	// enough for path+100 bytes of command.
	FILE *result;
	int retval = 0;

	if (strlen(path) > 250) {		// um...no.  forget it.
		*x = *y = 1;
		return -1;
	}

	sprintf(buf, IDENTIFY, path);
	if (((result = popen(buf, "r")) == NULL) ||	// can't run.
	    (fgets(buf, 384, result) == NULL)    ||	// can't read.
	    (buf[0] < '0')                       ||	// not ...
	    (buf[0] > '9'))   {				// ... numeric.
		*x = *y = 1;
		retval = -1;
		goto out;
	}
	sscanf(buf, "%dx%d\n", x, y);

	// cancel absurdities.
	if ((*x < 1) || (*x > 5000))
		*x = 1;
	if ((*y < 1) || (*y > 5000))
		*y = 1;

out:
	pclose(result);
	return retval;
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
		    GdkWindow *window,
		    gint mod_type)
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

	for (path = strcasestr(text, "<img src=\"");
	     path;
	     path = strcasestr(path, "<img src=\"")) {

		if (window_y == -999) {
			/* we have images, but we don't know bounds yet */

			if (mod_type == PERCOM_TYPE)
				mod_type = COMMENTARY_TYPE; // equivalent

			// we need this sort of "if" here rather badly...
			// if (GTK_WIDGET_VISIBLE(window)) {
			// ...because this hack counter is poor at best.
			if (++mod_use_counter[mod_type] >= 1) {
				// call _get_size only if the window exists by now.
				gdk_drawable_get_size(window, &window_x, &window_y);
				if ((window_x > 200) || (window_y > 200)) {
					window_x -= 23;
					window_y -= 23;
				} else {
					window_x = (window_x * 93)/100;
					window_y = (window_y * 93)/100;
				}
			} else {
				window_x = window_y = 400; // degenerate: full resize.
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
				    "An image file's size could not be determined.\nIs ImageMagick's 'identify' not installed?\n(Or possibly the image is of type other than bmp/gif/jpg/png.)\nGnomeSword cannot resize images to fit window.");
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


char GTKEntryDisp::Display(SWModule &imodule)
{
	gchar *keytext = NULL;
	gchar *buf;
	SWBuf swbuf = "";
	gint mod_type;
	MOD_FONT *mf = get_font(imodule.Name());
	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
#ifdef USE_GTKMOZEMBED
	GeckoHtml *html = GECKO_HTML(gtkText);
#else
	GtkHTML *html = GTK_HTML(gtkText);
	PangoContext* pc = gtk_widget_create_pango_context(gtkText);
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(desc, ((mf->old_font)?mf->old_font:"Serirf"));
	gtk_widget_modify_font (gtkText, desc);	
#endif
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());

	const char *rework;	// for image size analysis rework.

	(const char *)imodule;	// snap to entry
	//GS_message(((const char *)imodule.getRawEntry()));
	main_set_global_options(ops);
	mod_type = backend->module_type(imodule.Name());
	GS_message(("mod_type: %d",mod_type));
	if (mod_type == BOOK_TYPE) {
		keytext = strdup(backend->treekey_get_local_name(
				settings.book_offset));
		GS_message((keytext));
	} else if (mod_type == DICTIONARY_TYPE)
		keytext = g_strdup((char*)imodule.KeyText());
	else
		keytext = strdup((char*)imodule.KeyText());

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

	buf=g_strdup_printf("<font color=\"%s\">",
			      settings.bible_verse_num_color);
	swbuf.append(buf);
	g_free(buf);

	buf=g_strdup_printf("<a href=\"gnomesword.url?action=showModInfo&value=%s&module=%s\">"
			      "[*%s*]</a></font>[%s]<br>",
			      imodule.Description(),
			      imodule.Name(),
			      imodule.Name(),
			      (gchar*)keytext);
	swbuf.append(buf);
	g_free(buf);
	
	if ((backend->module_type(imodule.Name()) == PERCOM_TYPE)) // ||
		// !strcmp(imodule.getConfigEntry("SourceType"),"ThML"))
		rework = (const char *)(const char *)imodule.getRawEntry();  //keytext);
	else if (!strcmp(imodule.Name(), "ISBE"))
		rework = (const char *)(const char *)imodule.StripText();  //keytext);
	else
		rework = (const char *)imodule;

		/********************/
	//	g_message((const char *)imodule.getRawEntry());
		/********************/
	swbuf.append(settings.imageresize
		     ? AnalyzeForImageSize(rework,
					   GDK_WINDOW(gtkText->window),
					   mod_type)
		     : rework /* left as-is */);

	swbuf.append("</font></body></html>");
	//GS_message(("\nswbuf.str:\n%s\nswbuf.length:\n%d",swbuf.c_str(),swbuf.length()));
	
	// html widgets are uptight about being handed
	// huge quantities of text -- producer/consumer problem,
	// and we mustn't overload the receiver.  10k chunks.

	int len = swbuf.length(), offset = 0, write_size;

#ifdef USE_GTKMOZEMBED
	gecko_html_open_stream(html,"text/html");
#else
	GtkHTMLStream *stream = gtk_html_begin(html);
	GtkHTMLStreamStatus status;
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
#endif

	while (len > 0) {
		write_size = min(10000, len);
#ifdef USE_GTKMOZEMBED
		gecko_html_write(html, swbuf.c_str()+offset, write_size);
#else
		gtk_html_write(html, stream, swbuf.c_str()+offset, write_size);
#endif
		offset += write_size;
		len -= write_size;
	}

#ifdef USE_GTKMOZEMBED
	gecko_html_close(html);
#else
	gtk_html_end(html, stream, status);
	gtk_html_set_editable(html, was_editable);
#endif
	
	free_font(mf);
	g_free(ops);
	if (keytext)
		g_free(keytext);
}


//
// utility function for block_render() below.
// having a word + annotation in hand, stuff them into the buffer.
// span class names are from CSS_BLOCK macros.
// we garbage-collect here so block_render doesn't have to.
//
void
block_dump(SWBuf& rendered,
	   const char **word,
	   const char **strongs,
	   const char **morph)
{
	int wlen, slen, mlen, min_length;
	char *s, *s0, *t;

	// unannotated words need no help.
	if (*word && (*strongs == NULL) && (*morph == NULL)) {
		rendered += *word;
		g_free((char *)*word);
		*word = NULL;
		rendered += " ";
		return;
	}

	if (*word == NULL) {
		// we need to cobble up something to take the place of
		// a word, in order that the strongs/morph not overlay.
#ifdef USE_GTKMOZEMBED
		*word = g_strdup("&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;&nbsp;");
#else
		*word = g_strdup(" ");	// gtkhtml3 has no alignment need.
#endif /* USE_GTKMOZEMBED */
	}

	rendered += "<span class=\"word\">";
	if (*strongs) {
		s = g_strrstr(*strongs, "</a>");
		*s = '\0';
		t = strrchr(*strongs, '>') + 1;
		*s = '<';
		slen = s - t;
#ifdef USE_GTKMOZEMBED
		s = strstr(*strongs, "&lt;");
		*s = *(s+1) = *(s+2) = *(s+3) = ' ';
		s = strstr(s, "&gt;");
		*s = *(s+1) = *(s+2) = *(s+3) = ' ';
#endif /* USE_GTKMOZEMBED */
	} else
		slen = 0;
	if (*morph) {
		s = s0 = strstr(*morph, "\">") + 2;
		t = strchr(s, '<');
		for (/* */; s < t; ++s)
			if (isupper(*s))
				*s = tolower(*s);
		for (s = strchr(s0, ' '); s && (s < t); s = strchr(s, ' '))
			*s = '-';
		s = g_strrstr(*morph, "</a>");
		*s = '\0';
		t = strrchr(*morph, '>') + 1;
		*s = '<';
		mlen = s - t;
#ifdef USE_GTKMOZEMBED
		s = strchr(*morph, '(');
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

	if (*strongs) {
		rendered += "<span class=\"strongs\">";
		rendered += *strongs;
		rendered += "</span>";
		g_free((char *)*strongs);
		*strongs = NULL;
	}
	if (*morph) {
		rendered += "<span class=\"morph\">";
		rendered += *morph;
		rendered += "</span>";
		g_free((char *)*morph);
		*morph = NULL;
	}
	rendered += "</span> ";
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
block_render_secondary(const char *text, SWBuf& rendered)
{
	const char *word    = NULL,
		   *strongs = NULL,
		   *morph   = NULL;
	int bracket;
	const char *s, *t;

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
				if ((t = strstr(s, end)) == NULL) {
					GS_warning(("No %s in %s\n", end, s));
					break;
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
				if ((t = strstr(s, "</small>")) == NULL) {
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
// utility function for filling headers from verses.
//
void
CacheHeader(ModuleCache::CacheVerse& cVerse, SWModule &mod)
{
	int x = 0;
	gchar heading[8];
	const gchar *preverse, *preverse2, *text;
 
	cVerse.SetHeader("");

	sprintf(heading, "%d", x);
	while ((preverse = backend->get_entry_attribute("Heading", "Preverse",
							heading)) != NULL) {
		preverse2 = mod.RenderText(preverse);
		text = g_strdup_printf("<br><b>%s</b><br><br>",
				       preverse2);
		cVerse.AppendHeader(text);
		g_free((gchar *)text);
		// g_free(preverse2); // does RenderText's result need free()?
		g_free((gchar *)preverse);
		++x;
		sprintf(heading, "%d", x);
	}
}

void set_morph_order(SWModule &imodule)
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

void GTKChapDisp::getVerseBefore(SWModule &imodule,
				 gboolean strongs_or_morph,
				 gboolean strongs_and_morph,
				 uint16_t cache_flags)
{
	gchar *utf8_key;
	gchar *buf;
	SWMgr *mgr = be->get_main_mgr();

	const char *ModuleName = imodule.Name();
	SWModule *mod_top = mgr->getModule(ModuleName);
	mod_top->setSkipConsecutiveLinks(true);
	*mod_top = sword::TOP;

	sword::VerseKey key_top( mod_top->KeyText() );
	SWModule *mod = mgr->getModule(ModuleName);
	mod->setKey(imodule.getKey());

	VerseKey *key = (VerseKey *)(SWKey *)*mod;
	int chapter = key->Chapter();
	key->Verse(1);

	if (!key->_compare(key_top)) {
		if ((!strcmp(settings.MainWindowModule, "KJV"))){
			buf=g_strdup_printf("<div style=\"text-align: center\">%s<hr></div>",
					mod->Description());
			swbuf.append(buf);
			g_free(buf);
		}
		else {
			buf=g_strdup_printf("<div style=\"text-align: center\"><p>%s</p><b>%s %d</b></div>",
					mod->Description(),
					_("Chapter"),
					chapter);
			swbuf.append(buf);
			g_free(buf);
		}
	} else {
		(*mod)--;

		if (strongs_and_morph)
			set_morph_order(*mod);

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [((key->Testament() == 1) ? 0 : 39 ) + key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render((const char *)*mod)
					: (const char *)*mod),
				       cache_flags);

		utf8_key = strdup((char*)key->getText());

		if (is_rtol)
			swbuf.append("<DIV ALIGN=right>");

		buf=g_strdup_printf(settings.showversenum
				? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
				  "<font size=\"%+d\" color=\"%s\">%d</font></A> "
				: "&nbsp; <A NAME=\"%d\"> </A>",
				0,
				utf8_key,
				((settings.versestyle)
				 ? settings.verse_num_font_size + settings.base_font_size
				 : settings.base_font_size - 2),
				settings.bible_verse_num_color,
				key->Verse());
		swbuf.append(buf);
		g_free(buf);

		buf=g_strdup_printf(
				"%s%s<br><hr><div style=\"text-align: center\"><b>%s %d</b></div>",
				cVerse.GetText(),
				// extra break when excess strongs/morph space.
				(strongs_or_morph ? "<br>" : ""),
				_("Chapter"), chapter);
		swbuf.append(buf);
		g_free(buf);

		if (is_rtol)
			swbuf.append("</DIV>");

		(*mod)++;
	}

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
		    [((key->Testament() == 1) ? 0 : 39 ) + key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render((const char *)*mod)
					: (const char *)*mod), cache_flags);

		buf=g_strdup_printf("%s<br />", cVerse.GetText());
		swbuf.append(buf);
		g_free(buf);
	}

	key->AutoNormalize(oldAutoNorm);
}

void GTKChapDisp::getVerseAfter(SWModule &imodule,
				gboolean strongs_or_morph,
				gboolean strongs_and_morph,
				uint16_t cache_flags)
{
	gchar *utf8_key;
	gchar *buf;
	SWMgr *mgr = be->get_main_mgr();
	const char *ModuleName = imodule.Name();
	SWModule *mod_bottom = mgr->getModule(ModuleName);
		mod_bottom->setSkipConsecutiveLinks(true);
	*mod_bottom = sword::BOTTOM;
	sword::VerseKey key_bottom( mod_bottom->KeyText() );
	SWModule *mod = mgr->getModule(ModuleName);
	mod->setKey(imodule.getKey());
	VerseKey *key = (VerseKey *)(SWKey *)*mod;

	if (key_bottom._compare(key) < 1) {
		buf=g_strdup_printf(
			"%s<hr><div style=\"text-align: center\"><p>%s</p></div>",
			// extra break when excess strongs/morph space.
			(strongs_or_morph ? "<br>" : ""),
			mod->Description());
		swbuf.append(buf);
		g_free(buf);
	} else {
		int chapter = key->Chapter();
		if ((!strcmp(settings.MainWindowModule, "KJV"))){
			buf=g_strdup_printf(
				"%s<hr><b>%s %d.</b><br><br>",
				(strongs_or_morph ? "<br>" : ""),
				_("Chapter"), chapter);
			swbuf.append(buf);
			g_free(buf);
		}
		else {
			buf=g_strdup_printf(
				"%s<hr><div style=\"text-align: center\"><b>%s %d</b></div>",
				(strongs_or_morph ? "<br>" : ""),
				_("Chapter"), chapter);
			swbuf.append(buf);
			g_free(buf);
		}

		utf8_key = strdup((char*)key->getText());

		if (is_rtol)
			swbuf.append("<DIV ALIGN=right>");

		buf=g_strdup_printf(settings.showversenum
				? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
				  "<font size=\"%+d\" color=\"%s\">%d</font></A> "
				: "&nbsp; <A NAME=\"%d\"> </A>",
				0,
				utf8_key,
				((settings.versestyle)
				 ? settings.verse_num_font_size + settings.base_font_size
				 : settings.base_font_size - 2),
				settings.bible_verse_num_color,
				key->Verse());
		swbuf.append(buf);
		g_free(buf);

		if (strongs_and_morph)
			set_morph_order(*mod);

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [((key->Testament() == 1) ? 0 : 39 ) + key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render((const char *)*mod)
					: (const char *)*mod),
				       cache_flags);

		swbuf.append(cVerse.GetText());
		if (is_rtol)
			swbuf.append("</DIV>");
	}
}

//
// Read aloud some text, i.e. the current verse.
// Text is cleaned of '"', <tokens>, "&symbols;", and *n/*x strings,
// then scribbled out the local static socket with (SayText "...").
// Non-zero verse param is prefixed onto supplied text.
//
void ReadAloud(unsigned int verse, const char *suppliedtext)
{
	static int tts_socket = -1;	// no initial connection.
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
				system("festival --server &");
				sleep(2); // give festival a moment to init.

				if (connect(tts_socket, (const sockaddr *)&service,
					    sizeof(service)) != 0) {
					// it still didn't work -- missing.
					char msg[256];
					sprintf(msg, "%s\n%s, %s",
						"TTS \"festival\" not started -- perhaps not installed",
						"TTS connect failed", strerror(errno));
					close(tts_socket);
					tts_socket = -1;
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
			if (t = strstr(s, "</span>")) {
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
			if (t = strstr(s, "</span>")) {
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
			if (t = strchr(s, '>')) {
				while (s <= t)
					*(s++) = ' ';
			} else {
				GS_message(("ReadAloud: Unmatched <> in %s\n", s));
				goto out;
			}
		}

		// clean: no &lt;...&gt; sequences.  (Strong's ref, "<1234>".)
		for (s = strstr(text->str, "&lt;"); s; s = strstr(s, "&lt;")) {
			if (t = strstr(s, "&gt;")) {
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
			if (t = strchr(s, ';')) {
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
		if ((write(tts_socket, "(SayText \"", 10) < 0)  ||
		    (write(tts_socket, text->str, strlen(text->str)) < 0) ||
		    (write(tts_socket, "\")\r\n", 4) < 0))
		{
			char msg[256];
			sprintf(msg, "TTS disappeared?\nTTS write failed: %s",
				strerror(errno));
			shutdown(tts_socket, SHUT_RDWR);
			close(tts_socket);
			tts_socket = -1;
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
			shutdown(tts_socket, SHUT_RDWR);
			close(tts_socket);
			tts_socket = -1;
		}
		use_counter++;
		return;
	}
}

char GTKChapDisp::Display(SWModule &imodule)
{
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curTestament = key->Testament();
	int curPos = 0;
	gchar *utf8_key;
	gchar *buf;
	gchar *paragraphMark = NULL;
	gchar *br = NULL;

	char *ModuleName = imodule.Name();
	GLOBAL_OPS * ops = main_new_globals(ModuleName);
	uint16_t cache_flags = ConstructFlags(ops);
	is_rtol = main_is_mod_rtol(ModuleName);
	gboolean newparagraph = FALSE;
	mf = get_font(ModuleName);
#ifdef USE_GTKMOZEMBED	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");
#else
	GtkHTML *html = GTK_HTML(gtkText);
	PangoContext* pc = gtk_widget_create_pango_context(gtkText);
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(desc, ((mf->old_font)?mf->old_font:"Serirf"));
	gtk_widget_modify_font (gtkText, desc);

	gboolean was_editable = gtk_html_get_editable(html);
	GtkHTMLStream *stream = gtk_html_begin(html);
	GtkHTMLStreamStatus status;
#endif

	gboolean strongs_and_morph = ((ops->strongs || ops->lemmas) &&
				      ops->morphs);
	gboolean strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
				      ops->morphs);
	if (strongs_and_morph)
		set_morph_order(imodule);

	// when strongs/morph are on, the anchor boundary must be smaller.
	gint display_boundary = (strongs_or_morph ? 1 : 2);

	if (!strcmp(ModuleName, "KJV"))
		paragraphMark = "&para;";
	else
		paragraphMark = "";

	swbuf = "";
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_text), 0);

	buf=g_strdup_printf(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
			      "<font face=\"%s\" size=\"%+d\">",
			      // strongs & morph specs win over dblspc.
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
			       : settings.base_font_size));
	swbuf.append(buf);
	g_free(buf);

	if (is_rtol)
		swbuf.append("<DIV ALIGN=right>");

#ifdef USE_GTKMOZEMBED		
	gecko_html_write(html,swbuf.c_str(),swbuf.length());
#else
	gtk_html_write(html, stream, swbuf.c_str(), swbuf.length());
#endif
	
	swbuf = "";
	main_set_global_options(ops);
	strongs_on = ops->strongs;
	getVerseBefore(imodule, strongs_or_morph, strongs_and_morph, cache_flags);
#ifdef USE_GTKMOZEMBED	
	gecko_html_write(html,swbuf.c_str(),swbuf.length());
#else
	gtk_html_write(html, stream, swbuf.c_str(), swbuf.length());
#endif
	
	swbuf = "";

	for (key->Verse(1);
	     (key->Book()    == curBook) &&
	     (key->Chapter() == curChapter) &&
	     !imodule.Error();
	     imodule++) {

		ModuleCache::CacheVerse& cVerse = ModuleMap
		    [ModuleName]
		    [((curTestament == 1) ? 0 : 39 ) + key->Book()]
		    [key->Chapter()]
		    [key->Verse()];

		// use the module cache rather than re-accessing Sword.
		if (!cVerse.CacheIsValid(cache_flags))
			cVerse.SetText((strongs_or_morph
					? block_render((const char *)imodule)
					: (const char *)imodule),
				       cache_flags);
		if (!cVerse.HeaderIsValid())
			CacheHeader(cVerse, imodule);

		if (cache_flags & ModuleCache::Headings)
			swbuf.append(cVerse.GetHeader());
		else
			cVerse.InvalidateHeader();

		utf8_key = strdup((char*)key->getText());

		// special contrasty highlighting
		if ((key->Verse() == curVerse) && settings.versehighlight) {
			buf=g_strdup_printf(
			    "<table bgcolor=\"%s\"><tr><td>"
			    "<font face=\"%s\" size=\"%+d\">",
			    settings.highlight_bg,
			    ((mf->old_font) ? mf->old_font : ""),
			    ((mf->old_font_size)
			     ? atoi(mf->old_font_size) + settings.base_font_size
			     : settings.base_font_size));
			swbuf.append(buf);
			g_free(buf);
		}

		buf=g_strdup_printf(settings.showversenum
			? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
			  "<font size=\"%+d\" color=\"%s\">%d</font></A> "
			: "&nbsp; <A NAME=\"%d\"> </A>",
			key->Verse(),
			utf8_key,
			settings.verse_num_font_size + settings.base_font_size,
			((settings.versehighlight && (key->Verse() == curVerse))
			 ? settings.highlight_fg
			 : settings.bible_verse_num_color),
			key->Verse());
		swbuf.append(buf);
		g_free(buf);

		if (key->Verse() == curVerse) {
			buf=g_strdup_printf("<font color=\"%s\">",
						      (settings.versehighlight
						       ? settings.highlight_fg
						       : settings.currentverse_color));
			swbuf.append(buf);
			g_free(buf);
		}
/*				
		if (key->Verse() == curVerse   ||
		    key->Verse() == curVerse-1 ||
		    key->Verse() == curVerse-2 ||
		    key->Verse() == curVerse+2 ||
		    key->Verse() == curVerse+1 ) {
			main_set_strongs_morphs(ops);
		} else {
			main_set_strongs_morphs_off(ops);
		}
*/		
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

			g_string_printf(text, "%s", cVerse.GetText());
			if (!strcmp(text->str + text->len - 6, "<br />")) {
				text->len -= 6;
				*(text->str + text->len) = '\0';
			}
			else if (!strcmp(text->str + text->len - 4, "<br>")) {
				text->len -= 4;
				*(text->str + text->len) = '\0';
			}
			swbuf.append(text->str);
			g_string_free(text, TRUE);
		} else
			swbuf.append(cVerse.GetText());
		
		if (key->Verse() == curVerse) {
			swbuf.append("</font>");
			ReadAloud(curVerse, cVerse.GetText());
		}

		if (settings.versestyle) {
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
		if ((key->Verse() == curVerse) && settings.versehighlight)
			swbuf.append("</font></td></tr></table>");

#ifdef USE_GTKMOZEMBED		
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
#else
		gtk_html_write(html, stream, swbuf.c_str(), swbuf.length());
#endif
		
		swbuf = "";
	}
	swbuf = "";
	getVerseAfter(imodule, strongs_or_morph, strongs_and_morph, cache_flags);

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);

	if (is_rtol)
		swbuf.append("</DIV></font></body></html>");
	else
		swbuf.append("</font></body></html>");
	
#ifdef USE_GTKMOZEMBED
	if (swbuf.length()) 
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
	gecko_html_close(html);
	if (curVerse > display_boundary) {
		buf = g_strdup_printf("%d", curVerse - display_boundary);
		gecko_html_jump_to_anchor (html,buf);
		//embed_go_to_anchor(html, buf);
		g_free(buf);
	}
#else
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
	if (swbuf.length())
		gtk_html_write(html, stream, swbuf.c_str(), swbuf.length());
		//gtk_html_load_from_string(html, swbuf.c_str(), swbuf.length());
	gtk_html_end(html, stream, status);
	gtk_html_set_editable(html, was_editable);
	if (curVerse > display_boundary) {
		buf = g_strdup_printf("%d", curVerse - display_boundary);
		gtk_html_jump_to_anchor(html, buf);
		g_free(buf);
	}
	gtk_html_flush (html);
#endif	
	free_font(mf);
	g_free(ops);
}


/******************************************************************************
 * Name
 *   GTKTextviewChapDisp::Display
 *
 * Synopsis
 *   #include "main/display.hh"
 *
 *   char GTKTextviewChapDisp::Display(SWModule &imodule)
 *
 * Description
 *   display right to left Sword Bible texts a chapter at a time
 *   using GtkTextView
 *
 * Return value
 *   void
 */

#ifndef USE_GTKMOZEMBED
char GTKTextviewChapDisp::Display(SWModule &imodule)
{
	char tmpBuf[255];
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	GtkTextMark   *mark = NULL;
	GtkTextIter iter, startiter, enditer;
	static GtkTextTag *font_tag = NULL;
	MOD_FONT *mf = get_font(imodule.Name());
	GtkTextView *textview = GTK_TEXT_VIEW(gtkText);
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
	GString *str = g_string_new(NULL);
	const gchar *mark_name = "CurrentVerse";

	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_text), 1);
	if (font_tag)
		gtk_text_tag_table_remove(gtk_text_buffer_get_tag_table (buffer),
                                font_tag);

	font_tag = gtk_text_buffer_create_tag (buffer, "rtl_font", NULL);
        g_object_set (G_OBJECT (font_tag),
		"font", mf->old_font,
		NULL);

	//* clear the buffer *
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_delete(buffer, &startiter, &enditer);

	main_set_global_options(ops);
	for (key->Verse(1); (key->Book() == curBook && key->Chapter()
				== curChapter && !imodule.Error()); imodule++) {

		g_string_printf(str, "%d", key->Verse());
	        gtk_text_buffer_get_end_iter(buffer, &iter);
		gtk_text_buffer_insert_with_tags(buffer,
				&iter,
				str->str,
				str->len,
				gtk_text_tag_table_lookup (
				gtk_text_buffer_get_tag_table (buffer),
						       "verseNumber"),
				NULL);

		g_string_printf(str, " %s", (const char *)imodule.StripText());

		if (key->Verse() == curVerse) {
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			mark = gtk_text_buffer_create_mark (buffer,
						mark_name,
						&iter,
						TRUE);
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_currentverse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),
                                        NULL);
		} else {
			gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_insert_with_tags (buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_verse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),
                                        NULL);
		}
		gtk_text_buffer_get_end_iter(buffer, &iter);
		if (settings.versestyle)
			gtk_text_buffer_insert(buffer, &iter, "\n", strlen("\n"));
		else
			gtk_text_buffer_insert(buffer, &iter, " ", strlen(" "));
	}

	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_apply_tag_by_name(buffer,
			"rtl_text",
			&startiter,
			&enditer);

	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textview),
				mark,
				FALSE,
				TRUE,
				0.0,
				0.0);
	gtk_text_buffer_delete_mark(buffer, mark);

	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	free_font(mf);
	g_free(ops);
}
#endif


char DialogEntryDisp::Display(SWModule &imodule)
{
	GString *str = g_string_new(NULL);
	const gchar *keytext = NULL;
	int curPos = 0;
	int type = d->mod_type;  //be->module_type(imodule.Name());
	MOD_FONT *mf = get_font(imodule.Name());
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	gint font_size;
#ifdef USE_GTKMOZEMBED
	GeckoHtml *html = GECKO_HTML(gtkText);
#else
	GtkHTML *html = GTK_HTML(gtkText);
	PangoContext* pc = gtk_widget_create_pango_context(gtkText);
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(desc, ((mf->old_font)?mf->old_font:"Serirf"));
	gtk_widget_modify_font (gtkText, desc);
#endif

	(const char *)imodule;	// snap to entry
	main_set_global_options(ops);

	if (type == 3)
		keytext = be->treekey_get_local_name(d->offset);
	else
		keytext = imodule.KeyText();

	font_size = ((mf->old_font_size)
		     ? atoi(mf->old_font_size) + settings.base_font_size
		     : settings.base_font_size);
	if (type == 4)
		g_string_printf(str, HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<font face=\"%s\" size=\"%+d\">%s"
				"</font></body></html>",
				(settings.doublespace ? DOUBLE_SPACE : ""),
				settings.bible_bg_color,
				settings.bible_text_color,
				settings.link_color,
				((mf->old_font) ? mf->old_font : ""),
				font_size,
				(settings.imageresize
				 ? AnalyzeForImageSize((const char *)imodule,
						       GDK_WINDOW(gtkText->window),
						       type)
				 : (const char *)imodule /* untouched */));
	else
		g_string_printf(str, HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<font face=\"%s\" size=\"%+d\">"
				"<a href=\"gnomesword.url?action=showModInfo&value=%s&module=%s\">"
				"<font color=\"%s\">[%s]</font></a>[%s]<br>"
				"%s</font></body></html>",
				(settings.doublespace ? DOUBLE_SPACE : ""),
				settings.bible_bg_color,
				settings.bible_text_color,
				settings.link_color,
				((mf->old_font) ? mf->old_font : ""),
				font_size,
				imodule.Description(),
				imodule.Name(),
				settings.bible_verse_num_color,
				imodule.Name(),
				(gchar*)keytext,
				(settings.imageresize
				 ? AnalyzeForImageSize((const char *)imodule,
						       GDK_WINDOW(gtkText->window),
						       type)
				 : (const char *)imodule /* untouched */));

	// html widgets are uptight about being handed
	// huge quantities of text -- producer/consumer problem,
	// and we mustn't overload the receiver.  10k chunks.

	int len = str->len, offset = 0, write_size;

#ifdef USE_GTKMOZEMBED
	gecko_html_open_stream(html,"text/html");
#else
	GtkHTMLStream *stream = gtk_html_begin(html);
	GtkHTMLStreamStatus status;
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
#endif
	
	while (len > 0) {
		write_size = min(10000, len);
#ifdef USE_GTKMOZEMBED
		gecko_html_write(html, str->str+offset, write_size);
#else
		gtk_html_write(html, stream, str->str+offset, write_size);
#endif
		offset += write_size;
		len -= write_size;
	}

#ifdef USE_GTKMOZEMBED
	gecko_html_close(html); 
#else
	gtk_html_end(html, stream, status);
	gtk_html_set_editable(html, was_editable);
#endif

	g_string_free(str, TRUE);
	free_font(mf);
	g_free(ops);
}


char DialogChapDisp::Display(SWModule &imodule)
{
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	MOD_FONT *mf = get_font(imodule.Name());
	GString *str = g_string_new(NULL);
	gchar *buf;
	gchar *buf2;
	gchar *preverse = NULL;
	gchar *paragraphMark = "&para;";
	gchar *br = NULL;
	gchar heading[32];
	gboolean newparagraph = FALSE;	
#ifdef USE_GTKMOZEMBED
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");
#else
	GtkHTML *html = GTK_HTML(gtkText);
	PangoContext* pc = gtk_widget_create_pango_context(gtkText);
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(desc,
					  ((mf->old_font)?mf->old_font:"Serif"));
	gtk_widget_modify_font (gtkText, desc);
	gboolean was_editable = gtk_html_get_editable(html);
	GtkHTMLStream *stream = gtk_html_begin(html);
	GtkHTMLStreamStatus status;
#endif
	gboolean strongs_and_morph, strongs_or_morph;
	gint display_boundary;

	gint versestyle;
	gchar *file = NULL, *style = NULL;

	file = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	style = get_conf_file_item(file, imodule.Name(), "style");
	if ((style) && strcmp(style, "verse"))
		versestyle = FALSE;
	else
		versestyle = TRUE;
	g_free(style);
	g_free(file);

	main_dialog_set_global_options((BackEnd*)be, ops);
	strongs_and_morph = ((ops->strongs || ops->lemmas) &&
			     ops->morphs);
	strongs_or_morph  = ((ops->strongs || ops->lemmas) ||
			     ops->morphs);
	display_boundary = (strongs_or_morph ? 1 : 2);
	if (strongs_and_morph) {
		for (FilterList::const_iterator it =
			 imodule.getRenderFilters().begin();
		     it != imodule.getRenderFilters().end();
		     it++) {
			OSISHTMLHREF *f = dynamic_cast<OSISHTMLHREF *>(*it);
			if (f)
				f->setMorphFirst();
		}
	}

	g_string_printf(str,
			HTML_START
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

#ifdef USE_GTKMOZEMBED	
	gecko_html_write(html, str->str, str->len);
#else
	gtk_html_write(html, stream, str->str, str->len);
#endif
	
	str = g_string_erase(str, 0, -1);

	for (key->Verse(1);
	     key->Book() == curBook && key->Chapter() == curChapter && !imodule.Error();
	     imodule++) {
		int x = 0;
		sprintf(heading, "%d", x);
		while ((preverse
			= be->get_entry_attribute("Heading", "Preverse",
							    heading)) != NULL) {
			buf = g_strdup_printf("<br><b>%s</b><br><br>", preverse);
			str = g_string_append(str, buf);
			g_free(preverse);
			g_free(buf);
			++x;
			sprintf(heading, "%d", x);
		}

		// special contrasty highlighting
		if ((key->Verse() == curVerse) && settings.versehighlight) {
			buf = g_strdup_printf(
			    "<table bgcolor=\"%s\"><tr><td>"
			    "<font face=\"%s\" size=\"%+d\">",
			    settings.highlight_bg,
			    ((mf->old_font) ? mf->old_font : ""),
			    ((mf->old_font_size)
			     ? atoi(mf->old_font_size) + settings.base_font_size
			     : settings.base_font_size));
			str = g_string_append(str, buf);
			g_free(buf);
		}

		buf = g_strdup_printf(settings.showversenum
			? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
			  "<font size=\"%+d\" color=\"%s\">%d</font></A> "
			: "&nbsp; <A NAME=\"%d\"> </A>",
			key->Verse(),
			(char*)key->getText(),
			settings.verse_num_font_size + settings.base_font_size,
			((settings.versehighlight && (key->Verse() == curVerse))
			 ? settings.highlight_fg
			 : settings.bible_verse_num_color),
			key->Verse());
		str = g_string_append(str, buf);
		g_free(buf);

		if (key->Verse() == curVerse) {
			buf = g_strdup_printf(
				"<font color=\"%s\">",
				(settings.versehighlight
				 ? settings.highlight_fg
				 : settings.currentverse_color));

			str = g_string_append(str, buf);
			g_free(buf);
		}

		if (newparagraph && versestyle) {
			newparagraph = FALSE;
			str = g_string_append(str, paragraphMark);
		}
		/*
		if (key->Verse() == curVerse   ||
		    key->Verse() == curVerse-1 ||
		    key->Verse() == curVerse-2 ||
		    key->Verse() == curVerse+2 ||
		    key->Verse() == curVerse+1 ) {
			main_set_dialog_strongs_morphs(be, ops);
		} else {
			main_set_dialog_strongs_morphs_off(be, ops);
		}
		*/
		// same forced line break glitch in highlighted current verse.
		if (settings.versehighlight &&		// doing <table> h/l.
		    !versestyle &&			// paragraph format.
		    (key->Verse() == curVerse)) {
			GString *text = g_string_new(NULL);

			g_string_printf(text, "%s", (const char *)imodule);
			if (!strcmp(text->str + text->len - 6, "<br />")) {
				text->len -= 6;
				*(text->str + text->len) = '\0';
			}
			else if (!strcmp(text->str + text->len - 4, "<br>")) {
				text->len -= 4;
				*(text->str + text->len) = '\0';
			}
			str = g_string_append(str, 
					      (strongs_or_morph
					       ? block_render(text->str)
					       : text->str));
			g_string_free(text, TRUE);
		} else
			str = g_string_append(str,
					      (strongs_or_morph
					       ? block_render((const char *)imodule)
					       : (const char *)imodule));

		if (key->Verse() == curVerse)
			str = g_string_append(str, "</font>");

		buf = g_strdup_printf("%s", (const char *)imodule);

		if (versestyle) {
			buf2 = g_strdup_printf("%s",
					       ((key->Verse() == curVerse) &&
						settings.versehighlight)
					       ? "" : "<br>");

			if ((strstr(buf, "<!P>") == NULL) &&
			     (strstr(buf, "<p>") == NULL) ) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		} else {
			if (strstr(buf, "<!P>") == NULL)
				buf2 = g_strdup(" ");
			else
				buf2 = g_strdup_printf(" %s", "<p>");
		}
		str = g_string_append(str, buf2);
		g_free(buf2);
		g_free(buf);

		// special contrasty highlighting
		if ((key->Verse() == curVerse) && settings.versehighlight)
		    	str = g_string_append(str, ("</font></td></tr></table>"));
#ifdef USE_GTKMOZEMBED	
		gecko_html_write(html, str->str, str->len);
#else
		gtk_html_write(html, stream, str->str, str->len);
#endif
		str = g_string_erase(str, 0, -1);
	}
	str = g_string_erase(str, 0, -1);
	buf = g_strdup_printf("%s", "</font></body></html>");
	str = g_string_append(str, buf);
	g_free(buf);
	
#ifdef USE_GTKMOZEMBED
	if (str->len) {
		gecko_html_write(html, str->str, str->len);
		gecko_html_close(html); 
		if (curVerse > display_boundary) {
			buf = g_strdup_printf("%d", curVerse - display_boundary);
			gecko_html_jump_to_anchor(html, buf);
			g_free(buf);
		}
	}
#else
	if (str->len) {
		gtk_html_write(html, stream, str->str, str->len);
		//gtk_html_load_from_string(html, str->str, str->len);
	}
	gtk_html_end(html, stream, status);
	gtk_html_set_editable(html, was_editable);
	if (curVerse > display_boundary) {
		buf = g_strdup_printf("%d", curVerse - display_boundary);
		gtk_html_jump_to_anchor(html, buf);
		g_free(buf);
	}
	gtk_html_flush (html);
#endif
	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	free_font(mf);
}


#ifndef USE_GTKMOZEMBED
char DialogTextviewChapDisp::Display(SWModule &imodule)
{
	char tmpBuf[255];
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curPos = 0;
	gfloat adjVal;
	GtkTextMark   *mark = NULL;
	GtkTextIter iter, startiter, enditer;
	static GtkTextTag *font_tag = NULL;
	MOD_FONT *mf = get_font(imodule.Name());
	GtkTextView *textview = GTK_TEXT_VIEW(gtkText);
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(textview);
	GString *str = g_string_new(NULL);
	const gchar *mark_name = "CurrentVerse";

	gint versestyle;
	gchar *file = NULL, *style = NULL;

	file = g_strdup_printf("%s/modops.conf", settings.gSwordDir);
	style = get_conf_file_item(file, imodule.Name(), "style");
	if ((style) && strcmp(style, "verse"))
		versestyle = FALSE;
	else
		versestyle = TRUE;
	g_free(style);
	g_free(file);

	if (!font_tag) {
		font_tag = gtk_text_buffer_create_tag (buffer, "rtl_font", NULL);
		g_object_set (G_OBJECT (font_tag), "font", mf->old_font,
					NULL);
	}

	//* clear the buffer *
	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_delete(buffer, &startiter, &enditer);

	main_set_global_options(ops);
	for (key->Verse(1); (key->Book() == curBook && key->Chapter()
				== curChapter && !imodule.Error()); imodule++) {

		g_string_printf(str, "%d", key->Verse());
	        gtk_text_buffer_get_end_iter(buffer, &iter);
		gtk_text_buffer_insert_with_tags(buffer,
				&iter,
				str->str,
				str->len,
				gtk_text_tag_table_lookup (
				gtk_text_buffer_get_tag_table (buffer),
						       "verseNumber"),
				NULL);

		g_string_printf(str, " %s", (const char *)imodule);

		if (key->Verse() == curVerse) {
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			mark = gtk_text_buffer_create_mark(buffer,
						mark_name,
						&iter,
						TRUE);
		        gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_insert_with_tags(buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_currentverse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),
                                        NULL);
		} else {
			gtk_text_buffer_get_end_iter(buffer, &iter);
			gtk_text_buffer_insert_with_tags(buffer, &iter,
                                        str->str,
                                        str->len,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
                                        "fg_verse"),
                                        font_tag,
                                        gtk_text_tag_table_lookup (
					gtk_text_buffer_get_tag_table (buffer),
					"large"),
                                        NULL);
		}
		gtk_text_buffer_get_end_iter(buffer, &iter);
		if (versestyle)
			gtk_text_buffer_insert(buffer, &iter, "\n", 1);
		else
			gtk_text_buffer_insert(buffer, &iter, " ", 1);
	}

	gtk_text_buffer_get_start_iter(buffer, &startiter);
	gtk_text_buffer_get_end_iter(buffer, &enditer);
	gtk_text_buffer_apply_tag_by_name(buffer,
					  "rtl_text",
					  &startiter,
					  &enditer);

	gtk_text_view_scroll_to_mark(GTK_TEXT_VIEW(textview),
				     mark,
				     FALSE,
				     TRUE,
				     0.0,
				     0.0);
	gtk_text_buffer_delete_mark(buffer, mark);

	if (font_tag)
		gtk_text_tag_table_remove(gtk_text_buffer_get_tag_table (buffer),
					  font_tag);
	font_tag = NULL;
	g_string_free(str, TRUE);
	key->Verse(1);
	key->Chapter(1);
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);
	free_font(mf);
	g_free(ops);
}
#endif

char GTKPrintEntryDisp::Display(SWModule &imodule)
{
#ifdef USE_GTKMOZEMBED
	gchar *keytext = NULL;
	gchar *buf;
	SWBuf swbuf = "";
	gint mod_type;
	MOD_FONT *mf = get_font(imodule.Name());
	gint font_size;
	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");
	
	
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());

	const char *rework;	// for image size analysis rework.

	(const char *)imodule;	// snap to entry
	GS_message(((const char *)imodule.getRawEntry()));
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
			      "<a href=\"gnomesword.url?action=showModInfo&value=%s&module=%s\">"
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
}

char GTKPrintChapDisp::Display(SWModule &imodule)
{
#ifdef USE_GTKMOZEMBED
	VerseKey *key = (VerseKey *)(SWKey *)imodule;
	int curVerse = key->Verse();
	int curChapter = key->Chapter();
	int curBook = key->Book();
	int curTestament = key->Testament();
	int curPos = 0;
	gchar *utf8_key;
	gchar *buf;
	gchar *preverse = NULL;
	gchar *paragraphMark = NULL;
	gchar *br = NULL;
	gchar heading[32];
	SWBuf swbuf;
	
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	gboolean is_rtol = main_is_mod_rtol(imodule.Name());
	gboolean newparagraph = FALSE;
	mf = get_font(imodule.Name());
	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");
	
	if (!strcmp(imodule.Name(), "KJV"))
		paragraphMark = "&para;";
	else
		paragraphMark = "";

	swbuf = "";
	gtk_notebook_set_current_page(GTK_NOTEBOOK(widgets.notebook_text), 0);

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

	if (is_rtol)
		swbuf.append("<DIV ALIGN=right>");
	
	gecko_html_write(html,swbuf.c_str(),swbuf.length());
	
	swbuf = "";
	main_set_global_options(ops);
	strongs_on = ops->strongs;
	
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

		buf=g_strdup_printf(settings.showversenum
			? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
			  "<font size=\"%+d\" color=\"%s\">%d</font></A> "
			: "&nbsp; <A NAME=\"%d\"> </A>",
			key->Verse(),
			utf8_key,
			settings.verse_num_font_size + settings.base_font_size,
			settings.bible_verse_num_color,
			key->Verse());
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

	if (is_rtol)
		swbuf.append("</DIV></font></body></html>");
	else
		swbuf.append("</font></body></html>");
	
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
}
