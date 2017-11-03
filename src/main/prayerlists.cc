/*
 * Xiphos Bible Study Tool
 * prayerlists.cc - code to create several different prayer lists
 *
 * Copyright (C) 2008-2017 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <gtk/gtk.h>
#include <glib/gstdio.h>
#include <swmodule.h>
#include <swmgr.h>
#include <treekeyidx.h>
#include <rawgenbook.h>
#include <swconfig.h>
#include <iostream>
#include <string>
#include <stdio.h>
#include <unistd.h>

#include "main/mod_mgr.h"
#include "main/prayerlists.h"
#include "main/sword_treekey.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"

#include "gui/dialog.h"
#include "gui/sidebar.h"

#include "backend/sword_main.hh"

#include "gui/debug_glib_null.h"

// sword stuff for canonical book names + chapter counts
// to construct prototype all-chapters outline. (#725)
#include "sword/versificationmgr.h"
#include "sword/canon.h"

#ifndef NO_SWORD_NAMESPACE
using sword::TreeKeyIdx;
using sword::RawGenBook;
using sword::SWKey;
using sword::SWConfig;
using sword::SWModule;
#endif

enum {
	COL_OPEN_PIXBUF,
	COL_CLOSED_PIXBUF,
	COL_CAPTION,
	COL_MODULE,
	COL_OFFSET,
	N_COLUMNS
};

/* for constructing journals */
extern int month_day_counts[];
extern char *(month_names[]);

/*********************************    *************************************/

static void
setEntryText(RawGenBook *book, const gchar *text)
{
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);
	if (treeKey->getOffset()) {
		(*book) << text;
	}
}

static void
appendChild(TreeKeyIdx *treeKey, const gchar *name)
{
	treeKey->appendChild();
	treeKey->setLocalName(name);
	treeKey->save();
	XI_message(("name: %s\nlocalName: %s", name,
		    treeKey->getLocalName()));
}

static void
appendSibling(TreeKeyIdx *treeKey, const gchar *name)
{
	if (treeKey->getOffset()) {
		treeKey->append();
		treeKey->setLocalName(name);
		treeKey->save();
	}
}

static void
add_prayer_list_sections(RawGenBook *book, TreeKeyIdx *treeKey)
{
	appendChild(treeKey, _("Growth"));
	setEntryText(book, _("<b>For Growth</b><br/>"));
	appendSibling(treeKey, _("Salvation"));
	setEntryText(book, _("<b>For Salvation</b><br/>"));
	appendSibling(treeKey, _("Health"));
	setEntryText(book, _("<b>For Health</b><br/>"));
	appendSibling(treeKey, _("Misc"));
	setEntryText(book, _("<b>Miscellaneous</b><br/>"));

	treeKey->parent();
}

static void
add_outline_subsections(RawGenBook *book, TreeKeyIdx *treeKey)
{
	appendChild(treeKey, _("Point x"));
	setEntryText(book, _("<b>(x)</b><br/>"));
	appendSibling(treeKey, _("Point y"));
	setEntryText(book, _("<b>(y)</b><br/>"));
	appendSibling(treeKey, _("Point z"));
	setEntryText(book, _("<b>(z)</b><br/>"));

	treeKey->parent();
}

static void
add_outline_sections(RawGenBook *book, TreeKeyIdx *treeKey)
{
	appendChild(treeKey, _("Minor Topic 1"));
	setEntryText(book, _("<b>Subtopic 1</b><br/>"));
	add_outline_subsections(book, treeKey);
	appendSibling(treeKey, _("Minor Topic 2"));
	setEntryText(book, _("<b>Subtopic 2</b><br/>"));
	add_outline_subsections(book, treeKey);
	appendSibling(treeKey, _("Minor Topic 3"));
	setEntryText(book, _("<b>Subtopic 3</b><br/>"));
	add_outline_subsections(book, treeKey);
	appendSibling(treeKey, _("Minor Topic 4"));
	setEntryText(book, _("<b>Subtopic 4</b><br/>"));
	add_outline_subsections(book, treeKey);

	treeKey->parent();
}

/******************************************************************************
 * Name
 *   prayerlist_construct
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   void prayerlist_construct(gchar *listname, gchar *summary)
 *
 * Description
 *   manufacture prayer list configuration fundamentals.
 *
 */

void
prayerlist_construct(gchar *listname, gchar *summary)
{
	// configuration file generation.
	gchar *path = g_strdup_printf("%s/" DOTSWORD "/mods.d/%s.conf",
				      settings.homedir, listname);
	SWConfig config(path);
	g_free(path);

	// configuration file content.
	path = g_strdup_printf("./modules/genbook/rawgenbook/%s/%s",
			       listname, listname);
	config[listname]["DataPath"] = path;
	g_free(path);
	config[listname]["ModDrv"] = "RawGenBook";
	config[listname]["GSType"] = "PrayerList";
	config[listname]["Encoding"] = "UTF-8";
	config[listname]["SourceType"] = "ThML";
	config[listname]["Lang"] = sword_locale; // set at startup.
	config[listname]["Version"] = "0.1";
	config[listname]["MinimumVersion"] = "1.5.11";
	config[listname]["DisplayLevel"] = "2"; // ?
	config[listname]["Description"] = listname;
	config[listname]["About"] = summary; // as provided.
	config.save();

	// fundamentals are complete.  update.
	main_update_module_lists();
	main_load_module_tree(sidebar.module_list);
	settings.havebook = 1;
}

/******************************************************************************
 * Name
 *   prayerlist_fundamentals
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   gchar *prayerlist_fundamentals(gchar *summary)
 *
 * Description
 *   manufacture prayer list configuration fundamentals.
 *
 * Return value
 *   name of created module; NULL if failure.
 */

gchar *
prayerlist_fundamentals(gchar *summary,
			gchar *defaultname)
{
	char *listname = NULL; // assume failure.
	GS_DIALOG *info;
	gint test;
	GString *path = g_string_new(NULL);

	// name selection dialog.
	info = gui_new_dialog();
	info->stock_icon = (gchar *)
#if GTK_CHECK_VERSION(3, 10, 0)
	    "dialog-question";
#else
	    GTK_STOCK_DIALOG_QUESTION;
#endif
	info->title = _("Prayer List/Journal");
	info->label_top = _("Name for new prayer list or journal");
	info->label1 = _("Name: ");
	info->text1 = g_strdup(defaultname);
	info->ok = TRUE;
	info->cancel = TRUE;
	test = gui_gs_dialog(info);

	// result of name selection.
	if (test != GS_OK) {
		goto out; // cancelled.
	}

	for (char *s = info->text1; *s; ++s) {
		if (!isalnum(*s) && (*s != '_')) {
			gui_generic_warning(_("Module names must contain [A-Za-z0-9_] only."));
			goto out;
		}
	}

	if (main_is_module(info->text1)) {
		gui_generic_warning(_("Xiphos already knows a module by that name."));
		goto out;
	}

	// path creation based on name selection.
	g_string_printf(path, "%s/" DOTSWORD "/modules/genbook/rawgenbook/%s",
			settings.homedir, info->text1);
	if (g_access(path->str, F_OK) == 0) {
		gui_generic_warning(_("Xiphos finds that prayer list already."));
		goto out;
	} else {
		g_string_printf(path, "%s/" DOTSWORD "/modules/genbook",
				settings.homedir);
		Mkdir(path->str, S_IRWXU); // ignore return value -- harmless
		g_string_append(path, "/rawgenbook");
		Mkdir(path->str, S_IRWXU); // ignore return value -- harmless

		g_string_append(path, "/");
		g_string_append(path, info->text1);
		if ((Mkdir(path->str, S_IRWXU)) != 0) { // this one matters.
			char *msg = g_strdup_printf(_("Xiphos cannot create module's path:\n%s"),
						    path->str);
			gui_generic_warning(msg);
			g_free(msg);
			goto out;
		}
	}

	// wild, raving applause: we got through all the sanity checks.
	listname = g_strdup(info->text1);

	prayerlist_construct(listname, summary);

out:
	// both success and failure fall through here.
	g_string_free(path, TRUE);
	g_free(info->text1);
	g_free(info);
	return listname;
}

/******************************************************************************
 * Name
 *   main_prayerlist_basic_create
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   gint main_prayerlist_basic_create(void)
 *
 * Description
 *   create a simple prayer list module & add to sidebar module tree
 *
 * Return value
 *   gboolean
 */

gboolean
main_prayerlist_basic_create(void)
{
	char *listname = prayerlist_fundamentals(_("A basic prayer list. \\par\\par Module created by Xiphos."),
						 _("BasicPrayerList"));
	if (listname == NULL)
		return FALSE;

	gchar *path = g_strdup_printf("%s/" DOTSWORD "/modules/genbook/rawgenbook/%s/%s",
				      settings.homedir, listname, listname);
	g_free(listname);
	RawGenBook::createModule(path);
	RawGenBook *book = new RawGenBook(path);
	g_free(path);

	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);

	appendChild(treeKey, _("MyPrayerList"));
	setEntryText(book, _("<b>People:</b><br/>Bob<br/>Sam<br/>Sue<br/><br/><b>Church:</b><br/>pews<br/>fellowship<br/>Bibles for missionaries<br/><br/><br/>"));

	delete treeKey;
	return TRUE;
}

/******************************************************************************
 * Name
 *   main_prayerlist_subject_create
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   gint main_prayerlist_subject_create(void)
 *
 * Description
 *   create a subject prayer list module & add to sidebar module tree
 *
 * Return value
 *   gboolean
 */

gboolean
main_prayerlist_subject_create(void)
{
	char *listname = prayerlist_fundamentals(_("A subject-based prayer list. \\par\\par Module created by Xiphos."),
						 _("SubjectPrayerList"));
	if (listname == NULL)
		return FALSE;

	gchar *path = g_strdup_printf("%s/" DOTSWORD "/modules/genbook/rawgenbook/%s/%s",
				      settings.homedir, listname, listname);
	g_free(listname);
	RawGenBook::createModule(path);
	RawGenBook *book = new RawGenBook(path);
	g_free(path);

	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);

	appendChild(treeKey, _("Salvation"));
	setEntryText(book, _("Bob<br/>Sam<br/>Sue<br/>John<br/>"));
	appendSibling(treeKey, _("Spiritual Growth"));
	setEntryText(book, _("Mike<br/>Steve<br/>"));
	appendSibling(treeKey, _("Health"));
	setEntryText(book, _("Sue<br/>John<br/>"));

	delete treeKey;
	return TRUE;
}

/******************************************************************************
 * Name
 *   main_prayerlist_wild_create
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   gint main_prayerlist_wild_create(void)
 *
 * Description
 *   create a monthly prayer list module & add to sidebar module tree
 *
 * Return value
 *   gboolean
 */

gboolean
main_prayerlist_monthly_create(void)
{
	char *listname = prayerlist_fundamentals(_("A monthly prayer list. \\par\\par Module created by Xiphos."),
						 _("MonthlyPrayerList"));
	if (listname == NULL)
		return FALSE;

	gchar *path = g_strdup_printf("%s/" DOTSWORD "/modules/genbook/rawgenbook/%s/%s",
				      settings.homedir, listname, listname);
	g_free(listname);
	RawGenBook::createModule(path);
	RawGenBook *book = new RawGenBook(path);
	g_free(path);

	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);

	for (int month = 0; month < 12; ++month) {
		char monthnum[4], monthstring[32];
		snprintf(monthnum, 3, "%02d", month + 1);
		snprintf(monthstring, 31, "<b>%s</b><br/>", _(month_names[month]));

		if (month == 0)
			appendChild(treeKey, monthnum);
		else
			appendSibling(treeKey, monthnum);
		setEntryText(book, monthstring);
		add_prayer_list_sections(book, treeKey);
	}

	delete treeKey;
	return TRUE;
}

/******************************************************************************
 * Name
 *   main_prayerlist_journal_create
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   gint main_prayerlist_journal_create(void)
 *
 * Description
 *   create a daily journal prayer list module & add to sidebar module tree
 *
 * Return value
 *   gboolean
 */

gboolean
main_prayerlist_journal_create(void)
{
	char *listname = prayerlist_fundamentals(_("A daily journal. \\par\\par Module created by Xiphos."),
						 _("DailyJournal"));
	if (listname == NULL)
		return FALSE;

	gchar *path = g_strdup_printf("%s/" DOTSWORD "/modules/genbook/rawgenbook/%s/%s",
				      settings.homedir, listname, listname);
	g_free(listname);
	RawGenBook::createModule(path);
	RawGenBook *book = new RawGenBook(path);
	g_free(path);

	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);

	for (int month = 0; month < 12; ++month) {
		char monthnum[4], monthstring[32];
		snprintf(monthnum, 3, "%02d", month + 1);
		snprintf(monthstring, 31, "<b>%s</b><br/>", _(month_names[month]));

		if (month == 0)
			appendChild(treeKey, monthnum);
		else
			appendSibling(treeKey, monthnum);
		setEntryText(book, monthstring);

		for (int day = 0; day < month_day_counts[month]; ++day) {
			char daynum[4], monthday[32];
			snprintf(daynum, 3, "%02d", day + 1);
			snprintf(monthday, 31, "<b>%s %d</b><br/>", _(month_names[month]), day + 1);

			if (day == 0)
				appendChild(treeKey, daynum);
			else
				appendSibling(treeKey, daynum);

			setEntryText(book, monthday);
		}
		treeKey->parent();
	}

	delete treeKey;
	return TRUE;
}

/******************************************************************************
 * Name
 *   main_prayerlist_outlined_topic_create
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   gint main_prayerlist_outlined_topic_create(void)
 *
 * Description
 *   create a sermon or other topical outline
 *
 * Return value
 *   gboolean
 */

gboolean
main_prayerlist_outlined_topic_create(void)
{
	char *listname = prayerlist_fundamentals(_("An outlined topic (e.g. sermon). \\par\\par Module created by Xiphos."),
						 _("OutlinedTopic"));
	if (listname == NULL)
		return FALSE;

	gchar *path = g_strdup_printf("%s/" DOTSWORD "/modules/genbook/rawgenbook/%s/%s",
				      settings.homedir, listname, listname);
	g_free(listname);
	RawGenBook::createModule(path);
	RawGenBook *book = new RawGenBook(path);
	g_free(path);

	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);

	appendChild(treeKey, _("Major Topic A"));
	setEntryText(book, _("<b>Major Topic A</b><br/>"));
	add_outline_sections(book, treeKey);
	appendSibling(treeKey, _("Major Topic B"));
	setEntryText(book, _("<b>Major Topic B</b><br/>"));
	add_outline_sections(book, treeKey);
	appendSibling(treeKey, _("Major Topic C"));
	setEntryText(book, _("<b>Major Topic C</b><br/>"));
	add_outline_sections(book, treeKey);

	delete treeKey;
	return TRUE;
}

/******************************************************************************
 * Name
 *   main_prayerlist_book_chapter_create
 *
 * Synopsis
 *   #include "main/prayer_list.h"
 *   gint main_prayerlist_book_chapter_create(void)
 *
 * Description
 *   create a per-chapter outline for the entire bible
 *
 * Return value
 *   gboolean
 */

gboolean
main_prayerlist_book_chapter_create(void)
{
	char *listname = prayerlist_fundamentals(_("A journal sectioned by Bible book and chapter. \\par\\par Module created by Xiphos."),
						 _("BookChapter"));
	if (listname == NULL)
		return FALSE;

	gchar *path = g_strdup_printf("%s/" DOTSWORD "/modules/genbook/rawgenbook/%s/%s",
				      settings.homedir, listname, listname);
	g_free(listname);
	RawGenBook::createModule(path);
	RawGenBook *book = new RawGenBook(path);
	g_free(path);

	TreeKeyIdx root = *((TreeKeyIdx *)((SWKey *)(*book)));
	TreeKeyIdx *treeKey = (TreeKeyIdx *)(SWKey *)(*book);

	struct sbook *sbook;
	struct sbook *otnt[2] = { sword::otbooks, sword::ntbooks };
	gboolean first = TRUE;

	for (int idx = 0; idx < 2; idx++) {
		for (sbook = otnt[idx]; sbook->chapmax != 0; ++sbook) {
			if (first) {
				appendChild(treeKey, sbook->name);
				first = FALSE;
			} else
				appendSibling(treeKey, sbook->name);

			SWBuf content = "";
			char buf[10];
			for (int i = 1; i <= sbook->chapmax; ++i) {
				sprintf(buf, "%d\t<br/>", i);
				content += (SWBuf)buf;
			}
			setEntryText(book, content.c_str());
		}
	}

	delete treeKey;
	return TRUE;
}
