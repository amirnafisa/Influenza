#ifndef __INTERFACE_MAIN__
#define __INTERFACE_MAIN__

#include <gtk/gtk.h>
#include <iostream>
#include <string>
#include "interface_calls.h"

static void activate (GtkApplication* app, gpointer user_data);
int run_gui_interface(int argc, char* argv[]);
static GtkWidget* add_button (const gchar text[], void (*func_cb)());

#endif
