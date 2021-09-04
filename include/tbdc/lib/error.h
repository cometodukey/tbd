#ifndef __ERROR_H__
#define __ERROR_H__

#include <tbdc/frontend/tokens.h>
#include <tbdc/lib/format-specifiers.h>

void source_error(const char *file,
                  const size_t size,
                  const char *data,
                  const struct loc *loc,
                  const char *fmt, ...);
void compile_error(const char *file, const char *reason);

#endif /* __ERROR_H__ */
