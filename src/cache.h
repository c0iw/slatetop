/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <glib.h>

G_BEGIN_DECLS

gboolean slatetop_cache_is_valid(void);
GList   *slatetop_cache_load(void);
void     slatetop_cache_save(GList *list);

G_END_DECLS
