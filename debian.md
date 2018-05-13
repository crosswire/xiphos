Debian packages
===============

1. Build Xiphos from source
---------------------------
untar the source files in your working folder.
make a build directory

    $ make build && cd build

run cmake:

    $ cmake -DWEBKIT1 ..

For more informations, see INSTALL.md

2. Make a DEB package
---------------------
In the build dir, run:

    $ make package

You'll then find DEB packages for your architecture
in your build dir.
