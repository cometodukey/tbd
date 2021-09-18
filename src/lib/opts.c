#include <stdio.h>

#include <tbdc/lib/opts.h>
#include <tbdc/lib/string.h>

const char *program_name; /* This is not static as it is referenced
                             in other compilation units for generating errors. */

static void usage(void)
{
    fprintf(stderr, "TODO\n");
}

const struct opts *parse_opts(size_t argc, const char **argv)
{
    program_name = *argv++;
    argc--;

    if (argc == 0)
    {
        usage();
        return NULL;
    }

    static struct opts opts = {
        .cmd = OPT_OTHER,
        .path = "."
    };

    const char *cmd = *argv++;
    // const char **args = argv;
    argc--;

    if (str_equal(cmd, "compile"))
        opts.cmd = OPT_COMPILE;
    else if (str_equal(cmd, "explain"))
        opts.cmd = OPT_EXPLAIN;
    else if (str_equal(cmd, "help"))
        opts.cmd = OPT_HELP;
    else
        opts.cmd = OPT_OTHER;

    opts.argc = argc;
    opts.argv = argv;

    return &opts;
}
