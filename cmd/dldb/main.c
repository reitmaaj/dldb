/* main.c - dldb command line: init, load, query, dump, check. */

#include <dldb89.h>
#include <dlq89.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int read_file(const char *path, unsigned char **out, size_t *out_size)
{
    FILE *f;
    long n;
    unsigned char *buf;
    size_t got;

    f = fopen(path, "rb");
    if (f == NULL)
    {
        fprintf(stderr, "dldb: cannot open %s\n", path);
        return 1;
    }
    if (fseek(f, 0, SEEK_END) != 0)
    {
        fclose(f);
        return 1;
    }
    n = ftell(f);
    if (n < 0)
    {
        fclose(f);
        return 1;
    }
    if (fseek(f, 0, SEEK_SET) != 0)
    {
        fclose(f);
        return 1;
    }
    buf = (unsigned char *)malloc(n == 0 ? 1 : (size_t)n);
    if (buf == NULL)
    {
        fclose(f);
        return 1;
    }
    got = fread(buf, 1, (size_t)n, f);
    fclose(f);
    if (got != (size_t)n)
    {
        free(buf);
        return 1;
    }
    *out = buf;
    *out_size = got;
    return 0;
}

static int print_error(dldb89 *db, const char *what)
{
    const dldb89_error *e;

    e = dldb89_last_error(db);
    if (e == NULL)
    {
        fprintf(stderr, "dldb: %s failed\n", what);
        return 1;
    }
    fprintf(stderr, "dldb: %s: %s: %s\n", what, dldb89_status_name(e->status),
            e->message != NULL ? e->message : "");
    return 1;
}

static int cmd_init(const char *path)
{
    dldb89 *db;
    dldb89_status st;

    db = NULL;
    st = dldb89_create(&db, path, NULL);
    if (st != DLDB89_OK)
    {
        fprintf(stderr, "dldb: init: %s\n", dldb89_status_name(st));
        return 1;
    }
    dldb89_close(db);
    return 0;
}

static int cmd_load(const char *path, const char *program)
{
    unsigned char *data;
    size_t size;
    dldb89 *db;
    dldb89_status st;
    int rc;

    if (read_file(program, &data, &size) != 0)
    {
        return 1;
    }
    db = NULL;
    st = dldb89_open(&db, path, NULL);
    if (st != DLDB89_OK)
    {
        fprintf(stderr, "dldb: load: %s\n", dldb89_status_name(st));
        free(data);
        return 1;
    }
    st = dldb89_load_text(db, data, size);
    free(data);
    rc = 0;
    if (st != DLDB89_OK)
    {
        rc = print_error(db, "load");
    }
    dldb89_close(db);
    return rc;
}

static int cmd_replace(const char *path, const char *program)
{
    unsigned char *data;
    size_t size;
    dldb89 *db;
    dlx89_program *prog;
    dlx89_error error;
    dlx89_status xst;
    dldb89_status st;
    int rc;

    if (read_file(program, &data, &size) != 0)
    {
        return 1;
    }
    prog = NULL;
    xst = dlx89_parse(&prog, data, size, NULL, &error);
    free(data);
    if (xst != DLX89_OK)
    {
        fprintf(stderr, "dldb: replace: DLX parse failed\n");
        return 1;
    }
    db = NULL;
    st = dldb89_open(&db, path, NULL);
    if (st != DLDB89_OK)
    {
        fprintf(stderr, "dldb: replace: %s\n", dldb89_status_name(st));
        dlx89_program_destroy(prog);
        return 1;
    }
    st = dldb89_replace_rules(db, prog);
    dlx89_program_destroy(prog);
    rc = 0;
    if (st != DLDB89_OK)
    {
        rc = print_error(db, "replace");
    }
    dldb89_close(db);
    return rc;
}

static int print_table(dlq89_result *result)
{
    size_t columns;
    size_t c;
    const dlq89_value *row;
    dlq89_status st;

    columns = dlq89_result_column_count(result);
    for (c = 0; c < columns; ++c)
    {
        size_t n;
        const unsigned char *name;

        name = dlq89_result_column_name(result, c, &n);
        if (c != 0)
        {
            printf("\t");
        }
        if (name != NULL)
        {
            fwrite(name, 1, n, stdout);
        }
    }
    printf("\n");
    for (;;)
    {
        st = dlq89_result_next_row(result, &row);
        if (st == DLQ89_END)
        {
            break;
        }
        if (st != DLQ89_OK)
        {
            return 1;
        }
        for (c = 0; c < columns; ++c)
        {
            if (c != 0)
            {
                printf("\t");
            }
            if (row[c].kind == DLQ89_VALUE_STRING)
            {
                printf("\"");
                if (row[c].size != 0)
                {
                    fwrite(row[c].data, 1, row[c].size, stdout);
                }
                printf("\"");
            }
            else if (row[c].size != 0)
            {
                fwrite(row[c].data, 1, row[c].size, stdout);
            }
        }
        printf("\n");
    }
    return 0;
}

static int cmd_query(const char *path, const char *source)
{
    dldb89 *db;
    dldb89_status dst;
    dlq89_document *doc;
    dlq89_cursor *cursor;
    dlq89_result *result;
    dlq89_diagnostic diag;
    dlq89_status st;
    int rc;

    db = NULL;
    dst = dldb89_open(&db, path, NULL);
    if (dst != DLDB89_OK)
    {
        fprintf(stderr, "dldb: query: %s\n", dldb89_status_name(dst));
        return 1;
    }
    doc = NULL;
    st = dlq89_parse((const unsigned char *)source, strlen(source), &doc,
                     &diag);
    if (st != DLQ89_OK)
    {
        fprintf(stderr, "dldb: query: %s\n",
                diag.message != NULL ? diag.message : dlq89_status_name(st));
        dldb89_close(db);
        return 1;
    }
    cursor = NULL;
    st = dlq89_execute(doc, dldb89_query_db(db), &cursor, &diag);
    if (st != DLQ89_OK)
    {
        fprintf(stderr, "dldb: query: %s\n",
                diag.message != NULL ? diag.message : dlq89_status_name(st));
        dlq89_document_destroy(doc);
        dldb89_close(db);
        return 1;
    }
    rc = 0;
    for (;;)
    {
        st = dlq89_cursor_next_result(cursor, &result);
        if (st == DLQ89_END)
        {
            break;
        }
        if (st != DLQ89_OK)
        {
            rc = print_error(db, "query");
            break;
        }
        if (dlq89_result_kindof(result) == DLQ89_BOOLEAN)
        {
            printf("%s\n", dlq89_result_boolean(result) != 0 ? "true"
                                                             : "false");
        }
        else
        {
            rc = print_table(result);
            if (rc != 0)
            {
                break;
            }
        }
    }
    dlq89_cursor_destroy(cursor);
    dlq89_document_destroy(doc);
    dldb89_close(db);
    return rc;
}

static int stdout_sink_write(void *ctx, const unsigned char *bytes, size_t len)
{
    (void)ctx;
    if (len != 0)
    {
        fwrite(bytes, 1, len, stdout);
    }
    return 0;
}

static int cmd_dump(const char *path)
{
    dldb89 *db;
    dlx89_sink sink;
    dldb89_status st;
    int rc;

    db = NULL;
    st = dldb89_open(&db, path, NULL);
    if (st != DLDB89_OK)
    {
        fprintf(stderr, "dldb: dump: %s\n", dldb89_status_name(st));
        return 1;
    }
    sink.write = stdout_sink_write;
    sink.ctx = NULL;
    st = dldb89_dump(db, &sink);
    rc = 0;
    if (st != DLDB89_OK)
    {
        rc = print_error(db, "dump");
    }
    dldb89_close(db);
    return rc;
}

static int cmd_check(const char *path, int full)
{
    dldb89 *db;
    dldb89_status st;
    int rc;

    db = NULL;
    st = dldb89_open(&db, path, NULL);
    if (st != DLDB89_OK)
    {
        fprintf(stderr, "dldb: check: %s\n", dldb89_status_name(st));
        return 1;
    }
    st = dldb89_check(db, full);
    rc = 0;
    if (st != DLDB89_OK)
    {
        rc = print_error(db, "check");
    }
    else
    {
        printf("ok\n");
    }
    dldb89_close(db);
    return rc;
}

static void usage(void)
{
    fprintf(stderr,
            "usage: dldb init FILE\n"
            "       dldb load FILE PROGRAM.dlx\n"
            "       dldb replace FILE RULES.dlx\n"
            "       dldb query FILE '?- atom(X).'\n"
            "       dldb dump FILE\n"
            "       dldb check [--full] FILE\n");
}

int main(int argc, char **argv)
{
    if (argc < 3)
    {
        usage();
        return 2;
    }
    if (strcmp(argv[1], "init") == 0 && argc == 3)
    {
        return cmd_init(argv[2]);
    }
    if (strcmp(argv[1], "load") == 0 && argc == 4)
    {
        return cmd_load(argv[2], argv[3]);
    }
    if (strcmp(argv[1], "replace") == 0 && argc == 4)
    {
        return cmd_replace(argv[2], argv[3]);
    }
    if (strcmp(argv[1], "query") == 0 && argc == 4)
    {
        return cmd_query(argv[2], argv[3]);
    }
    if (strcmp(argv[1], "dump") == 0 && argc == 3)
    {
        return cmd_dump(argv[2]);
    }
    if (strcmp(argv[1], "check") == 0 && argc == 3)
    {
        return cmd_check(argv[2], 0);
    }
    if (strcmp(argv[1], "check") == 0 && argc == 4 &&
        strcmp(argv[2], "--full") == 0)
    {
        return cmd_check(argv[3], 1);
    }
    usage();
    return 2;
}
