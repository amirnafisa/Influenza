#ifndef __INTERFACE_CALLS__
#define __INTERFACE_CALLS__

#include "mpi.h"
#include <mpi_interface.h>
#include "simulation_calls.h"

void start_button_cb (GtkWidget*btn, gpointer user_data);
void pause_button_cb (GtkWidget*btn, gpointer user_data);
void continue_button_cb (GtkWidget*btn, gpointer user_data);
void combo_box_cb (GtkComboBox *combo_box, gpointer user_data);
void update_trends(INF_TREE_VIEW* view);
gint timeout_loop(gpointer view);
void exit ();

#endif
