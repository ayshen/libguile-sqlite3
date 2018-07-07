/*
This file is part of libguile-sqlite3.

libguile-sqlite3 is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

libguile-sqlite3 is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with libguile-sqlite3.  If not, see <https://www.gnu.org/licenses/>.
*/


#ifndef LIBGUILE_SQLITE3_H
#define LIBGUILE_SQLITE3_H

#include <sqlite3.h>
#include <libguile.h>


typedef
struct
{
    sqlite3 * pDb;
    char * filename;
}
libguile_sqlite3_connection_t;


typedef
struct
{
    sqlite3_stmt * pStmt;
    char * zSql;
}
libguile_sqlite3_statement_t;


#define LIBGUILE_SQLITE3_ERROR "sqlite3-error"


SCM sqlite3_open_wrapper (SCM filename);

SCM sqlite3_close_wrapper (SCM connection_object);

SCM sqlite3_prepare_wrapper (SCM connection_object,
                             SCM sql);

SCM sqlite3_finalize_wrapper (SCM statement_object);

SCM sqlite3_bind_wrapper (SCM statement_object,
                          SCM column_number,
                          SCM value);

SCM sqlite3_step_wrapper (SCM statement_object);

SCM sqlite3_column_count_wrapper (SCM statement_object);

SCM sqlite3_column_wrapper (SCM statement_object,
                            SCM column_number);


#endif /* LIBGUILE_SQLITE3_H */
