# This file is part of libguile-sqlite3.
#
# libguile-sqlite3 is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# libguile-sqlite3 is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with libguile-sqlite3.  If not, see <https://www.gnu.org/licenses/>.


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
