/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include "desktop.h"

#include <string.h>

static const char *const app_dirs[] = {
    "/usr/share/applications",
    "/usr/local/share/applications",
    NULL
};

static void
slatetop_app_free(SlatetopApp *app)
{
    if (app == NULL)
        return;

    g_free(app->name);
    g_free(app->exec);
    g_free(app->icon);
    g_free(app->desktop_path);
    g_free(app);
}

void
slatetop_app_list_free(GList *list)
{
    g_list_free_full(list, (GDestroyNotify)slatetop_app_free);
}

static SlatetopApp *
slatetop_app_new_from_file(const char *path)
{
    g_autoptr(GKeyFile) key_file = NULL;
    g_autofree char *no_display = NULL;
    SlatetopApp *app;

    key_file = g_key_file_new();
    if (!g_key_file_load_from_file(key_file, path, G_KEY_FILE_NONE, NULL))
        return NULL;

    no_display = g_key_file_get_string(key_file, "Desktop Entry",
                                       "NoDisplay", NULL);
    if (no_display != NULL && g_strcmp0(no_display, "true") == 0)
        return NULL;

    app = g_new0(SlatetopApp, 1);
    app->name = g_key_file_get_string(key_file, "Desktop Entry", "Name", NULL);
    app->exec = g_key_file_get_string(key_file, "Desktop Entry", "Exec", NULL);
    app->icon = g_key_file_get_string(key_file, "Desktop Entry", "Icon", NULL);
    app->desktop_path = g_strdup(path);

    if (app->name == NULL || app->exec == NULL) {
        slatetop_app_free(app);
        return NULL;
    }

    return app;
}

static void
slatetop_app_list_scan_dir(GList **list, const char *dir_path)
{
    g_autoptr(GDir) dir = NULL;
    const char *entry;

    dir = g_dir_open(dir_path, 0, NULL);
    if (dir == NULL)
        return;

    while ((entry = g_dir_read_name(dir)) != NULL) {
        g_autofree char *full_path = NULL;
        SlatetopApp *app;

        if (!g_str_has_suffix(entry, ".desktop"))
            continue;

        full_path = g_build_filename(dir_path, entry, NULL);
        app = slatetop_app_new_from_file(full_path);
        if (app != NULL)
            *list = g_list_prepend(*list, app);
    }
}

GList *
slatetop_app_list_load(void)
{
    GList *list = NULL;
    g_autofree char *user_dir = NULL;
    int i;

    for (i = 0; app_dirs[i] != NULL; i++)
        slatetop_app_list_scan_dir(&list, app_dirs[i]);

    user_dir = g_build_filename(g_get_home_dir(),
                                ".local/share/applications", NULL);
    slatetop_app_list_scan_dir(&list, user_dir);

    return g_list_reverse(list);
}
