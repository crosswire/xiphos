pkg_packages := glib-2.0 gio-2.0

PKG_CFLAGS := $(shell pkg-config --cflags $(pkg_packages))
PKG_LDFLAGS := $(shell pkg-config --libs $(pkg_packages))

ADD_CFLAGS := -g -Wall -DG_DISABLE_DEPRECATED

CFLAGS := $(PKG_CFLAGS) $(ADD_CFLAGS) $(CFLAGS)
LDFLAGS := $(PKG_LDFLAGS) $(LDFLAGS)

targets = client

.PHONY: all clean
all: $(targets)

client: receive.o
	$(CC) $^ -o $@ $(LDFLAGS)

receive.o: ipc_client.c
	   $(CC) $(CFLAGS) -DPROGRNAME=\"ipc_client\" -c $< -o $@

clean:
	$(RM) $(targets) *.o
