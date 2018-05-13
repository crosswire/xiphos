
HOWTO: Translating Xiphos
=========================

Introduction
------------
Xiphos is written with the intent of being localisable. so that the language
the user sees from the program may be the user's native language.

Translation is a major part of this localisation process. Translations are
performed by native speakers on a volunteer basis. They take sentences in the
original English, supply the appropriate translation, and add the file
containing this information to the Xiphos Git repository or send the file by
email on xiphos-devel list so that the next release of the software contains
the new language.

This document tries to explain how to get involved in the translation process
for Xiphos.

Welcome new Translator!
-----------------------
We welcome new translations, but please be aware that translations are not just
one time jobs. They have to be maintained in order to be useful. Otherwise they
quickly get outdated and become obsolete, and useless. That said, we would be
happy if you could maintain a new translation.

For a time committment, translating the interface for the first time will take
approximately a weekend. After that, it takes approximately two hours every
three months to keep it updated. Xiphos Manual translations are more work than
the interface.

Skills Needed
-------------
Translating the interface at minimum simply requires the knowledge of the
language you are working on plus English. In addition, you will need to learn
to use a small simple program to edit the translation. More advanced users are
encouraged to use Git and other tools to keep up with the translation work.


The implementation of GNU gettext in Xiphos
===========================================
Here is a short summary of the translation process, as it is implemented in
Xiphos.

*GNU gettext* is an internationalization and localization (i18n) system commonly
used for writing multilingual programs.


1. Strings in source files needing translation
----------------------------------------------
It's up to the programmer to identify and especially mark all constant strings
in the program sources needing translation.
Normally, strings in C code just need to be enclosed with the `_( )` function.
For example, in this excerpt from src/gnome2/main_window.c, the string
`Xiphos - Bible Study Software` is tagged as translatable:

    // The toplevel Xiphos window
    widgets.app = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(widgets.app), _("Xiphos - Bible Study Software"));
    <....>

2. The PO Template File (xiphos.pot)
------------------------------------
Once the sources have been modified, the *xgettext* program is used to find and
extract all translatable strings, and create a PO template file out of all
these. This xiphos.pot file contains all original program strings. It has sets
of pointers to exactly where in sources each string is used.

All translations are set to empty. The letter `t` in .pot marks this as a Template
PO file, not yet oriented towards any particular language.

You will find the `xiphos.pot` file in the po/ directory and should look like this:

    # SOME DESCRIPTIVE TITLE.
    # Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER
    # This file is distributed under the same license as the PACKAGE package.
    # FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.
    #
    #, fuzzy
    msgid ""
    msgstr ""
    "Project-Id-Version: PACKAGE VERSION\n"
    "Report-Msgid-Bugs-To: \n"
    "POT-Creation-Date: 2017-11-13 22:53+0100\n"
    "PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
    "Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
    "Language-Team: LANGUAGE <LL@li.org>\n"
    "Language: \n"
    "MIME-Version: 1.0\n"
    "Content-Type: text/plain; charset=UTF-8\n"
    "Content-Transfer-Encoding: 8bit\n"

    #: ../src/backend/sword_main.cc:1149
    msgid "Configuration not found"
	msgstr ""

3. PO files (in po/)
--------------------
PO files are the files which contain the actual translations. Each language will
have its own PO file, for example, for French there would be a `fr.po` file, for
German there would be a `de.po`, for American English there might be `en-US.po`.

The core of each PO file is very simple; it is barely a copy of the POT file
made of simply pairs of the source text - that which is found in the code - and
target text, the text which is wanted in the translation.

For example, in the French PO file you might have a translation of a string as
follows:

    #: ../src/backend/sword_main.cc:1149
	msgid "Configuration not found"
	msgstr "Configuration non trouvée"

The msgid contains the text actually in the code, and the msgstr contains the
text into which it is translated. If there is no translation given in the
msgstr, then the default msgid string will be displayed.

PO files are simply text files and can thus be edited by any text editor, but
there are also many tools available to make editing them easier.

You will find the PO files in the po/ directory as well.

4. LINGUAS file (in po/)
------------------------
The po/ directory also receive a file named `LINGUAS`. This file contains the list
of available translations in Xiphos. It is a whitespace separated list.
Hash-marked comments and white lines are ignored.

5. MO files
-----------
Once the PO file is complete and dependable, the *msgfmt* program from *gettext*
utilities is used for turning the PO file into a machine-oriented format, which
may yield efficient retrieval of translations by Xiphos, at runtime.

You'll find the MO files in `usr/share/locale/{language-code}}/LC_MESSAGES`

6. Summary
----------
The full process starting from the source file `.c` to the `.mo` file is the
following (along with the *gettext* tools used to perform the extracting and
the compiling tasks):

              xgettext                 msgmerge            msgfmt
    source.c ---------> po/xiphos.pot ---------> po/fr.po -------> /usr...LC_MESSAGES/xiphos.mo



Language codes
==============
To begin with, you will need to find out what your language code is. This is a
two- or three-letter code. More popular languages will tend to have two-letter
codes, whereas secondilarily languages will tend to get a three-letter one. For
languages spoken in more than one country, a translation specific to a country
will be followed by an underscore and the two-letter country code capitalized.
This can further be appended with an at-sign and more qualifying information.
For now, know that language codes typically look like `fr` (French) or `en_GB`
(English as used in Great Britain).

If you do not know what the code for your language is, try looking around to see
if there are translation efforts for other projects (such as KDE, Mozilla and
OpenOffice) in your language — these will (should) use the same code.
For a list of language codes, see also:
https://www.gnu.org/software/gettext/manual/html_node/Usual-Language-Codes.html

Xiphos uses standard codes as defined in ISO standard 639 (for language codes)
and ISO standard 3166 (for country codes).

Your language code is used to identify your localisation, for example in the
names of locales and files. Once you have your code, you can start translating.

Note:
-----
If you plan to work on a new translation and you want to submit it directly to
Xiphos, read on.

If it's an existing translation please try to contact the last translator first
using the contact information in the corresponding PO file.


HOWTO simply add a new translation
==================================
This is the fastest way to improve and fix translations. And we of course accept
changes done this way. However there is a better way too, by using *Git* and
working on a local copy of the whole source tree, but it is more work.

Synopsys
--------
The easiest and fastest way to add a new translation to Xiphos is to download
the xiphos.pot from GitHub to your computer, rename it to a PO file matching
your locale.

The PO files are text files so most editor programs can be used. However, it
is *highly* recomended to use a dedicated tool, as things can quickly be messed
up with a regular text editor.

Edit the file, save it to MO file and start Xiphos. The changes should be
visible in Xiphos.

What you will need to start
---------------------------

### A glossary
It is important that your translation consistently uses the same terms to avoid
confusing users. A standard glossary will help you ensure that you do not refer
to one thing by many names.

Try not to invent new terms but to reuse existing ones — look for a governing
body for your language, or translations used in other projects, or terminology
used in Microsoft Windows or Apple OS X. In particular, it's preferable to be
consistent with other SWORD Frontends such as Bibletime, AndBible.

If you do have to create new terminology, or decide to use one word rather than
another, do document your choice (and the reasoning behind it) in your glossary.
Write your glossary at the beginning of your translation (.po) file. This saves
confusion later.

Expect to change some of your initial glossary suggestions after some time
translating. It's only once you start translating that you start to notice
problems.

### The Gettext tools
*gettext* is a package which contains tools for creating translatable strings,
turning those strings into a list which can be translated, updating these
strings, and converting them into a format the computer can use.

The  *gettext* package is installed by default on many Linux distributions.
Where it is not installed, it is still usually available. You also need to
install *intltool*. This is a front-end to gettext and is needed by Xiphos.

### Editors and editor tricks
A number of editors have special modes for editing .po files, offering a
specific syntax highlighting for .po filesand or droppin you into the
appropriate mode automatically.

For those of you already using [Emacs](https://www.gnu.org/software/emacs/),
PO mode has a lot to offer, including the validation of PO files.

There is a plugin for editing .po files in [Vim](http://www.vim.org/).
It is available on the Vim website as, unsurprisingly, po.vim.

Other Recommended dedicated PO file editors are (in no particular order):
- Lokalize - https://userbase.kde.org/Lokalize/
- poEdit - https://poedit.net/
- OmegaT (Java) - http://omegat.org/en/


Start editing the PO File
-------------------------
Adjust the header and special strings of your PO File.

The top of the `.po` file starts by comments and should be edited somewhat.

    # SOME DESCRIPTIVE TITLE.
    # Copyright (C) YEAR THE PACKAGE'S COPYRIGHT HOLDER
    # This file is distributed under the same license as the PACKAGE package.
    # FIRST AUTHOR <EMAIL@ADDRESS>, YEAR.

The comments should be changed to be current, Example from French:

    # Traduction en français de Xiphos
    # Copyright (C) 2002, 2017 The Xiphos Development Team
    # Ce fichier est distribué sous la même license que Xiphos

and change the "FIRST AUTHOR" string to your name

The next section is a special entry and should be edited also.

    #, fuzzy
    msgid ""
    msgstr ""
    "Project-Id-Version: PACKAGE VERSION\n"
    "Report-Msgid-Bugs-To: \n"
    "POT-Creation-Date: 2017-11-13 22:59+0100\n"
    "PO-Revision-Date: YEAR-MO-DA HO:MI+ZONE\n"
    "Last-Translator: FULL NAME <EMAIL@ADDRESS>\n"
    "Language-Team: LANGUAGE <LL@li.org>\n"
    "Language: \n"
    "MIME-Version: 1.0\n"
    "Content-Type: text/plain; charset=UTF-8\n"
    "Content-Transfer-Encoding: 8bit\n"

The lines as shown in the previous section will contain several capitalized
entries. Replace all the words in capitals with something appropriate.

In this case:

	Project-Id-Version: Xiphos
	Report-Msgid-Bugs-To : https://github.com/crosswire/xiphos/issues
	Last-Translator: you will be the first author of the translation, and also
	                 the last translator of it.
	Language-Team: none, unless you are a member of a translation team
    Language: Language Code of your language

Remove the `#, fuzzy` line once you have specified the items in capitals,
because once this is done the header entry is no longer fuzzy.

For more information about the PO Header, see:
https://www.gnu.org/software/gettext/manual/html_node/Header-Entry.html#Header-Entry

Translating the .po file
------------------------
Finally. You are ready to do some translating!

A record in a po file has the following form:

    <empty or only white-space>
    #  translator-comments
	#. extracted-comments
    #: reference…
    #, flag…
    #| msgid previous-untranslated-string
    msgid untranslated-string
    msgstr translated-string

- The *empty or only white-space* line is the record separator.
- In `translator-comments` you can put your own notes.
- The `extracted-comments` are notes from the programmers for you.
- One ore more `references` tell you, where the message appears in the sources.
- The most important `flags` will be explained below.
- The empty `msgstr` string has to be filled with the translation for the string
shown after `msgid`.

Example
    Here is an example of translating some text into Spanish:

Before:

    #: ../src/backend/sword_main.cc:1149
    msgid "Configuration not found"
    msgstr ""

After, the translation in the `es.po` file:

    #: ../src/backend/sword_main.cc:1149
    msgid "Configuration not found"
    msgstr "Configuración no encontrada"

If you translate a string that has the flag `#, fuzzy`, remove the word fuzzy.
A fuzzy translation means that there is no translation. The computer took some
guess about what the translation might be, but as long as it is marked
`#, fuzzy`, this guess will simply be ignored and this string will stay
untranslated.

You just produce a translation for all entries in the PO file, one after
another, respecting the overall file format and the quoting needed for special
characters, when needed.

**You must never edit the original string, the msgid.**

Special characters and other tips
---------------------------------

Depending on the context a few characters have a special meaning and need some
special treatment:

### "_" (underline)

In menu and dialog entries the following character will become the accelerator
or hotkey, which can be used together with a superkey [ctrl] or [alt] to jump to
the entry. More specific under Linux you reach a main menu entry with
[alt]+[key] and its submenus and other menu entries with [key]. In dialogs
always use [alt]+[key]. So the key should be unique in its context.

wrong:

    "do _this" # Hotkey: t
    "do _that" # Hotkey: t => unreachable

right:

    "do th_is" # Hotkey: i
    "do th_at" # Hotkey: a

Characters to avoid:

* Already used on buttons like in English: `Close`, `Help`. Others depend on the
context.
* Characters breaking the baseline like in latin script: `j`,`p`,`q`,`y`.
At least in some fonts the underline becomes invisible - leaving the user
clueless.


### "\" (backslash)

It is the escape character in many programming languages. The following
character has a special meaning like e.g.:

    "\n" (New line)

The most often used special character in our strings. If msgid contains "\n"
keep the layout and add them to msgstr too - at the same places.

    Use "\\" to print a backslash.


### "%" (percent)

In C based programming languages "%" marks the beginning of a format specifier,
e.g. `%d6` means insert the next variable here in decimal format with 6 digits.

If no real format specifiers are in the string, file a bugreport and tell the
developers the location of the string (the lines above the msgid), where they
should insert a comment containing xgettext:no-c-format before the gettext call.

To continue with your work just remove the c-format flag from the #, comment
line above. If no other flags are in the line, simply remove the line.

To output `%` if a string contains format specifiers, use `%%` in your string.


### #Reordering parameters

Assume a string `In %d cases the result will be %d.`, but in your language you
would prefer to write `%d will be the result in %d cases.` Now you would get
the wrong numbers.

Solution

Insert the ordinal number of the parameter, followed by `$` in the format
specifier: `%2$d will be the result in %1$d cases.`.


Check syntax and statistics of your .po file
--------------------------------------------
When your translation work is done (or at any other time), you can run a check
on common mistakes, using this command:

    msgfmt -cv --statistics LL.po

If that program reports one or more fatal errors for your language, you should
review the criticized lines of your file.

In a second run you might wish to see, where you forgot to add an accelerator:

    msgfmt -cv --check-accelerators="_" --statistics LL.po


Run Xiphos with your new translation
------------------------------------
Once you have a valid PO file, you can compile the translation (you will
probably have to be root to do this) and test the results in your current
Xiphos install:

    msgfmt -o /usr/share/locales/YOUR-LANG/LC_MESSAGES/xiphos.mo YOUR-LANG.po

Be sure there are no duplicate accelerators.


HOWTO use Git for Xiphos translators
====================================

What you will need:
-------------------
NOTE: Run all install commands as the `root` user or by using `sudo`.


### 1. C, C++ and Development Tools
On RHEL/CentOS/Fedora, install the Development Tools:

    dnf group install 'Development Tools'

On Debian/Ubuntu, install the Build-Essential package:

    apt install build-essential


### 2. Xiphos dependencies and build dependencies
You need to install the following dependencies:

    biblesync			Protocol to support Bible software shared co-navigation
    dbus-glib		    API for use of D-Bus from GLib applications
    gconfmm		        C++ wrappers for GConf
    gtk+-3.0            The GIMP Toolkit
    gtkhtml	            Lightweight HTML rendering/editing engine
    libgsf		        The G Structured File Library
    libuuid		        Universally unique identifier library
    libxml2             XML C parser and toolkit
    sword >= 1.8.0	    CrossWire Bible Society's Bible software
    webkit1 or webkit2  Port to Gtk+ of the WebKit rendering engine

On RHEL/CentOS/Fedora, run:

    dnf install intltool gtk3-devel dbus-glib-devel gtkhtml3-devel
	webkitgtk4-devel libxml2-devel libgsf-devel gconfmm26-devel sword-devel
	libuuid-devel biblesync-devel

On Debian/Ubuntu:

    apt install intltool libdbus-glib-1-dev libwebkitgtk-3.0-dev
	libxml2-dev libgsf-1-dev libgconfmm-2.6-dev libsword-dev uuid-dev
    libwebkitgtk-dev libglade2-dev

You'll also need to install the following utilities:

    intltool
    appstream-util
    desktop-file-validate
    itstool
    xmllint
    yelp-build


### 3. CMake
CMake is cross-platform free and open-source software for managing the build
process of software.

For installing cmake on RHEL/CentOS/Fedora:

	dnf install cmake

On Debian/Ubuntu:

	apt install git


### 4. Git
*git* is a version control system for coordinating work on source files among
multiple people. *git* was originally developed by Linus Torvalds for managing
Linux source code without requiring a central server.

*git* allow each developer/translator to clone a copy of the repository of
the source code from GitHub and then to work on their own personal clone
separately from other developer/translator.

With *git* everybody has all the tools available and can do whatever they want
to do with their local copy. Moreover, almost all operations are local, so the
operations are fast, and can be done without connection to the original
repository.

If you know nothing about version control, read [Getting Started - About Version
Control](http://git-scm.com/book/en/v2/Getting-Started-About-Version-Control).

Another good place to start is [Good Resources for Learning Git and GitHub]
(https://help.github.com/articles/good-resources-for-learning-git-and-github/)

For translating purposes, you only need how to do *git clone* and *git pull*
requests.

For installing git on RHEL/CentOS/Fedora:

	dnf install git

On Debian/Ubuntu:

	apt install git


### 5. The latest source files of Xiphos
You will need the latest versions of the source files of Xiphos. To do so,
using *git*, get a checkout of Xiphos from GitHub:

    $ git clone https://github.com/crosswire/xiphos.git

After this initial git checkout, you can later update your local files using

	$ git pull --rebase


Setting-up your build environnement
-----------------------------------
The first step to building a project is configuring it. This involves running
CMake, which will determine the necessary information about your system and, if
it finds everything it needs, generate the build system.

Unless you have a *very* good reason to do otherwise, you should create a build
directory separate from your source file directory.

For example, you have pulled Xiphos source from GitHub in a directory, if you
list the contents of this directory, you have:

    $ ls
	xiphos

You then create a build tree that is separate from the source tree:

    $ mkdir xiphos-build
	$ ls
	xiphos   xiphos-build

You are now ready to use your build environnement.

    cd xiphos-build


Configuring and Building Xiphos
-------------------------------
For configuring Xiphos, on RHEL/CentOS/Fedora, run:

	cmake ../xiphos

On Debian/Ubuntu:
Note: You can't actually build Xiphos on Debian Sid due to missing dependencies.

	cmake ../xiphos -DWEBKIT1=ON -DGTKHTML=OFF

Default install directory is /usr/local, if you want to change to another, for
example /opt, add `-DCMAKE_INSTALL_PREFIX:PATH=/opt` to the cmake command:

	cmake ../xiphos -DCMAKE_INSTALL_PREFIX:PATH=/opt

For building and installing Xiphos, run

    make
	make install

You can now try Xiphos by running:

    /usr/local/bin/xiphos


Adding a new translation to Xiphos:
-----------------------------------
1. Add your language to the `po/LINGUAS` file
The translation files are in the xiphos/po/ directory, the po/ directory of
the source tree you get from GitHub, *not* in your build (xiphos-build)
directory.

Go to the xiphos/po directory:

    cd ../xiphos/po

Then add your language code to the LINGUAS file:
For more information, see above the 'Language Code' chapter.

    echo -n " <YOUR-LANGUAGE-CODE>" >> LINGUAS

or, better launch and edit `LINGUAS` in your favorite text editor.

2. Create your PO file
Go to your build directory (xiphos-build) and re-run cmake as above.

    cmake ../xiphos
	make

your xiphos/LL.po file should be created


Translating the .po file
------------------------
You are now ready to produce a translation for all entries in the PO file, one
after another, respecting the overall file format.

See above the 'HOWTO simply add a new translation' section for more information.

You can compile Xiphos whenever you want to check the result of your work.


Submitting your PO file
-----------------------
Even if you have not completed your translation, Don't forget to send your PO
file to `xiphos-devel@crosswire.org`.

If you want to start contributing your translations to Xiphos on a regular
basis, especially if your language is not yet available on Xiphos and you are
proposing yourself as being the future maintainer for the language, or if your
language contributor is not active / responding to pings anymore and you need
to take over its role, maybe you should consider signing up for an account on
GitHub.


### Links
https://wiki.gnome.org/TranslationProject/GitHowTo



"..and ye shall be witnesses unto me both in Jerusalem, and in all
Judaea, and in Samaria, and unto the uttermost part of the earth. "
-- Acts 1:8

The Xiphos Development Team. 2017
