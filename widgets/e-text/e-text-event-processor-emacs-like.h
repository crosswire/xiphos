/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* e-text-event-processor-emacs-like.h
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
#ifndef __E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_H__
#define __E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_H__

#include <gnome.h>
#include "e-text-event-processor.h"

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

/* ETextEventProcessorEmacsLike - Turns events on a text widget into commands.  Uses an emacs-ish interface.
 *
 */

#define E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_TYPE			(e_text_event_processor_emacs_like_get_type ())
#define E_TEXT_EVENT_PROCESSOR_EMACS_LIKE(obj)			(GTK_CHECK_CAST ((obj), E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_TYPE, ETextEventProcessorEmacsLike))
#define E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_CLASS(klass)		(GTK_CHECK_CLASS_CAST ((klass), E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_TYPE, ETextEventProcessorEmacsLikeClass))
#define E_IS_TEXT_EVENT_PROCESSOR_EMACS_LIKE(obj)		        (GTK_CHECK_TYPE ((obj), E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_TYPE))
#define E_IS_TEXT_EVENT_PROCESSOR_EMACS_LIKE_CLASS(klass)	        (GTK_CHECK_CLASS_TYPE ((obj), E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_TYPE))


typedef struct _ETextEventProcessorEmacsLike       ETextEventProcessorEmacsLike;
typedef struct _ETextEventProcessorEmacsLikeClass  ETextEventProcessorEmacsLikeClass;

struct _ETextEventProcessorEmacsLike
{
	ETextEventProcessor parent;
	
	/* object specific fields */
	gboolean mouse_down;
};

struct _ETextEventProcessorEmacsLikeClass
{
	ETextEventProcessorClass parent_class;
};


GtkType    e_text_event_processor_emacs_like_get_type (void);
ETextEventProcessor *e_text_event_processor_emacs_like_new (void);

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __E_TEXT_EVENT_PROCESSOR_EMACS_LIKE_H__ */
