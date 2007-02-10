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


#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <gbfosis.h>
#include <thmlosis.h>

#include <regex.h>
#include <string.h>

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

#include "gui/utilities.h"
#include "gui/widgets.h"
#include "gui/dialog.h"

#include "backend/sword_main.hh"
#include "backend/gs_osishtmlhref.h"


#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><STYLE type=\"text/css\"><!--A { text-decoration:none }%s--></STYLE></head>"
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
					window_x -= 20;
					window_y -= 20;
				} else {
					window_x = (window_x * 93)/100;
					window_y = (window_y * 93)/100;
				}
			} else {
				window_x = window_y = 10000; // degenerate: no resize.
			}
		}

		path += IMGSRC_LENGTH;
		save = *path;
		*path = '\0';
                resized += trail;
		*path = save;
		trail = path;

		// some play fast-n-loose with proper file spec.
		if (strncmp(path, "file:///", 8) == 0) {
			path += 7;
			resized += "file://";
		} else if (strncmp(path, "file:/", 6) == 0) {
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
			gui_generic_warning(
			    "An image file's size could not be determined.\nIs ImageMagick's 'identify' not installed?\n(Or possibly the image is of type other than bmp/gif/jpg/png.)\nGnomeSword cannot resize images to fit window.");
			settings.imageresize = 0;
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
	SWBuf swbuf = "";
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;
	gint mod_type;
	MOD_FONT *mf = get_font(imodule.Name());
	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");
	
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());

	const char *rework;	// for image size analysis rework.

	(const char *)imodule;	// snap to entry
	//g_message((const char *)imodule.getRawEntry());
	main_set_global_options(ops);
	mod_type = backend->module_type(imodule.Name());

	if (mod_type == BOOK_TYPE)
		keytext = g_convert(backend->treekey_get_local_name(
				settings.book_offset),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
	else if (mod_type == DICTIONARY_TYPE)
		keytext = g_strdup((char*)imodule.KeyText());
	else
		keytext = g_convert((char*)imodule.KeyText(),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
	swbuf.appendFormatted(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
			      (settings.doublespace ? DOUBLE_SPACE : ""),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color);

	swbuf.appendFormatted(  "<font color=\"%s\">",
				settings.bible_verse_num_color);

	swbuf.appendFormatted(	"<a href=\"gnomesword.url?action=showModInfo&value=%s&module=%s\">"
				"[*%s*]</a></font>[%s]<br>"
				"<font face=\"%s\" size=\"%s\">",
				imodule.Description(),
				imodule.Name(),
				imodule.Name(),
				(gchar*)keytext,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0");

	if ((backend->module_type(imodule.Name()) == PERCOM_TYPE) ||
		 !strcmp(imodule.getConfigEntry("SourceType"),"ThML"))
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

	if (swbuf.length())
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
	gecko_html_close(html);
	
	free_font(mf);
	g_free(ops);
	if (keytext)
		g_free(keytext);
}


void GTKChapDisp::getVerseBefore(SWModule &imodule)
{
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;
	gchar *utf8_key;
	SWMgr *mgr = be->get_main_mgr();

	SWModule *mod_top = mgr->getModule(imodule.Name());
	mod_top->setSkipConsecutiveLinks(true);
	*mod_top = sword::TOP;

	sword::VerseKey key_top( mod_top->KeyText() );
	SWModule *mod = mgr->getModule(imodule.Name());
	mod->setKey(imodule.getKey());

	VerseKey *key = (VerseKey *)(SWKey *)*mod;
	int chapter = key->Chapter();
	key->Verse(1);

	if (!key->_compare(key_top)) {
		swbuf.appendFormatted("<font face=\"%s\" size=\"%s\" color=\"%s\">",
			(mf->old_font)?mf->old_font:"",
			(mf->old_font_size)?mf->old_font_size:"+0",
			settings.bible_text_color);

		if ((!strcmp(settings.MainWindowModule, "KJV")))
			swbuf.appendFormatted("</font><div style=\"text-align: center\">%s<hr></div>",
					mod->Description());
		else
			swbuf.appendFormatted("</font><div style=\"text-align: center\"><p>%s</p><b>%s %d</b></div>",
					mod->Description(),
					_("Chapter"),
					chapter);
	} else {
		(*mod)--;

		utf8_key = g_convert((char*)key->getText(),
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     error);
		
		if (is_rtol)
			swbuf += "<DIV ALIGN=right>";
		swbuf.appendFormatted(settings.showversenum
				? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
				  "<font size=\"%s\" color=\"%s\">%d</font></A> "
				: "&nbsp; <A NAME=\"%d\"> </A>",
				0,
				utf8_key,
				(settings.versestyle) ? settings.verse_num_font_size : "-2",
				settings.bible_verse_num_color,
				key->Verse());

		swbuf.appendFormatted(
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				settings.bible_text_color);
		swbuf.appendFormatted(
				"%s</font><br><hr><div style=\"text-align: center\"><b>%s %d</b></div>",
					(const char *)*mod, _("Chapter"), chapter);
		if (is_rtol)
			swbuf += ("</DIV>");
	}
}

void GTKChapDisp::getVerseAfter(SWModule &imodule)
{
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;
	gchar *utf8_key;
	SWMgr *mgr = be->get_main_mgr();
	SWModule *mod_bottom = mgr->getModule(imodule.Name());
		mod_bottom->setSkipConsecutiveLinks(true);
	*mod_bottom = sword::BOTTOM;
	sword::VerseKey key_bottom( mod_bottom->KeyText() );
	SWModule *mod = mgr->getModule(imodule.Name());
	mod->setKey(imodule.getKey());
	VerseKey *key = (VerseKey *)(SWKey *)*mod;

	if (key_bottom._compare(key) < 1) {
		swbuf.appendFormatted(
			"<font face=\"%s\" size=\"%s\" color=\"%s\">",
			(mf->old_font)?mf->old_font:"",
			(mf->old_font_size)?mf->old_font_size:"+0",
			settings.bible_text_color);

		swbuf.appendFormatted(
			"</font><hr><div style=\"text-align: center\"><p>%s</p></div>",
					mod->Description());
	} else {
		int chapter = key->Chapter();
		if ((!strcmp(settings.MainWindowModule, "KJV")))
			swbuf.appendFormatted(
				"<hr><b>%s %d.</b><br><br>",
				_("Chapter"), chapter);
		else
			swbuf.appendFormatted(
				"<hr><div style=\"text-align: center\"><b>%s %d</b></div>",
				_("Chapter"), chapter);

		utf8_key = g_convert((char*)key->getText(),
				     -1,
				     UTF_8,
				     OLD_CODESET,
				     &bytes_read,
				     &bytes_written,
				     error);
		
		if (is_rtol)
			swbuf += "<DIV ALIGN=right>";
		swbuf.appendFormatted(settings.showversenum
				? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
				  "<font size=\"%s\" color=\"%s\">%d</font></A> "
				: "&nbsp; <A NAME=\"%d\"> </A>",
				0,
				utf8_key,
				(settings.versestyle)
				?settings.verse_num_font_size
				:"-2",
				settings.bible_verse_num_color,
				key->Verse());

		swbuf.appendFormatted(
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				settings.bible_text_color);
		swbuf += (const char *)*mod;
		swbuf+= "</font>";
		if (is_rtol)
			swbuf += ("</DIV>");
	}
}

//
// Read aloud some text, i.e. the current verse.
// Text is cleaned of '"', <tokens>, "&symbols;", and *n/*x strings,
// then scribbled out the local static socket with (SayText "...").
// Non-zero verse param is prefixed onto supplied text.
//
void GTKChapDisp::ReadAloud(unsigned int verse, const char *suppliedtext)
{
	static int tts_socket = -1;	// no initial connection.
	static int use_counter = -2;	// to shortcircuit early uses.

	if (settings.readaloud) {
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
		if (++use_counter < 1)
			return;

		GString *text = g_string_new(NULL);
		if (verse != 0)
			g_string_printf(text, "%d. ...  %s", verse, suppliedtext);
			// use of ". ..." is to induce proper pauses.
		else
			g_string_printf(text, "%s", suppliedtext);
#ifdef DEBUG
		g_message("ReadAloud: dirty: %s\n", text->str);
#endif

		// clean: no quotes (conflict w/festival syntax).
		for (s = strchr(text->str, '"'); s; s = strchr(s, '"'))
			*s = ' ';

		// clean: no <tokens>.
		for (s = strchr(text->str, '<'); s; s = strchr(s, '<')) {
			if (t = strchr(s, '>')) {
				while (s <= t)
					*(s++) = ' ';
			} else {
#ifdef DEBUG
				g_message("ReadAloud: Unmatched <> in %s\n", s);
#endif
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
#ifdef DEBUG
				g_message("ReadAloud: Unmatched &lt;&gt; in %s\n", s);
#endif
				goto out;
			}
		}

		// clean: no other &symbols;.
		for (s = strchr(text->str, '&'); s; s = strchr(s, '&')) {
			if (t = strchr(s, ';')) {
				while (s <= t)
					*(s++) = ' ';
			} else {
#ifdef DEBUG
				g_message("ReadAloud: Incomplete &xxx; in %s\n", s);
#endif
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
#ifdef DEBUG
		g_message("ReadAloud: clean: %s\n", text->str);
#endif

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
	gchar *preverse = NULL;
	gchar *paragraphMark = NULL;
	gchar *br = NULL;
	gchar heading[32];
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;

	GLOBAL_OPS * ops = main_new_globals(imodule.Name());
	is_rtol = main_is_mod_rtol(imodule.Name());
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
	swbuf.appendFormatted(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
			      (settings.doublespace ? DOUBLE_SPACE : ""),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color);
	if (is_rtol)
		swbuf += "<DIV ALIGN=right>";
	
	gecko_html_write(html,swbuf.c_str(),swbuf.length());
	
	swbuf = "";
	main_set_global_options(ops);
	strongs_on = ops->strongs;
	getVerseBefore(imodule);
	
	gecko_html_write(html,swbuf.c_str(),swbuf.length());
	
	swbuf = "";

	for (key->Verse(1); (key->Book() == curBook && key->Chapter()
				== curChapter && !imodule.Error()); imodule++) {
		int x = 0;
		sprintf(heading, "%d", x);
		while ((preverse
			= backend->get_entry_attribute("Heading", "Preverse",
							    heading)) != NULL) {
			const char *preverse2 = imodule.RenderText(preverse);
			swbuf.appendFormatted("<br><b>%s</b><br><br>", preverse2);
			g_free(preverse);
			++x;
			sprintf(heading, "%d", x);
		}
		utf8_key = g_convert((char*)key->getText(),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);

		// special contrasty highlighting
		if ((key->Verse() == curVerse) && settings.versehighlight)
			swbuf.appendFormatted("<table bgcolor=\"%s\"><tr><td>",
					      settings.highlight_bg);

		swbuf.appendFormatted(settings.showversenum
			? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
			  "<font size=\"%s\" color=\"%s\">%d</font></A> "
			: "&nbsp; <A NAME=\"%d\"> </A>",
			key->Verse(),
			utf8_key,
			settings.verse_num_font_size,
			settings.bible_verse_num_color,
			key->Verse());

		swbuf.appendFormatted(
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(key->Verse() == curVerse)
				? (settings.versehighlight
				   ? settings.highlight_fg
				   : settings.currentverse_color)
				: settings.bible_text_color);
				
		if (key->Verse() == curVerse   ||
		    key->Verse() == curVerse-1 ||
		    key->Verse() == curVerse-2 ||
		    key->Verse() == curVerse+2 ||
		    key->Verse() == curVerse+1 ) {
			main_set_strongs_morphs(ops);
		} else {
			main_set_strongs_morphs_off(ops);
		}
		
		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			swbuf += paragraphMark;;
		}

		// correct a highlight glitch: in poetry verses which end in
		// a forced line break, we must remove the break to prevent
		// the enclosing <table> from producing a double break.
		if (settings.versehighlight &&		// doing <table> h/l.
		    !settings.versestyle &&		// paragraph format.
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
			swbuf += text->str;
			g_string_free(text, TRUE);
		} else
			swbuf += (const char *)imodule;
		
		buf = g_strdup_printf("%s", (const char *)imodule);

		if (key->Verse() == curVerse)
			GTKChapDisp::ReadAloud(curVerse, imodule);

		if (settings.versestyle) {
			if (g_strstr_len(buf, strlen(buf), "<!p>")) {
				newparagraph = TRUE;
			} else {
				newparagraph = FALSE;
			}
			swbuf.append(((key->Verse() == curVerse) &&
				      settings.versehighlight)
				     ? "</font>"
				     : "</font><br>");
		} else {
			swbuf.append("</font>");
		}

		// special contrasty highlighting
		if ((key->Verse() == curVerse) && settings.versehighlight)
			swbuf.appendFormatted("</td></tr></table>");

		g_free(buf);
		
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
		
		swbuf = "";
	}
	swbuf = "";
	getVerseAfter(imodule);

	// Reset the Bible location before GTK gets access:
	// Mouse activity destroys this key, so we must be finished with it.
	key->Book(curBook);
	key->Chapter(curChapter);
	key->Verse(curVerse);

	if (is_rtol)
		swbuf += ("</DIV></body></html>");
	else
		swbuf += "</body></html>";
	
	if (swbuf.length()) 
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
	gecko_html_close(html);
	if (curVerse > 2) {
		buf = g_strdup_printf("%d", curVerse - 2);
		gecko_html_jump_to_anchor (html,buf);
		//embed_go_to_anchor(html, buf);
		g_free(buf);
	}	
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
#ifdef USE_GTKMOZEMBED
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GtkMozEmbed *html = GTK_MOZ_EMBED(gtkText);
#else
	GtkHTML *html = GTK_HTML(gtkText);
	PangoContext* pc = gtk_widget_create_pango_context(gtkText);
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(desc, ((mf->old_font)?mf->old_font:"Serirf"));
	gtk_widget_modify_font (gtkText, desc);
	gboolean was_editable = gtk_html_get_editable(html);
	if (was_editable)
		gtk_html_set_editable(html, FALSE);
#endif

	(const char *)imodule;	// snap to entry
	main_set_global_options(ops);

	if (type == 3)
		keytext = be->treekey_get_local_name(d->offset);
	else
		keytext = imodule.KeyText();

	if (type == 4)
		g_string_printf(str, HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<font face=\"%s\" size=\"%s\">%s"
				"</font></body></html>",
				(settings.doublespace ? DOUBLE_SPACE : ""),
				settings.bible_bg_color,
				settings.bible_text_color,
				settings.link_color,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(settings.imageresize
				 ? AnalyzeForImageSize((const char *)imodule,
						       GDK_WINDOW(gtkText->window),
						       type)
				 : (const char *)imodule /* untouched */));
	else
		g_string_printf(str, HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">"
				"<a href=\"gnomesword.url?action=showModInfo&value=%s&module=%s\"><font color=\"%s\">"
				"[%s]</font></a>[%s]<br>"
				"<font face=\"%s\" size=\"%s\">%s"
				"</font></body></html>",
				(settings.doublespace ? DOUBLE_SPACE : ""),
				settings.bible_bg_color,
				settings.bible_text_color,
				settings.link_color,
				imodule.Description(),
				imodule.Name(),
				settings.bible_verse_num_color,
				imodule.Name(),
				(gchar*)keytext,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(settings.imageresize
				 ? AnalyzeForImageSize((const char *)imodule,
						       GDK_WINDOW(gtkText->window),
						       type)
				 : (const char *)imodule /* untouched */));

#ifdef USE_GTKMOZEMBED
	if (str->len)
		gtk_moz_embed_render_data(html, str->str, str->len,
					"file:///sword",
					"text/html");
#else
	if (str->len)
		gtk_html_load_from_string(html, str->str, str->len);
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
	GtkMozEmbed *html = GTK_MOZ_EMBED(gtkText);
#else
	GtkHTML *html = GTK_HTML(gtkText);
	PangoContext* pc = gtk_widget_create_pango_context(gtkText);
	PangoFontDescription *desc = pango_context_get_font_description(pc);
	pango_font_description_set_family(desc,
					  ((mf->old_font)?mf->old_font:"Serif"));
	gtk_widget_modify_font (gtkText, desc);
	gboolean was_editable = gtk_html_get_editable(html);
#endif
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

	g_string_printf(str,	HTML_START
				"<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
				(settings.doublespace ? DOUBLE_SPACE : ""),
				settings.bible_bg_color,
				settings.bible_text_color,
				settings.link_color);
	main_dialog_set_global_options((BackEnd*)be, ops);
	for (key->Verse(1); (key->Book() == curBook && key->Chapter()
				== curChapter && !imodule.Error()); imodule++) {
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
			buf = g_strdup_printf("<table bgcolor=\"%s\"><tr><td>",
					      settings.highlight_bg);
			str = g_string_append(str, buf);
			g_free(buf);
		}

		buf = g_strdup_printf(settings.showversenum
			? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
			  "<font size=\"%s\" color=\"%s\">%d</font></A> "
			: "&nbsp; <A NAME=\"%d\"> </A>",
			key->Verse(),
			(char*)key->getText(),
			settings.verse_num_font_size,
			settings.bible_verse_num_color,
			key->Verse());
		str = g_string_append(str, buf);
		g_free(buf);
		buf = g_strdup_printf(
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				(key->Verse() == curVerse)
				? (settings.versehighlight
				   ? settings.highlight_fg
				   : settings.currentverse_color)
				: settings.bible_text_color);

		str = g_string_append(str, buf);
		g_free(buf);

		if (newparagraph && versestyle) {
			newparagraph = FALSE;
			str = g_string_append(str, paragraphMark);
		}
		if (key->Verse() == curVerse   ||
		    key->Verse() == curVerse-1 ||
		    key->Verse() == curVerse-2 ||
		    key->Verse() == curVerse+2 ||
		    key->Verse() == curVerse+1 ) {
			main_set_dialog_strongs_morphs(be, ops);
		} else {
			main_set_dialog_strongs_morphs_off(be, ops);
		}

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
			str = g_string_append(str, text->str);
			g_string_free(text, TRUE);
		} else
			str = g_string_append(str, (const char *)imodule);

		buf = g_strdup_printf("%s", (const char *)imodule);

		if (versestyle) {
			buf2 = g_strdup_printf(" %s",
					       ((key->Verse() == curVerse) &&
						settings.versehighlight)
					       ? "</font>"
					       : "</font><br>");

			if ((strstr(buf, "<!P>") == NULL) &&
			     (strstr(buf, "<p>") == NULL) ) {
				newparagraph = FALSE;
			} else {
				newparagraph = TRUE;
			}
		} else {
			if (strstr(buf, "<!P>") == NULL)
				buf2 = g_strdup_printf(" %s", "</font>");
			else
				buf2 = g_strdup_printf(" %s", "</font><p>");
		}
		str = g_string_append(str, buf2);
		g_free(buf);
		g_free(buf2);

		// special contrasty highlighting
		if ((key->Verse() == curVerse) && settings.versehighlight)
		    str = g_string_append(str, ("</td></tr></table>"));
	}
	buf = g_strdup_printf("%s", "</body></html>");
	str = g_string_append(str, buf);
	g_free(buf);
#ifdef USE_GTKMOZEMBED
	if (str->len)
		gtk_moz_embed_render_data(html, str->str, str->len,
					  "file:///sword",
					  "text/html");
	if (curVerse > 2) {
		buf = g_strdup_printf("%d", curVerse - 2);
		//embed_go_to_anchor(html, buf);
		g_free(buf);
	}
#else
	if (str->len) {
		gtk_html_load_from_string(html, str->str, str->len);
	}
	gtk_html_set_editable(html, was_editable);
	if (curVerse > 2) {
		buf = g_strdup_printf("%d", curVerse - 2);
		gtk_html_jump_to_anchor(html, buf);
		g_free(buf);
	}
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
	gchar *keytext = NULL;
	SWBuf swbuf = "";
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;
	gint mod_type;
	MOD_FONT *mf = get_font(imodule.Name());
	
	if(!GTK_WIDGET_REALIZED(GTK_WIDGET(gtkText))) return 0;
	GeckoHtml *html = GECKO_HTML(gtkText);
	gecko_html_open_stream(html,"text/html");
	
	
	GLOBAL_OPS * ops = main_new_globals(imodule.Name());

	const char *rework;	// for image size analysis rework.

	(const char *)imodule;	// snap to entry
	//g_message((const char *)imodule.getRawEntry());
	main_set_global_options(ops);
	mod_type = backend->module_type(imodule.Name());

	if (mod_type == BOOK_TYPE)
		keytext = g_convert(backend->treekey_get_local_name(
				settings.book_offset),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
	else if (mod_type == DICTIONARY_TYPE)
		keytext = g_strdup((char*)imodule.KeyText());
	else
		keytext = g_convert((char*)imodule.KeyText(),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);
	swbuf.appendFormatted(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
			      (settings.doublespace ? DOUBLE_SPACE : ""),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color);

	swbuf.appendFormatted(  "<font color=\"%s\">",
				settings.bible_verse_num_color);

	swbuf.appendFormatted(	"<a href=\"gnomesword.url?action=showModInfo&value=%s&module=%s\">"
				"[*%s*]</a></font>[%s]<br>"
				"<font face=\"%s\" size=\"%s\">",
				imodule.Description(),
				imodule.Name(),
				imodule.Name(),
				(gchar*)keytext,
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0");
				
	swbuf.append((const char *)imodule);
	
	swbuf.append("</font></body></html>");

	if (swbuf.length())
		gecko_html_write(html,swbuf.c_str(),swbuf.length());
	gecko_html_close(html);
	free_font(mf);
	g_free(ops);
	if (keytext)
		g_free(keytext);
}

char GTKPrintChapDisp::Display(SWModule &imodule)
{
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
	gsize bytes_read;
	gsize bytes_written;
	GError **error = NULL;
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
	swbuf.appendFormatted(HTML_START
			      "<body bgcolor=\"%s\" text=\"%s\" link=\"%s\">",
			      (settings.doublespace ? DOUBLE_SPACE : ""),
			      settings.bible_bg_color,
			      settings.bible_text_color,
			      settings.link_color);
	if (is_rtol)
		swbuf += "<DIV ALIGN=right>";
	
	gecko_html_write(html,swbuf.c_str(),swbuf.length());
	
	swbuf = "";
	main_set_global_options(ops);
	strongs_on = ops->strongs;
	
	gecko_html_write(html,swbuf.c_str(),swbuf.length());
	
	swbuf = "";

	for (key->Verse(1); (key->Book() == curBook && key->Chapter()
				== curChapter && !imodule.Error()); imodule++) {
		int x = 0;
		sprintf(heading, "%d", x);
		while ((preverse
			= backend->get_entry_attribute("Heading", "Preverse",
							    heading)) != NULL) {
			const char *preverse2 = imodule.RenderText(preverse);
			swbuf.appendFormatted("<br><b>%s</b><br><br>", preverse2);
			g_free(preverse);
			++x;
			sprintf(heading, "%d", x);
		}
		utf8_key = g_convert((char*)key->getText(),
                             -1,
                             UTF_8,
                             OLD_CODESET,
                             &bytes_read,
                             &bytes_written,
                             error);

		// special contrasty highlighting
		/* if ((key->Verse() == curVerse) && settings.versehighlight)
			swbuf.appendFormatted("<table bgcolor=\"%s\"><tr><td>",
					      settings.highlight_bg); */

		swbuf.appendFormatted(settings.showversenum
			? "&nbsp; <A NAME=\"%d\" HREF=\"sword:///%s\">"
			  "<font size=\"%s\" color=\"%s\">%d</font></A> "
			: "&nbsp; <A NAME=\"%d\"> </A>",
			key->Verse(),
			utf8_key,
			settings.verse_num_font_size,
			settings.bible_verse_num_color,
			key->Verse());

		swbuf.appendFormatted(
				"<font face=\"%s\" size=\"%s\" color=\"%s\">",
				(mf->old_font)?mf->old_font:"",
				(mf->old_font_size)?mf->old_font_size:"+0",
				settings.bible_text_color);

		if (newparagraph && settings.versestyle) {
			newparagraph = FALSE;
			swbuf += paragraphMark;;
		}

		swbuf += (const char *)imodule;

		buf = g_strdup_printf("%s", (const char *)imodule);

		if (settings.versestyle) {
			if (g_strstr_len(buf, strlen(buf), "<!p>")) {
				newparagraph = TRUE;
			} else {
				newparagraph = FALSE;
			}
			swbuf.append("</font><br>");
		} else {
			swbuf.append("</font>");
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
		swbuf += ("</DIV></body></html>");
	else
		swbuf += "</body></html>";
	
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
}
