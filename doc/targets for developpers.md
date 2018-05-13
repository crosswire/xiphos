Useful targets for developpers
==============================
All exemples are shown with `make`, but feel free to use `ninja` which is 20% faster. Obviously, you can also enter `cmake --build . --target <target>`.

targets that do not alter the source tree:
------------------------------------------
build main lib

    $ make main

build backend lib

    $ make backend

build editor lib

    $ make editor

build webkit lib

    $ make webkit

build xiphos_html lib

    $ make xiphos_html

build help

    $ make handbook


targets for updating source files:
----------------------------------
updating xiphos pot file

    $ make pot

updating all po files

    $ make po

updating a specific po file

    $ make <lang>.po

updating help pot file

    $ make help_pot

updating help all po files

    $ make help_po

updating a specific help po file

    $ make help_<lang>.po
