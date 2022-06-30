/* raider.c
 *
 * Copyright 2022 Alan Beveridge
 *
 * raider is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * raider is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <gtk/gtk.h>
#include "raider.h"
#include "raider-window.h"
#include "raider-preferences.h"

#include <glib/gi18n.h>

G_DEFINE_TYPE (Raider, raider, GTK_TYPE_APPLICATION);

static GActionEntry app_entries[] =
{
    { "about", about_activated, NULL, NULL, NULL },
    { "new_window", new_window_activated, NULL, NULL, NULL },
    { "help", help_activated, NULL, NULL, NULL },
    { "quit", quit_activated, NULL, NULL, NULL },
    { "preferences", preferences_activated, NULL, NULL, NULL }

};

/* Create a new window loading a file */
static void
raider_new_window (GApplication *app, GFile *file)
{
    RaiderWindow *window = raider_window_new(RAIDER_APPLICATION(app));
    gtk_window_present(GTK_WINDOW(window));
}

/* GApplication implementation */
static void
raider_activate (GApplication *application)
{
    raider_new_window (application, NULL);
}

static void
raider_open (GApplication  *application,
             GFile        **files,
             gint           n_files,
             const gchar   *hint)
{
    RaiderWindow *window = raider_window_new(RAIDER_APPLICATION(application));
    gint i;

    for (i = 0; i < n_files; i++)
    {
        gchar *path = g_file_get_path(files[i]);
        raider_window_open(path, window);
    }

    gtk_window_present (GTK_WINDOW(window));
}

static void
raider_init (Raider *object)
{
}

static void
raider_finalize (GObject *object)
{
    G_OBJECT_CLASS (raider_parent_class)->finalize (object);
}

static void
raider_startup (GApplication *app)
{

    const char *quit_accels[] = { "<Ctrl>Q", NULL };

    G_APPLICATION_CLASS (raider_parent_class)->startup (app);

    g_action_map_add_action_entries (G_ACTION_MAP (app),
                                     app_entries, G_N_ELEMENTS (app_entries),
                                     app);

    gtk_application_set_accels_for_action (GTK_APPLICATION (app),
                                           "app.quit",
                                           quit_accels);
}
static void
raider_class_init (RaiderClass *klass)
{
    G_APPLICATION_CLASS (klass)->activate = raider_activate;
    G_APPLICATION_CLASS (klass)->open = raider_open;
    G_APPLICATION_CLASS (klass)->startup = raider_startup;
    G_OBJECT_CLASS (klass)->finalize = raider_finalize;
}

Raider *
raider_new (void)
{
    return g_object_new (raider_get_type (),
                         "application-id", "com.github.ADBeveridge.Raider",
                         "flags", G_APPLICATION_HANDLES_OPEN,
                         NULL);
}



void
about_activated (GSimpleAction *simple,
                 GVariant      *parameter,
                 gpointer       user_data)
{
    GtkApplication *app = GTK_APPLICATION (user_data);
    GtkWindow *win = gtk_application_get_active_window(app);

    const gchar *artists[] =
    {
        "noëlle",
        NULL
    };
    const gchar *authors[] =
    {
        "Alan Beveridge",
        NULL
    };

    g_autofree gchar *program_name = g_strdup("File Shredder");

    gtk_show_about_dialog (win,
                           "transient-for", win,
                           "modal", TRUE,
                           "program-name", program_name,
                           "version", _("0.1.0"),
                           "comments", _("Securely delete your files"),
                           "license-type", GTK_LICENSE_GPL_3_0,
                           "artists", artists,
                           "authors", authors,
                           "translator-credits", _("translator-credits"),
                           "logo-icon-name", "com.github.ADBeveridge.Raider",
                           NULL);
}

void
new_window_activated (GSimpleAction *simple,
                      GVariant *parameter,
                      gpointer user_data)
{
    RaiderWindow *window = raider_window_new(RAIDER_APPLICATION(user_data));
    gtk_window_present (GTK_WINDOW(window));
}

void
help_activated (GSimpleAction *simple,
                GVariant *parameter,
                gpointer user_data)
{
    GError *error = NULL;
    GtkWindow *current_window = gtk_application_get_active_window(GTK_APPLICATION(user_data));

    gtk_show_uri_on_window (current_window, "help:raider", GDK_CURRENT_TIME, &error);

    if (error)
    {
        g_printerr("%s", error->message);
    }
    else
    {
        return;
    }
}

void
quit_activated (GSimpleAction *simple,
                GVariant *parameter,
                gpointer user_data)
{
    GtkWindow *current_window = gtk_application_get_active_window (GTK_APPLICATION(user_data));
    gtk_window_close(current_window);
}

void
preferences_activated (GSimpleAction *simple,
                       GVariant *parameter,
                       gpointer user_data)
{
    RaiderPreferences *prefs;
    GtkWindow *win;

    win = gtk_application_get_active_window (GTK_APPLICATION (user_data));
    prefs = raider_preferences_new (RAIDER_WINDOW (win));
    gtk_window_present (GTK_WINDOW (prefs));
}
