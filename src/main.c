/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include <gtk/gtk.h>
#include "slatetop.h"

int
main (int argc, char *argv[])
{
  g_autoptr(SlatetopApplication) app = NULL;
  int status;

  app = slatetop_application_new ();
  status = g_application_run (G_APPLICATION (app), argc, argv);

  return status;
}
