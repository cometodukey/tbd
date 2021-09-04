#include <tbdc/core/compile.h>
#include <tbdc/lib/assert.h>
#include <tbdc/lib/error.h>
#include <tbdc/lib/log.h>
#include <tbdc/lib/util.h>
#include <tbdc/lib/xmalloc.h>

struct tokens *tokens_create(void)
{
    struct tokens *tokens = xmalloc(sizeof(*tokens));
    tokens->size = 0;
    return tokens;
}

void tokens_destroy(struct tokens **tokens)
{
    xfree((void **)tokens);
}

void token_push(struct compile *ctx, const struct token *token)
{
    assert_nonnull(ctx);
    assert_nonnull(ctx->tokens);
    assert_nonnull(ctx->source);
    assert_nonnull(ctx->source->file);

    struct tokens *tokens = ctx->tokens;

    if (tokens->size >= lengthof(tokens->token))
    {
        compile_error(ctx->source->file, "Exhausted token scratch space");
    }

    tokens->token[tokens->size++] = *token;
}

static const char *stringify_type(const enum token_type type)
{
    #define ADD_CASE(type) case type: return #type;

    switch (type)
    {
        ADD_CASE(TOKEN_COLON)
        ADD_CASE(TOKEN_COMMA)
        ADD_CASE(TOKEN_SEMICOLON)
        ADD_CASE(TOKEN_LBRACE)
        ADD_CASE(TOKEN_RBRACE)
        ADD_CASE(TOKEN_LPAREN)
        ADD_CASE(TOKEN_RPAREN)
        ADD_CASE(TOKEN_MINUS)

        ADD_CASE(TOKEN_PLUS)

        ADD_CASE(TOKEN_FUNC)
        ADD_CASE(TOKEN_RETURN)

        ADD_CASE(TOKEN_IDENT)
        ADD_CASE(TOKEN_INT)

        ADD_CASE(TOKEN_EOF)

        default:
            BUG("%d is an unknown token type", type);
            return NULL;
    }

    #undef ADD_CASE
}

void tokens_dump(const struct tokens *tokens)
{
    assert_nonnull(tokens);

    for (size_t i = 0; i < tokens->size; i++)
    {
        const struct token *token = &tokens->token[i];
        DEBUG("Token "SIZE_FMT": { loc: { start: "SIZE_FMT", end "SIZE_FMT", line: "SIZE_FMT" }, type: %s (%d), lexeme: \"%.*s\" }",
                SIZE_ARG(i), SIZE_ARG(token->loc.start), SIZE_ARG(token->loc.end), SIZE_ARG(token->loc.line),
                stringify_type(token->type), token->type,
                token->lexeme.size, token->lexeme.str);
    }
}
