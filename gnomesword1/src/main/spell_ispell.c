/*
 * GnomeSword Bible Study Tool
 * spell_ispell.c - spell checking using ispell
 *
 * Copyright (C) 2000,2001,2002 GnomeSword Developer Team
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
#  include "config.h"
#endif

#ifdef USE_ISPELL

#ifdef USE_GNOME2
#include <glib-2.0/glib.h>
#else
#include <glib-1.2/glib.h>
#endif
#include <string.h>

#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <ctype.h>

#if TIME_WITH_SYS_TIME
# include <sys/time.h>
# include <time.h>
#else
# if HAVE_SYS_TIME_H
#  include <sys/time.h>
# else
#  include <time.h>
# endif
#endif

#ifdef HAVE_SYS_SELECT_H
#include <sys/select.h>
#endif

#include "main/spell.h"



static FILE *in, *out;		/* streams to communicate with ispell */
pid_t isp_pid = -1;		/* pid for the `ispell' process */

static int isp_fd;

typedef struct {
	gint flag;
	gint count;
	gchar **misses;
} isp_result;

/* Spellchecker status */
enum {
	ISP_OK = 1,
	ISP_ROOT,
	ISP_COMPOUNDWORD,
	ISP_UNKNOWN,
	ISP_MISSED,
	ISP_IGNORE
};

static isp_result *result;
gint count = 0;

/* void sigchldhandler(int sig); */
void sigchldhandler(pid_t pid, int *status);

/* extern void sigchldchecker(int sig); */
extern void sigchldchecker(pid_t pid, int *status);

static
void create_ispell_pipe()
{
	static char o_buf[BUFSIZ];
	int pipein[2], pipeout[2];
	char *argv[14];
	int argc;
	int i;
	gchar buf[2048];
	fd_set infds;
	struct timeval tv;
	int retval = 0;

	g_print("Creating ispell pipe ...\n");

	isp_pid = -1;

	if (pipe(pipein) == -1 || pipe(pipeout) == -1) {
		g_printerr("GnomeSword: Can't create pipe for spellchecker!");
		return;
	}

	if ((out = fdopen(pipein[1], "w")) == NULL) {
		g_printerr("GnomeSword: Can't create stream for pipe for spellchecker!");
		return;
	}

	if ((in = fdopen(pipeout[0], "r")) == NULL) {
		g_printerr("GnomeSword: Can't create stream for pipe for spellchecker!");
		return;
	}

	setvbuf(out, o_buf, _IOLBF, BUFSIZ);

	isp_fd = pipeout[0];

	isp_pid = fork();

	if (isp_pid == -1) {
		g_printerr("GnomeSword: Can't create child process for spellchecker!");
		return;
	}

	if (isp_pid == 0) {
		/* child process */

		/* NOTE: Don't use DEBUG_MSG in this part since the output is redirected! */
		g_print("Spell-checker child process \n");

		dup2(pipein[0], STDIN_FILENO);
		dup2(pipeout[1], STDOUT_FILENO);
		close(pipein[0]);
		close(pipein[1]);
		close(pipeout[0]);
		close(pipeout[1]);

		argc = 0;
		argv[argc++] = "ispell";
		argv[argc++] = g_strdup("-a");	// "Pipe" mode
		/* Report run-together words with
		   missing blanks as errors */
		argv[argc++] = g_strdup("-B");		
		argv[argc++] = NULL;		
		execvp("ispell", (char *const *) argv);
		/* free the memory used  */
		for (i = 0; i < argc - 1; i++)
			g_free(argv[i]);

		g_printerr("GnomeSwrod: Failed to start ispell!\n");
		_exit(0);
	}

	/* Parent process: Read ispells identification message 
	   Hmm...what are we using this id msg for? Nothing? (Lgb)
	   Actually I used it to tell if it's truly Ispell or if it's
	   aspell -- (kevinatk@home.com) */

	FD_ZERO(&infds);
	FD_SET(pipeout[0], &infds);
	tv.tv_sec = 15;		/*  fifteen second timeout. Probably too much,
				   but it can't really hurt. */
	tv.tv_usec = 0;

	/* Configure provides us with macros which are supposed to do
	   the right typecast. */

	retval = select((SELECT_TYPE_ARG1) (pipeout[0] + 1),
			&infds, //SELECT_TYPE_ARG234 (&infds),
			0, 
			0, 
			SELECT_TYPE_ARG5(&tv));

	if (retval > 0) {
		/* Ok, do the reading. We don't have to FD_ISSET since
		   there is only one fd in infds. */
		fgets(buf, 2048, in);
		g_print("\nIspell pipe created\n");
		g_print("%s\n", buf);

	} else if (retval == 0) {
		/* timeout. Give nice message to user. */
		g_printerr("Ispell read timed out, what now?\n");
		isp_pid = -1;
		close(pipeout[0]);
		close(pipeout[1]);
		close(pipein[0]);
		close(pipein[1]);
		isp_fd = -1;
	} else {
		/* Select returned error */
		g_printerr("Select on ispell returned error, what now?\n");
	}
}


static inline void ispell_terse_mode(void)
{
	fputs("!\n", out);	/* Set terse mode (silently accept correct words) */
}


/******************************************************************************
 * Name
 *   check_for_error
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   void check_for_error(void)
 *
 * Description
 *   check for ispell error and print it if found
 *
 * Return value
 *   void
 */

void check_for_error(void)
{
	
}


/******************************************************************************
 * Name
 *   store_replacement
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   void store_replacement(const gchar * missed_word, const gchar * new_word)
 *
 * Description
 *   helps aspell learn
 *
 * Return value
 *   void
 */

void store_replacement(const gchar * missed_word, const gchar * new_word)
{
	
}


/******************************************************************************
 * Name
 *   add_to_session
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   int add_to_session(const gchar * word)
 *
 * Description
 *   add word to current session so it will not be flagged again
 *
 * Return value
 *   int
 */

int add_to_session(const gchar * word)
{
	
	fputc('@', out);	// Accept in this session 
	fputs(word, out);
	fputc('\n', out);
	return 0;
}


/******************************************************************************
 * Name
 *   add_to_personal
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   int add_to_personal(const gchar * word)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int add_to_personal(const gchar * word)
{
	fputc('*', out);	/* Insert word in personal dictionary */
	fputs(word, out);
	fputc('\n', out);
	return 1;
}


/******************************************************************************
 * Name
 *   get_suggest_list
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   GList * get_suggest_list(const gchar * word)
 *
 * Description
 *   returns a list of suggested words to replace word
 *
 * Return value
 *   GList * 
 */

GList * get_suggest_list(const gchar * word)
{
	GList * retval = NULL;
	
	gint i; 
	
        for (i = 0; i < count; i++){
		retval = g_list_append(retval, g_strdup(result->misses[i]));
        } 
	return retval;
}


/******************************************************************************
 * Name
 *   check_word_spell
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   int check_word_spell(const char * word)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int check_word_spell(const char * word)
{	
	gchar buf[1024], *p, *np;
	gchar *nb;
	gint retval = 0, i;

	/* Coment from the original author:
	   I think we have to check if ispell is still alive here */

	if (isp_pid == -1)
		return -1;

	for (i = 0; i < count; i++)
		g_free(result->misses[i]);
	count = 0;
	
	fputs(word, out);
	fputc('\n', out);

	fgets(buf, 1024, in);
	


	result = g_malloc(sizeof(isp_result));

	switch (*buf) {
	case '*':		/* Word found */
		result->flag = ISP_OK;
		retval = 1;
		break;
	case '+':		/* Word found through affix removal */
		result->flag = ISP_ROOT;
		retval = 1;
		break;
	case '-':		/* Word found through compound formation */
		result->flag = ISP_COMPOUNDWORD;
		retval = 1;
		break;
	case '\n':		/* Number or when in terse mode: no problems */
		result->flag = ISP_IGNORE;
		retval = 1;
		break;
	case '#':		/* Not found, no near misses and guesses */
		result->flag = ISP_UNKNOWN;
		retval = 5;
		break;
	case '?':		/* Not found, and no near misses, but guesses (guesses are ignored) */
	case '&':		/* Not found, but we have near misses */
		{
			retval = 0;
			result->flag = ISP_MISSED;
			/* FIXME: why duplicate buffer ? */
			nb = g_strdup(buf);
			p = strpbrk(nb + 2, " ");
			sscanf(p, "%d", &count);	/* Get near misses count */
			result->count = count;
			if (count) {
				p = strpbrk(nb, ":");
				p += 2;

				/* remove the last '\n' from ispell output */
				np = p;
				while (*np != '\0')
					np++;
				np--;
				*np = '\0';
				result->misses =
				    g_strsplit(p, ", ", count);
				g_free(nb);
				/* debug */
				for (i = 0; i < count; i++)
					g_print("near_miss = \"%s\" \n",
						  result->misses[i]);
			}
			break;
		}
	default:		/* This shouldn't happend, but you know Murphy */
		result->flag = ISP_UNKNOWN;
	}

	*buf = 0;
	if (result->flag != ISP_IGNORE) {
		while (*buf != '\n')
			fgets(buf, 255, in);	/* wait for ispell to finish */
	}
	return retval;
}


/******************************************************************************
 * Name
 *   init_spell
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   int init_spell(void)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int init_spell(void)
{
/* create ispell process */	
	create_ispell_pipe();	
	if (isp_pid == -1) {
		return -1;
	}		
	
	/* Put ispell in terse mode to improve speed */
	ispell_terse_mode();	
		
	
	return 0;
}


/******************************************************************************
 * Name
 *   kill_spell
 *
 * Synopsis
 *   #include "main/spell.h"
 *
 *   int kill_spell(void)
 *
 * Description
 *   
 *
 * Return value
 *   int
 */

int kill_spell(void)
{		
	if (isp_pid != -1) {	/* checks if ispell is running */
		fputs("#\n", out);	/* Save personal dictionary */
		fflush(out);
		fclose(out);
		kill(isp_pid, SIGTERM);
		isp_pid = -1;	/* ispell is not running.FIXME: Can we check this ? */
	}
	return 0;
}

#endif /* end USE_ISPELL */
