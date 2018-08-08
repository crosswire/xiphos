Xiphos i18n
===========

Adding a new translation:
-------------------------

1. Add the Language name (LL) to po/LINGUAS
2. Place the PO Language file (LL.po) into po/


Updating xiphos.pot:
--------------------

For updating the Portable Object Template (xiphos.pot), this is the file that
you get when you extract texts from Xiphos sources, go to your CMake build
folder and run:

    make xiphos_pot

Updating a specific PO file:
----------------------------

For merging new strings into the Portable Object File,  this is the file that
you receive back from the translators, a file that includes the original texts
and the translations, go to your CMake build folder and run:

    make xiphos_<LANG>.po

Updating all PO files:
----------------------

For merging new strings in all PO files, go to your CMake build folder and run:

    make xiphos_po


Xiphos Manual i18n
==================

Mallard documents are translated with itstool and portable object (PO) files,
which are the standard way of translating messages on Unix-like systems.


1. Adding an new language:
--------------------------

For adding a new language, add its language code to HELP_LINGUAS in your Help
directory, the language code is the ISO 639-1 standard two-letter code of the
language.

For a list of language codes, see:
https://www.gnu.org/software/gettext/manual/html_node/Usual-Language-Codes.html

Then, in in your build directory, run:

    $ make mhelp

This will create the corresponding <LANG> folder and the <LANG>.po file into
this folder.

2. Updating the Portable Object Template (xiphos.pot):
------------------------------------------------------

go to your CMake build folder and run:

    make mhelp_pot

This takes the list of Mallard pages, extracts the translatable strings and
merges them into a PO template (POT) file which will be named xiphos.pot.
The POT file contains a list of translatable strings combined with information
about the location of the string in files and any comments which the help author
may have added.


3. Updating a specific PO File:
------------------------------

When a new version of the help file is available, it's easy to update the po
file.

In your build directory, run

    make mhelp_<LANG>.po

The POT file is then merged in the current PO file.


3. Updating all PO Files:
-------------------------

go to your CMake build folder and run:

    make mhelp_po

