interface_xml := ../gnome2/ipc-interface.xml

pkg_packages := dbus-1 dbus-glib-1

PKG_CFLAGS := $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS := $(shell pkg-config --libs $(pkg_packages))

ADD_CFLAGS := -g -Wall -DG_DISABLE_DEPRECATED

CFLAGS := $(PKG_CFLAGS) $(ADD_CFLAGS) $(CFLAGS)
LDFLAGS := $(PKG_LDFLAGS) $(LDFLAGS)

cleanfiles := ipc-client-stub.h

targets = client

.PHONY: all clean
all: $(targets)

client: receive.o
	$(CC) $^ -o $@ $(LDFLAGS)

receive.o: ipc_client.c ipc-client-stub.h
	   $(CC) $(CFLAGS) -DPROGRNAME=\"ipc_client\" -c $< -o $@

ipc-client-stub.h: $(interface_xml)
		   dbus-binding-tool --prefix=ipc_object --mode=glib-client \
		   $< > $@

clean:
	$(RM) $(targets) $(cleanfiles) *.o
