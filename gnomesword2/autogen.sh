#!/bin/sh
# Run this to generate all the initial makefiles, etc.

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

if test -z "$GNOME2_DIR" ; then
  GNOME_COMMON_DATADIR="/usr/share"
else
  GNOME_COMMON_DATADIR="$GNOME2_DIR/share"
fi

GNOME_COMMON_MACROS_DIR="macros"

export GNOME_COMMON_DATADIR
export GNOME_COMMON_MACROS_DIR

ACLOCAL_FLAGS="-I $GNOME_COMMON_MACROS_DIR $ACLOCAL_FLAGS"
export ACLOCAL_FLAGS

PKG_NAME="the package"

. $srcdir/$GNOME_COMMON_MACROS_DIR/autogen.sh
