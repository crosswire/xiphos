/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* e-text-event-processor.h
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

/*
 * Modified by the GTK+ Team and others 1997-1999.  See the AUTHORS
 * file for a list of people on the GTK+ Team.  See the ChangeLog
 * files for a list of changes.  These files are distributed with
 * GTK+ at ftp://ftp.gtk.org/pub/gtk/. 
 */

#ifndef __E_TEXT_EVENT_PROCESSOR_TYPES_H__
#define __E_TEXT_EVENT_PROCESSOR_TYPES_H__

#ifdef __cplusplus
extern "C" {
#pragma }
#endif /* __cplusplus */

#include <gdk/gdktypes.h>

typedef enum _ETextEventProcessorCommandPosition ETextEventProcessorCommandPosition;
typedef enum _ETextEventProcessorCommandAction ETextEventProcessorCommandAction;
typedef struct _ETextEventProcessorCommand ETextEventProcessorCommand;

typedef union _ETextEventProcessorEvent ETextEventProcessorEvent;
typedef struct _ETextEventProcessorEventButton ETextEventProcessorEventButton;
typedef struct _ETextEventProcessorEventKey ETextEventProcessorEventKey;
typedef struct _ETextEventProcessorEventMotion ETextEventProcessorEventMotion;

enum _ETextEventProcessorCommandPosition {
	E_TEP_VALUE,
	E_TEP_SELECTION,

	E_TEP_START_OF_BUFFER,
	E_TEP_END_OF_BUFFER,

	E_TEP_START_OF_LINE,
	E_TEP_END_OF_LINE,

	E_TEP_FORWARD_CHARACTER,
	E_TEP_BACKWARD_CHARACTER,

	E_TEP_FORWARD_WORD,
	E_TEP_BACKWARD_WORD,

	E_TEP_FORWARD_LINE,
	E_TEP_BACKWARD_LINE,

	E_TEP_FORWARD_PARAGRAPH,
	E_TEP_BACKWARD_PARAGRAPH,

	E_TEP_FORWARD_PAGE,
	E_TEP_BACKWARD_PAGE,

	E_TEP_SELECT_WORD,
	E_TEP_SELECT_ALL

};

enum _ETextEventProcessorCommandAction {
	E_TEP_MOVE,
	E_TEP_SELECT,
	E_TEP_DELETE,
	E_TEP_INSERT,

	E_TEP_COPY,
	E_TEP_PASTE,
	E_TEP_GET_SELECTION,
	E_TEP_SET_SELECT_BY_WORD,
	E_TEP_ACTIVATE,
	
	E_TEP_GRAB,
	E_TEP_UNGRAB,

	E_TEP_NOP
};

struct _ETextEventProcessorCommand {
	ETextEventProcessorCommandPosition position;
	ETextEventProcessorCommandAction action;
	int value;
	char *string;
	guint32 time;
};

struct _ETextEventProcessorEventButton {
	GdkEventType type;
	guint32 time;
	guint state;
	guint button;
	gint position;
};

struct _ETextEventProcessorEventKey {
	GdkEventType type;
	guint32 time;
	guint state;
	guint keyval;
	gint length;
	gchar *string;
};

struct _ETextEventProcessorEventMotion {
	GdkEventType type;
	guint32 time;
	guint state;
	gint position;
};

union _ETextEventProcessorEvent {
	GdkEventType type;
	ETextEventProcessorEventButton button;
	ETextEventProcessorEventKey key;
	ETextEventProcessorEventMotion motion;
};

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif /* __E_TEXT_EVENT_PROCESSOR_TYPES_H__ */
