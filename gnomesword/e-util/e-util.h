#ifndef _E_UTIL_H_
#define _E_UTIL_H_

#include <glib.h>
#include <gtk/gtktypeutils.h>

#define E_MAKE_TYPE(l,str,t,ci,i,parent) \
GtkType l##_get_type(void)\
{\
	static GtkType type = 0;\
	if (!type){\
		GtkTypeInfo info = {\
			str,\
			sizeof (t),\
			sizeof (t##Class),\
			(GtkClassInitFunc) ci,\
			(GtkObjectInitFunc) i,\
			NULL, /* reserved 1 */\
			NULL, /* reserved 2 */\
			(GtkClassInitFunc) NULL\
		};\
                type = gtk_type_unique (parent, &info);\
	}\
	return type;\
}

typedef enum {
  E_FOCUS_NONE,
  E_FOCUS_CURRENT,
  E_FOCUS_START,
  E_FOCUS_END
} EFocus;

int  g_str_compare  (const void *x, const void *y);
int  g_int_compare  (const void *x, const void *y);

char *e_strdup_strip  (char *string);

void  e_free_object_list  (GList *list);
void  e_free_string_list  (GList *list);

char *e_read_file  (const char *filename);
gint e_write_file(const char *filename, const char *data, int flags);

gchar **e_strsplit (const gchar *string,
		    const gchar *delimiter,
		    gint         max_tokens);

const gchar *e_strstrcase (const gchar *haystack,
			   const gchar *needle);

void e_marshal_INT__INT_INT_POINTER (GtkObject * object,
				     GtkSignalFunc func,
				     gpointer func_data, GtkArg * args);

void e_marshal_NONE__OBJECT_DOUBLE_DOUBLE_BOOL (GtkObject * object,
						GtkSignalFunc func,
						gpointer func_data, GtkArg * args);

void e_marshal_DOUBLE__OBJECT_DOUBLE_DOUBLE_BOOL (GtkObject * object,
						  GtkSignalFunc func,
						  gpointer func_data, GtkArg * args);

void e_marshal_BOOL__OBJECT_DOUBLE_DOUBLE_BOOL (GtkObject * object,
						GtkSignalFunc func,
						gpointer func_data, GtkArg * args);

#define e_marshal_NONE__INT_INT_POINTER_POINTER_UINT_UINT e_marshal_NONE__INT_INT_POINTER_POINTER_INT_INT
void e_marshal_NONE__INT_INT_POINTER_POINTER_INT_INT (GtkObject * object,
						      GtkSignalFunc func,
						      gpointer func_data,
						      GtkArg * args);

#define e_marshal_NONE__INT_INT_POINTER_UINT e_marshal_NONE__INT_INT_POINTER_INT
void e_marshal_NONE__INT_INT_POINTER_INT (GtkObject * object,
					  GtkSignalFunc func,
					  gpointer func_data, GtkArg * args);

#define e_marshal_BOOL__INT_INT_POINTER_INT_INT_UINT e_marshal_BOOL__INT_INT_POINTER_INT_INT_INT
void e_marshal_BOOL__INT_INT_POINTER_INT_INT_INT (GtkObject * object,
						  GtkSignalFunc func,
						  gpointer func_data,
						  GtkArg * args);

#define e_marshal_NONE__INT_INT_POINTER_INT_INT_POINTER_UINT_UINT e_marshal_NONE__INT_INT_POINTER_INT_INT_POINTER_INT_INT
void e_marshal_NONE__INT_INT_POINTER_INT_INT_POINTER_INT_INT (GtkObject * object,
							      GtkSignalFunc func,
							      gpointer func_data,
							      GtkArg * args);

#endif /* _E_UTIL_H_ */
