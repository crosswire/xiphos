/* 
 * added to gnomesword 2001-01-06  for spell checking
*/

/* Bluefish HTML Editor
 * spell.c - Check Spelling
 *
 * Copyright (C)2000 Pablo De Napoli (for this module)
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * Note: some code has been taken from Lyx (wich is also covered by the
 * GNU Public Licence).
*/

/* For the select function */
#define _GNU_SOURCE

/*********************************************************************
 * The spellchecker and it's GUI run like this:
 *
 * run_spell_checker
 * 	create_spc_window
 * 		spc_start_button_clicked_lcb
 * 			run_spell_checker
 * 				create_ispell_pipe
 * 				--
 * 				get_next_word
 * 				check_word
 * 					ispell_check_word
 * 					correct_word
 * 				--
 * 				ispell_terminate
 * 				destroy_spc_window
 * 	
 **********************************************************************/


/* Print debug messages using standard error */
/* This is needed when the stdin is redirected */
#define MY_DEBUG_MSG g_printerr

#ifdef HAVE_CONFIG_H
#  include "config.h"
#endif

#include <glib.h>
#include <gtk/gtk.h>

#include "spell.h"
#include "debug.h"
#include "char_table.h"
#include "support.h"

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

GtkWidget *text_widget;
extern GtkWidget *myspell_app;
/* Spellchecker status */
enum {
	ISP_OK = 1,
	ISP_ROOT,
	ISP_COMPOUNDWORD,
	ISP_UNKNOWN,
	ISP_MISSED,
	ISP_IGNORE
};



static FILE *in, *out;		/* streams to communicate with ispell */
pid_t isp_pid = -1;		/* pid for the `ispell' process */

static int isp_fd;

/* void sigchldhandler(int sig); */
void sigchldhandler(pid_t pid, int *status);

/* extern void sigchldchecker(int sig); */
extern void sigchldchecker(pid_t pid, int *status);

typedef struct {
	gint flag;
	gint count;
	gchar **misses;
} isp_result;

/* message from the spell checker GUI */
gint spc_message;

/*  flag to know if we reach the end of text */
static gboolean end_of_text;

#define SPC_CHAR_LENGTH 20

/* get_next_char:
  returns the next char in the text.
  set flags end_of_text
  converts special characters like &aacute; to iso characters
*/

static gchar get_next_char()
/* FIXME: a more eficient implementation? *//* use an HTML parser like gnome-xml ? */
{
	static gint current_position = 0;
	gint old_position, i;
	gchar c, old_c;
	gchar token[SPC_CHAR_LENGTH];
	
	DEBUG_MSG("getting next char\n");
	current_position = gtk_text_get_point(GTK_TEXT(text_widget));
	DEBUG_MSG("current_position = %i\n", current_position);
	c = GTK_TEXT_INDEX(GTK_TEXT(text_widget) ,current_position);
	/* macro to get a single character */
	if (current_position < gtk_text_get_length(GTK_TEXT(text_widget))) {
		current_position++;
		gtk_text_set_point(GTK_TEXT (text_widget), current_position);
		end_of_text = 0;
		if (c == '&') {
			DEBUG_MSG("Character & detected\n");
			old_position = current_position;
			token[0] = c;
			old_c = c;
			for (i = 1; i < SPC_CHAR_LENGTH - 2; i++) {
				c = get_next_char();
				token[i] = c;
				if (c == ';')
					break;
				else if (end_of_text)
					break;
			};
			i++;
			token[i] = '\0';
			DEBUG_MSG
			    ("Converting special character = \"%s\" \n",
			     token);
			c = convert_from_html_chars(token, ANY_CHAR_SET);
			if (c == '\0') {
				current_position = old_position;
				c = old_c;
				/* FIXME: mark for gettext / beter dialog ? */
				g_printerr
				    ("Invalid especial character! %s\n",
				     token);
			}
		}
	} else
		end_of_text = 1;
	DEBUG_MSG("next_char ='%c'\n", c);
	return (c);
}				/* end get_next_char */

typedef struct {
	gchar *text;
	gint begin;
	gint end;
	gint space_length;
	gint index;
} Tword;

#define WORD_BUFFER_SIZE 20

inline static Tword *new_word(Tword * word)
{
	word = (Tword *) g_malloc(sizeof(Tword));
	word->index = 0;
	word->begin = 0;
	word->end = 0;
	word->space_length = WORD_BUFFER_SIZE;
	word->text = g_malloc((word->space_length) * sizeof(gchar));
	return (word);
}

inline static void free_word(Tword * word)
{
	DEBUG_MSG("Free word \"%s\" \n", word->text);
	g_free(word->text);
	g_free(word);
}

#define SPECIAL_CHAR_LENGTH 10

static void store_char_in_word(Tword * word, gchar c)
{
	DEBUG_MSG("indice =\n %i\n", word->index);
	/* enlarge the buffer if needed */
	if (word->index >= word->space_length - 1) {
		DEBUG_MSG("enlarging word buffer\n");
		word->space_length = word->space_length + WORD_BUFFER_SIZE;
		DEBUG_MSG("space length=%i\n", word->space_length);
		word->text =
		    g_realloc(word->text,
			      word->space_length * sizeof(gchar));
	}
	(word->text)[word->index] = c;
	(word->index)++;
}

inline static gboolean is_empty_word(Tword * word)
{
	return ((word->index) == 0);
}

/* Non alphabetical characters are ignored */
/* and also the text between < > (asumed to be html tags) */
/*  in PHP code you can have different numbers of > and <, so this should be extended!
 like in if (a < b) { */
/* we want to be sure that only valid characters are pased to ispell */

static Tword *get_next_word(void)
{
	Tword *next_word;
	gchar c = '\0';
	DEBUG_MSG("getting next word\n");
	next_word = new_word(next_word);
	next_word->begin =
	    gtk_text_get_point(GTK_TEXT(text_widget));
	

	while (!end_of_text) {
		DEBUG_MSG("Not end of text\n");
		c = get_next_char();
		if (isalpha((char) c) || isalpha_iso((char) c))
			store_char_in_word(next_word, c);
		/* FIXME: accept iso special characters! */
		else {
			DEBUG_MSG
			    ("Ignoring non alphabethical character\n");
			break;	/* if it is a non alphabetical character exit the loop */
		};
	};
	store_char_in_word(next_word, '\0');
	if (is_empty_word(next_word))
		return next_word;
	/* at the end of text , do not substract 1 */
	next_word->end = gtk_text_get_point(GTK_TEXT(text_widget)) - (!end_of_text);
	DEBUG_MSG("next word ends = \"%d\" \n", next_word->end);
	DEBUG_MSG("next word= \"%s\" \n", next_word->text);
	/* ignore html tags */
	if (c == '<') {
		DEBUG_MSG("Ignoring html tag\n");
		while (((c = get_next_char()) != '>') && (!end_of_text));
	}
	return next_word;
}


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

	MY_DEBUG_MSG("Creating ispell pipe ...\n");

	isp_pid = -1;

	if (pipe(pipein) == -1 || pipe(pipeout) == -1) {
		g_printerr(_
			   ("GnomeSword: Can't create pipe for spellchecker!"));
		return;
	}

	if ((out = fdopen(pipein[1], "w")) == NULL) {
		g_printerr(_
			   ("GnomeSword: Can't create stream for pipe for spellchecker!"));
		return;
	}

	if ((in = fdopen(pipeout[0], "r")) == NULL) {
		g_printerr(_
			   ("GnomeSword: Can't create stream for pipe for spellchecker!"));
		return;
	}

	setvbuf(out, o_buf, _IOLBF, BUFSIZ);

	isp_fd = pipeout[0];

	isp_pid = fork();

	if (isp_pid == -1) {
		g_printerr(_
			   ("GnomeSword: Can't create child process for spellchecker!"));
		return;
	}

	if (isp_pid == 0) {
		/* child process */

		/* NOTE: Don't use DEBUG_MSG in this part since the output is redirected! */



		MY_DEBUG_MSG("Spell-checker child process \n");

		dup2(pipein[0], STDIN_FILENO);
		dup2(pipeout[1], STDOUT_FILENO);
		close(pipein[0]);
		close(pipein[1]);
		close(pipeout[0]);
		close(pipeout[1]);

		argc = 0;
#if USE_ASPELL
		argv[argc++] = "aspell";
#else
		argv[argc++] = "ispell";
#endif /* USE_ASPELL */
		argv[argc++] = g_strdup("-a");	// "Pipe" mode
/*
		if ((g_strcasecmp(main_v->props.cfg_spc_lang, "default") !=
		     0) && (*(main_v->props.cfg_spc_lang) != '\0')) {
			if (main_v->props.cfg_spc_lang != NULL) {
				argv[argc++] = g_strdup("-d");	// Dictionary file
				argv[argc++] = main_v->props.cfg_spc_lang;
			};
		}
*/		/* Consider run-together words as legal compounds */
/*
		if (main_v->props.spc_accept_compound)
			argv[argc++] = g_strdup("-C");
		else
*/
			/* Report run-together words with
			   missing blanks as errors */
			argv[argc++] = g_strdup("-B");

		
		

		
		argv[argc++] = NULL;
#ifdef DEBUG
		MY_DEBUG_MSG("Executing:\n");
		for (i = 0; i < argc - 1; i++)
			MY_DEBUG_MSG("%s ", argv[i]);
#endif
#if USE_ASPELL
		execvp("aspell", (char *const *) argv);
#else
		execvp("ispell", (char *const *) argv);
#endif /* USE_ASPELL */
		/* free the memory used  */
		for (i = 0; i < argc - 1; i++)
			g_free(argv[i]);

		g_printerr(_("GnomeSwrod: Failed to start ispell!\n"));
		_exit(0);
	}

	/* Parent process: Read ispells identification message 
	   Hmm...what are we using this id msg for? Nothing? (Lgb)
	   Actually I used it to tell if it's truly Ispell or if it's
	   aspell -- (kevinatk@home.com) */

#ifdef WITH_WARNINGS
#warning verify that this works.
#endif

	FD_ZERO(&infds);
	FD_SET(pipeout[0], &infds);
	tv.tv_sec = 15;		/*  fifteen second timeout. Probably too much,
				   but it can't really hurt. */
	tv.tv_usec = 0;

	/* Configure provides us with macros which are supposed to do
	   the right typecast. */

	retval = select((SELECT_TYPE_ARG1) (pipeout[0] + 1),
			SELECT_TYPE_ARG234 (&infds),
			0, 
			0, 
			SELECT_TYPE_ARG5(&tv));

	if (retval > 0) {
		/* Ok, do the reading. We don't have to FD_ISSET since
		   there is only one fd in infds. */
		fgets(buf, 2048, in);
		MY_DEBUG_MSG("\nIspell pipe created\n");
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
		g_printerr(_
			   ("Select on ispell returned error, what now?\n"));
	}
}


static inline void ispell_terse_mode(void)
{
	fputs("!\n", out);	/* Set terse mode (silently accept correct words) */
}


static inline void ispell_insert_word(char const *word)
{
	fputc('*', out);	/* Insert word in personal dictionary */
	fputs(word, out);
	fputc('\n', out);
}


static
inline void ispell_accept_word(char const *word)
{
	fputc('@', out);	/* Accept in this session */
	fputs(word, out);
	fputc('\n', out);
}

/* Send word to ispell and get reply */
static isp_result *ispell_check_word(gchar * word)
{
	isp_result *result;
	gchar buf[1024], *p, *np;
	gchar *nb;
	gint count;

#ifdef DEBUG
	gint i;
#endif

	/* Coment from the original author:
	   I think we have to check if ispell is still alive here */

	if (isp_pid == -1)
		return (isp_result *) NULL;

	DEBUG_MSG("Sending word \"%s\" to ispell\n", word);
	fputs(word, out);
	fputc('\n', out);

	fgets(buf, 1024, in);

	DEBUG_MSG("Message from ispell=\"%s\"\n", buf);

	result = g_malloc(sizeof(isp_result));

	switch (*buf) {
	case '*':		/* Word found */
		result->flag = ISP_OK;
		DEBUG_MSG("Ispell: Word found\n");
		break;
	case '+':		/* Word found through affix removal */
		result->flag = ISP_ROOT;
		DEBUG_MSG("Ispell: Word found through affix removal\n");
		break;
	case '-':		/* Word found through compound formation */
		result->flag = ISP_COMPOUNDWORD;
		DEBUG_MSG
		    ("Ispell: Word found through compound formation\n");
		break;
	case '\n':		/* Number or when in terse mode: no problems */
		result->flag = ISP_IGNORE;
		DEBUG_MSG("Ispell: Ignore Word\n");
		break;
	case '#':		/* Not found, no near misses and guesses */
		result->flag = ISP_UNKNOWN;
		DEBUG_MSG("Ispell: Unknown Word\n");
		break;
	case '?':		/* Not found, and no near misses, but guesses (guesses are ignored) */
	case '&':		/* Not found, but we have near misses */
		{
			result->flag = ISP_MISSED;
			DEBUG_MSG("Ispell: Near Misses\n");
			/* FIXME: why duplicate buffer ? */
			nb = g_strdup(buf);
			p = strpbrk(nb + 2, " ");
			sscanf(p, "%d", &count);	/* Get near misses count */
			result->count = count;
			if (count) {
				p = strpbrk(nb, ":");
				p += 2;

				/* remove the last '\n' from ispell output */
				DEBUG_MSG("Removing end-of-line \n");
				np = p;
				while (*np != '\0')
					np++;
				np--;
				*np = '\0';

				DEBUG_MSG("misses list \"%s\" \n", p);

				result->misses =
				    g_strsplit(p, ", ", count);
				g_free(nb);
#ifdef DEBUG
				for (i = 0; i < count; i++)
					DEBUG_MSG("near_miss = \"%s\" \n",
						  result->misses[i]);
#endif
			}


			break;
		}
	default:		/* This shouldn't happend, but you know Murphy */
		result->flag = ISP_UNKNOWN;
		DEBUG_MSG("Ispell: Unknown\n");
	}

	*buf = 0;
	if (result->flag != ISP_IGNORE) {
		while (*buf != '\n')
			fgets(buf, 255, in);	/* wait for ispell to finish */
	}
	return result;
}

GdkColor misspelled_color;
static inline void select_word(Tword * word)
{	
	
	
	
	DEBUG_MSG("\nword begins = \"%d\" \n", word->begin);
	DEBUG_MSG("word ends = \"%d\" \n", word->end);
	
	gtk_text_set_point(GTK_TEXT(text_widget), word->begin);
	gtk_text_forward_delete(GTK_TEXT(text_widget),
					word->end - word->begin);

	gtk_text_insert(GTK_TEXT(text_widget), 0, &misspelled_color, 0,
				word->text, -1);
	gtk_text_thaw(GTK_TEXT(text_widget));			
	/* gtk_editable_select_region(GTK_EDITABLE(text_widget)), word->begin, word->end); */
}

/*for gtk_text_insert*/
#define ALL_THE_STRING -1


static void replace_word_with(Tword * word, gchar * replace_with)
{
	gtk_text_set_point(GTK_TEXT(text_widget), word->begin);
	gtk_text_forward_delete(GTK_TEXT(text_widget), word->end - word->begin);
	DEBUG_MSG("inserting replace text =\"%s\" \n", replace_with);
	gtk_editable_insert_text(GTK_EDITABLE
				 (text_widget),
				 replace_with, strlen(replace_with),
				 &(word->begin));
}
/*
 *change slected word color back to original color
 */
static void change_word_color(Tword * word)
{
	gtk_text_set_point(GTK_TEXT(text_widget), word->begin);
	gtk_text_forward_delete(GTK_TEXT(text_widget), word->end - word->begin);
	DEBUG_MSG("inserting replace text =\"%s\" \n", replace_with);
	gtk_editable_insert_text(GTK_EDITABLE
				 (text_widget),
				 word->text, strlen(word->text),
				 &(word->begin));
}

static void correct_word(Tword * word)
/* ask the user how to correct a word, and do it */
{
	gboolean word_corrected = 0;
	select_word(word);
	
	do {
		spc_message = SPC_NONE;
		
		while (gtk_events_pending())
			gtk_main_iteration();
		switch (spc_message) {
		case SPC_INSERT:
			ispell_insert_word(word->text);
			change_word_color(word);
			word_corrected = 1;
			break;
		case SPC_ACCEPT:
			ispell_accept_word(word->text);
			change_word_color(word);
			word_corrected = 1;
			break;
		case SPC_IGNORE:
		        change_word_color(word);
			word_corrected = 1;
			break;
		case SPC_REPLACE:
			replace_word_with(word,
					  gtk_entry_get_text(GTK_ENTRY
							     (spc_gui.
							      replace_entry)));
			word_corrected = 1;
			break;
		case SPC_CLOSE:
			DEBUG_MSG("close message\n");
			change_word_color(word);
			word_corrected = 1;	/* to exit the while loop */
			break;
		}
	} while (!word_corrected);
	/* gtk_editable_delete_selection (GTK_EDITABLE(text_widget)); */
	DEBUG_MSG("Word corrected\n");
};

static void check_word(Tword * word)
{
	isp_result *result;
	gchar *converted_string;
	gint i;
	gboolean outhtml = FALSE;
/* FIXME: Is this really needed ? */
	while (gtk_events_pending())
		gtk_main_iteration();

	result = ispell_check_word(word->text);
	switch (result->flag) {
	case ISP_MISSED:	/* creates near misses list */
		DEBUG_MSG("Creating misses list\n");
		for (i = 0; i < result->count; i++) {
			if (outhtml) {
				converted_string =
				    convert_string_iso_to_html(result->
							       misses[i]);
				g_free(result->misses[i]);
				result->misses[i] = converted_string;
			};
			DEBUG_MSG
			    ("Adding \"%s\" to the near_misses_list\n",
			     result->misses[i]);
			gtk_clist_append(GTK_CLIST
					 (spc_gui.near_misses_clist),
					 &(result->misses[i]));
		};
	case ISP_UNKNOWN:
		gtk_entry_set_text(GTK_ENTRY(spc_gui.word_entry),
				   word->text);
		gtk_entry_set_text(GTK_ENTRY(spc_gui.replace_entry),
				   word->text);
		correct_word(word);
		DEBUG_MSG("spc_message = %i\n", spc_message);
	};			/* end switch */

	/* do a clean up */
	switch (result->flag) {
	case ISP_MISSED:
		DEBUG_MSG("free & clean near_misses_lists ...\n");
		gtk_clist_clear(GTK_CLIST(spc_gui.near_misses_clist));
		for (i = 0; i < result->count; i++)
			g_free(result->misses[i]);
		DEBUG_MSG("free & cleaning near_misses_list done\n");

	case ISP_UNKNOWN:
		gtk_entry_set_text(GTK_ENTRY(spc_gui.word_entry), "");
		gtk_entry_set_text(GTK_ENTRY(spc_gui.replace_entry), "");
		DEBUG_MSG("word & replace entries cleaned\n");
	};			/*end switch */
	g_free(result);
#ifdef DEBUG
	if (spc_message == SPC_CLOSE)
		DEBUG_MSG("Passing SPC_CLOSE message\n");
#endif
	DEBUG_MSG("word checked\n");
}				/* end check_word  */


inline static void update_progress_bar(void)
{
	gfloat percentage;
	gfloat current_position;
	gfloat text_length;
	current_position =
	    (gfloat)
	    gtk_text_get_point(GTK_TEXT(text_widget));
	text_length =
	    (gfloat)
	    gtk_text_get_length(GTK_TEXT(text_widget));
	/* percentage is betweeen 0 and 1 */
	percentage = current_position / text_length;
	DEBUG_MSG("percentage done = %f\n", percentage);
	gtk_progress_bar_update(GTK_PROGRESS_BAR(spc_gui.progress_bar),
				percentage);
};

void ispell_terminate()
{
	if (isp_pid != -1) {	/* checks if ispell is running */
		fputs("#\n", out);	/* Save personal dictionary */
		fflush(out);
		fclose(out);
		kill(isp_pid, SIGTERM);
		isp_pid = -1;	/* ispell is not running.FIXME: Can we check this ? */
	}
}

/* Perform an ispell session */
gboolean run_spell_checker(void)
{
	Tword *word;
	unsigned int word_count = 0;
	GdkColormap *cmap;
	gtk_text_freeze(GTK_TEXT(text_widget));
	
	cmap = gdk_colormap_get_system();
	misspelled_color.red = 0xffff;
	misspelled_color.green = 0;
	misspelled_color.blue = 0;
	if (!gdk_color_alloc(cmap, &misspelled_color)) {
		g_error("couldn't allocate required colors");
	}
	
	
	/* create ispell process */	
	create_ispell_pipe();
	gtk_text_set_point(GTK_TEXT(text_widget), 0);
	end_of_text = 0;	/* needed for get_next_word to work properly */

	if (isp_pid == -1) {
		g_warning("oops");
	/*
		error_dialog(_("GnomeSword Spell Checker Error"),
			     _("\n\n"
			       "The ispell-process has died for some reason. *One* possible reason\n"
			       "could be that you do not have a dictionary file\n"
			       "for the language of this document installed.\n"
			       "Check /usr/lib/ispell or set another\n"
			       "dictionary in the Spellchecker Options menu."));
		fclose(out);
		return TRUE;
	*/
	};			/*end if */

	gtk_grab_add(spc_gui.window);	/* This window becames modal */

	/* Put ispell in terse mode to improve speed */
	ispell_terse_mode();
	do {
		word = get_next_word();
		if (!is_empty_word(word)) {
			word_count++;
			spc_message = SPC_NONE;
			check_word(word);
			free_word(word);
			update_progress_bar();
			if (spc_message == SPC_CLOSE) {
				DEBUG_MSG("Close message received\n");
				break;
			};
		};
	} while (!end_of_text);
	DEBUG_MSG("Spell checking done\n");
	gtk_text_thaw(GTK_TEXT(text_widget));
	ispell_terminate();
	DEBUG_MSG("Removing grab\n");
	gtk_grab_remove(spc_gui.window);
	DEBUG_MSG("Removing status \n");
	//statusbar_remove(GINT_TO_POINTER(spc_gui.status_bar_count));
	DEBUG_MSG("Destroying window\n");
	gtk_widget_destroy(spc_gui.window);

	return (spc_message != SPC_CLOSE);
}				/* end run_spell_checker */

