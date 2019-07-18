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

  INF_VBOX main_box (root_window.window, FALSE, TRUE, FALSE);

  INF_HBOX button_box (main_box.vbox, FALSE);

  INF_HBOX sub_box (main_box.vbox, FALSE);
  INF_VBOX output_box (sub_box.hbox, TRUE, FALSE, TRUE);

  INF_RESULT_VIEW *result_display = new INF_RESULT_VIEW (output_box.vbox, combo_box_cb);

  INF_INPUT_VIEW *constants_display = new INF_INPUT_VIEW (sub_box.hbox);


  INF_BUTTON start_button (button_box.hbox, "START", start_button_cb, result_display);
  INF_BUTTON pause_button (button_box.hbox, "PAUSE", pause_button_cb, NULL);
  INF_BUTTON continue_button (button_box.hbox, "CONTINUE", continue_button_cb, NULL);
  INF_BUTTON populate_button (button_box.hbox, "GET SIM INFO", populate_button_cb, constants_display);
  INF_BUTTON retrieve_button (button_box.hbox, "SET SIM INFO", retrieve_button_cb, constants_display);

  g_timeout_add (50, timeout_loop, (gpointer) result_display);

  gtk_widget_show_all (root_window.window);

}
