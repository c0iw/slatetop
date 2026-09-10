/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include "cache.h"
#include "desktop.h"

#include <glib/gstdio.h>
#include <time.h>

static const char *const app_dirs[] = {
    "/usr/share/applications",
    "/usr/local/share/applications",
    NULL
};

static char *
cache_file_path(void)
{
    return g_build_filename(g_get_user_cache_dir(), "slatetop", "apps.cache", NULL);
}

static time_t
current_timestamp(void)
{
    time_t t = 0;
    int i;
    g_autofree char *user_dir = NULL;
    GStatBuf st;

    for (i = 0; app_dirs[i] != NULL; i++) {
        if (g_stat(app_dirs[i], &st) == 0)
            t += st.st_mtime;
    }

    user_dir = g_build_filename(g_get_home_dir(), ".local/share/applications", NULL);
    if (g_stat(user_dir, &st) == 0)
        t += st.st_mtime;

    return t;
}

gboolean
slatetop_cache_is_valid(void)
{
    g_autofree char *path = cache_file_path();
    g_autoptr(GKeyFile) kf = NULL;
    g_autofree char *ts_str = NULL;
    time_t saved;
    time_t current;

    if (!g_file_test(path, G_FILE_TEST_EXISTS))
        return FALSE;

    kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, NULL))
        return FALSE;

    ts_str = g_key_file_get_string(kf, "Cache", "timestamp", NULL);
    if (ts_str == NULL)
        return FALSE;

    saved = (time_t)g_ascii_strtoll(ts_str, NULL, 10);
    current = current_timestamp();

    return saved == current;
}

GList *
slatetop_cache_load(void)
{
    g_autofree char *path = cache_file_path();
    g_autoptr(GKeyFile) kf = NULL;
    GList *list = NULL;
    int count;
    int i;

    kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, NULL))
        return NULL;

    count = g_key_file_get_integer(kf, "Cache", "count", NULL);
    for (i = 0; i < count; i++) {
        g_autofree char *group = g_strdup_printf("app%d", i);
        SlatetopApp *app = g_new0(SlatetopApp, 1);

        app->name = g_key_file_get_string(kf, group, "name", NULL);
        app->exec = g_key_file_get_string(kf, group, "exec", NULL);
        app->icon = g_key_file_get_string(kf, group, "icon", NULL);
        app->desktop_path = g_key_file_get_string(kf, group, "desktop_path", NULL);

        list = g_list_prepend(list, app);
    }

    return g_list_reverse(list);
}

void
slatetop_cache_save(GList *list)
{
    g_autofree char *path = cache_file_path();
    g_autofree char *dir = g_path_get_dirname(path);
    g_autoptr(GKeyFile) kf = NULL;
    g_autofree char *data = NULL;
    gsize len;
    GList *l;
    int i = 0;

    g_mkdir_with_parents(dir, 0755);

    kf = g_key_file_new();

    g_autofree char *ts_str = g_strdup_printf("%ld", (long)current_timestamp());
    g_key_file_set_string(kf, "Cache", "timestamp", ts_str);
    g_key_file_set_integer(kf, "Cache", "count", g_list_length(list));

    for (l = list; l != NULL; l = l->next, i++) {
        SlatetopApp *app = l->data;
        g_autofree char *group = g_strdup_printf("app%d", i);

        g_key_file_set_string(kf, group, "name", app->name);
        g_key_file_set_string(kf, group, "exec", app->exec);
        if (app->icon != NULL)
            g_key_file_set_string(kf, group, "icon", app->icon);
        g_key_file_set_string(kf, group, "desktop_path", app->desktop_path);
    }

    data = g_key_file_to_data(kf, &len, NULL);
    g_file_set_contents(path, data, len, NULL);
}
