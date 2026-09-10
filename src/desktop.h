/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glib.h>

G_BEGIN_DECLS

typedef struct {
    char *name;
    char *exec;
    char *icon;
    char *desktop_path;
} SlatetopApp;

GList *slatetop_app_list_load(void);
void slatetop_app_list_free(GList *list);

G_END_DECLS
