/*
  generic s-exp evaluator class
*/
#ifndef _E_SEXP_H
#define _E_SEXP_H

#include <glib.h>
#include <gtk/gtk.h>

#define E_SEXP(obj)         GTK_CHECK_CAST (obj, e_sexp_get_type (), ESExp)
#define E_SEXP_CLASS(klass) GTK_CHECK_CLASS_CAST (klass, e_sexp_get_type (), ESExpClass)
#define FILTER_IS_SEXP(obj)      GTK_CHECK_TYPE (obj, e_sexp_get_type ())

typedef struct _ESExp      ESExp;
typedef struct _ESExpClass ESExpClass;

typedef struct _ESExpSymbol ESExpSymbol;
typedef struct _ESExpResult ESExpResult;
typedef struct _ESExpTerm ESExpTerm;

typedef struct _ESExpResult *(ESExpFunc)(struct _ESExp *sexp,
						   int argc,
						   struct _ESExpResult **argv,
						   void *data);

typedef struct _ESExpResult *(ESExpIFunc)(struct _ESExp *sexp,
						    int argc,
						    struct _ESExpTerm **argv,
						    void *data);
enum _ESExpResultType {
	ESEXP_RES_ARRAY_PTR=0,	/* type is a ptrarray, what it points to is implementation dependant */
	ESEXP_RES_INT,		/* type is a number */
	ESEXP_RES_STRING,	/* type is a pointer to a single string */
	ESEXP_RES_BOOL,		/* boolean type */
	ESEXP_RES_UNDEFINED	/* unknown type */
};

struct _ESExpResult {
	enum _ESExpResultType type;
	union {
		GPtrArray *ptrarray;
		int number;
		char *string;
		int bool;
	} value;
};

enum _ESExpTermType {
	ESEXP_TERM_INT	= 0,	/* integer literal */
	ESEXP_TERM_BOOL,	/* boolean literal */
	ESEXP_TERM_STRING,	/* string literal */
	ESEXP_TERM_FUNC,	/* normal function, arguments are evaluated before calling */
	ESEXP_TERM_IFUNC,	/* immediate function, raw terms are arguments */
	ESEXP_TERM_VAR,		/* variable reference */
};

struct _ESExpSymbol {
	int type;		/* ESEXP_TERM_FUNC or ESEXP_TERM_VAR */
	char *name;
	void *data;
	union {
		ESExpFunc *func;
		ESExpIFunc *ifunc;
	} f;
};

struct _ESExpTerm {
	enum _ESExpTermType type;
	union {
		char *string;
		int number;
		int bool;
		struct {
			struct _ESExpSymbol *sym;
			struct _ESExpTerm **terms;
			int termcount;
		} func;
		struct _ESExpSymbol *var;
	} value;
};



struct _ESExp {
	GtkObject object;

	GScanner *scanner;	/* for parsing text version */
	ESExpTerm *tree;	/* root of expression tree */
};

struct _ESExpClass {
	GtkObjectClass parent_class;

};

guint		e_sexp_get_type		(void);
ESExp 	       *e_sexp_new		(void);
void		e_sexp_add_function  	(ESExp *f, int scope, char *name, ESExpFunc *func, void *data);
void		e_sexp_add_ifunction  	(ESExp *f, int scope, char *name, ESExpIFunc *func, void *data);
void		e_sexp_add_variable  	(ESExp *f, int scope, char *name, ESExpTerm *value);
void		e_sexp_remove_symbol	(ESExp *f, int scope, char *name);
int		e_sexp_set_scope		(ESExp *f, int scope);

void		e_sexp_input_text		(ESExp *f, const char *text, int len);
void		e_sexp_input_file		(ESExp *f, int fd);


void		e_sexp_parse		(ESExp *f);
ESExpResult    *e_sexp_eval		(ESExp *f);

ESExpResult    *e_sexp_term_eval		(struct _ESExp *f, struct _ESExpTerm *t);
ESExpResult    *e_sexp_result_new	(int type);
void		e_sexp_result_free		(struct _ESExpResult *t);

/* utility functions for creating s-exp strings. */
void		e_sexp_encode_bool(GString *s, gboolean state);
void		e_sexp_encode_string(GString *s, const char *string);

#endif /* _E_SEXP_H */
