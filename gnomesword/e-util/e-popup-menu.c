/*
 * e-popup-menu.c: popup menu display
 *
 * Author:
 *   Miguel de Icaza (miguel@kernel.org)
 *   Jody Goldberg (jgoldberg@home.com)
 *
 * (C) 2000 Helix Code, Inc.
 */
#include <config.h>
#include <gnome.h>
#include "e-popup-menu.h"
#include "e-gui-utils.h"

/*
 * Creates an item with an optional icon
 */
static GtkWidget *
make_item (GtkMenu *menu, const char *name, const char *pixname)
{
	GtkWidget *label, *item;
	guint label_accel;

	if (*name == '\0')
		return gtk_menu_item_new ();
	
	/*
	 * Ugh.  This needs to go into Gtk+
	 */
	label = gtk_accel_label_new ("");
	label_accel = gtk_label_parse_uline (GTK_LABEL (label), name);
	gtk_misc_set_alignment (GTK_MISC (label), 0.0, 0.5);
	gtk_widget_show (label);
	
	item = gtk_pixmap_menu_item_new	();
	gtk_container_add (GTK_CONTAINER (item), label);
	
	if (label_accel != GDK_VoidSymbol){
		gtk_widget_add_accelerator (
			item,
			"activate_item",
			gtk_menu_ensure_uline_accel_group (GTK_MENU (menu)),
			label_accel, 0,
			GTK_ACCEL_LOCKED);
	}

	if (pixname){
		GtkWidget *pixmap = gnome_stock_pixmap_widget (item, pixname);
		
		gtk_widget_show (pixmap);
		gtk_pixmap_menu_item_set_pixmap (
			GTK_PIXMAP_MENU_ITEM (item), pixmap);
	}

	return item;
}

GtkMenu *
e_popup_menu_create (EPopupMenu *menu_list, int disable_mask, void *closure)
{
	GtkMenu *menu = GTK_MENU (gtk_menu_new ());
	int i;
	
	for (i = 0; menu_list [i].name; i++){
		GtkWidget *item;
		
		item = make_item (menu, menu_list [i].name,
				  menu_list [i].pixname);

		if (menu_list [i].fn)
			gtk_signal_connect (
				GTK_OBJECT (item), "activate",
				GTK_SIGNAL_FUNC (menu_list [i].fn),
				closure);

		if (menu_list [i].disable_mask & disable_mask)
			gtk_widget_set_sensitive (item, FALSE);
		
		gtk_widget_show (item);
		gtk_menu_append (menu, item);
	}

	return menu;
	
}

void
e_popup_menu_run (EPopupMenu *menu_list, GdkEventButton *event, int disable_mask, void *closure)
{
	GtkMenu *menu;
	
	g_return_if_fail (menu_list != NULL);
	g_return_if_fail (event != NULL);

	menu = e_popup_menu_create (menu_list, disable_mask, closure);

	e_popup_menu (menu, event);
}

