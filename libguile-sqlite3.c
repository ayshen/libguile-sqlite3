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


#include "libguile-sqlite3.h"

#include <stdlib.h>


SCM
sqlite3_open_wrapper (SCM filename)
{
    libguile_sqlite3_connection_t * connection;
    int ret;

    connection = (libguile_sqlite3_connection_t *)
                 malloc(sizeof(libguile_sqlite3_connection_t));
    if (!connection)
    {
        scm_throw(
            scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
            scm_list_1(
                scm_from_utf8_string(
                    "not enough memory to allocate a "
                    "SQLite connection object")));
    }

    connection->filename = scm_to_utf8_stringn(filename, NULL);

    if ((ret = sqlite3_open(
            connection->filename,
            &connection->pDb)) != SQLITE_OK)
    {
        free(connection->filename);
        free(connection);
        scm_throw(
            scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
            scm_list_1(
                scm_from_utf8_string(
                    sqlite3_errstr(ret))));
    }

    return scm_from_pointer(connection, NULL);
}


SCM
sqlite3_close_wrapper (SCM connection_object)
{
    libguile_sqlite3_connection_t * connection;
    int ret;

    connection = (libguile_sqlite3_connection_t *)
                 scm_to_pointer(connection_object);

    if ((ret = sqlite3_close(connection->pDb)) != SQLITE_OK)
    {
        scm_throw(
            scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
            scm_list_1(
                scm_from_utf8_string(
                    sqlite3_errstr(ret))));
    }

    if (connection->filename)
    {
        free(connection->filename);
        connection->filename = NULL;
    }

    free(connection);
    connection = NULL;

    return SCM_UNSPECIFIED;
}


SCM
sqlite3_prepare_wrapper (SCM connection_object,
                         SCM sql)
{
    libguile_sqlite3_connection_t * connection;
    libguile_sqlite3_statement_t * statement;
    int ret;

    connection = (libguile_sqlite3_connection_t *)
                 scm_to_pointer(connection_object);

    statement = (libguile_sqlite3_statement_t *)
                malloc(sizeof(libguile_sqlite3_statement_t));
    if (!statement)
    {
        scm_throw(
            scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
            scm_list_1(
                scm_from_utf8_string(
                    "not enough memory to allocate a "
                    "SQLite statement object")));
    }

    statement->zSql = scm_to_utf8_stringn(sql, NULL);

    if ((ret = sqlite3_prepare_v2(
            connection->pDb,
            statement->zSql,
            scm_to_int(scm_string_length(sql)),
            &statement->pStmt,
            NULL)) != SQLITE_OK)
    {
        free(statement->zSql);
        free(statement);
        scm_throw(
            scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
            scm_list_1(
                scm_from_utf8_string(
                    sqlite3_errstr(ret))));
    }

    return scm_from_pointer(statement, NULL);
}


SCM
sqlite3_finalize_wrapper (SCM statement_object)
{
    libguile_sqlite3_statement_t * statement;
    int ret;

    statement = (libguile_sqlite3_statement_t *)
                scm_to_pointer(statement_object);

    if ((ret = sqlite3_finalize(statement->pStmt)) != SQLITE_OK)
    {
        scm_throw(
            scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
            scm_list_1(
                scm_from_utf8_string(
                    sqlite3_errstr(ret))));
    }

    if (statement->zSql)
    {
        free(statement->zSql);
        statement->zSql = NULL;
    }

    free(statement);
    statement = NULL;

    return SCM_UNSPECIFIED;
}


SCM
sqlite3_bind_wrapper (SCM statement_object,
                      SCM column_number,
                      SCM value)
{
    libguile_sqlite3_statement_t * statement;
    int column;
    int ret;

    statement = (libguile_sqlite3_statement_t *)
                scm_to_pointer(statement_object);
    column = scm_to_int(column_number);

    if (scm_is_integer(value))
    {
        ret = sqlite3_bind_int(
            statement->pStmt,
            column,
            scm_to_int(value));
    }
    else if (scm_is_real(value))
    {
        ret = sqlite3_bind_double(
            statement->pStmt,
            column,
            scm_to_double(value));
    }
    else if (scm_is_string(value))
    {
        SCM bv = scm_string_to_utf8(value);
        const char * string_parameter;

        string_parameter = (const char *)
            scm_to_pointer(
                scm_bytevector_to_pointer(
                    bv,
                    scm_from_int(0)));

        ret = sqlite3_bind_text(
            statement->pStmt,
            column,
            string_parameter,
            (int)scm_c_string_utf8_length(value),
            NULL);
    }
    else if (scm_is_bytevector(value))
    {
        const void * blob_parameter;

        blob_parameter = scm_to_pointer(
            scm_bytevector_to_pointer(value, scm_from_int(0)));

        ret = sqlite3_bind_blob( 
            statement->pStmt,
            column,
            blob_parameter,
            scm_to_int(scm_bytevector_length(value)),
            NULL);
    }

    if (ret != SQLITE_OK)
    {
        scm_throw(
            scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
            scm_list_1(
                scm_from_utf8_string(
                    sqlite3_errstr(ret))));
    }

    return SCM_UNSPECIFIED;
}


SCM
sqlite3_step_wrapper (SCM statement_object)
{
    libguile_sqlite3_statement_t * statement;
    int ret;

    statement = (libguile_sqlite3_statement_t *)
        scm_to_pointer(statement_object);

    ret = sqlite3_step(statement->pStmt);

    if (ret == SQLITE_ROW)
    {
        return SCM_BOOL_T;
    }
    else if (ret == SQLITE_DONE)
    {
        return SCM_BOOL_F;
    }

    scm_throw(
        scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
        scm_list_1(
            scm_from_utf8_string(
                sqlite3_errstr(ret))));
    return SCM_UNSPECIFIED;
}


SCM
sqlite3_column_count_wrapper (SCM statement_object)
{
    libguile_sqlite3_statement_t * statement;

    statement = (libguile_sqlite3_statement_t *)
        scm_to_pointer(statement_object);

    return scm_from_int(sqlite3_column_count(statement->pStmt));
}


SCM
sqlite3_column_wrapper (SCM statement_object,
                        SCM column_number)
{
    libguile_sqlite3_statement_t * statement;
    int column;
    int columnType;

    statement = (libguile_sqlite3_statement_t *)
        scm_to_pointer(statement_object);
    column = scm_to_int(column_number);

    columnType = sqlite3_column_type(
        statement->pStmt, 
        column);

    if (columnType == SQLITE_INTEGER)
    {
        return scm_from_int(
            sqlite3_column_int(
                statement->pStmt,
                column));
    }
    else if (columnType == SQLITE_FLOAT)
    {
        return scm_from_double(
            sqlite3_column_double(
                statement->pStmt,
                column));
    }
    else if (columnType == SQLITE_TEXT)
    {
        return scm_from_utf8_string(
            (const char *)
            sqlite3_column_text(
                statement->pStmt,
                column));
    }
    else if (columnType == SQLITE_BLOB)
    {
        int blobSize = sqlite3_column_bytes(
            statement->pStmt,
            column);

        const void * blob = sqlite3_column_blob(
            statement->pStmt,
            column);

        void * blobCopy = scm_gc_malloc_pointerless(
            (size_t)blobSize,
            (const char*)NULL); /* FIXME */

        if (!blobCopy)
        {
            scm_throw(
                scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
                scm_list_1(
                    scm_from_utf8_string(
                        "not enough memory to copy the blob")));
            return SCM_UNSPECIFIED;
        }

        memcpy(blobCopy, blob, (size_t)blobSize);
        return scm_pointer_to_bytevector(
            scm_from_pointer(blobCopy, NULL),
            scm_from_int(blobSize),
            scm_from_int(0),
            scm_from_utf8_symbol("u8"));
    }
    else if (columnType == SQLITE_NULL)
    {
        return SCM_UNDEFINED;
    }

    scm_throw(
        scm_from_utf8_symbol(LIBGUILE_SQLITE3_ERROR),
        scm_list_1(
            scm_from_utf8_string(
                "unrecognized column type")));
    return SCM_UNSPECIFIED;
}


void
libguile_sqlite3_init (void * unused)
{
    /* sqlite3-open    filename-or-uri
    Open a database connection to the given database file or URI.
    If successful, return a foreign pointer to the connection
    structure that will be used to work with the database.
    Throws an exception on failure.
    */
    scm_c_define_gsubr("sqlite3-open", 1, 0, 0,
                       sqlite3_open_wrapper);
    scm_c_export("sqlite3-open");

    /* sqlite3-close    connection
    Close the given database connection.
    If successful, returns #<unspecified>.
    Throws an exception on failure.
    */
    scm_c_define_gsubr("sqlite3-close", 1, 0, 0,
                       sqlite3_close_wrapper);
    scm_c_export("sqlite3-close");

    /* sqlite3-prepare    connection    sql
    Prepare a SQL statement for execution on the given connection.
    If successful, returns a foreign pointer to the statement
    object that can be used to execute the given SQL.
    Throws an exception on failure.
    */
    scm_c_define_gsubr("sqlite3-prepare", 2, 0, 0,
                       sqlite3_prepare_wrapper);
    scm_c_export("sqlite3-prepare");

    /* sqlite3-finalize    statement
    Finalize and destroy a prepared SQL statement.
    If successful, returns #<unspecified>.
    Throws an exception on failure.
    */
    scm_c_define_gsubr("sqlite3-finalize", 1, 0, 0,
                       sqlite3_finalize_wrapper);
    scm_c_export("sqlite3-finalize");

    /* sqlite3-bind    statement    column    value
    Bind a parameter value for a prepared SQL statement.
    If successful, returns #<unspecified>.
    Throws an exception on failure.
    */
    scm_c_define_gsubr("sqlite3-bind", 3, 0, 0,
                       sqlite3_bind_wrapper);
    scm_c_export("sqlite3-bind");

    /* sqlite3-step    statement
    Step to the next row of results from a prepared SQL statement.
    Returns #t if more rows are available afterward, #f if the
    statement is now on the last row of results.
    */
    scm_c_define_gsubr("sqlite3-step", 1, 0, 0,
                       sqlite3_step_wrapper);
    scm_c_export("sqlite3-step");

    /* sqlite3-column-count    statement
    Get the number of columns that may be available when the
    given prepared SQL statement is stepped.
    Note that since the statement may or may not have any rows,
    this column count may not be useful.
    Returns the number of columns. Does not throw.
    */
    scm_c_define_gsubr("sqlite3-column-count", 1, 0, 0,
                       sqlite3_column_count_wrapper);
    scm_c_export("sqlite3-column-count");

    /* sqlite3-column    statement    column
    Get the column value from the current row of results for the
    given prepared SQL statement.
    Returns the value if able.
    Throws an exception on failure.
    */
    scm_c_define_gsubr("sqlite3-column", 2, 0, 0,
                       sqlite3_column_wrapper);
    scm_c_export("sqlite3-column");
}


void
libguile_sqlite3_init_module ()
{
    scm_c_define_module(
        "experimental sqlite3",
        libguile_sqlite3_init,
        NULL);
}
