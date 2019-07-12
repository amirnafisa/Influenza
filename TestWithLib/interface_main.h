#ifndef __INTERFACE_MAIN__
#define __INTERFACE_MAIN__

#include "simulation_calls.h"
#include "interface_calls.h"

static void activate (GtkApplication* app, gpointer user_data);
gint run_gui_interface(gint argc, gchar* argv[]);
static GtkWidget* add_button (const gchar text[], void (*func_cb)(GtkWidget*, gpointer), GtkWidget* text_buffer);
static GtkWidget* add_text(const gchar text[]);

#endif
