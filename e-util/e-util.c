/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* 
 * e-xml-utils.c
 * Copyright (C) 2000  Helix Code, Inc.
 * Author: Chris Lahey <clahey@helixcode.com>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation; either version 2 of the
 * License, or (at your option) any later version.
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

#include <glib.h>
#include <gtk/gtkobject.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <ctype.h>
#include <string.h>

#include "e-util.h"

int
g_str_compare(const void *x, const void *y)
{
  return strcmp(x, y);
}

int
g_int_compare(const void *x, const void *y)
{
  if ( GPOINTER_TO_INT(x) < GPOINTER_TO_INT(y) )
    return -1;
  else if ( GPOINTER_TO_INT(x) == GPOINTER_TO_INT(y) )
    return 0;
  else
    return 1;
}

char *
e_strdup_strip(char *string)
{
	int i;
	int length = 0;
	int initial = 0;
	for ( i = 0; string[i]; i++ ) {
		if (initial == i && isspace(string[i])) {
			initial ++;
		}
		if (!isspace(string[i])) {
			length = i - initial + 1;
		}
	}
	return g_strndup(string + initial, length);
}

void
e_free_object_list (GList *list)
{
	GList *p;

	for (p = list; p != NULL; p = p->next)
		gtk_object_unref (GTK_OBJECT (p->data));

	g_list_free (list);
}

void
e_free_string_list (GList *list)
{
	GList *p;

	for (p = list; p != NULL; p = p->next)
		g_free (p->data);

	g_list_free (list);
}

#define BUFF_SIZE 1024

char *
e_read_file(const char *filename)
{
	int fd;
	char buffer[BUFF_SIZE];
	GList *list = NULL, *list_iterator;
	GList *lengths = NULL, *lengths_iterator;
	int length = 0;
	int bytes;
	char *ret_val;

	fd = open(filename, O_RDONLY);
	if (fd == -1)
		return NULL;
	bytes = read(fd, buffer, BUFF_SIZE);
	while (bytes) {
		if (bytes > 0) {
			list = g_list_prepend(list, g_strndup(buffer, bytes));
			lengths = g_list_prepend(lengths, GINT_TO_POINTER(bytes));
			length += bytes;
		} else {
			if (errno != EINTR) {
				close(fd);
				g_list_foreach(list, (GFunc) g_free, NULL);
				g_list_free(list);
				g_list_free(lengths);
				return NULL;
			}
		}
		bytes = read(fd, buffer, BUFF_SIZE);
	}
	ret_val = g_new(char, length + 1);
	ret_val[length] = 0;
	lengths_iterator = lengths;
	list_iterator = list;
	for ( ; list_iterator; list_iterator = list_iterator->next, lengths_iterator = lengths_iterator->next) {
		int this_length = GPOINTER_TO_INT(lengths_iterator->data);
		length -= this_length;
		memcpy(ret_val + length, list_iterator->data, this_length);
	}
	close(fd);
	g_list_foreach(list, (GFunc) g_free, NULL);
	g_list_free(list);
	g_list_free(lengths);
	return ret_val;
}

gint
e_write_file(const char *filename, const char *data, int flags)
{
	int fd;
	int length = strlen(data);
	int bytes;
	fd = open(filename, flags, 0666);
	if (fd == -1)
		return errno;
	while (length > 0) {
		bytes = write(fd, data, length);
		if (bytes > 0) {
			length -= bytes;
			data += bytes;
		} else {
			if (errno != EINTR && errno != EAGAIN) {
				int save_errno = errno;
				close(fd);
				return save_errno;
			}
		}
	}
	close(fd);
	return 0;
}

typedef gint (*GtkSignal_INT__INT_INT_POINTER) (GtkObject * object,
						gint arg1,
						gint arg2,
						gpointer arg3,
						gpointer user_data);

void
e_marshal_INT__INT_INT_POINTER (GtkObject * object,
				GtkSignalFunc func,
				gpointer func_data, GtkArg * args)
{
	GtkSignal_INT__INT_INT_POINTER rfunc;
	gint *return_val;
	return_val = GTK_RETLOC_INT (args[3]);
	rfunc = (GtkSignal_INT__INT_INT_POINTER) func;
	*return_val = (*rfunc) (object,
				GTK_VALUE_INT     (args[0]),
				GTK_VALUE_INT     (args[1]),
				GTK_VALUE_POINTER (args[2]),
				func_data);
}

typedef void (*GtkSignal_NONE__OBJECT_DOUBLE_DOUBLE_BOOL) (GtkObject * object,
							      GtkObject *arg1,
							      gdouble arg2,
							      gdouble arg3,
							      gboolean arg4,
							      gpointer user_data);

void
e_marshal_NONE__OBJECT_DOUBLE_DOUBLE_BOOL (GtkObject * object,
					   GtkSignalFunc func,
					   gpointer func_data, GtkArg * args)
{
	GtkSignal_NONE__OBJECT_DOUBLE_DOUBLE_BOOL rfunc;
	rfunc = (GtkSignal_NONE__OBJECT_DOUBLE_DOUBLE_BOOL) func;
	(*rfunc) (object,
		  GTK_VALUE_OBJECT (args[0]),
		  GTK_VALUE_DOUBLE (args[1]),
		  GTK_VALUE_DOUBLE (args[2]),
		  GTK_VALUE_BOOL   (args[3]),
		  func_data);
}

typedef gdouble (*GtkSignal_DOUBLE__OBJECT_DOUBLE_DOUBLE_BOOL) (GtkObject * object,
								GtkObject *arg1,
								gdouble arg2,
								gdouble arg3,
								gboolean arg4,
								gpointer user_data);

void
e_marshal_DOUBLE__OBJECT_DOUBLE_DOUBLE_BOOL (GtkObject * object,
					GtkSignalFunc func,
					gpointer func_data, GtkArg * args)
{
	GtkSignal_DOUBLE__OBJECT_DOUBLE_DOUBLE_BOOL rfunc;
	gdouble *return_val;
	return_val = GTK_RETLOC_DOUBLE (args[4]);
	rfunc = (GtkSignal_DOUBLE__OBJECT_DOUBLE_DOUBLE_BOOL) func;
	*return_val = (*rfunc) (object,
				GTK_VALUE_OBJECT (args[0]),
				GTK_VALUE_DOUBLE (args[1]),
				GTK_VALUE_DOUBLE (args[2]),
				GTK_VALUE_BOOL   (args[3]),
				func_data);
}

typedef gdouble (*GtkSignal_BOOL__OBJECT_DOUBLE_DOUBLE_BOOL) (GtkObject * object,
							      GtkObject *arg1,
							      gdouble arg2,
							      gdouble arg3,
							      gboolean arg4,
							      gpointer user_data);

void
e_marshal_BOOL__OBJECT_DOUBLE_DOUBLE_BOOL (GtkObject * object,
					GtkSignalFunc func,
					gpointer func_data, GtkArg * args)
{
	GtkSignal_BOOL__OBJECT_DOUBLE_DOUBLE_BOOL rfunc;
	gboolean *return_val;
	return_val = GTK_RETLOC_BOOL (args[4]);
	rfunc = (GtkSignal_BOOL__OBJECT_DOUBLE_DOUBLE_BOOL) func;
	*return_val = (*rfunc) (object,
				GTK_VALUE_OBJECT (args[0]),
				GTK_VALUE_DOUBLE (args[1]),
				GTK_VALUE_DOUBLE (args[2]),
				GTK_VALUE_BOOL   (args[3]),
				func_data);
}

typedef void (*GtkSignal_NONE__INT_INT_POINTER_POINTER_INT_INT) (GtkObject * object,
								 gint arg1, 
								 gint arg2,
								 gpointer arg3,
								 gpointer arg4,
								 gint arg5,
								 gint arg6,
								 gpointer user_data);
void
e_marshal_NONE__INT_INT_POINTER_POINTER_INT_INT (GtkObject * object,
					   GtkSignalFunc func,
					   gpointer func_data, GtkArg * args)
{
  GtkSignal_NONE__INT_INT_POINTER_POINTER_INT_INT rfunc;
  rfunc = (GtkSignal_NONE__INT_INT_POINTER_POINTER_INT_INT) func;
  (*rfunc) (object,
	    GTK_VALUE_INT (args[0]), GTK_VALUE_INT (args[1]), 
	    GTK_VALUE_POINTER (args[2]),
	    GTK_VALUE_POINTER (args[3]),
	    GTK_VALUE_INT (args[4]), GTK_VALUE_INT (args[5]), func_data);
}

typedef void (*GtkSignal_NONE__INT_INT_POINTER_INT) (GtkObject * object,
						     gint arg1, 
						     gint arg2,
						     gpointer arg3,
						     gint arg4, gpointer user_data);
void
e_marshal_NONE__INT_INT_POINTER_INT (GtkObject * object,
			       GtkSignalFunc func,
			       gpointer func_data, GtkArg * args)
{
  GtkSignal_NONE__INT_INT_POINTER_INT rfunc;
  rfunc = (GtkSignal_NONE__INT_INT_POINTER_INT) func;
  (*rfunc) (object,
	    GTK_VALUE_INT (args[0]), GTK_VALUE_INT (args[1]), 
	    GTK_VALUE_POINTER (args[2]), GTK_VALUE_INT (args[3]), func_data);
}

typedef gboolean (*GtkSignal_BOOL__INT_INT_POINTER_INT_INT_INT) (GtkObject * object,
								 gint arg1, 
								 gint arg2,
								 gpointer arg3,
								 gint arg4,
								 gint arg5,
								 gint arg6,
								 gpointer user_data);
void
e_marshal_BOOL__INT_INT_POINTER_INT_INT_INT (GtkObject * object,
				       GtkSignalFunc func,
				       gpointer func_data, GtkArg * args)
{
  GtkSignal_BOOL__INT_INT_POINTER_INT_INT_INT rfunc;
  gboolean *return_val;
  return_val = GTK_RETLOC_BOOL (args[6]);
  rfunc = (GtkSignal_BOOL__INT_INT_POINTER_INT_INT_INT) func;
  *return_val = (*rfunc) (object,
			  GTK_VALUE_INT (args[0]),
			  GTK_VALUE_INT (args[1]), 
			  GTK_VALUE_POINTER (args[2]),
			  GTK_VALUE_INT (args[3]),
			  GTK_VALUE_INT (args[4]),
			  GTK_VALUE_INT (args[5]), func_data);
}

typedef void (*GtkSignal_NONE__INT_INT_POINTER_INT_INT_POINTER_INT_INT) (GtkObject *
									 object,
									 gint arg1, 
									 gint arg2,
									 gpointer
									 arg3,
									 gint arg4,
									 gint arg5,
									 gpointer
									 arg6,
									 gint arg7,
									 gint arg8,
									 gpointer
									 user_data);
void
e_marshal_NONE__INT_INT_POINTER_INT_INT_POINTER_INT_INT (GtkObject * object,
						   GtkSignalFunc func,
						   gpointer func_data,
						   GtkArg * args)
{
  GtkSignal_NONE__INT_INT_POINTER_INT_INT_POINTER_INT_INT rfunc;
  rfunc = (GtkSignal_NONE__INT_INT_POINTER_INT_INT_POINTER_INT_INT) func;
  (*rfunc) (object,
	    GTK_VALUE_INT (args[0]), GTK_VALUE_INT (args[1]), 
	    GTK_VALUE_POINTER (args[2]),
	    GTK_VALUE_INT (args[3]),
	    GTK_VALUE_INT (args[4]),
	    GTK_VALUE_POINTER (args[5]),
	    GTK_VALUE_INT (args[6]), GTK_VALUE_INT (args[7]), func_data);
}

gchar**
e_strsplit (const gchar *string,
	    const gchar *delimiter,
	    gint         max_tokens)
{
  GSList *string_list = NULL, *slist;
  gchar **str_array, *s;
  guint i, n = 1;

  g_return_val_if_fail (string != NULL, NULL);
  g_return_val_if_fail (delimiter != NULL, NULL);

  if (max_tokens < 1)
    max_tokens = G_MAXINT;

  s = strstr (string, delimiter);
  if (s)
    {
      guint delimiter_len = strlen (delimiter);

      do
	{
	  guint len;
	  gchar *new_string;

	  len = s - string;
	  new_string = g_new (gchar, len + 1);
	  strncpy (new_string, string, len);
	  new_string[len] = 0;
	  string_list = g_slist_prepend (string_list, new_string);
	  n++;
	  string = s + delimiter_len;
	  s = strstr (string, delimiter);
	}
      while (--max_tokens && s);
    }

  n++;
  string_list = g_slist_prepend (string_list, g_strdup (string));

  str_array = g_new (gchar*, n);

  i = n - 1;

  str_array[i--] = NULL;
  for (slist = string_list; slist; slist = slist->next)
    str_array[i--] = slist->data;

  g_slist_free (string_list);

  return str_array;
}

const gchar *
e_strstrcase (const gchar *haystack, const gchar *needle)
{
	/* find the needle in the haystack neglecting case */
	gchar *ptr;
	guint len;

	g_return_val_if_fail (haystack != NULL, NULL);
	g_return_val_if_fail (needle != NULL, NULL);

	len = strlen(needle);
	if (len > strlen(haystack))
		return NULL;

	if (len == 0)
		return (char *)haystack;

	for (ptr = (char *)haystack; *(ptr + len - 1) != '\0'; ptr++)
		if (!g_strncasecmp(ptr, needle, len))
			return ptr;

	return NULL;
}
