#ifndef __FORMAT_SPECIFIERS_H__
#define __FORMAT_SPECIFIERS_H__

#include <tbdc/lib/platform.h>

#if defined(PLATFORM_WINDOWS)

/* Building on Windows. Format specifiers pulled from here - https://docs.microsoft.com/en-us/previous-versions/tcxf1dw6(v=vs.140)?redirectedfrom=MSDN */

#define SIZE_FMT "%Iu"

#else

/* Building on a sane platform. Uses standard C99 format specifiers. */

#define SIZE_FMT "%zu"

#endif

#endif /* __FORMAT_SPECIFIERS_H__ */
