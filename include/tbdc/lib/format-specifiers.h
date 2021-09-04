#ifndef __FORMAT_SPECIFIERS_H__
#define __FORMAT_SPECIFIERS_H__

#include <stdint.h>
#include <limits.h>

#include <tbdc/lib/platform.h>

#if defined(PLATFORM_WINDOWS)

/* Building on Windows.
 * Some of the preferred format specifiers for this platform are non-standard and using them with -Wpedantic generates a warning.
 * https://docs.microsoft.com/en-us/previous-versions/tcxf1dw6(v=vs.140)?redirectedfrom=MSDN
 */

/* Here lie hacks. */

#if SIZE_MAX <= ULONG_MAX
# define SIZE_FMT "%lu"
# define SIZE_ARG(x) ((unsigned long)x)
#else
# error SIZE_MAX and SIZE_ARG are not implemented for this precision of size_t
#endif

#else

/* Building on a sane platform. Uses standard C99 format specifiers. */

#define SIZE_FMT "%zu"
#define SIZE_ARG(x) x

#endif

#endif /* __FORMAT_SPECIFIERS_H__ */
