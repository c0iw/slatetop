/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include "config.h"

static char *
config_file_path(void)
{
    return g_build_filename(g_get_user_config_dir(), "slatetop", "slatetop.conf", NULL);
}

static void
slatetop_config_set_defaults(SlatetopConfig *c)
{
    c->font             = g_strdup("Open Sans 10");
    c->window_width     = 800;
    c->window_height    = 600;
    c->fullscreen       = TRUE;
    c->columns          = 4;
    c->bg_color         = g_strdup("#2e2e2e");
    c->fg_color         = g_strdup("#ffffff");
    c->selection_color  = g_strdup("#b4b4b4");
    c->opacity          = 0.85;
    c->font_size   = 14;
    c->label_size  = 14;
    c->icon_size = 64;
}

SlatetopConfig *
slatetop_config_load(void)
{
    SlatetopConfig *c;
    g_autofree char *path = config_file_path();
    g_autoptr(GKeyFile) kf = NULL;

    c = g_new0(SlatetopConfig, 1);
    slatetop_config_set_defaults(c);

    if (!g_file_test(path, G_FILE_TEST_EXISTS))
        return c;

    kf = g_key_file_new();
    if (!g_key_file_load_from_file(kf, path, G_KEY_FILE_NONE, NULL))
        return c;

    g_free(c->font);
    c->font = g_key_file_get_string(kf, "Appearance", "font", NULL);
    if (c->font == NULL)
        c->font = g_strdup("Open Sans 10");

    g_free(c->bg_color);
    c->bg_color = g_key_file_get_string(kf, "Appearance", "background", NULL);
    if (c->bg_color == NULL)
        c->bg_color = g_strdup("#2e2e2e");

    g_free(c->fg_color);
    c->fg_color = g_key_file_get_string(kf, "Appearance", "foreground", NULL);
    if (c->fg_color == NULL)
        c->fg_color = g_strdup("#ffffff");

    g_free(c->selection_color);
    c->selection_color = g_key_file_get_string(kf, "Appearance", "selection", NULL);
    if (c->selection_color == NULL)
        c->selection_color = g_strdup("#b4b4b4");

    c->opacity = g_key_file_get_double(kf, "Appearance", "opacity", NULL);
    if (c->opacity <= 0.0)
        c->opacity = 0.85;

    c->columns = g_key_file_get_integer(kf, "Grid", "columns", NULL);
    if (c->columns <= 0)
        c->columns = 4;

    c->fullscreen = g_key_file_get_boolean(kf, "Window", "fullscreen", NULL);
    c->window_width = g_key_file_get_integer(kf, "Window", "width", NULL);
    if (c->window_width <= 0)
        c->window_width = 800;
    c->window_height = g_key_file_get_integer(kf, "Window", "height", NULL);
    if (c->window_height <= 0)
        c->window_height = 600;

    c->font_size = g_key_file_get_integer(kf, "Appearance", "font_size", NULL);
    if (c->font_size <= 0)
        c->font_size = 14;

    c->label_size = g_key_file_get_integer(kf, "Appearance", "label_size", NULL);
    if (c->label_size <= 0)
        c->label_size = 14;

    c->icon_size = g_key_file_get_integer(kf, "Appearance", "icon_size", NULL);
    if (c->icon_size <= 0)
       c->icon_size = 64;

    return c;
}

void
slatetop_config_free(SlatetopConfig *c)
{
    if (c == NULL)
        return;

    g_free(c->font);
    g_free(c->bg_color);
    g_free(c->fg_color);
    g_free(c->selection_color);
    g_free(c);
}
