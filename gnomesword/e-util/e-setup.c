/*
 * Sets up the ~/evolution directory
 *
 * Author:
 *    Miguel de Icaza (miguel@kernel.org)
 *
 * (C) 2000 Helix Code, Inc. http://www.helixcode.com
 */
#include <config.h>
#include <sys/stat.h>
#include <errno.h>
#include <unistd.h>
#include <gnome.h>
#include "e-setup.h"

char *evolution_dir = NULL;
char *evolution_folders_dir = NULL;
char *evolution_shortcuts_dir = NULL;
char *evolution_private = NULL;
char *evolution_public = NULL;

/* Try to ensure the existence of a directory, by checking for it and
 * creating it if necessary. It returns FALSE if it doesn't exist and
 * can't be created */
static gboolean
mkdir_if_necessary (char *dirname)
{
	struct stat s;
	
	g_assert (dirname);

	/* If we can't stat the dirname... */
	if (stat (dirname, &s) == -1) {

		/* ...it may be because there's no such directory */
		if (errno == ENOENT) {
			g_print ("Directory %s doesn't exist; creating...",
				 dirname);
			if (mkdir (dirname, S_IRWXU) == -1) {
				g_print ("failed! %s\n", g_strerror (errno));
				return FALSE;
			}
			else /* directory created! */
				g_print ("success!\n");
		}
		/* ..or maybe there's some other problem with the directory */
		else {
			
			g_print ("There's a problem with accessing "
				 "\"%s\": %s\n",
				 dirname, g_strerror(errno));
			return FALSE;
		}
	}
	/* There's a file or directory there. */
	else {
		/* if it's a file, complain; otherwise, we're all set */
		if (!S_ISDIR (s.st_mode)) {
			g_print ("Evolution is trying to create a directory,\n"
				 "\"%s\". But there appears to be a file in\n"
				 "the way. Move it away.\n",
				 dirname);
			return FALSE;
		}
	}
	return TRUE;
}


gboolean 
e_setup_base_dir (void)
{
	gboolean success = FALSE;

	/* try to get the evolution home directory from gnome-config;
	   if we can't, we'll make a new one at ~/evolution */
	evolution_dir = gnome_config_get_string("/Evolution/directories/home");
	
	if (!evolution_dir) evolution_dir =
		g_concat_dir_and_file (g_get_home_dir (), "evolution");

	if (!evolution_folders_dir)
		evolution_folders_dir =
			g_concat_dir_and_file (evolution_dir, "folders");

	if (!evolution_shortcuts_dir)
		evolution_shortcuts_dir =
			g_concat_dir_and_file (evolution_dir, "shortcuts");
	
	if (mkdir_if_necessary (evolution_dir) &&
	    mkdir_if_necessary (evolution_folders_dir) &&
	    mkdir_if_necessary (evolution_shortcuts_dir)) {

		success = TRUE;
		gnome_config_set_string ("/Evolution/directories/home",
					 evolution_dir);
		gnome_config_sync();
	}
		
	return success;
}

