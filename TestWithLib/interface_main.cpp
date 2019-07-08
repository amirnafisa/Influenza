#include "interface_main.h"

extern int current_state;

int run_gui_interface(int argc, char* argv[]) {
  GtkApplication *app;
  int gtk_status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  current_state = STOPPED;

  gtk_status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return 0;
}

static void activate (GtkApplication* app, gpointer user_data)
{
  GtkWidget *main_grid;
  GtkWidget *window;
  GtkWidget *start_button, *pause_button, *continue_button;

  main_grid = gtk_grid_new();

  window = gtk_application_window_new (app);
  gtk_window_set_title (GTK_WINDOW (window), "Window");
  gtk_window_set_default_size (GTK_WINDOW (window), 200, 200);

  gtk_container_add(GTK_CONTAINER(window), main_grid);

  start_button = add_button("Start", start_run);
  gtk_grid_attach (GTK_GRID (main_grid), start_button, 0, 0, 1, 1);

  pause_button = add_button("Pause", pause_run);
  gtk_grid_attach (GTK_GRID (main_grid), pause_button, 1, 0, 1, 1);

  continue_button = add_button("Continue", continue_run);
  gtk_grid_attach (GTK_GRID (main_grid), continue_button, 2, 0, 1, 1);

  gtk_widget_show_all (window);

}

static GtkWidget* add_button (const gchar text[], void (*func_cb)())
{
  GtkWidget *button;

  button = gtk_button_new_with_label (text);
  g_signal_connect (button, "clicked", G_CALLBACK (func_cb), NULL);

  return button;
}
