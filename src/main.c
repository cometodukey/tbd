#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include <tbdc/core/compile.h>
#include <tbdc/lib/opts.h>

int main(int argc, const char *argv[])
{
    const struct opts *opts = parse_opts(argc, argv);
    if (opts == NULL)
        return EXIT_FAILURE;

    switch (opts->cmd)
    {
        case OPT_COMPILE:
            compile(opts);
            break;

        case OPT_EXPLAIN:
            break;

        case OPT_HELP:
            // usage();
            break;

        case OPT_OTHER: /* FALLTHROUGH */
        default:
            // usage();
            break; // TODO: Handle error here
    }

    return EXIT_SUCCESS;
}
