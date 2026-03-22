#include "log/log.h"

#include "error.h"

#include <stdarg.h>
#include <stdio.h>
#include <time.h>

#include "poison.h"

struct fx_log {
    FILE *out;
    fx_log_level_t level;
};

fx_log_t *fx_log_init(TALLOC_CTX *ctx, FILE *out, fx_log_level_t level)
{
    fx_log_t *log = talloc_zero(ctx, fx_log_t);
    if (!log) PANIC("Out of memory"); // LCOV_EXCL_BR_LINE

    log->out = out;
    log->level = level;
    return log;
}

void fx_log_info(fx_log_t *log, const char *fmt, ...)
{
    if (FX_LOG_INFO < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    fprintf(log->out, "%s INFO ", ts);
    va_list args;
    va_start(args, fmt);
    vfprintf(log->out, fmt, args);
    va_end(args);
    fprintf(log->out, "\n");
}

void fx_log_warn(fx_log_t *log, const char *fmt, ...)
{
    if (FX_LOG_WARN < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    fprintf(log->out, "%s WARN ", ts);
    va_list args;
    va_start(args, fmt);
    vfprintf(log->out, fmt, args);
    va_end(args);
    fprintf(log->out, "\n");
}

void fx_log_error(fx_log_t *log, const char *fmt, ...)
{
    if (FX_LOG_ERROR < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    fprintf(log->out, "%s ERROR ", ts);
    va_list args;
    va_start(args, fmt);
    vfprintf(log->out, fmt, args);
    va_end(args);
    fprintf(log->out, "\n");
}

#ifdef DEBUG
void fx_log_debug(fx_log_t *log, const char *fmt, ...)
{
    if (FX_LOG_DEBUG < log->level) {
        return;
    }

    time_t now = time(NULL);
    struct tm tm_buf;
    gmtime_r(&now, &tm_buf);
    char ts[32];
    strftime(ts, sizeof(ts), "%Y-%m-%dT%H:%M:%SZ", &tm_buf);

    fprintf(log->out, "%s DEBUG ", ts);
    va_list args;
    va_start(args, fmt);
    vfprintf(log->out, fmt, args);
    va_end(args);
    fprintf(log->out, "\n");
}

#endif
