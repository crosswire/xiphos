
#ifndef _FIND_DLG_DEFS_H_
#define _FIND_DLG_DEFS_H_

#ifdef __cplusplus
extern "C" {
#endif	

struct _gs_find_dialog {
	GnomeDialog *dialog;
	GtkWidget *htmlwidget;
	GtkWidget *entry;
	GtkWidget *backward;
	GtkWidget *case_sensitive;
	gboolean regular;
};	
	
	
#ifdef __cplusplus
}
#endif

#endif

