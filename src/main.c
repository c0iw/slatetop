/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include <gtk/gtk.h>
#include <string.h>
#include "slatetop.h"
#include "desktop.h"
#include "cache.h"

int
main (int argc, char *argv[])
{
  int i;
  gboolean reload_only = FALSE;

  for (i = 1; i < argc; i++) {
    if (g_strcmp0(argv[i], "--reload-apps") == 0)
      reload_only = TRUE;
  }

  if (reload_only) {
    GList *list = slatetop_app_list_load();
    slatetop_cache_save(list);
    slatetop_app_list_free(list);
    g_print("Cache rebuilt.\n");
    return 0;
  }

  g_autoptr(SlatetopApplication) app = NULL;
  int status;

  app = slatetop_application_new();
  status = g_application_run(G_APPLICATION(app), argc, argv);

  return status;
}
