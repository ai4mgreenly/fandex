#ifndef FX_WATCH_H
#define FX_WATCH_H

#include "error.h"
#include "log/log.h"

#include <stdbool.h>
#include <stdint.h>
#include <talloc.h>

typedef struct fx_watch fx_watch_t;

// Initialize a fanotify watcher on the mount containing watch_path.
// Allocates fx_watch_t on ctx and logs at info level on success.
// Returns OK(fx_watch_t *) or ERR_IO on failure.
res_t fx_watch_init(TALLOC_CTX *ctx, fx_log_t *log, const char *watch_path);

// Poll loop: reads fanotify events, resolves paths, logs events under watch_path.
// Runs until g_shutdown is set.
void fx_watch_run(fx_watch_t *w);

// Close fanotify fd and free the struct. Logs at info level.
void fx_watch_free(fx_watch_t *w);

// Map a single fanotify event mask bit to a human-readable string.
// Returns NULL for unrecognised masks.
const char *fx_watch_event_name(uint32_t mask);

// Return true if path is under watch_path (exact match or path separator follows prefix).
// Handles trailing slashes on watch_path. Prevents false positives on prefix matches.
bool fx_watch_path_under(const char *watch_path, const char *path);

#endif // FX_WATCH_H
