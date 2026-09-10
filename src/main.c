/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include <gtk/gtk.h>
#include "slatetop.h"
#include "desktop.h"

int
main (int argc, char *argv[])
{
  g_autoptr(SlatetopApplication) app = NULL;
  g_autoptr(GList) list = NULL;
  GList *l;
  int status;

  list = slatetop_app_list_load ();
  for (l = list; l != NULL; l = l->next) {
    SlatetopApp *a = l->data;
    g_print ("%s  ->  %s\n", a->name, a->exec);
  }

  app = slatetop_application_new ();
  status = g_application_run (G_APPLICATION (app), argc, argv);

  return status;
}
