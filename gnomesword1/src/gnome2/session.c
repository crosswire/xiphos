/*
 * GnomeSword Bible Study Tool
 * session.c - create and maintain a GNOME session
 *
 * based on code from gedit
 * gedit-session - Basic session management for gedit
 * Copyright (C) 2002 Ximian, Inc.
 * Authors Federico Mena-Quintero <federico@ximian.com>
 *
 * Copyright (C) 2000,2001,2002,2003 GnomeSword Developer Team
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

#include <libgnome/gnome-config.h>
#include <libgnomeui/gnome-client.h>
#include "gui/session.h"
#include "gui/gnomesword.h"

/* The master client  */
static GnomeClient *master_client = NULL;

/* argv[0] from main(); used as the command to restart the program */
static const char *program_argv0 = NULL;

static void 
interaction_function (GnomeClient *client, gint key, GnomeDialogType dialog_type, gpointer shutdown)
{
	const gchar *prefix;

	/* Save all unsaved files */

/*	if (GPOINTER_TO_INT (shutdown))		
		gedit_file_save_all ();*/

	/* Save session data */

	prefix = gnome_client_get_config_prefix (client);

	gnome_config_push_prefix (prefix);

	gnome_config_pop_prefix ();
	gnome_config_sync ();

	gnome_interaction_key_return (key, FALSE);
}

/* save_yourself handler for the master client */
static gboolean
client_save_yourself_cb (GnomeClient *client,
			 gint phase,
			 GnomeSaveStyle save_style,
			 gboolean shutdown,
			 GnomeInteractStyle interact_style,
			 gboolean fast,
			 gpointer data)
{
	const gchar *prefix;

	char *argv[] = { "rm", "-r", NULL };

	gnome_client_request_interaction (client, 
					  GNOME_DIALOG_NORMAL, 
					  interaction_function,
					  GINT_TO_POINTER (shutdown));
	
	prefix = gnome_client_get_config_prefix (client);

	/* Tell the session manager how to discard this save */

	argv[2] = gnome_config_get_real_path (prefix);
	gnome_client_set_discard_command (client, 3, argv);

	/* Tell the session manager how to clone or restart this instance */

	argv[0] = (char *) program_argv0;
	argv[1] = NULL; /* "--debug-session"; */
	
	gnome_client_set_clone_command (client, 1 /*2*/, argv);
	gnome_client_set_restart_command (client, 1 /*2*/, argv);

	return TRUE;
}

/* die handler for the master client */
static void
client_die_cb (GnomeClient *client, gpointer data)
{
/*	if (!client->save_yourself_emitted)
		gedit_file_close_all ();*/
	
	shutdown_frontend();
	/* shutdown the sword stuff */
	shutdown_backend();

	gtk_main_quit ();
}

/**
 * gs_session_init:
 * 
 * Initializes session management support.  This function should be called near
 * the beginning of the program.
 **/
void
gs_session_init (const char *argv0)
{
	g_print("\nInitiating GNOME session handler\n\n");
	if (master_client)
		return;

	program_argv0 = argv0;

	master_client = gnome_master_client ();

	g_signal_connect (master_client, "save_yourself",
			  G_CALLBACK (client_save_yourself_cb),
			  NULL);
	g_signal_connect (master_client, "die",
			  G_CALLBACK (client_die_cb),
			  NULL);
}

/**
 * gs_session_is_restored:
 * 
 * Returns whether this gnomesword is running from a restarted session.
 * 
 * Return value: TRUE if the session manager restarted us, FALSE otherwise.
 * This should be used to determine whether to pay attention to command line
 * arguments in case the session was not restored.
 **/
gboolean
gs_session_is_restored (void)
{
	gboolean restored;

	if (!master_client)
		return FALSE;

	restored = (gnome_client_get_flags (master_client) & GNOME_CLIENT_RESTORED) != 0;

	return restored;
}


/**
 * gs_session_load:
 * 
 * Loads the session by fetching the necessary information from the session
 * manager and opening files.
 * 
 * Return value: TRUE if the session was loaded successfully, FALSE otherwise.
 **/
gboolean
gs_session_load (void)
{
	int retval;

	gnome_config_push_prefix (gnome_client_get_config_prefix (master_client));

	gnome_config_pop_prefix ();
	return retval;
}
