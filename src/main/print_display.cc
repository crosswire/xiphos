 /*
 * GnomeSword Bible Study Tool
 * sword_print.cc - glue 
 *
 * Copyright (C) 2007 GnomeSword Developer Team
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
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <swmgr.h>
#include <swmodule.h>
#include <versekey.h>
#include <gbfosis.h>
#include <thmlosis.h>

#include <regex.h>
#include <string.h>

#include <sys/socket.h>
#include <netinet/in.h>
#include <errno.h>


#include "gecko/gecko-html.h"

#include "main/print_display.h"
#include "main/settings.h"
#include "main/global_ops.hh"
#include "main/sword.h"

#include "gui/utilities.h"
#include "gui/widgets.h"

#include "backend/sword_main.hh"
#include "backend/gs_osishtmlhref.h"


#define HTML_START "<html><head><meta http-equiv=\"content-type\" content=\"text/html; charset=utf-8\"><STYLE type=\"text/css\"><!--A { text-decoration:none }%s--></STYLE></head>"
#define DOUBLE_SPACE " * { line-height: 2em ! important; }"

using namespace sword;
using namespace std;

int strongs_on;
