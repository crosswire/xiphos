/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8 -*- */
/* e-html-utils.c */
/* 
 * Copyright (C) 2000  Helix Code, Inc.
 * Author: Dan Winship <danw@helixcode.com>
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

#ifndef __E_HTML_UTILS__
#define __E_HTML_UTILS__

#define E_TEXT_TO_HTML_PRE            (1 << 0)
#define E_TEXT_TO_HTML_CONVERT_NL     (1 << 1)
#define E_TEXT_TO_HTML_CONVERT_SPACES (1 << 2)
#define E_TEXT_TO_HTML_CONVERT_URLS   (1 << 3)

char *e_text_to_html (const char *input, unsigned int flags);

#endif /* __E_HTML_UTILS__ */
