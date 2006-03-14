/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* e-html-utils.c
 * Copyright (C) 2000  Ximian, Inc.
 * Author: Dan Winship <danw@ximian.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include <config.h>
#include <libgnome/gnome-i18n.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <glib.h>

#include "e-html-utils.h"

static char *
check_size (char **buffer, int *buffer_size, char *out, int len)
{
	if (out + len + 1> *buffer + *buffer_size) {
		int index = out - *buffer;

		*buffer_size = MAX (index + len + 1, *buffer_size * 2);
		*buffer = g_realloc (*buffer, *buffer_size);
		out = *buffer + index;
	}
	return out;
}

/* 1 = non-email-address chars: ()<>@,;:\"[]`'|  */
/* 2 = trailing url garbage:    ,.!?;:>)]}`'-_|  */
static int special_chars[] = {
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /*  nul - 0x0f */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /* 0x10 - 0x1f */
	1, 2, 1, 0, 0, 0, 0, 3, 1, 3, 0, 0, 3, 2, 2, 0,    /*   sp - /    */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 3, 1, 0, 3, 2,    /*    0 - ?    */
	1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /*    @ - O    */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 3, 0, 2,    /*    P - _    */
	3, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,    /*    ` - o    */
	0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 3, 2, 0, 0     /*    p - del  */
};

#define is_addr_char(c) (isprint (c) && !(special_chars[c] & 1))
#define is_trailing_garbage(c) (!isprint(c) || (special_chars[c] & 2))

static char *
url_extract (const unsigned char **text, gboolean check)
{
	const unsigned char *end = *text, *p;
	char *out;

	while (*end && !isspace (*end) && (*end != '"') && (*end < 0x80))
		end++;

	/* Back up if we probably went too far. */
	while (end > *text && is_trailing_garbage (*(end - 1)))
		end--;

	if (check) {
		/* Make sure we weren't fooled. */
		p = memchr (*text, ':', end - *text);
		if (!p || end - p < 4)
			return NULL;
	}

	out = g_strndup (*text, end - *text);
	*text = end;
	return out;
}

static char *
email_address_extract (const unsigned char **cur, char **out, const unsigned char *linestart)
{
	const unsigned char *start, *end, *dot;
	char *addr;

	/* *cur points to the '@'. Look backward for a valid local-part */
	for (start = *cur; start - 1 >= linestart && is_addr_char (*(start - 1)); start--)
		;
	if (start == *cur)
		return NULL;

	/* Now look forward for a valid domain part */
	for (end = *cur + 1, dot = NULL; is_addr_char (*end); end++) {
		if (*end == '.' && !dot)
			dot = end;
	}
	if (!dot)
		return NULL;

	/* Remove trailing garbage */
	while (is_trailing_garbage (*(end - 1)))
		end--;
	if (dot > end)
		return NULL;

	addr = g_strndup (start, end - start);
	*out -= *cur - start;
	*cur = end;

	return addr;
}

static gboolean
is_citation (const unsigned char *c, gboolean saw_citation)
{
	gunichar u;
	gint i;

	/* A line that starts with a ">" is a citation, unless it's
	 * just mbox From-mangling...
	 */
	if (*c == '>') {
		const unsigned char *p;

		if (strncmp (c, ">From ", 6) != 0)
			return TRUE;

		/* If the previous line was a citation, then say this
		 * one is too.
		 */
		if (saw_citation)
			return TRUE;

		/* Same if the next line is */
		p = (const unsigned char *)strchr ((const char *)c, '\n');
		if (p && *++p == '>')
			return TRUE;

		/* Otherwise, it was just an isolated ">From" line. */
		return FALSE;
	}

	/* Check for "Rupert> " and the like... */
	for (i = 0; c && *c && g_unichar_validate (g_utf8_get_char (c)) && *c != '\n' && i < 10; i ++, c = g_utf8_next_char (c)) {
		u = g_utf8_get_char (c);
		if (u == '>')
			return TRUE;
		if (!g_unichar_isalnum (u))
			return FALSE;
	}
	return FALSE;
}

/**
 * e_text_to_html_full:
 * @input: a NUL-terminated input buffer
 * @flags: some combination of the E_TEXT_TO_HTML_* flags defined
 * in e-html-utils.h
 * @color: color for citation highlighting
 *
 * This takes a buffer of text as input and produces a buffer of
 * "equivalent" HTML, subject to certain transformation rules.
 *
 * The set of possible flags is:
 *
 *   - E_TEXT_TO_HTML_PRE: wrap the output HTML in <PRE> and </PRE>.
 *     Should only be used if @input is the entire buffer to be
 *     converted. If e_text_to_html is being called with small pieces
 *     of data, you should wrap the entire result in <PRE> yourself.
 *
 *   - E_TEXT_TO_HTML_CONVERT_NL: convert "\n" to "<BR>\n" on output.
 *     (should not be used with E_TEXT_TO_HTML_PRE, since that would
 *     result in double-newlines).
 *
 *   - E_TEXT_TO_HTML_CONVERT_SPACES: convert a block of N spaces
 *     into N-1 non-breaking spaces and one normal space. A space
 *     at the start of the buffer is always converted to a
 *     non-breaking space, regardless of the following character,
 *     which probably means you don't want to use this flag on
 *     pieces of data that aren't delimited by at least line breaks.
 *
 *     If E_TEXT_TO_HTML_CONVERT_NL and E_TEXT_TO_HTML_CONVERT_SPACES
 *     are both defined, then TABs will also be converted to spaces.
 *
 *   - E_TEXT_TO_HTML_CONVERT_URLS: wrap <a href="..."> </a> around
 *     strings that look like URLs.
 *
 *   - E_TEXT_TO_HTML_CONVERT_ADDRESSES: wrap <a href="mailto:..."> </a> around
 *     strings that look like mail addresses.
 *
 *   - E_TEXT_TO_HTML_MARK_CITATION: wrap <font color="..."> </font> around
 *     citations (lines beginning with "> ", etc).
 *
 *   - E_TEXT_TO_HTML_ESCAPE_8BIT: flatten everything to US-ASCII
 *
 *   - E_TEXT_TO_HTML_CITE: quote the text with "> " at the start of each
 *     line.
 **/
char *
e_text_to_html_full (const char *input, unsigned int flags, guint32 color)
{
	const unsigned char *cur, *next, *linestart;
	char *buffer = NULL;
	char *out = NULL;
	int buffer_size = 0, col;
	gboolean colored = FALSE, saw_citation = FALSE;

	/* Allocate a translation buffer.  */
	buffer_size = strlen (input) * 2 + 5;
	buffer = g_malloc (buffer_size);

	out = buffer;
	if (flags & E_TEXT_TO_HTML_PRE)
		out += sprintf (out, "<PRE>");

	col = 0;

	for (cur = linestart = input; cur && *cur; cur = next) {
		gunichar u;

		if (flags & E_TEXT_TO_HTML_MARK_CITATION && col == 0) {
			saw_citation = is_citation (cur, saw_citation);
			if (saw_citation) {
				if (!colored) {
					gchar font [25];

					g_snprintf (font, 25, "<FONT COLOR=\"#%06x\">", color);

					out = check_size (&buffer, &buffer_size, out, 25);
					out += sprintf (out, "%s", font);
					colored = TRUE;
				}
			} else if (colored) {
				gchar *no_font = "</FONT>";

				out = check_size (&buffer, &buffer_size, out, 9);
				out += sprintf (out, "%s", no_font);
				colored = FALSE;
			}

			/* Display mbox-mangled ">From" as "From" */
			if (*cur == '>' && !saw_citation)
				cur++;
		} else if (flags & E_TEXT_TO_HTML_CITE && col == 0) {
			out = check_size (&buffer, &buffer_size, out, 5);
			out += sprintf (out, "&gt; ");
		}

		u = g_utf8_get_char (cur);
		if (g_unichar_isalpha (u) &&
		    (flags & E_TEXT_TO_HTML_CONVERT_URLS)) {
			char *tmpurl = NULL, *refurl = NULL, *dispurl = NULL;

			if (!strncasecmp (cur, "http://", 7) ||
			    !strncasecmp (cur, "https://", 8) ||
			    !strncasecmp (cur, "ftp://", 6) ||
			    !strncasecmp (cur, "nntp://", 7) ||
			    !strncasecmp (cur, "mailto:", 7) ||
			    !strncasecmp (cur, "news:", 5)) {
				tmpurl = url_extract (&cur, TRUE);
				if (tmpurl) {
					refurl = e_text_to_html (tmpurl, 0);
					dispurl = g_strdup (refurl);
				}
			} else if (!strncasecmp (cur, "www.", 4) &&
				   (*(cur + 4) < 0x80) &&
				   g_unichar_isalnum (*(cur + 4))) {
				tmpurl = url_extract (&cur, FALSE);
				dispurl = e_text_to_html (tmpurl, 0);
				refurl = g_strdup_printf ("http://%s",
							  dispurl);
			}

			if (tmpurl) {
				out = check_size (&buffer, &buffer_size, out,
						  strlen (refurl) +
						  strlen (dispurl) + 15);
				out += sprintf (out,
						"<a href=\"%s\">%s</a>",
						refurl, dispurl);
				col += strlen (tmpurl);
				g_free (tmpurl);
				g_free (refurl);
				g_free (dispurl);
			}

			if (!*cur)
				break;
			u = g_utf8_get_char (cur);
		}

		if (u == '@' && (flags & E_TEXT_TO_HTML_CONVERT_ADDRESSES)) {
			char *addr, *dispaddr, *outaddr;

			addr = email_address_extract (&cur, &out, linestart);
			if (addr) {
				dispaddr = e_text_to_html (addr, 0);
				outaddr = g_strdup_printf ("<a href=\"mailto:%s\">%s</a>",
							   addr, dispaddr);
				out = check_size (&buffer, &buffer_size, out, strlen (outaddr));
				out += sprintf (out, "%s", outaddr);
				col += strlen (addr);
				g_free (addr);
				g_free (dispaddr);
				g_free (outaddr);

				u = g_utf8_get_char (cur);
			}
		}

		if (!g_unichar_validate (u)) {
			/* Sigh. Someone sent undeclared 8-bit data.
			 * Assume it's iso-8859-1.
			 */
			u = *cur;
			next = cur + 1;
		} else
			next = g_utf8_next_char (cur);

		out = check_size (&buffer, &buffer_size, out, 10);

		switch (u) {
		case '<':
			strcpy (out, "&lt;");
			out += 4;
			col++;
			break;

		case '>':
			strcpy (out, "&gt;");
			out += 4;
			col++;
			break;

		case '&':
			strcpy (out, "&amp;");
			out += 5;
			col++;
			break;

		case '"':
			strcpy (out, "&quot;");
			out += 6;
			col++;
			break;

		case '\n':
			if (flags & E_TEXT_TO_HTML_CONVERT_NL) {
				strcpy (out, "<br>");
				out += 4;
			}
			*out++ = *cur;
			linestart = cur;
			col = 0;
			break;

		case '\t':
			if (flags & (E_TEXT_TO_HTML_CONVERT_SPACES |
				     E_TEXT_TO_HTML_CONVERT_NL)) {
				do {
					out = check_size (&buffer, &buffer_size,
						    out, 7);
					strcpy (out, "&nbsp;");
					out += 6;
					col++;
				} while (col % 8);
				break;
			}
			/* otherwise, FALL THROUGH */

		case ' ':
			if (flags & E_TEXT_TO_HTML_CONVERT_SPACES) {
				if (cur == (const unsigned char *)input ||
				    *(cur + 1) == ' ' || *(cur + 1) == '\t' ||
				    *(cur - 1) == '\n') {
					strcpy (out, "&nbsp;");
					out += 6;
					col++;
					break;
				}
			}
			/* otherwise, FALL THROUGH */

		default:
			if ((u >= 0x20 && u < 0x80) ||
			    (u == '\r' || u == '\t')) {
				/* Default case, just copy. */
				*out++ = u;
			} else {
				if (flags & E_TEXT_TO_HTML_ESCAPE_8BIT)
					*out++ = '?';
				else
					out += g_snprintf(out, 9, "&#%d;", u);
			}
			col++;
			break;
		}
	}

	out = check_size (&buffer, &buffer_size, out, 7);
	if (flags & E_TEXT_TO_HTML_PRE)
		strcpy (out, "</PRE>");
	else
		*out = '\0';

	return buffer;
}

char *
e_text_to_html (const char *input, unsigned int flags)
{
	return e_text_to_html_full (input, flags, 0);
}
