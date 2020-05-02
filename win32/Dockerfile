FROM fedora:30

VOLUME /source

# Installs dependencies once and for all
RUN dnf -y update --refresh && \
    dnf -y install cmake git fpc gettext glib2-devel itstool libxslt make yelp-tools zip && \
    dnf -y install mingw32-biblesync mingw32-gettext mingw32-sword mingw32-minizip mingw32-libglade2 mingw32-webkitgtk mingw32-gtk3 mingw32-libtiff mingw32-libidn mingw32-gdb mingw32-nsis mingw32-dbus-glib && \
    dnf -y install mingw64-biblesync mingw64-gettext mingw64-sword mingw64-minizip mingw64-libglade2 mingw64-webkitgtk mingw64-gtk3 mingw64-libtiff mingw64-libidn mingw64-gdb mingw32-nsis mingw64-dbus-glib && \
    dnf clean all

ENTRYPOINT ["/bin/bash", "/source/win32/xc-xiphos-win.sh"]

CMD ["-win32", "-win64"]
