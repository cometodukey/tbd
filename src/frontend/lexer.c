#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include <tbdc/frontend/lexer.h>
#include <tbdc/lib/assert.h>
#include <tbdc/lib/error.h>
#include <tbdc/lib/log.h>
#include <tbdc/lib/string.h>
#include <tbdc/lib/unicode.h>
#include <tbdc/lib/util.h>
#include <tbdc/lib/xmalloc.h>

enum lexer_status
{
    LEX_OK,
    LEX_TOK_ERR,
    LEX_UTF8_ERR
};

struct keywords
{
    const char *keyword;
    enum token_type type;
};

static const struct keywords keywords[] = {
    { "func", TOKEN_FUNC },
    { "return", TOKEN_RETURN }
};

static inline bool at_eof(struct compile *ctx)
{
    assert_le(ctx->lexer->index, ctx->source->size);

    return ctx->lexer->index == ctx->source->size;
}

// static bool peek_match(struct compile *ctx, const unicode_char c);

// static inline bool at_eol(struct compile *ctx)
// {
//     return at_eof(ctx) || peek_match(ctx, '\n');
// }

static bool peek(struct compile *ctx, unicode_char *c)
{
    assert_nonnull(ctx);
    assert_nonnull(c);

    if (at_eof(ctx))
        return false;

    size_t index = ctx->lexer->index;
    const char *start = &ctx->source->data[index];
    int e;
    utf8_decode(start, c, &e);
    if (e)
        *c = UNICODE_REPLACEMENT;

    return true;
}

// static bool peek_match(struct compile *ctx, const unicode_char c)
// {
//     unicode_char c2;
//     if (peek(ctx, &c2) && c == c2)
//         return true;
//     else
//         return false;
// }

/**
 * Decode a codepoint from the current position in the source file.
 */
static unicode_char consume(struct compile *ctx, size_t *len)
{
    assert_false(at_eof(ctx));

    unicode_char c;
    int e;
    size_t *index = &ctx->lexer->index;
    const char *start = &ctx->source->data[*index];
    *len = utf8_decode(start, &c, &e);

    if (e)
    {
        *len = 0;
        c = UNICODE_REPLACEMENT;
    }

    *index += *len;
    ctx->lexer->col++;

    return c;
}

/**
 * Unconsume a codepoint.
 */
static void unconsume(struct lexer *lexer, const size_t len)
{
    assert_nonnull(lexer);

    lexer->index -= len;
    lexer->col--;
}

static bool is_alpha(const unicode_char c)
{
    return (c >= 'A' && c <= 'Z') || (c >= 'a' && c <= 'z');
}

static bool is_digit(const unicode_char c)
{
    return c >= '0' && c <= '9';
}

static bool is_ident_start(const unicode_char c)
{
    return is_alpha(c) || c == '_';
}

static bool is_ident_cont(const unicode_char c)
{
    return is_ident_start(c) || is_digit(c);
}

static enum token_type scan_word(struct compile *ctx)
{
    assert_nonnull(ctx);
    assert_nonnull(ctx->lexer);

    const size_t start = ctx->lexer->index;

    unicode_char c;
    size_t scratch_len;
    while (!at_eof(ctx) && peek(ctx, &c) && is_ident_cont(c))
        consume(ctx, &scratch_len);

    const size_t end = ctx->lexer->index;

    size_t len = end - start;

    for (size_t i = 0; i < lengthof(keywords); i++)
    {
        if (strn_equal(keywords[i].keyword, &ctx->source->data[start], len))
            return keywords[i].type;
    }

    return TOKEN_IDENT;
}

enum lexer_status scan_token(struct compile *ctx)
{
    assert_nonnull(ctx);
    assert_nonnull(ctx->lexer);

    struct lexer *lexer = ctx->lexer;

    const size_t start_col = lexer->col;
    const size_t start_index = lexer->index;

    size_t len;
    const unicode_char c = consume(ctx, &len);
    if (len == 0)
    {
        /* TODO: Improve this error message */
        struct loc loc = { .start = start_col, .end = lexer->col, .line = lexer->line };
        source_error(ctx->source->file, ctx->source->size, ctx->source->data,
                     &loc, "Invalid UTF-8 byte sequence");
        return LEX_UTF8_ERR;
    }

    enum token_type type = TOKEN_EOF;

    switch (c)
    {
        /* Single character tokens */
        case ';': type = TOKEN_SEMICOLON; break;
        case ',': type = TOKEN_COMMA;     break;
        case '{': type = TOKEN_LBRACE;    break;
        case '}': type = TOKEN_RBRACE;    break;
        case '(': type = TOKEN_LPAREN;    break;
        case ')': type = TOKEN_RPAREN;    break;
        case '+': type = TOKEN_PLUS;      break;
        case ':': type = TOKEN_COLON;     break;
        case '-': type = TOKEN_MINUS;     break;

        /* Single or double character tokens */
            /* None yet. */

        /* Match insignificant whitespace */
        case '\t': /* FALLTHROUGH */
        case ' ':
            return 0;

        case '\n':
            lexer->line++;
            lexer->col = 1;
            return 0;

        default:
            if (is_ident_start(c))
            {
                unconsume(lexer, len); /* Push back the stream so this character can be re-read */
                type = scan_word(ctx);
            }
            else
            {
                type = TOKEN_UNKNOWN;
            }
    }

    struct token tok = {
        .loc = { .start = start_col, .end = lexer->col, .line = lexer->line },
        .type = type,
        .lexeme = { .size = lexer->index - start_index, .str = &ctx->source->data[start_index] }
    };

    if (tok.type == TOKEN_UNKNOWN)
    {
        /* TODO: Handle grapheme clusters */
        source_error(ctx->source->file,
                     ctx->source->size,
                     ctx->source->data,
                     &tok.loc,
                     "${bold}Unknown token '"SV_FMT"'.", SV_ARG(tok.lexeme));
        return LEX_TOK_ERR;
    }

    token_push(ctx, &tok);

    return LEX_OK;
}

int lex(struct compile *ctx)
{
    assert_nonnull(ctx);
    assert_nonnull(ctx->source);
    assert_nonnull(ctx->lexer);

    /* Skip the UTF-8 BOM if present. */
    if (ctx->source->size >= 3
        && memcmp(ctx->source->data, "\xEF\xBB\xBF", 3) == 0)
    {
        ctx->lexer->index += 3;
    }

    bool had_error = false;

    while (!at_eof(ctx))
    {
        const enum lexer_status status = scan_token(ctx);
        if (status == LEX_OK)
            {} /* Nothing to do */
        else if (status == LEX_TOK_ERR)
        {
            had_error = true;
        }
        else if (status == LEX_UTF8_ERR)
        {
            had_error = true;
            break;
        }
    }

    struct token tok = {
        .loc = { .start = ctx->lexer->col, .end = ctx->lexer->col, .line = ctx->lexer->line },
        .type = TOKEN_EOF,
        .lexeme = { .size = 3, .str = "EOF" }
    };
    token_push(ctx, &tok);

    return had_error ? -1 : 0;
}

struct lexer *lexer_create(void)
{
    struct lexer *lexer = xmalloc(sizeof(*lexer));
    lexer->index = 0;
    lexer->line = 1;
    lexer->col = 1;
    return lexer;
}

void lexer_destroy(struct lexer **lexer)
{
    assert_nonnull(lexer);

    xfree((void **)lexer);
}
