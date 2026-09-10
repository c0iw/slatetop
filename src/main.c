/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include <gtk/gtk.h>
#include "slatetop.h"
#include "desktop.h"
#include "cache.h"

int
main (int argc, char *argv[])
{
  g_autoptr(SlatetopApplication) app = NULL;
  GList *list;
  int status;

  if (slatetop_cache_is_valid()) {
    list = slatetop_cache_load();
    g_print("[cache] loaded\n");
  } else {
    list = slatetop_app_list_load();
    slatetop_cache_save(list);
    g_print("[cache] rebuilt\n");
  }

  slatetop_app_list_free(list);

  app = slatetop_application_new();
  status = g_application_run(G_APPLICATION(app), argc, argv);

  return status;
}
