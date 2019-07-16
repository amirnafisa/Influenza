#include "interface_main.h"

gint run_gui_interface(gint argc, gchar* argv[]) {
  GtkApplication *app;
  gint gtk_status;

  app = gtk_application_new ("org.gtk.example", G_APPLICATION_FLAGS_NONE);
  g_signal_connect (app, "activate", G_CALLBACK (activate), NULL);

  gtk_status = g_application_run (G_APPLICATION (app), argc, argv);
  g_object_unref (app);

  return 0;
}

static void activate (GtkApplication* app, gpointer user_data)
{

  INF_MAINWINDOW root_window (app, "Influenza Simulation for Michigan");

  INF_VBOX main_box (root_window.window);
  INF_HBOX button_box (main_box.vbox);
  INF_HBOX output_box (main_box.vbox);

  INF_TEXT *result_display = new INF_TEXT (output_box.hbox);

  INF_BUTTON start_button (button_box.hbox, "START", start_button_cb, result_display);
  INF_BUTTON pause_button (button_box.hbox, "PAUSE", pause_button_cb, NULL);
  INF_BUTTON continue_button (button_box.hbox, "CONTINUE", continue_button_cb, NULL);

  g_timeout_add (50, timeout_loop, (gpointer) result_display);

  gtk_widget_show_all (root_window.window);

}
