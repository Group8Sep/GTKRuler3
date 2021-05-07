#include <gtk/gtk.h>
#include "gtkruler.c"
#include "gtkruler.h"

static void
activate (GtkApplication* app,
          gpointer        user_data)
{
    GtkWidget *window;
    GtkWidget *gtk_hruler_new();

    gtk_ruler_set_range(GTK_RULER(gtk_hruler_new), 0, 800, 0, 800);

    window = gtk_application_window_new (app);
    gtk_window_set_title (GTK_WINDOW (window), "Window");
    gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

    //ruler = g_object_new(_GtkRuler , NULL);

    //gtk_ruler_init(reinterpret_cast<GtkRuler *>(ruler));
    //gtk_container_add(GTK_CONTAINER(window), ruler);


    gtk_widget_show_all (window);
}

int
main (int    argc,
      char **argv)
{
    GtkApplication *app;
    int status;

    app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
    g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);
    status = g_application_run (G_APPLICATION (app), argc, argv);
    g_object_unref (app);

    return status;
}