#include "log/log.h"

#include "error.h"

#include "poison.h"

#include <pthread.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>

struct fx_log {
    FILE *out;
    fx_log_level_t level;
    pthread_mutex_t mutex;
};

// Talloc destructor: releases the mutex before the struct is freed.
static int log_destructor(fx_log_t *log)
{
    pthread_mutex_destroy(&log->mutex);
    return 0;
}

fx_log_t *fx_log_init(TALLOC_CTX *ctx, FILE *out, fx_log_level_t level)
{
    fx_log_t *log = talloc_zero(ctx, fx_log_t);
    if (!log) PANIC("Out of memory"); // LCOV_EXCL_BR_LINE

    log->out = out;
    log->level = level;
    pthread_mutex_init(&log->mutex, NULL);
    talloc_set_destructor(log, log_destructor);
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

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s INFO ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
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

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s WARN ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
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

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s ERROR ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
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

    va_list args;
    va_start(args, fmt);
    pthread_mutex_lock(&log->mutex);
    fprintf(log->out, "%s DEBUG ", ts);
    vfprintf(log->out, fmt, args);
    fprintf(log->out, "\n");
    pthread_mutex_unlock(&log->mutex);
    va_end(args);
}

#endif
