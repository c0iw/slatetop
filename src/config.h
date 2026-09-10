/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glib.h>

G_BEGIN_DECLS

typedef struct {
    char    *font;
    int      font_size;
    int      label_size;
    int      window_width;
    int      window_height;
    gboolean fullscreen;
    int      columns;
    char    *bg_color;
    char    *fg_color;
    char    *selection_color;
    double   opacity;
} SlatetopConfig;

SlatetopConfig *slatetop_config_load(void);
void            slatetop_config_free(SlatetopConfig *config);

G_DEFINE_AUTOPTR_CLEANUP_FUNC(SlatetopConfig, slatetop_config_free)

G_END_DECLS
