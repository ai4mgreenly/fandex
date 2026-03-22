#ifndef FX_LOG_H
#define FX_LOG_H

#include <stdio.h>
#include <talloc.h>

typedef enum {
    FX_LOG_DEBUG = 0,
    FX_LOG_INFO,
    FX_LOG_WARN,
    FX_LOG_ERROR,
} fx_log_level_t;

typedef struct fx_log fx_log_t;

// Allocate a logger on ctx writing to out at the given level.
// OOM calls PANIC(). Caller owns the returned pointer (freed with ctx).
fx_log_t *fx_log_init(TALLOC_CTX *ctx, FILE *out, fx_log_level_t level);

void fx_log_info(fx_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void fx_log_warn(fx_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
void fx_log_error(fx_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));

#ifdef DEBUG
void fx_log_debug(fx_log_t *log, const char *fmt, ...) __attribute__((format(printf, 2, 3)));
#else
#define fx_log_debug(log, ...) ((void)0)
#endif

#endif // FX_LOG_H
