/* vi: set sw=4 ts=4 wrap ai: */
/*
 * Copyright (C) 2019 Wu Xiaotian <yetist@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 * */

#include <stdlib.h>
#include <stdio.h>
#include "zip.h"

int main (int argc, char **argv)
{

    system("echo 2-1 > foo-2.1.txt");
    system("echo 2-2 > foo-2.2.txt");
    system("echo 2-3 > foo-2.3.txt");

    struct zip_t *zip = zip_open("foo.zip", ZIP_DEFAULT_COMPRESSION_LEVEL, 'w');

    zip_entry_open(zip, "foo-1.txt");
    const char *buf = "Some data here...\0";
    zip_entry_write(zip, buf, strlen(buf));
    zip_entry_close(zip);

    zip_entry_open(zip, "mods.d/foo-2.txt");
    // merge 3 files into one entry and compress them on-the-fly.
    zip_entry_fwrite(zip, "foo-2.1.txt");
    zip_entry_fwrite(zip, "foo-2.2.txt");
    zip_entry_fwrite(zip, "foo-2.3.txt");
    zip_entry_close(zip);

    zip_close(zip);

    printf("foo.zip is created.\n");

    return 0;
}
