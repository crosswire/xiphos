/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* e-text-event-processor.c
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

#include <gnome.h>
#include "e-text-event-processor-emacs-like.h"
static void e_text_event_processor_emacs_like_init		(ETextEventProcessorEmacsLike		 *card);
static void e_text_event_processor_emacs_like_class_init	(ETextEventProcessorEmacsLikeClass	 *klass);
static gint e_text_event_processor_emacs_like_event (ETextEventProcessor *tep, ETextEventProcessorEvent *event);

static ETextEventProcessorClass *parent_class = NULL;

/* The arguments we take */
enum {
	ARG_0
};

static const ETextEventProcessorCommand control_keys[26] =
{
	{ E_TEP_START_OF_LINE,      E_TEP_MOVE, 0, "" }, /* a */
	{ E_TEP_BACKWARD_CHARACTER, E_TEP_MOVE, 0, "" }, /* b */
	{ E_TEP_SELECTION,          E_TEP_COPY, 0, "" }, /* c */
	{ E_TEP_FORWARD_CHARACTER,  E_TEP_DELETE, 0, "" }, /* d */
	{ E_TEP_END_OF_LINE,        E_TEP_MOVE, 0, "" }, /* e */
	{ E_TEP_FORWARD_CHARACTER,  E_TEP_MOVE, 0, "" }, /* f */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* g */
	{ E_TEP_BACKWARD_CHARACTER, E_TEP_DELETE, 0, "" }, /* h */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* i */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* j */
	{ E_TEP_END_OF_LINE,        E_TEP_DELETE, 0, "" }, /* k */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* l */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* m */
	{ E_TEP_FORWARD_LINE,       E_TEP_MOVE, 0, "" }, /* n */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* o */
	{ E_TEP_BACKWARD_LINE,      E_TEP_MOVE, 0, "" }, /* p */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* q */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* r */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* s */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* t */
	{ E_TEP_START_OF_LINE,      E_TEP_DELETE, 0, "" }, /* u */
	{ E_TEP_SELECTION,          E_TEP_PASTE, 0, "" }, /* v */
	{ E_TEP_SELECTION,          E_TEP_DELETE, 0, "" }, /* w */
	{ E_TEP_SELECTION,          E_TEP_DELETE, 0, "" }, /* x */
	{ E_TEP_SELECTION,          E_TEP_PASTE, 0, "" }, /* y */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" }           /* z */
};

static const ETextEventProcessorCommand alt_keys[26] =
{
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* a */
	{ E_TEP_BACKWARD_WORD,      E_TEP_MOVE, 0, "" }, /* b */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* c */
	{ E_TEP_FORWARD_WORD,       E_TEP_DELETE, 0, "" }, /* d */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* e */
	{ E_TEP_FORWARD_WORD,       E_TEP_MOVE, 0, "" }, /* f */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* g */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* h */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* i */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* j */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* k */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* l */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* m */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* n */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* o */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* p */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* q */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* r */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* s */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* t */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* u */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* v */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* w */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* x */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" },           /* y */
	{ E_TEP_SELECTION, E_TEP_NOP, 0, "" }           /* z */

};

GtkType
e_text_event_processor_emacs_like_get_type (void)
{
  static GtkType text_event_processor_emacs_like_type = 0;

  if (!text_event_processor_emacs_like_type)
    {
      static const GtkTypeInfo text_event_processor_emacs_like_info =
      {
        "ETextEventProcessorEmacsLike",
        sizeof (ETextEventProcessorEmacsLike),
        sizeof (ETextEventProcessorEmacsLikeClass),
        (GtkClassInitFunc) e_text_event_processor_emacs_like_class_init,
        (GtkObjectInitFunc) e_text_event_processor_emacs_like_init,
        /* reserved_1 */ NULL,
        /* reserved_2 */ NULL,
        (GtkClassInitFunc) NULL,
      };

      text_event_processor_emacs_like_type = gtk_type_unique (e_text_event_processor_get_type (), &text_event_processor_emacs_like_info);
    }

  return text_event_processor_emacs_like_type;
}

static void
e_text_event_processor_emacs_like_class_init (ETextEventProcessorEmacsLikeClass *klass)
{
  GtkObjectClass *object_class;
  ETextEventProcessorClass *processor_class;

  object_class = (GtkObjectClass*) klass;
  processor_class = (ETextEventProcessorClass*) klass;

  parent_class = gtk_type_class (e_text_event_processor_get_type ());

  processor_class->event = e_text_event_processor_emacs_like_event;
}

static void
e_text_event_processor_emacs_like_init (ETextEventProcessorEmacsLike *tep)
{
}

static gint
e_text_event_processor_emacs_like_event (ETextEventProcessor *tep, ETextEventProcessorEvent *event)
{
	ETextEventProcessorCommand command;
	ETextEventProcessorEmacsLike *tep_el = E_TEXT_EVENT_PROCESSOR_EMACS_LIKE(tep);
	command.action = E_TEP_NOP;
	switch (event->type) {
	case GDK_BUTTON_PRESS:
		if (event->button.button == 1) {
			command.action = E_TEP_GRAB;
			command.time = event->button.time;
			gtk_signal_emit_by_name (GTK_OBJECT (tep), "command", &command);
			if (event->button.state & GDK_SHIFT_MASK)
				command.action = E_TEP_SELECT;
			else
				command.action = E_TEP_MOVE;
			command.position = E_TEP_VALUE;
			command.value = event->button.position;
			command.time = event->button.time;
			tep_el->mouse_down = TRUE;
		}
		break;
	case GDK_2BUTTON_PRESS:
		if (event->button.button == 1) {
			command.action = E_TEP_SELECT;
			command.position = E_TEP_SELECT_WORD;
			command.time = event->button.time;
		}
		break;
	case GDK_3BUTTON_PRESS:
		if (event->button.button == 1) {
			command.action = E_TEP_SELECT;
			command.position = E_TEP_SELECT_ALL;
			command.time = event->button.time;
		}
		break;
	case GDK_BUTTON_RELEASE:
		if (event->button.button == 1) {
			command.action = E_TEP_UNGRAB;
			command.time = event->button.time;
			gtk_signal_emit_by_name (GTK_OBJECT (tep), "command", &command);
			command.time = event->button.time;
			tep_el->mouse_down = FALSE;
			command.action = E_TEP_NOP;
		} else if (event->button.button == 2) {
			command.action = E_TEP_MOVE;
			command.position = E_TEP_VALUE;
			command.value = event->button.position;
			command.time = event->button.time;
			gtk_signal_emit_by_name (GTK_OBJECT (tep), "command", &command);			

			command.action = E_TEP_GET_SELECTION;
			command.position = E_TEP_SELECTION;
			command.value = 0;
			command.time = event->button.time;
		}
		break;
	case GDK_MOTION_NOTIFY:
		if (tep_el->mouse_down) {
			command.action = E_TEP_SELECT;
			command.position = E_TEP_VALUE;
			command.time = event->motion.time;
			command.value = event->motion.position;
		}
		break;
	case GDK_KEY_PRESS: 
		{
			ETextEventProcessorEventKey key = event->key;
			command.time = event->key.time;
			if (key.state & GDK_SHIFT_MASK)
				command.action = E_TEP_SELECT;
			else
				command.action = E_TEP_MOVE;
			switch(key.keyval) {
			case GDK_Home:
				if (key.state & GDK_CONTROL_MASK)
					command.position = E_TEP_START_OF_BUFFER;
				else
					command.position = E_TEP_START_OF_LINE;
				break;
			case GDK_End:
				if (key.state & GDK_CONTROL_MASK)
					command.position = E_TEP_END_OF_BUFFER;
				else
					command.position = E_TEP_END_OF_LINE;
				break;
			case GDK_Page_Up: command.position = E_TEP_BACKWARD_PAGE; break;
			case GDK_Page_Down: command.position = E_TEP_FORWARD_PAGE; break;
				/* CUA has Ctrl-Up/Ctrl-Down as paragraph up down */
			case GDK_Up:        command.position = E_TEP_BACKWARD_LINE; break;
			case GDK_Down:      command.position = E_TEP_FORWARD_LINE; break;
			case GDK_Left:
				if (key.state & GDK_CONTROL_MASK)
					command.position = E_TEP_BACKWARD_WORD;
				else
					command.position = E_TEP_BACKWARD_CHARACTER;
				break;
			case GDK_Right:     
				if (key.state & GDK_CONTROL_MASK)
					command.position = E_TEP_FORWARD_WORD;
				else
					command.position = E_TEP_FORWARD_CHARACTER;
				break;
	  
			case GDK_BackSpace:
				command.action = E_TEP_DELETE;
				if (key.state & GDK_CONTROL_MASK)
					command.position = E_TEP_BACKWARD_WORD;
				else
					command.position = E_TEP_BACKWARD_CHARACTER;
				break;
			case GDK_Clear:
				command.action = E_TEP_DELETE;
				command.position = E_TEP_END_OF_LINE;
				break;
			case GDK_Insert:
				if (key.state & GDK_SHIFT_MASK) {
					command.action = E_TEP_PASTE;
					command.position = E_TEP_SELECTION;
				} else if (key.state & GDK_CONTROL_MASK) {
					command.action = E_TEP_COPY;
					command.position = E_TEP_SELECTION;
				} else {
				/* gtk_toggle_insert(text) -- IMPLEMENT */
				}
				break;
			case GDK_Delete:
				if (key.state & GDK_CONTROL_MASK){
					command.action = E_TEP_DELETE;
					command.position = E_TEP_FORWARD_WORD;
				} else if (key.state & GDK_SHIFT_MASK) {
					command.action = E_TEP_COPY;
					command.position = E_TEP_SELECTION;
					gtk_signal_emit_by_name (GTK_OBJECT (tep), "command", &command);
				
					command.action = E_TEP_DELETE;
					command.position = E_TEP_SELECTION;
				} else {
					command.action = E_TEP_DELETE;
					command.position = E_TEP_FORWARD_CHARACTER;
				}
				break;
			case GDK_Tab:
				/* Don't insert literally */
				command.action = E_TEP_NOP;
				command.position = E_TEP_SELECTION;
				break;
			case GDK_Return:
				if (key.state & GDK_CONTROL_MASK) {
					command.action = E_TEP_ACTIVATE;
					command.position = E_TEP_SELECTION;
				} else {
					command.action = E_TEP_INSERT;
					command.position = E_TEP_SELECTION;
					command.value = 1;
					command.string = "\n";
				}
				break;
			case GDK_Escape:
				/* Don't insert literally */
				command.action = E_TEP_NOP;
				command.position = E_TEP_SELECTION;
				break;
	  
			default:
				if (key.state & GDK_CONTROL_MASK) {
					if ((key.keyval >= 'A') && (key.keyval <= 'Z'))
						key.keyval -= 'A' - 'a';
					
					if ((key.keyval >= 'a') && (key.keyval <= 'z')) {
						command.position = control_keys[(int) (key.keyval - 'a')].position;
						if (control_keys[(int) (key.keyval - 'a')].action != E_TEP_MOVE)
							command.action = control_keys[(int) (key.keyval - 'a')].action;
						command.value = control_keys[(int) (key.keyval - 'a')].value;
						command.string = control_keys[(int) (key.keyval - 'a')].string;
					}

					if (key.keyval == 'x' || key.keyval == 'w') {
						command.action = E_TEP_COPY;
						command.position = E_TEP_SELECTION;
						gtk_signal_emit_by_name (GTK_OBJECT (tep), "command", &command);
						
						command.action = E_TEP_DELETE;
						command.position = E_TEP_SELECTION;
					}
					
					break;
				} else if (key.state & GDK_MOD1_MASK) {
					if ((key.keyval >= 'A') && (key.keyval <= 'Z'))
						key.keyval -= 'A' - 'a';
					
					if ((key.keyval >= 'a') && (key.keyval <= 'z')) {
						command.position = alt_keys[(int) (key.keyval - 'a')].position;
						if (alt_keys[(int) (key.keyval - 'a')].action != E_TEP_MOVE)
							command.action = alt_keys[(int) (key.keyval - 'a')].action;
						command.value = alt_keys[(int) (key.keyval - 'a')].value;
						command.string = alt_keys[(int) (key.keyval - 'a')].string;
					}
				} else if (key.length > 0) {
					command.action = E_TEP_INSERT;
					command.position = E_TEP_SELECTION;
					command.value = strlen(key.string);
					command.string = key.string;
					
				} else {
					command.action = E_TEP_NOP;
				}
			}
			break;
		case GDK_KEY_RELEASE:
			command.time = event->key.time;
			command.action = E_TEP_NOP;
			break;
		default:
			command.action = E_TEP_NOP;
			break;
		}
	}
	if (command.action != E_TEP_NOP) {
		gtk_signal_emit_by_name (GTK_OBJECT (tep), "command", &command);
		return 1;
	}
	else
		return 0;
}

ETextEventProcessor *
e_text_event_processor_emacs_like_new (void)
{
	ETextEventProcessorEmacsLike *retval = gtk_type_new (e_text_event_processor_emacs_like_get_type ());
	return E_TEXT_EVENT_PROCESSOR (retval);
}

