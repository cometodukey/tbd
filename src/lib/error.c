#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <tbdc/lib/assert.h>
#include <tbdc/lib/error.h>
#include <tbdc/lib/log.h>

struct table_chars
{
    const char *column;
    const char *row;
    const char *left_lower_corner;
    const char *left_upper_corner;
};

static struct table_chars ascii_table = {
    .column = "|",
    .row = "-",
    .left_lower_corner = "`",
    .left_upper_corner = ""
};

static struct table_chars utf8_table = {
    .column = "│",
    .row = "─",
    .left_lower_corner = "└",
    .left_upper_corner = "┌"
};


static struct table_chars *get_table_chars(void)
{
    if (1 /*term_supports_utf8()*/)
        return &utf8_table;
    return &ascii_table;
}

static const char *column_char(void)
{
    return get_table_chars()->column;
}

static const char *row_char(void)
{
    return get_table_chars()->row;
}

static const char *left_lower_corner_char(void)
{
    return get_table_chars()->left_lower_corner;
}

static const char *left_upper_corner_char(void)
{
    return get_table_chars()->left_upper_corner;
}

static const char *find_start_of_line(const size_t size, const char data[size], const size_t line)
{
    assert_ge(line, 1);

    const char *curr = data;
    size_t curr_line = 1;
    size_t remaining = size;

    while (curr_line != line)
    {
        const char *next = memchr(curr, '\n', remaining);
        if (next == NULL)
            return curr;

        const size_t delta = next - curr + 1;
        curr += delta;
        remaining -= delta;

        curr_line++;
    }

    return curr;
}

void source_error(const char *file,
                  const size_t size,
                  const char *data,
                  const struct loc *loc,
                  const char *fmt, ...)
{
    assert_nonnull(file);
    assert_nonnull(data);
    assert_nonnull(loc);
    assert_nonnull(fmt);

    log_error("In %s:"SIZE_FMT":"SIZE_FMT":", file, loc->line, loc->start);

    fputc('\n', stderr);

    const char *start_of_line = find_start_of_line(size, data, loc->line);

    const char *end_of_line = memchr(start_of_line, '\n', (data + size) - start_of_line);
    if (end_of_line == NULL)
        end_of_line = data + size;

    log_padln("${bold}%s:${reset}\n", file);
    log_padln(""SIZE_FMT" %s %.*s", loc->line, column_char(), (int)(end_of_line - start_of_line), start_of_line);

    const int line_len = snprintf(NULL, 0, ""SIZE_FMT"", loc->line);

    const size_t len = loc->end - loc->start - 1;
    const char *corner = len > 1 ? left_upper_corner_char() : column_char();
    log_pad("%*.s %s ", line_len, "", column_char());
    log_none("%*.s${magenta}%s", loc->start - 2, "", corner);

    for (size_t i = 0; i < len; i++)
        log_none("%s", row_char());

    log_noneln("${reset}");

    log_pad("%*.s %s ", line_len, "", column_char());
    log_none("%*.s${magenta}%s${reset}  ", loc->start - 2, "", left_lower_corner_char());

    va_list args;
    va_start(args, fmt);
    vlog(LOG_NONE, fmt, args, true);
    va_end(args);

    fputc('\n', stderr);
}

void compile_error(const char *file, const char *reason)
{
    log_fatal("Failed to compile input file '%s'. %s.", file, reason);
}

// TODO: Syntax highlighting on error messages, function names, levenshtein distance, handle binary data, invalid unicode (overlong, etc reporting)
