libguile-sqlite3
================

A minimal [SQLite](https://sqlite.org/index.html) driver for
[GNU Guile](https://www.gnu.org/software/guile/).

Installation
------------

These packages are required in order to build this:

- a C compiler (tested with `gcc`)
- [libtool](https://www.gnu.org/software/libtool/)
- pkg-config
- libsqlite3-dev
- guile-2.0-dev

    make
    sudo make install

Loading
-------

Once the shared object is installed, you should be able to load
it in Guile:

    (load-extension
        "libguile-sqlite3"
        "libguile_sqlite3_init_module")

    (use-modules (experimental sqlite3))

A nicer module loading experience is planned. For now, this will
have to do.

Usage
-----

The module exposes the procedures listed below. They are
relatively analogous to the C functions of similar names, but
also provide some conveniences such as auto-boxing of query
parameters and auto-unboxing of column values.

- [sqlite3-open](https://sqlite.org/c3ref/open.html)
- [sqlite3-close](https://sqlite.org/c3ref/close.html)
- [sqlite3-prepare](https://sqlite.org/c3ref/prepare.html)
- [sqlite3-finalize](https://sqlite.org/c3ref/finalize.html)
- [sqlite3-bind](https://sqlite.org/c3ref/bind_blob.html)
- [sqlite3-step](https://sqlite.org/c3ref/step.html)
- [sqlite3-column-count](https://sqlite.org/c3ref/column_count.html)
- [sqlite3-column](https://sqlite.org/c3ref/column_blob.html)

See the [SQLite documentation](https://sqlite.org/docs.html)
for more information.

    (let
        ((connection (sqlite3-open ":memory:")))
        (begin
            (let
                ((create-table-statment
                    (sqlite3-prepare
                        connection
                        (string-append
                            "CREATE TABLE demo ("
                            "a INTEGER PRIMARY KEY, "
                            "b FLOAT, "
                            "c TEXT);"))))
                (begin
                    (sqlite3-step create-table-statement)
                    (sqlite3-finalize create-table-statement)
                )
            )
            (let
                ((insert-statement
                    (sqlite3-prepare
                        connection
                        (string-append
                            "INSERT INTO demo "
                            "(a, b, c) VALUES (?, ?, ?);"))))
                (begin
                    (sqlite3-bind insert-statement 1 42)
                    (sqlite3-bind insert-statement 2 3.14159)
                    (sqlite3-bind insert-statement 3 "hello")
                    (sqlite3-step insert-statement)
                    (sqlite3-finalize insert-statement)
                )
            )
            (letrec (
                (select-statement
                    (sqlite3-prepare
                        connection
                        "SELECT * FROM demo;"))
                (display-all-rows
                    (lambda (statement)
                        (begin
                            (format #t "~d ~f ~a~%"
                                (sqlite3-column statement 0)
                                (sqlite3-column statement 1)
                                (sqlite3-column statement 2))
                            (if (sqlite3-step statement)
                                (display-all-rows statement)))))
            )
                (begin
                    (display-all-rows select-statement)
                    (sqlite3-finalize select-statement)
                )
            )
            (sqlite3-close connection)
        )
    )
