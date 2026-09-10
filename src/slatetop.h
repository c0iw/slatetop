/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SLATETOP_TYPE_APPLICATION (slatetop_application_get_type())

G_DECLARE_FINAL_TYPE(SlatetopApplication, slatetop_application,
                     SLATETOP, APPLICATION, GtkApplication)

SlatetopApplication *slatetop_application_new(void);

G_END_DECLS
