#ifndef E_CURSORS_H
#define E_CURSORS_H

/* Copied from Gnumeric */

typedef enum {
	E_CURSOR_THIN_CROSS,
	E_CURSOR_ARROW,
	E_CURSOR_MOVE,
	E_CURSOR_SIZE_X,
	E_CURSOR_SIZE_Y,
	E_NUM_CURSORS
} ECursorType;

void    e_cursors_init      (void);
void    e_cursors_shutdown  (void);

#define e_cursor_set(win, c)					   \
G_STMT_START {							   \
     if (win) 							   \
         gdk_window_set_cursor (win, e_cursor_get (c)); \
} G_STMT_END

#define e_cursor_set_widget(w, c)						     \
G_STMT_START {									     \
     if (GTK_WIDGET (w)->window)						     \
	gdk_window_set_cursor (GTK_WIDGET (w)->window, e_cursor_get (c)); \
} G_STMT_END

GdkCursor *e_cursor_get (ECursorType type);

#endif /* E_CURSORS_H */

