#ifndef __LOG_H__
#define __LOG_H__

#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>

#include <tbdc/lib/format-specifiers.h>

#define BUG(...) log_bug(__VA_ARGS__)
#define DEBUG(...) log_debug(__VA_ARGS__)

enum log_type
{
    LOG_NONE,
    LOG_INFO,
    LOG_NOTE,
    LOG_PAD,
    LOG_DEBUG,
    LOG_SUCCESS,
    LOG_WARNING,
    LOG_ERROR,
    LOG_FATAL,
    LOG_BUG
};

void vlog(const enum log_type type, const char *fmt, va_list args, const bool newline);
void log_none(const char *fmt, ...);
void log_noneln(const char *fmt, ...);
void log_info(const char *fmt, ...);
void log_note(const char *fmt, ...);
void log_pad(const char *fmt, ...);
void log_padln(const char *fmt, ...);
void log_debug(const char *fmt, ...);
void log_success(const char *fmt, ...);
void log_warning(const char *fmt, ...);
void log_error(const char *fmt, ...);
void log_fatal(const char *fmt, ...);
void log_bug(const char *fmt, ...);

#endif /* __LOG_H__ */
