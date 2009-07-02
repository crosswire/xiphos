/*
 * Xiphos Bible Study Tool
 * parallel_view.h - support for displaying multiple modules
 *
 * Copyright (C) 2004-2009 Xiphos Developer Team
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Library General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301, USA.
 */

#ifndef __PARALLEL_VIEW_H_
#define __PARALLEL_VIEW_H_

#ifdef __cplusplus
extern "C" {
#endif
	
typedef enum
{
	PARALLEL1,
	PARALLEL2,
	PARALLEL3,
	PARALLEL4,
	PARALLEL5
	
}GSParallel;


typedef struct _para_labels PARA_LABELS;
struct _para_labels {
	GtkWidget *label_1;
	GtkWidget *label_2;
	GtkWidget *label_3;
	GtkWidget *label_4;
	GtkWidget *label_5;
};
extern PARA_LABELS plabels;


gchar *main_parallel_change_verse(void);
void main_load_g_ops_parallel(GtkWidget *menu);
void main_set_parallel_options_at_start(void);
void main_load_menu_form_mod_list(GtkWidget * pmInt, gchar * label,
				   GCallback mycallback);
void main_change_parallel_module(GSParallel parallel, gchar * mod_name);	
void main_check_parallel_modules(void);	
void main_set_parallel_options_at_start(void);
void main_update_parallel_page(void);
void main_update_parallel_page_detached(void);
void main_swap_parallel_with_main(char *intmod);

void main_init_parallel_view(void);
void main_delete_parallel_view(void);

	
#ifdef __cplusplus
}
#endif

#endif
