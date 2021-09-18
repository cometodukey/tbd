#ifndef __OPTS_H__
#define __OPTS_H__

#include <stddef.h>

enum opt_cmd
{
    OPT_COMPILE,
    OPT_EXPLAIN,
    OPT_HELP,

    OPT_OTHER
};

struct opts
{
    enum opt_cmd cmd;
    const char *path;
    size_t argc;
    const char **argv;
};

const struct opts *parse_opts(size_t argc, const char **argv);

#endif /* __OPTS_H__ */
