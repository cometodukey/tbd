#include <tbdc/frontend/parser.h>
#include <tbdc/lib/assert.h>
#include <tbdc/lib/error.h>
#include <tbdc/lib/log.h>
#include <tbdc/lib/string.h>
#include <tbdc/lib/xmalloc.h>

struct parser *parser_create(void)
{
    struct parser *parser = xmalloc(sizeof(*parser));
    parser->index = 0;
    return parser;
}

void parser_destroy(struct parser **parser)
{
    xfree((void **)parser);
}

static int parser_error(const struct compile *ctx, const char *expected, const struct token *tok)
{
    if (tok->type == TOKEN_EOF)
        source_error(ctx->source->file, ctx->source->size, ctx->source->data, &tok->loc,
                     "Unexpected end-of-file. Expected %s.", expected);
    else
        source_error(ctx->source->file, ctx->source->size, ctx->source->data, &tok->loc,
                     "Expected %s but found '"SV_FMT"'.", expected, SV_ARG(tok->lexeme));
    return -1;
}

/**
 * Read the next token but do not advance the parser.
 */
static const struct token *peek(struct compile *ctx)
{
    /* `consume()` unconditionally advances the parser so we use assert here to check that we never `consume()` after EOF. */
    assert_le(ctx->parser->index, ctx->tokens->size);

    return &ctx->tokens->token[ctx->parser->index];
}

/**
 * Consume the next token. Advances the parser.
 */
static const struct token *consume(struct compile *ctx)
{
    const struct token *tok = peek(ctx);
    ctx->parser->index++;
    return tok;
}

static int parse_func_params(struct compile *ctx)
{
    size_t count = 0;

    while (peek(ctx)->type != TOKEN_RPAREN)
    {
        const struct token *name = consume(ctx);
        if (name->type != TOKEN_IDENT)
            return parser_error(ctx, "identifier", name);

        const struct token *colon = consume(ctx);
        if (colon->type != TOKEN_COLON)
            return parser_error(ctx, "':'", colon);

        const struct token *type = consume(ctx);
        if (type->type != TOKEN_IDENT)
            return parser_error(ctx, "type", type);

        DEBUG("Parameter "SIZE_FMT": { name: "SV_FMT", type: "SV_FMT" }",
              SIZE_ARG(count++), SV_ARG(name->lexeme), SV_ARG(type->lexeme));

        const struct token *next = peek(ctx);
        if (next->type == TOKEN_COMMA)
            consume(ctx); /* Ignore the result. */
        else if (next->type == TOKEN_RPAREN)
            continue;
        else
            return parser_error(ctx, "')', or ','", next);
    }

    return 0;
}

static int parse_expr(struct compile *ctx)
{
    compile_error(ctx->source->file, "Expression parsing is not implemented");
    return -1;
}

static int parse_return(struct compile *ctx)
{
    const struct token *next = peek(ctx);

    if (next->type == TOKEN_SEMICOLON)
        return 0; /* End of return */

    if (parse_expr(ctx) == -1)
        return -1;

    const struct token *semicolon = consume(ctx);
    if (semicolon->type != TOKEN_SEMICOLON)
        return parser_error(ctx, "';'", semicolon);

    return 0;
}

static int parse_stmt(struct compile *ctx)
{
    const struct token *tok = consume(ctx);

    switch (tok->type)
    {
        case TOKEN_RETURN:
            return parse_return(ctx);
        default:
            return parser_error(ctx, "'return'", tok);
    }
}

static int parse_stmts(struct compile *ctx)
{
    /* FIXME */
    return 0;
    return parse_stmt(ctx);
}

static int parse_func(struct compile *ctx)
{
    const struct token *name = consume(ctx);
    if (name->type != TOKEN_IDENT)
        return parser_error(ctx, "identifier", name);

    const struct token *lparen = consume(ctx);
    if (lparen->type != TOKEN_LPAREN)
        return parser_error(ctx, "'('", lparen);

    if (parse_func_params(ctx) == -1)
        return -1;

    const struct token *rparen = consume(ctx);
    if (rparen->type != TOKEN_RPAREN)
        return parser_error(ctx, "')'", rparen);

    const struct token *colon = consume(ctx);
    if (colon->type != TOKEN_COLON)
        return parser_error(ctx, "':'", colon);

    /* TODO: Parse return type */
    const struct token *ret = consume(ctx);
    if (ret->type != TOKEN_IDENT)
        return parser_error(ctx, "type", ret);

    const struct token *lbrace = consume(ctx);
    if (lbrace->type != TOKEN_LBRACE)
        return parser_error(ctx, "'{'", lbrace);

    if (parse_stmts(ctx) == -1)
        return -1;

    const struct token *rbrace = consume(ctx);
    if (rbrace->type != TOKEN_RBRACE)
        return parser_error(ctx, "'}'", rbrace);

    // ast_func_create(ctx, &name->lexeme);

    return 0;
}

int parse(struct compile *ctx)
{
    const struct token *tok;
    while ((tok = consume(ctx))->type != TOKEN_EOF)
    {
        switch (tok->type)
        {
            case TOKEN_FUNC:
                if (parse_func(ctx) == -1)
                    return -1;
                break;
            default:
                return parser_error(ctx, "'func'", tok);
        }
    }

    return -1;
}
