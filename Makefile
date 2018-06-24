GENERAL_CFLAGS = -g -Wall -Werror
SQLITE3_CFLAGS != pkg-config --cflags sqlite3
LIBGUILE_CFLAGS != pkg-config --cflags guile-2.0

SQLITE3_LFLAGS = -lsqlite3
LIBGUILE_LFLAGS = -lguile-2.0

CFLAGS = $(GENERAL_CFLAGS) $(SQLITE3_CFLAGS) $(LIBGUILE_CFLAGS)
LFLAGS = $(SQLITE3_LFLAGS) $(LIBGUILE_LFLAGS)

LIBTOOL != which libtool
LIBTOOL += --tag=CC

# FIXME:
# There has to be a portable way to figure out where to install
# the shared libraries! Let me know if you know how.
RPATH = /usr/local/lib


all: libguile-sqlite3.la

libguile-sqlite3.la: libguile-sqlite3.lo
	$(LIBTOOL) --mode=link $(CC) -g -o $@ $^ -rpath $(RPATH) $(LFLAGS)

libguile-sqlite3.lo: libguile-sqlite3.c
	$(LIBTOOL) --mode=compile $(CC) -c -o $@ $(CFLAGS) $^

clean:
	rm -r -f .libs
	rm -f libguile-sqlite3.so
	rm -f libguile-sqlite3.lo
	rm -f libguile-sqlite3.la
	rm -f libguile-sqlite3.o

install:
	$(LIBTOOL) --mode=install install libguile-sqlite3.la $(RPATH)

uninstall:
	$(LIBTOOL) --mode=uninstall rm $(RPATH)/libguile-sqlite3.la

.PHONY: clean install uninstall
