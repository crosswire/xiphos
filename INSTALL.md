Installation instructions
==========================

On Linux install Xiphos through you package manager, for example on Fedora:

    $ sudo dnf install xiphos-gtk3

Or on Debian, Ubuntu, or Linux Mint:

    $ sudo apt-get update
    $ sudo apt-get install xiphos

If you use Windows download the .exe file from: http://xiphos.org/download/

Compile Xiphos from source
==========================

To compile Xiphos from source you will need to get a checkout of Xiphos from
GitHub. To do so, do:

    $ git clone https://github.com/crosswire/xiphos.git

Now to compile the GTK3 version of Xiphos you need to make sure you have these
packages installed:

    $ sudo dnf install gcc-c++ intltool gtk3-devel dbus-glib-devel gtkhtml3-devel webkitgtk4-devel libxml2-devel libgsf-devel gconfmm26-devel sword-devel libuuid-devel gnome-doc-utils rarian rarian-compat biblesync-devel

Or for ubuntu, both GTK3 and GTK2 versions, install:

    $ sudo apt-get install cpp intltool libdbus-glib-1-dev libwebkitgtk-3.0-dev libxml2-dev libgsf-1-dev libgconfmm-2.6-dev libsword-dev uuid-dev gnome-doc-utils rarian-compat libwebkitgtk-dev libglade2-dev

Next we need to configure everything:

    $ ./waf configure --prefix=/usr --debug-level=optimized --enable-webkit-editor

Now we build:

    $ ./waf build

Then install:

    $ sudo ./waf install
