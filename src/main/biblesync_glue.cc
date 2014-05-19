/*
 * Xiphos Bible Study Tool
 * biblesync_glue.cc
 *
 * Copyright (C) 2000-2014 Xiphos Developer Team
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

#include <gtk/gtk.h>
#include <glib.h>
#include <glib/gstdio.h>

#include "gui/main_window.h"
#include "gui/widgets.h"
#include "gui/dialog.h"
#include "gui/xiphos.h"
#include "gui/tabbed_browser.h"
#include "gui/utilities.h"

#include "main/biblesync_glue.h"
#include "main/display.hh"
#include "main/previewer.h"
#include "main/settings.h"
#include "main/sidebar.h"
#include "main/sword.h"
#include "main/url.hh"

#include "biblesync/biblesync.hh"

#include "gui/debug_glib_null.h"

BibleSync *biblesync;

using namespace sword;

typedef struct _speaker {
    string user;
    string ipaddr;
    string app;
    string version;
} speaker;

std::map < string, speaker > speakers;

/******************************************************************************
 * Name
 *  biblesync_navigate
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_navigate(cmd, bible, ref, alt, info, dump)
 *
 * Description
 *   navigates per incoming BibleSync request, or handles
 *   announcement or error instead.
 *   used as a callback out of class BibleSync.
 *
 * Return value
 *   void
 */
void
biblesync_navigate(char cmd,
		   string bible, string ref, string alt,
		   string group, string domain,
		   string info,  string dump)
{
    string message;

    // parameter overload usage.
    string &presence = alt;
    string &user = bible, &ipaddr = ref, &app = group, &version = domain;

    // lockout: prevent re-xmit of what we're processing.
    settings.bs_receiving = TRUE;

    switch (cmd)
    {

    // error
    case 'E':
	if (settings.bs_debug)
	{
	    // recv error occurred: explanation in info, pkt in dump.
	    message = info + "\n\n" + dump;
	    gui_generic_warning((char *)message.c_str());
	}
	break;

    // mismatch
    case 'M':
	if (settings.bs_mismatch)
	{
	    message = "BibleSync: "
		+ (string)_("Mismatched packet\n\n")
		+ info
		+ ((info.substr(0,8) == "announce")
		   ? ("\nMessage: " + presence)
		   : ("\nBible: " + bible + "; Ref: " + ref + "; Alt: " + alt
		      + "\nGroup: " + group + "; Domain: " + domain))
		+ (settings.bs_debug
		   ? ("\n\n" + (string)dump)
		   : "");
	    gui_generic_warning((char *)message.c_str());
	}
	break;

    // announce
    case 'A':
	if (settings.bs_presence)
	{
	    if (settings.bs_debug)
	    {
		message = _("Presence announcement:\n\n") + dump;
		gui_generic_warning((char *)message.c_str());
	    }
	    gui_generic_warning((char *)presence.c_str());
	}
	break;

    // navigation
    case 'N':
	if (settings.bs_debug)
	{
	    message = _("Navigation packet:\n\n") + dump;
	    gui_generic_warning((char *)message.c_str());
	}

	if (backend->is_module(bible.c_str()))
	{
	    // Xiphos does nothing with "alt", the alternate reference.

	    // direct navigation, or via verse list?
	    if (settings.bs_navdirect &&
		(strpbrk(ref.c_str(), "-;,") == NULL))	// not a multi-ref
	    {
		// Xiphos interprets param "group" as a tab#.
		char tab = group.c_str()[0];
		if ((group.length() == 1) &&		// 1-character string
		    (tab >= '1') && (tab <= '9'))
		{
		    tab -= '1';				// 0-based list
		    
		    // select tab if not already current, avoid screen flash.
		    if (cur_passage_tab &&
			(tab != gtk_notebook_page_num(
				 GTK_NOTEBOOK(widgets.notebook_main),
				 cur_passage_tab->page_widget)))
		    {
			gui_select_nth_tab(tab);
		    }
		    settings.showtexts = 1;		// make panes visible
		    settings.showcomms = 1;
		    settings.comm_showing = 1;
		    gui_recompute_shows(FALSE);
		}
		//main_display_bible(bible.c_str(), ref.c_str());
		main_url_handler(((string)"sword://"
				  + bible
				  + "/"
				  + ref).c_str(), 1);
	    }
	    else
	    {
		// verse list as general preference, or due to multi-ref.
		main_display_verse_list_in_sidebar
		    (settings.currentverse,
		     (gchar*)bible.c_str(), (gchar*)ref.c_str());
	    }
	}
	else
	{
	    message = "BibleSync: " + (string)_("Unknown module ") + bible;
	    gui_generic_warning((char *)message.c_str());
	}
	break;

    // new speaker discovery.
    case 'S':
	GS_message(("new speaker: u [%s], ip [%s], a [%s], v [%s]",
		    user.c_str(), ipaddr.c_str(), app.c_str(), version.c_str()));
	break;

    // dead speaker -- timed out from lack of beacons.
    case 'D':
	GS_message(("dead speaker: u [%s], ip [%s], a [%s], v [%s]",
		    user.c_str(), ipaddr.c_str(), app.c_str(), version.c_str()));
	break;

    default:
	message = _("ERROR: unknown BibleSync indicator: ")
	    + cmd
	    + (string)".\n"
	    + (string)_("Other indications are:")
	    + "\nbible: " + bible
	    + "\nref: " + ref
	    + "\nalt: " + alt
	    + "\ngroup: " + group
	    + "\ndomain: " + domain
	    + "\ninfo: " + info
	    + "\ndump:\n" + dump;
	gui_generic_warning((char *)message.c_str());
	break;
    }

    // unlock.
    settings.bs_receiving = FALSE;
}


/******************************************************************************
 * Name
 *  biblesync_mode_select
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_mode_select(int m)
 *
 * Description
 *   checks incoming argument and assigns mode appropriately.
 *   starts receiver if we didn't disable.
 *
 * Return value
 *   void
 */
int
biblesync_mode_select(int m, char *p)
{
    BibleSync_mode mode, old_mode = biblesync->getMode();

    switch (m)
    {
    case 0:
	mode = BSP_MODE_DISABLE;    break;
    case 1:
	mode = BSP_MODE_PERSONAL;   break;
    case 2:
	mode = BSP_MODE_SPEAKER;    break;
    case 3:
	mode = BSP_MODE_AUDIENCE;   break;
    default:
	mode = N_BSP_MODE;          break;	// error
    }
    if (mode != N_BSP_MODE)
    {
	if (((mode = biblesync->setMode(mode, biblesync_navigate, p))
	     != BSP_MODE_DISABLE) &&
	    (old_mode == BSP_MODE_DISABLE))
	{
	    // whenever we move to any active mode, we must start receiver.
	    // glib will stop it on its own any time we go back to disable.
	    g_timeout_add(1000, (GSourceFunc)BibleSync::Receive, biblesync);
	}

	switch (mode)
	{
	case BSP_MODE_DISABLE:
	    m = 0; break;
	case BSP_MODE_PERSONAL:
	    m = 1; break;
	case BSP_MODE_SPEAKER:
	    m = 2; break;
	case BSP_MODE_AUDIENCE:
	    m = 3; break;
	default:
	    m = 99; break;
	}
    }

    return m;
}

/******************************************************************************
 * Name
 *  biblesync_personal
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_personal()
 *
 * Description
 *   determines whether BibleSync is set in Personal mode.
 *
 * Return value
 *   int
 */
int biblesync_personal()
{
    return biblesync->getMode() == BSP_MODE_PERSONAL;
}

/******************************************************************************
 * Name
 *  biblesync_active
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_active()
 *
 * Description
 *   determines whether BibleSync is active or disabled.
 *
 * Return value
 *   int
 */
int biblesync_active()
{
    return biblesync->getMode() != BSP_MODE_DISABLE;
}

/******************************************************************************
 * Name
 *  biblesync_active_xmit_allowed
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   void biblesync_active_xmit_allowed()
 *
 * Description
 *   determines whether BibleSync is alive and this user may send.
 *
 * Return value
 *   int
 */
int biblesync_active_xmit_allowed()
{
    return biblesync->getMode() == BSP_MODE_PERSONAL ||
	   biblesync->getMode() == BSP_MODE_SPEAKER;
}

/******************************************************************************
 * Name
 *  biblesync_get_passphrase
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *biblesync_get_passphrase()
 *
 * Description
 *   retrieves the currently-set session passphrase.
 *
 * Return value
 *   void
 */
const char *biblesync_get_passphrase()
{
    return biblesync->getPassphrase().c_str();
}

/******************************************************************************
 * Name
 *  biblesync_transmit_verse_list
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *biblesync_transmit_verse_list()
 *
 * Description
 *   ships out a multi-ref from verse list handler.
 *
 * Return value
 *   void
 */
void biblesync_transmit_verse_list(char *modname, char *vlist)
{
    biblesync->Transmit(BSP_SYNC, (string)modname, (string)vlist);
    // remaining args irrelevant => defaulted.
}

/******************************************************************************
 * Name
 *  biblesync_privacy
 *
 * Synopsis
 *   #include "backend/biblesync.hh"
 *   const char *biblesync_privacy(gboolean)
 *
 * Description
 *   en/disables privacy in personal mode (TTL 0).
 *
 * Return value
 *   void
 */
void biblesync_privacy(gboolean privacy)
{
    biblesync->setPrivate(privacy);
}


/******************************************************************************
 * Name
 *   biblesync_prep_and_xmit()
 *
 * Synopsis
 *   #include "main/sword.h"
 *   void biblesync_prep_and_xmit(void)
 *
 * Description
 *   regular xmit of user's current nav point.
 *
 * Return value
 *   void
 */

void biblesync_prep_and_xmit(const char *mod_name, const char *key)
{
    if (!settings.bs_receiving &&		// no re-xmit of recv'd nav.
	((biblesync->getMode() == BSP_MODE_PERSONAL) ||
	 (biblesync->getMode() == BSP_MODE_SPEAKER)))
	// audience does not xmit nav.
    {
	string group = "1";		// default if determination fails.
	char pchar[2];
	gint pagenum = (cur_passage_tab
			? gtk_notebook_page_num(
			    GTK_NOTEBOOK(widgets.notebook_main),
			    cur_passage_tab->page_widget)
			: 1);

	if ((pagenum != -1) && (pagenum < 9))
	{
	    pchar[0] = pagenum + '1';		// 0-based list.
	    pchar[1] = '\0';
	    group = (string)pchar;
	}

	// ** this key is not to be freed. **
	char *osis_key = (char *)main_get_osisref_from_key(mod_name, key);
	biblesync->Transmit(BSP_SYNC,
			    (string)mod_name, (string)osis_key,
			    (string)"", group);
    }
}

/******************************************************************************
 * Name
 *   biblesync_listen()
 *
 * Synopsis
 *   #include "main/sword.h"
 *   void biblesync_listen(gboolean listen, char *speakerkey)
 *
 * Description
 *   say whether to listen to a given speaker.
 *   (just type conversions on the way from C to C++.)
 *
 * Return value
 *   void
 */

void biblesync_listen(gboolean listen, char *speakerkey)
{
    biblesync->listenToSpeaker((bool)listen, (string)speakerkey);
}
