/*
 * cursors.c: cursor handling for Evolution.
 *            copied from Gnumeric.
 *
 * Authors:
 *   Miguel de Icaza (miguel@gnu.org)
 */
#include <config.h>
#include <gnome.h>
#include "e-cursors.h"

static GdkColor black, white;

#define GDK_INTERNAL_CURSOR -1

typedef struct {
	GdkCursor *cursor;
	int       hot_x, hot_y;
	char      **xpm;
} CursorDef;

static CursorDef cursors [] = {
	{ NULL, GDK_INTERNAL_CURSOR,   GDK_CROSSHAIR,         NULL },
	{ NULL, GDK_INTERNAL_CURSOR,   GDK_LEFT_PTR,          NULL },
	{ NULL, GDK_INTERNAL_CURSOR,   GDK_FLEUR,             NULL },
	{ NULL, GDK_INTERNAL_CURSOR,   GDK_SB_H_DOUBLE_ARROW, NULL },
	{ NULL, GDK_INTERNAL_CURSOR,   GDK_SB_V_DOUBLE_ARROW, NULL },
	{ NULL, 0,    0,  NULL }
};

static int inited = FALSE;

static void
create_bitmap_and_mask_from_xpm (GdkBitmap **bitmap, GdkBitmap **mask, gchar **xpm)
{
	int height, width, colors;
	char pixmap_buffer [(32 * 32)/8];
	char mask_buffer [(32 * 32)/8];
	int x, y, pix, yofs;
	int transparent_color, black_color;

	sscanf (xpm [0], "%d %d %d %d", &height, &width, &colors, &pix);

	g_assert (height == 32);
	g_assert (width  == 32);
	g_assert (colors <= 3);

	transparent_color = ' ';
	black_color = '.';

	yofs = colors + 1;
	for (y = 0; y < 32; y++){
		for (x = 0; x < 32;){
			char value = 0, maskv = 0;

			for (pix = 0; pix < 8; pix++, x++){
				if (xpm [y + yofs][x] != transparent_color){
					maskv |= 1 << pix;

					/*
					 * Invert the colours here because it seems
					 * to workaround a bug the Matrox G100 Xserver?
					 * We reverse the foreground & background in the next
					 * routine to compensate.
					 */
					if (xpm [y + yofs][x] == black_color){
						value |= 1 << pix;
					}
				}
			}
			pixmap_buffer [(y * 4 + x/8)-1] = value;
			mask_buffer [(y * 4 + x/8)-1] = maskv;
		}
	}
	*bitmap = gdk_bitmap_create_from_data (NULL, pixmap_buffer, 32, 32);
	*mask   = gdk_bitmap_create_from_data (NULL, mask_buffer, 32, 32);
}

void
e_cursors_init (void)
{
	GdkColormap *colormap;
	int i;

	colormap = gtk_widget_get_default_colormap ();
	gdk_color_white (colormap, &white);
	gdk_color_black (colormap, &black);
	
	for (i = 0; cursors [i].hot_x; i++){
		GdkBitmap *bitmap, *mask;

		if (cursors [i].hot_x < 0)
			cursors [i].cursor = gdk_cursor_new (cursors [i].hot_y);
		else {
			create_bitmap_and_mask_from_xpm (&bitmap, &mask, cursors [i].xpm);

			/* The foreground and background colours are reversed.
			 * See comment above for explanation.
			 */
			cursors [i].cursor =
				gdk_cursor_new_from_pixmap (
					bitmap, mask,
					&black, &white,
					cursors [i].hot_x,
					cursors [i].hot_y);
		}
	}

	inited = TRUE;
	
	g_assert (i == E_NUM_CURSORS);
}

void
e_cursors_shutdown (void)
{
	int i;

	for (i = 0; cursors [i].hot_x; i++)
		gdk_cursor_destroy (cursors [i].cursor);
}


/* Returns a cursor given its type */
GdkCursor *
e_cursor_get (ECursorType type)
{
	g_return_val_if_fail (type >= 0 && type < E_NUM_CURSORS, NULL);

	if (!inited)
		g_warning ("e_cursors_init not called");
	
	return cursors [type].cursor;
}
