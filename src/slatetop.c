/* Slatetop - application launcher for X11
 * Copyright (C) 2026 Akhmetshin Islam Rustemovich
 * SPDX-License-Identifier: MIT
 */

#include "slatetop.h"
#include "desktop.h"
#include "cache.h"
#include "config.h"

#include <string.h>

struct _SlatetopApplication
{
    GtkApplication  parent_instance;

    SlatetopConfig *config;
    GList          *apps;

    GtkWidget      *window;
    GtkWidget      *entry;
    GtkWidget      *flowbox;
};

G_DEFINE_TYPE(SlatetopApplication, slatetop_application, GTK_TYPE_APPLICATION)

static void
launch_app(SlatetopApp *app)
{
    g_autofree char *cmd = NULL;
    char *p;

    if (app == NULL || app->exec == NULL)
        return;

    cmd = g_strdup(app->exec);

    /* Убираем плейсхолдеры %u %U %f %F и т.д. */
    p = strstr(cmd, "%");
    while (p != NULL) {
        if (p[1] == 'u' || p[1] == 'U' || p[1] == 'f' || p[1] == 'F' ||
            p[1] == 'd' || p[1] == 'D' || p[1] == 'i' || p[1] == 'c' ||
            p[1] == 'k' || p[1] == 'v' || p[1] == 'm') {
            p[0] = ' ';
            p[1] = ' ';
        }
        p = strstr(p + 1, "%");
    }

    g_spawn_command_line_async(cmd, NULL);
    gtk_window_close(GTK_WINDOW(((SlatetopApplication *)g_application_get_default())->window));
}

static void
filter_apps(SlatetopApplication *self, const char *text)
{
    GList *children;
    GList *l;
    GtkFlowBoxChild *first_visible = NULL;

    children = gtk_container_get_children(GTK_CONTAINER(self->flowbox));

    for (l = children; l != NULL; l = l->next) {
        GtkWidget *child = GTK_WIDGET(l->data);
        SlatetopApp *app = g_object_get_data(G_OBJECT(child), "app");
        gboolean visible;

        if (text == NULL || *text == '\0') {
            visible = TRUE;
        } else {
            visible = (g_strstr_len(app->name, -1, text) != NULL);
        }

        gtk_widget_set_visible(child, visible);

        if (visible && first_visible == NULL)
            first_visible = GTK_FLOW_BOX_CHILD(child);
    }

    g_list_free(children);

    if (first_visible != NULL)
        gtk_flow_box_select_child(GTK_FLOW_BOX(self->flowbox), first_visible);
    else
        gtk_flow_box_unselect_all(GTK_FLOW_BOX(self->flowbox));
}

static void
on_entry_changed(GtkEntry *entry, gpointer user_data)
{
    SlatetopApplication *self = user_data;
    const char *text = gtk_entry_get_text(entry);

    filter_apps(self, text);
}

static void
on_child_activated(GtkFlowBox *box, GtkFlowBoxChild *child, gpointer user_data)
{
    SlatetopApp *app = g_object_get_data(G_OBJECT(child), "app");

    (void)box;
    (void)user_data;

    launch_app(app);
}

static gboolean
on_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
    SlatetopApplication *self = user_data;

    (void)widget;

    if (event->keyval == GDK_KEY_Escape) {
        gtk_window_close(GTK_WINDOW(self->window));
        return TRUE;
    }

    if (event->keyval == GDK_KEY_Return) {
        GList *selected = gtk_flow_box_get_selected_children(GTK_FLOW_BOX(self->flowbox));
        if (selected != NULL) {
            GtkFlowBoxChild *child = GTK_FLOW_BOX_CHILD(selected->data);
            SlatetopApp *app = g_object_get_data(G_OBJECT(child), "app");
            launch_app(app);
            g_list_free(selected);
            return TRUE;
        }
    }

    return FALSE;
}

static gboolean
on_draw_background(GtkWidget *widget, cairo_t *cr, gpointer user_data)
{
    SlatetopApplication *self = user_data;
    GtkAllocation alloc;
    GdkRGBA color;

    gtk_widget_get_allocation(widget, &alloc);

    if (!gdk_rgba_parse(&color, self->config->bg_color)) {
        color.red = 0.18;
        color.green = 0.18;
        color.blue = 0.18;
    }

    cairo_set_source_rgba(cr, color.red, color.green, color.blue,
                          self->config->opacity);
    cairo_rectangle(cr, 0, 0, alloc.width, alloc.height);
    cairo_fill(cr);

    return FALSE;
}

static char *
build_css(SlatetopConfig *c)
{
    return g_strdup_printf(
        "window { background-color: rgba(0,0,0,0); }"
        "entry { "
        "  background: transparent; "
        "  color: %s; "
        "  border: 1px solid %s; "
        "  border-radius: 6px; "
        "  padding: 10px; "
        "  font-family: Open Sans; "
        "  font-size: %dpx; "
        "}"
        "flowbox { background: transparent; }"
        "flowboxchild { border-radius: 8px; padding: 12px; margin: 4px; }"
        "flowboxchild:selected { background: rgba(180,180,180,0.25); }"
        ".app-label { color: %s; font-size: %dpx; margin-top: 4px; }",
        c->fg_color, c->fg_color, c->font_size, c->fg_color, c->label_size);
}

static void
populate_flowbox(SlatetopApplication *self)
{
    GList *l;

    for (l = self->apps; l != NULL; l = l->next) {
        SlatetopApp *app = l->data;
        GtkWidget *child;
        GtkWidget *box;
        GtkWidget *image;
        GtkWidget *label;

        child = gtk_flow_box_child_new();

        box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        gtk_widget_set_halign(box, GTK_ALIGN_CENTER);
        gtk_widget_set_valign(box, GTK_ALIGN_CENTER);

        if (app->icon != NULL)
            image = gtk_image_new_from_icon_name(app->icon, GTK_ICON_SIZE_DIALOG);
        else
            image = gtk_image_new_from_icon_name("application-x-executable",
                                                 GTK_ICON_SIZE_DIALOG);

       gtk_image_set_pixel_size(GTK_IMAGE(image), self->config->icon_size);
       gtk_box_pack_start(GTK_BOX(box), image, FALSE, FALSE, 0);

        label = gtk_label_new(app->name);
        gtk_label_set_xalign(GTK_LABEL(label), 0.5);
        gtk_label_set_ellipsize(GTK_LABEL(label), PANGO_ELLIPSIZE_END);
        gtk_widget_set_size_request(label, 100, -1);
        gtk_style_context_add_class(gtk_widget_get_style_context(label), "app-label");
        gtk_box_pack_start(GTK_BOX(box), label, FALSE, FALSE, 0);

        gtk_container_add(GTK_CONTAINER(child), box);
        g_object_set_data(G_OBJECT(child), "app", app);
        gtk_container_add(GTK_CONTAINER(self->flowbox), child);
    }
}

static void
on_window_destroy(GtkWidget *widget, gpointer user_data)
{
    SlatetopApplication *self = SLATETOP_APPLICATION(user_data);
    self->window = NULL;
    (void)widget;
}


static void
slatetop_application_activate(GApplication *app)
{
    SlatetopApplication *self = SLATETOP_APPLICATION(app);
    if (self->window != NULL) {
        gtk_window_present(GTK_WINDOW(self->window));
        return;
    }
    GtkWidget *vbox;
    GtkWidget *scrolled;
    GtkCssProvider *provider;
    char *css;
    GdkScreen *screen;
    GdkVisual *visual;

    /* --- Загрузка конфига и приложений --- */
    self->config = slatetop_config_load();

    if (slatetop_cache_is_valid())
        self->apps = slatetop_cache_load();
    else {
        self->apps = slatetop_app_list_load();
        slatetop_cache_save(self->apps);
    }

    /* --- Окно --- */
    self->window = gtk_application_window_new(GTK_APPLICATION(app));
    g_signal_connect(self->window, "destroy",
                 G_CALLBACK(on_window_destroy), self);
    gtk_window_set_title(GTK_WINDOW(self->window), "Slatetop");
    gtk_window_set_decorated(GTK_WINDOW(self->window), FALSE);

    if (self->config->fullscreen)
        gtk_window_fullscreen(GTK_WINDOW(self->window));
    else
        gtk_window_set_default_size(GTK_WINDOW(self->window),
                                    self->config->window_width,
                                    self->config->window_height);

    gtk_widget_set_app_paintable(self->window, TRUE);

    screen = gdk_screen_get_default();
    visual = gdk_screen_get_rgba_visual(screen);
    if (visual != NULL)
        gtk_widget_set_visual(self->window, visual);

    /* --- CSS --- */
    provider = gtk_css_provider_new();
    css = build_css(self->config);
    gtk_css_provider_load_from_data(provider, css, -1, NULL);
    g_free(css);

    gtk_style_context_add_provider_for_screen(
        screen,
        GTK_STYLE_PROVIDER(provider),
        GTK_STYLE_PROVIDER_PRIORITY_USER + 1);
    g_object_unref(provider);

    /* --- Поле поиска --- */
    self->entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(self->entry), "Поиск...");

    /* --- FlowBox --- */
    self->flowbox = gtk_flow_box_new();
    gtk_flow_box_set_selection_mode(GTK_FLOW_BOX(self->flowbox),
                                    GTK_SELECTION_BROWSE);
    gtk_flow_box_set_min_children_per_line(GTK_FLOW_BOX(self->flowbox),
                                           self->config->columns);
    gtk_flow_box_set_max_children_per_line(GTK_FLOW_BOX(self->flowbox),
                                           self->config->columns);
    gtk_flow_box_set_homogeneous(GTK_FLOW_BOX(self->flowbox), TRUE);
    gtk_flow_box_set_activate_on_single_click(GTK_FLOW_BOX(self->flowbox), FALSE);

    populate_flowbox(self);

    /* --- Компоновка --- */
    vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 20);
    gtk_box_pack_start(GTK_BOX(vbox), self->entry, FALSE, FALSE, 0);

    scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled),
                                   GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(scrolled), self->flowbox);
    gtk_box_pack_start(GTK_BOX(vbox), scrolled, TRUE, TRUE, 0);

    gtk_container_add(GTK_CONTAINER(self->window), vbox);

    /* --- Сигналы --- */
    g_signal_connect(self->entry, "changed",
                     G_CALLBACK(on_entry_changed), self);
    g_signal_connect(self->flowbox, "child-activated",
                     G_CALLBACK(on_child_activated), self);
    g_signal_connect(self->window, "key-press-event",
                     G_CALLBACK(on_key_press), self);
    g_signal_connect(self->window, "draw",
                     G_CALLBACK(on_draw_background), self);

    gtk_widget_grab_focus(self->entry);
    gtk_widget_show_all(self->window);

    /* Выделяем первый элемент */
    filter_apps(self, "");
}

static void
slatetop_application_dispose(GObject *object)
{
    SlatetopApplication *self = SLATETOP_APPLICATION(object);

    g_clear_pointer(&self->config, slatetop_config_free);
    g_clear_pointer(&self->apps, slatetop_app_list_free);

    G_OBJECT_CLASS(slatetop_application_parent_class)->dispose(object);
}

static void
slatetop_application_class_init(SlatetopApplicationClass *klass)
{
    GApplicationClass *app_class = G_APPLICATION_CLASS(klass);
    GObjectClass *obj_class = G_OBJECT_CLASS(klass);

    app_class->activate = slatetop_application_activate;
    obj_class->dispose = slatetop_application_dispose;
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
