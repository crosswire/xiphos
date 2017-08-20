/*
 * Xiphos Bible Study Tool
 * url.cc - support functions
 *
 * Copyright (C) 2004-2017 Xiphos Developer Team
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

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <ctype.h>
#include <url.h>

#include <swbuf.h>

#include <swmgr.h>
#include <swmodule.h>

#include <gtk/gtk.h>
#include <glib/gstdio.h>

#ifdef USE_WEBKIT_EDITOR
#include "editor/webkit_editor.h"
#else
#include "editor/slib-editor.h"
#endif

#include "gui/about_modules.h"
#include "gui/xiphos.h"
#include "gui/widgets.h"
#include "gui/utilities.h"
#include "gui/sidebar.h"
#include "gui/dialog.h"
#include "gui/parallel_dialog.h"
#include "gui/parallel_view.h"
#include "gui/main_window.h"
//#include "gui/toolbar_nav.h"

#include "main/url.hh"
#include "main/lists.h"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/module_dialogs.h"
#include "main/parallel_view.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/xml.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

gint in_url;

using namespace sword;

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

static void alert_url_not_found(const gchar *url)
{
	GS_DIALOG *dialog;
	GString *dialog_text = g_string_new(NULL);

	dialog = gui_new_dialog();
	dialog->stock_icon =
#if GTK_CHECK_VERSION(3, 10, 0)
	    (gchar *)"dialog-information";
#else
	    (gchar *)GTK_STOCK_DIALOG_INFO;
#endif
	g_string_printf(dialog_text,
			"<span weight=\"bold\">%s</span>\n\n%s",
			_("URL not found:"),
			url);

	dialog->label_top = dialog_text->str;
	dialog->ok = TRUE;

	gui_alert_dialog(dialog);
	g_free(dialog);
	g_string_free(dialog_text, TRUE);
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint (const gchar * filename, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_studypad(const gchar *filename, gboolean clicked)
{
	editor_create_new(filename, NULL, STUDYPAD_EDITOR);
	return 1;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint (const gchar * filename, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

const char *display_progs[] = {
    "/usr/bin/gvfs-open",
    "/usr/bin/gnome-open",
    "/usr/bin/display",
    NULL};

static gint show_separate_image(const gchar *filename, gboolean clicked)
{
	if (clicked) {
#ifdef WIN32
		gboolean result;
		gchar *win_path = g_strdup(filename);
		//ShellExecute can handle all / or all \\ but not a combination
		win_path = g_strdelimit(win_path, "/", '\\');
		result = xiphos_open_default((const gchar *)win_path);
		g_free(win_path);
		if (result == FALSE) {
			gui_generic_warning((char *)"Could not display that image");
		}
#else
		FILE *result;
		GString *cmd = g_string_new(NULL);
		int i;

		for (i = 0; display_progs[i]; i++) {
			if (g_access(display_progs[i], X_OK) == 0)
				break;
		}
		if (display_progs[i] == NULL) {
			gui_generic_warning((char *)"Xiphos cannot find\nan image display program.");
			return 0;
		}

		XI_print(("file = %s\n", filename));
		g_string_printf(cmd, "%s \"%s\" < /dev/null > /dev/null 2>&1 &",
				display_progs[i], filename);

		if ((result = popen(cmd->str, "r")) == NULL) {
			g_string_printf(cmd,
					_("Xiphos could not execute %s"),
					display_progs[i]);
			gui_generic_warning(cmd->str);
		} else {
			gchar output[258];
			if (fgets(output, 256, result) != NULL) {
				g_string_truncate(cmd, 0);
				g_string_append(cmd,
						_("Viewer error:\n"));
				g_string_append(cmd, output);
				gui_generic_warning(cmd->str);
			}
			pclose(result);
		}
		g_string_free(cmd, TRUE);
#endif
	} else {
		gui_set_statusbar(filename);
	}
	return 1;
}

/******************************************************************************
 * Name
 *   show_mod_info
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_mod_info(const gchar * module, const gchar * description,
 *						gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_mod_info(const gchar *module, const gchar *description,
			  gboolean clicked)
{
	if (clicked) {
		XI_print(("module = %s\n", module));
		gui_display_about_module_dialog((gchar *)module);
	} else {
		/* some mod descriptions contain fun(ny) characters */
		GString *desc_clean = hex_decode(description);
		XI_print(("description = %s\n", desc_clean->str));
		gui_set_statusbar(desc_clean->str);
		g_string_free(desc_clean, TRUE);
	}
	return 1;
}

/******************************************************************************
 * Name
 *   show_parallel
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_parallel(const gchar * value, const gchar * type,gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_parallel(const gchar *svalue, const gchar *stype,
			  gboolean clicked)
{
	if (!strcmp(stype, "swap")) {
		if (clicked) {
			main_swap_parallel_with_main((gchar *)svalue);
		} else {
			gchar *buf = g_strdup_printf(
			    _("Show %s in main window"), svalue);
			gui_set_statusbar(buf);
			g_free(buf);
		}
	}
	if (!strcmp(stype, "verse")) {
		if (clicked) {
			gtk_entry_set_text(GTK_ENTRY(navbar_parallel.lookup_entry), svalue);
			gtk_widget_activate(navbar_parallel.lookup_entry);
		}
	}
	return 1;
}

/******************************************************************************
 * Name
 *   show_morph
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_morph(const gchar *module_name,
 *		     const gchar *type,
 *		     const gchar *value,
 *		     gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_morph(const char *module_name,
		       const gchar *stype,
		       const gchar *svalue,
		       gboolean clicked)
{
	const gchar *modbuf = NULL;

	if (!strcmp(stype, "Greek") ||
	    strstr(stype, "x-Robinson") ||
	    strstr(stype, "robinson") ||
	    strstr(stype, "Robinson")) {
		if (backend->get_key_testament(module_name,
					       settings.currentverse) == 2) {
			if (backend->is_module("Robinson"))
				modbuf = "Robinson";
		} else {
			if (backend->is_module("Packard"))
				modbuf = "Packard";
		}
	}
	//XI_message(("modbuf = %s", modbuf));
	if (clicked) {
		main_display_dictionary(modbuf, (gchar *)svalue);
	} else {
		gchar *mybuf = main_get_rendered_text(modbuf, (gchar *)svalue);
		//XI_message(("mybuf = %s", mybuf));
		if (mybuf) {
			main_information_viewer(modbuf,
						mybuf,
						(gchar *)svalue,
						"showMorph",
						(gchar *)stype,
						NULL,
						NULL);
			g_free(mybuf);
		}
	}
	return 1;
}

/******************************************************************************
 * Name
 *   show_strongs
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_strongs(const gchar * type, const gchar * value, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_strongs(const gchar *stype, const gchar *svalue,
			 gboolean clicked)
{
	const gchar *modbuf = NULL;

	if (!strncmp(settings.MainWindowModule, "NASB", 4)) {
		if (!strcmp(stype, "Greek"))
			modbuf = "NASGreek";
		else
			modbuf = "NASHebrew";
	} else {
		if (stype && (*stype != '\0')) {
			if (!strcmp(stype, "Greek"))
				modbuf = settings.lex_greek;
			else
				modbuf = settings.lex_hebrew;
		} else
			modbuf = "InvStrongsRealGreek";
	}

	if (clicked) {
		main_display_dictionary(modbuf, (gchar *)svalue);
	} else {
		gchar *mybuf = main_get_rendered_text(modbuf, (gchar *)svalue);
		if (mybuf) {
			main_information_viewer(
			    modbuf,
			    mybuf,
			    (gchar *)svalue,
			    "showStrongs",
			    (gchar *)stype,
			    NULL,
			    NULL);
			g_free(mybuf);
		}
	}

	return 1;
}

/******************************************************************************
 * Name
 *   show_note
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_note(module, passage, stype, svalue, gblooean clicked)
 *
 * Description
 *   show a footnote in the previewer.
 *
 * Return value
 *   gint
 */

static gint show_note(const gchar *module, const gchar *passage,
		      const gchar *stype, const gchar *svalue, gboolean clicked)
{
	gchar *tmpbuf = NULL;
	gchar *buf = NULL;
	gchar *work_buf = NULL;
	GString *str = g_string_new(NULL);
	GList *vlist = NULL, *chaser = NULL;

	if (!in_url || !svalue || !*svalue)
		return 1;

	if (!backend->is_module((gchar *)module))
		module = settings.MainWindowModule;

	if (strlen(passage) < 5)
		passage = settings.currentverse;

	//
	// if we are asking for a note/xref in n:0,
	// we must stop autonormalization for a moment.
	//
	int stop_autonorm = ((strstr(passage, ":0")) != NULL);

	if (stop_autonorm) {
		SWMgr *mgr = backend->get_mgr();
		backend->display_mod = mgr->Modules[module];
		backend->display_mod->setKey(passage);
		VerseKey *vkey = (VerseKey *)(SWKey *)(*backend->display_mod);
		int t = backend->module_type(module);
		if ((t == TEXT_TYPE) || (t == COMMENTARY_TYPE))
			vkey->setAutoNormalize(0);
		vkey->setChapter(vkey->getChapter() + 1);
		vkey->setVerse(0);
	} else
		backend->set_module_key((gchar *)module, (gchar *)passage);

	if (strchr(stype, 'x') && clicked) {
		tmpbuf = backend->get_entry_attribute("Footnote",
						      (gchar *)svalue,
						      "refList");
		if (tmpbuf) {
			main_display_verse_list_in_sidebar(settings.currentverse,
							   (gchar *)module,
							   tmpbuf);
			g_free(tmpbuf);
		}
	} else if (strchr(stype, 'n') && !clicked) {
		tmpbuf = backend->get_entry_attribute("Footnote",
						      (gchar *)svalue,
						      "body");
		buf = backend->render_this_text((gchar *)module, (gchar *)tmpbuf);
		if (tmpbuf)
			g_free(tmpbuf);
		if (buf) {
			main_information_viewer((gchar *)module,
						buf,
						(gchar *)svalue,
						"showNote",
						(gchar *)stype,
						NULL,
						NULL);
			if (buf)
				g_free(buf);
		}
	} else if (strchr(stype, 'x') && !clicked) {
		tmpbuf = backend->get_entry_attribute("Footnote",
						      (gchar *)svalue,
						      "refList");
		if (settings.xrefs_in_verse_list) {
			main_display_verse_list_in_sidebar(settings.currentverse,
							   (gchar *)module,
							   tmpbuf);
			g_free(tmpbuf);
		} else {
			vlist = chaser = backend->parse_verse_list(module, tmpbuf, settings.currentverse);
			while (chaser != NULL) {
				buf = g_strdup_printf(
				    "<a href=\"sword://%s/%s\">"
				    "<font color=\"%s\">%s,</font></a><br/>",
				    (gchar *)module,
				    (const char *)chaser->data,
				    settings.bible_text_color,
				    (const char *)chaser->data);
				str = g_string_append(str, buf);
				g_free(buf);
				g_free((char *)chaser->data);
				chaser = g_list_next(chaser);
			}
			g_list_free(vlist);
			g_free(tmpbuf);

			buf = g_strdup_printf("<a href=\"sword://%s/%s\">"
					      "<font color=\"%s\">%s%s</font></a><br/>",
					      (gchar *)module,
					      settings.currentverse,
					      settings.bible_text_color,
					      _("Back to "),
					      settings.currentverse);
			str = g_string_append(str, buf);
			g_free(buf);

			main_information_viewer((gchar *)module,
						str->str,
						(gchar *)svalue,
						"showNote",
						(gchar *)stype,
						NULL,
						NULL);
		}
	}

	if (work_buf)
		g_free(work_buf);
	g_string_free(str, TRUE);
	return 1;
}

/******************************************************************************
 * Name
 *   reference_uri
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint reference_uri(const gchar * url)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_ref(const gchar *module, const gchar *list, gboolean clicked)
{
	if (!clicked)
		return 1;

	if (!backend->is_module(module))
		module = settings.MainWindowModule;
	main_display_verse_list_in_sidebar(settings.currentverse,
					   (gchar *)module,
					   (gchar *)list);
	return 1;
}

/******************************************************************************
 * Name
 *   show_module_and_key
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint show_module_and_key(const gchar * module, const gchar * key,
					const gchar * type, gboolean clicked)
 *
 * Description
 *
 *
 *
 * Return value
 *   gint
 */

static int show_module_and_key(const char *module, const char *key,
			       const char *stype, gboolean clicked)
{
	gint mod_type;

	if (module && (strlen((char *)module) < 3) &&
	    backend->is_Bible_key((char *)module, key, settings.currentverse)) {
		module = settings.MainWindowModule;
	}
	if (!clicked) {
		return 1;
	}

	if (backend->is_module(module)) {
		if (!strcmp(stype, "newTab")) {
			main_open_bookmark_in_new_tab((gchar *)module,
						      (gchar *)key);
			return 1;
		}
		if (!strcmp(stype, "newDialog")) {
			if (module && (main_get_mod_type((gchar *)module) == PERCOM_TYPE)) {
				editor_create_new(module, key, TRUE);
				return 1;
			}
			main_dialog_goto_bookmark((gchar *)module,
						  (gchar *)key);
			return 1;
		}
		mod_type = backend->module_type((gchar *)module);
		switch (mod_type) {
		case TEXT_TYPE:
		case COMMENTARY_TYPE:
		case PERCOM_TYPE:
			if (strpbrk(key, "-;,")) { // >1 verse marked
				main_display_verse_list_in_sidebar(settings.currentverse,
								   (gchar *)module, (gchar *)key);
			} else {
				gchar *url =
				    g_strdup_printf("sword://%s/%s",
						    (module ? module : ""),
						    key);
				sword_uri(url, TRUE);
				g_free(url);
			}
			break;
		case DICTIONARY_TYPE:
			main_display_dictionary((gchar *)module,
						(gchar *)key);
			break;
		case BOOK_TYPE:
		case PRAYERLIST_TYPE:
			main_display_book((gchar *)module, (gchar *)key);
			if (gtk_notebook_get_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book)) != 1)
				gtk_notebook_set_current_page(
				    GTK_NOTEBOOK(widgets.notebook_comm_book),
				    1);
			break;
		}
	}
	settings.addhistoryitem = TRUE;
	return 1;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint (const gchar * type, const gchar * value,
			gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

static gint show_in_previewer(const gchar *url)
{

	gchar **work_buf = NULL;
	gchar *mybuf = NULL;

	work_buf = g_strsplit(url, "/", 4);

	// might be an abbrev.  get the real.
	const char *real_mod = main_get_name(work_buf[MODULE]);

	mybuf = main_get_rendered_text((real_mod ? real_mod : work_buf[MODULE]),
				       work_buf[KEY]);

	if (mybuf) {
		main_information_viewer(
		    (gchar *)(real_mod ? real_mod : work_buf[MODULE]),
		    mybuf,
		    (gchar *)work_buf[KEY],
		    NULL,
		    NULL,
		    NULL,
		    NULL);
		g_free(mybuf);
	}
	g_strfreev(work_buf);
	return 1;
}

/******************************************************************************
 * Name
 *   sword_uri
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint sword_uri(const gchar * url)
 *
 * Description
 *   handle a sword uri in the form 'sword://KJV/1John5:8'
 *                                   'sword://MHC/Genesis1:1'
 *                                   'sword:///Romans8:28'
 *   and display in the appropriate pane
 *
 * Return value
 *   gint
 */

gint sword_uri(const gchar *url, gboolean clicked)
{
	const gchar *key = NULL;
	gchar *tmpkey = NULL;
	gint mod_type;
	gint verse_count;
	gchar **work_buf = NULL;
	gchar *mod;

	// don't recurse between paratab and here.
	static gboolean handling_uri = FALSE;

	if (handling_uri)
		return 0;
	handling_uri = TRUE;

	if (!clicked) {
		gchar *name = g_strstr_len(url, 10, "://");
		if (!name)
			gui_set_statusbar(url);
		else {
			name += 3; // at name beginning.
			gchar *slash = g_strstr_len(name, 20, "/");
			if (slash)
				*slash = '\0'; // limit to name end.

			// might be an abbrev.  get the real.
			const char *real_mod = main_get_name(name);

			int mod_type = backend->module_type(real_mod ? real_mod : name);

			if (slash)
				*slash = '/';

			if (mod_type == DICTIONARY_TYPE)
				show_in_previewer(url);
			else
				gui_set_statusbar(name + ((*name == '/') ? 1 : 0));
		}
		handling_uri = FALSE;
		return 1;
	}

	work_buf = g_strsplit(url, "/", 4);
	if (!work_buf[MODULE] && !work_buf[KEY]) {
		alert_url_not_found(url);
		g_strfreev(work_buf);
		handling_uri = FALSE;
		return 0;
	}
	if (!work_buf[KEY]) {
		tmpkey = work_buf[MODULE];
	} else
		tmpkey = work_buf[KEY];
	if ((settings.special_anchor = strchr(tmpkey, '#')) ||
	    (settings.special_anchor = strchr(tmpkey, '!')))
		*(settings.special_anchor++) = '\0';

	XI_message(("work_buf: %s, %s",
		    work_buf[MODULE],
		    (work_buf[KEY] ? work_buf[KEY] : "-null-")));

	mod = (work_buf[MODULE] && *work_buf[MODULE]
		   ? work_buf[MODULE]
		   : settings.MainWindowModule);

	const char *real_mod = main_get_name(mod);
	if (real_mod)
		mod = (gchar *)real_mod;
	
	if (strpbrk(tmpkey, "-;,") && // >1 verse marked
	    (main_get_mod_type(mod) == TEXT_TYPE)) {
		main_display_verse_list_in_sidebar(settings.currentverse, mod, tmpkey);
		handling_uri = FALSE;
		return 1;
	}

	verse_count = 1; //backend->is_Bible_key(mykey, settings.currentverse);
	if (backend->is_module(mod)) {
		mod_type = backend->module_type(mod);
		switch (mod_type) {
		case TEXT_TYPE:
			key = main_update_nav_controls(mod, tmpkey);
			main_display_bible(mod, key);
			if (settings.comm_showing)
				main_display_commentary(NULL, key);
			main_keep_bibletext_dialog_in_sync((gchar *)key);
			editor_sync_with_main();

			if (key)
				g_free((gchar *)key);
			break;
		case COMMENTARY_TYPE:
			if (gtk_notebook_get_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book)) != 0) {
				gchar *save = settings.special_anchor;
				gtk_notebook_set_current_page(
				    GTK_NOTEBOOK(widgets.notebook_comm_book), 0);
				settings.special_anchor = save;
			}
			settings.comm_showing = TRUE;
			key = main_update_nav_controls(mod, tmpkey);
			main_display_commentary(mod, key);
			main_display_bible(NULL, key);
			main_keep_bibletext_dialog_in_sync((gchar *)key);
			if (key)
				g_free((gchar *)key);
			break;
		case DICTIONARY_TYPE:
			main_display_dictionary(mod, tmpkey);
			break;
		case BOOK_TYPE:
			if (gtk_notebook_get_current_page(GTK_NOTEBOOK(widgets.notebook_comm_book)) != 1) {
				gchar *save = settings.special_anchor;
				gtk_notebook_set_current_page(
				    GTK_NOTEBOOK(widgets.notebook_comm_book), 1);
				settings.special_anchor = save;
			}
			settings.comm_showing = FALSE;
			main_display_book(mod, tmpkey);
			break;
		}
	} else { /* module name not found or not given */
		if (verse_count) {
			key = main_update_nav_controls(settings.MainWindowModule, tmpkey);
			/* display in current Bible and Commentary */
			main_display_commentary(NULL, key);
			main_display_bible(NULL, key);
			main_keep_bibletext_dialog_in_sync((gchar *)key);

			editor_sync_with_main();

			if (key)
				g_free((gchar *)key);
		} else {
			alert_url_not_found(url);
		}
	}
	g_strfreev(work_buf);
	settings.addhistoryitem = TRUE;
	handling_uri = FALSE;
	return 1;
}

/******************************************************************************
 * Name
 *   main_url_handler
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *   gint main_url_handler(const gchar * url, gboolean clicked)
 *
 * Description
 *
 *
 * Return value
 *   gint
 */

gint main_url_handler(const gchar *url, gboolean clicked)
{
	gchar *action = NULL;
	gchar *stype = NULL;
	gchar *svalue = NULL;
	gchar *module = NULL;
	gchar *passage = NULL;
	gchar *morph = NULL;
	gchar *strongs = NULL;
	int retval = 0; // assume failure.

	XI_message(("main_url_handler => %s", url));

	if (strstr(url, "sword://") ||
	    strstr(url, "bible://")) {
		GString *url_clean = hex_decode(url);
		XI_message(("url_clean = %s", url_clean->str));

		retval = sword_uri(url_clean->str, clicked);
		g_string_free(url_clean, TRUE);
	} else if (strstr(url, "passagestudy.jsp") ||
		   strstr(url, "xiphos.url")) {

		// another minor nightmare: re-encode / and : in hex.
		gchar *place;
		gchar tmpbuf[1023];
		GString *tmpstr = g_string_new(NULL);

		place = (char *)strchr(url, '?'); // url's beginning, as-is.
		strncpy(tmpbuf, url, (++place) - url);
		tmpbuf[place - url] = '\0';
		tmpstr = g_string_append(tmpstr, tmpbuf);
		for (/* */; *place; ++place) {
			switch (*place) {
			case '/':
				tmpstr = g_string_append(tmpstr, "%2F");
				break;
			case ':':
				tmpstr = g_string_append(tmpstr, "%3A");
				break;
			case ' ':
				tmpstr = g_string_append(tmpstr, "%20");
				break;
			default:
				tmpstr = g_string_append_c(tmpstr, *place);
			}
		}

		/* passagestudy.jsp?action=showStrongs&type= */
		URL m_url((const char *)tmpstr->str);
		action  = g_strdup((gchar *)m_url.getParameterValue("action"));
		morph   = g_strdup((gchar *)m_url.getParameterValue("morph"));
		strongs = g_strdup((gchar *)m_url.getParameterValue("lemma"));
		stype   = g_strdup((gchar *)m_url.getParameterValue("type"));
		svalue  = g_strdup((gchar *)m_url.getParameterValue("value"));
		module  = g_strdup((gchar *)m_url.getParameterValue("module"));
		passage = g_strdup((gchar *)m_url.getParameterValue("passage"));

		// no module specified => use main module.
		if (!module || !strcmp(module, "")) {
			g_free(module);
			module = g_strdup(settings.MainWindowModule);
		}

		// XXX gross hack-fix
		// AraSVD is named "Smith & Van Dyke", using a literal '&'.
		// this is technically a Sword bug: Sword should encode it.
		// we work around it here: replace '&' with '+'.  *sigh*
		//if (svalue = strstr(, " & "))
		//	*(svalue+1) = '-';

		XI_message(("action = %s", action));
		XI_message(("morph = %s", morph));
		XI_message(("strongs = %s", strongs));
		XI_message(("type = %s", stype));
		XI_message(("value = %s", svalue));
		XI_message(("module = %s", module));
		XI_message(("passage = %s", passage));

		if (!strcmp(action, "showStrongs")) {
			show_strongs(stype, svalue, clicked);
		}

		else if (!strcmp(action, "showMorph")) {
			show_morph(module, stype, svalue, clicked);
		}

		else if (!strcmp(action, "showNote")) {
			show_note(module, passage, stype, svalue, clicked);
		}

		else if (!strcmp(action, "showUserNote")) {
			// need localized key, not the osisref that we've got.
			ModMap::iterator it = backend->get_mgr()->Modules.find(module);
			SWModule *m = (*it).second;
			VerseKey *vk = (VerseKey *)m->getKey();
			*vk = passage;

			main_information_viewer(module,
						(gchar *)svalue,
						(gchar *)m->getKeyText(),
						"showUserNote",
						(gchar *)"u",
						NULL,
						NULL);
		}

		else if (!strcmp(action, "showRef")) {
			if (!strcmp(stype, "scripRef"))
				show_ref(module, svalue, clicked);
		}

		else if (!strcmp(action, "showBookmark")) {
			show_module_and_key(module, svalue, stype, clicked);
		}

		else if (!strcmp(action, "showModInfo")) {
			show_mod_info(module, svalue, clicked);
		}

		else if (!strcmp(action, "showParallel")) {
			show_parallel(svalue, stype, clicked);
		}

		else if (!strcmp(action, "showStudypad")) {
			show_studypad(svalue, clicked);
		}

		else if (!strcmp(action, "showImage")) {
			show_separate_image((!strncmp(svalue, "file:", 5)
						 ? svalue + 5
						 : svalue),
					    clicked);
		}

		if (action)
			g_free(action);
		if (stype)
			g_free(stype);
		if (svalue)
			g_free(svalue);
		if (strongs)
			g_free(strongs);
		if (morph)
			g_free(morph);
		if (module)
			g_free(module);
		if (passage)
			g_free(passage);

		g_string_free(tmpstr, TRUE);
		retval = 1;
	} else if (clicked)
		xiphos_open_default(url);

	return retval;
}

/******************************************************************************
 * Name
 *
 *
 * Synopsis
 *   #include "main/url.hh"
 *
 *
 *
 * Description
 *
 *
 * Return value
 *
 */

const gchar *main_url_encode(const gchar *pram)
{
	SWBuf retval = URL::encode(pram);
	if (retval.length())
		return g_strdup(retval.c_str());
	else
		return g_strdup("");
}

/******************************************************************************
 * Name
 *   hex_decode
 *
 * Synopsis
 *   hex_decode(const gchar *url)
 *
 * Description
 *   extricates `+' and "%xx" encodings.
 *
 * Return value
 *   GString *
 */

GString *
hex_decode(const gchar *url)
{
	// handle `+' space substitutions and %xx encodings.
	GString *url_clean = g_string_new(NULL);
	const gchar *url_chase;
	char hex_template[] = {'0', '0', '\0'};
	unsigned long from_hex;

	for (url_chase = url; *url_chase; ++url_chase) {
		switch (*url_chase) {
		case '+':
			g_string_append_c(url_clean, ' ');
			break;
		case '%':
			if (isxdigit(*(url_chase + 1)) &&
			    isxdigit(*(url_chase + 2))) {
				hex_template[0] = *(url_chase + 1);
				hex_template[1] = *(url_chase + 2);
				from_hex = strtol(hex_template, NULL, 16);
				g_string_append_c(url_clean,
						  (gchar)from_hex);
				url_chase += 2;
			} else {
				// failed %xx encoding; normal character.
				// should we instead do nothing with this '%'?
				g_string_append_c(url_clean, '%');
			}
			break;
		default:
			g_string_append_c(url_clean, *url_chase);
			break;
		}
	}
	return url_clean;
}

/******   end of file   ******/
