#include "config/config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>

#include "poison.h"

static char *resolve_home(const char *suffix)
{
    const char *home = getenv("HOME");
    if (!home) {
        home = "/tmp";
    }

    size_t len = strlen(home) + strlen(suffix) + 1;
    char *path = malloc(len);
    if (!path) {
        return NULL;
    }

    (void)snprintf(path, len, "%s%s", home, suffix);
    return path;
}

static char *resolve_runtime(const char *suffix)
{
    uid_t uid = getuid();
    char buf[256];

    (void)snprintf(buf, sizeof(buf), "/run/user/%u%s", (unsigned)uid, suffix);
    return strdup(buf);
}

static char *env_or_home(const char *env_var, const char *default_suffix)
{
    const char *val = getenv(env_var);
    if (val) {
        return strdup(val);
    }
    return resolve_home(default_suffix);
}

static char *env_or_runtime(const char *env_var, const char *default_suffix)
{
    const char *val = getenv(env_var);
    if (val) {
        return strdup(val);
    }
    return resolve_runtime(default_suffix);
}

fx_config_t *fx_config_load(void)
{
    fx_config_t *cfg = calloc(1, sizeof(*cfg));
    if (!cfg) {
        return NULL;
    }

    cfg->watch_path   = env_or_home("FANDEX_WATCH_PATH",     "/projects");
    cfg->db_path      = env_or_home("FANDEX_DB_PATH",        "/.local/state/fandex/fandex.db");
    cfg->socket_path  = env_or_runtime("FANDEX_SOCKET_PATH", "/fandex/fandex.sock");

    return cfg;
}

void fx_config_free(fx_config_t *cfg)
{
    if (!cfg) {
        return;
    }

    free(cfg->watch_path);
    free(cfg->db_path);
    free(cfg->socket_path);
    free(cfg);
}
