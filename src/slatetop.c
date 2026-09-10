/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include "slatetop.h"

struct _SlatetopApplication
{
    GtkApplication parent_instance;
};

G_DEFINE_TYPE(SlatetopApplication, slatetop_application, GTK_TYPE_APPLICATION)

static void
slatetop_application_activate(GApplication *app)
{
    GtkWidget *window;

    window = gtk_application_window_new(GTK_APPLICATION(app));
    gtk_window_set_title(GTK_WINDOW(window), "Slatetop");
    gtk_window_set_default_size(GTK_WINDOW(window), 400, 300);
    gtk_widget_show_all(window);
}

static void
slatetop_application_class_init(SlatetopApplicationClass *klass)
{
    GApplicationClass *app_class = G_APPLICATION_CLASS(klass);

    app_class->activate = slatetop_application_activate;
}

static void
slatetop_application_init(SlatetopApplication *self G_GNUC_UNUSED)
{
}

SlatetopApplication *
slatetop_application_new(void)
{
    return g_object_new(SLATETOP_TYPE_APPLICATION,
                        "application-id", "org.slatetop.launcher",
                        "flags", G_APPLICATION_DEFAULT_FLAGS,
                        NULL);
}
