FROM fedora:30

VOLUME /source

# Installs dependencies once and for all
RUN dnf -y update --refresh && \
    dnf -y install autoconf automake cmake git fpc gettext glib2-devel itstool libxslt make subversion yelp-tools zip && \
    dnf -y install mingw32-biblesync mingw32-clucene mingw32-curl mingw32-dbus-glib mingw32-gcc mingw32-gdb mingw32-libgnurx mingw32-gettext mingw32-gtk3 mingw32-icu mingw32-libglade2 mingw32-libidn mingw32-libsoup mingw32-libtiff mingw32-minizip mingw32-nsis mingw32-webkitgtk mingw32-xz && \
    dnf -y install mingw64-biblesync mingw64-clucene mingw64-curl mingw64-dbus-glib mingw64-gcc mingw64-gdb mingw64-libgnurx mingw64-gettext mingw64-gtk3 mingw64-icu mingw64-libglade2 mingw64-libidn mingw64-libsoup mingw64-libtiff mingw64-minizip mingw32-nsis mingw64-webkitgtk mingw64-xz && \
    dnf clean all

ADD build_sword.sh /build_sword.sh

RUN /bin/bash /build_sword.sh

ENTRYPOINT ["/bin/bash", "/source/win32/xc-xiphos-win.sh"]

CMD ["-win32", "-win64"]
