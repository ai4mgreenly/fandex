#ifndef FX_CONFIG_H
#define FX_CONFIG_H

typedef struct fx_config {
    char *watch_path;
    char *db_path;
    char *socket_path;
} fx_config_t;

// Load config: env vars override compiled-in defaults.
//   FANDEX_WATCH_PATH  (default: ~/projects)
//   FANDEX_DB_PATH     (default: ~/.local/state/fandex/fandex.db)
//   FANDEX_SOCKET_PATH (default: ~/.local/state/fandex/fandex.sock)
//
// Caller must call fx_config_free() when done.
fx_config_t *fx_config_load(void);

void fx_config_free(fx_config_t *cfg);

#endif
