#ifndef E_POPUP_MENU_H
#define E_POPUP_MENU_H

#include <gtk/gtkwidget.h>

typedef struct {
	char const * const name;
	char const * const pixname;
	void         (*fn)(GtkWidget *widget, void *closure);
	int  disable_mask;
} EPopupMenu;

GtkMenu *e_popup_menu_create  (EPopupMenu     *menu_list,
			       int             disable_mask,
			       void           *closure);

void     e_popup_menu_run     (EPopupMenu     *menu_list,
			       GdkEventButton *event,
			       int             disable_mask,
			       void           *closure);

#endif /* E_POPUP_MENU_H */
