#include "interface_calls.h"

SIMULATION_RUNS runs;

void start_button_cb (GtkWidget*btn, gpointer user_data) {
  runs.start_new_run((INF_RESULT_VIEW*) user_data);
}

void pause_button_cb (GtkWidget*btn, gpointer user_data) {
  runs.pause_run();
}

void continue_button_cb (GtkWidget*btn, gpointer user_data) {
  runs.continue_run();
}

void populate_button_cb (GtkWidget*btn, gpointer user_data) {
  gdouble data[CONSTANTS_SIZE];
  runs.get_flu_manager_data(&data[0]);
  ((INF_INPUT_VIEW*)user_data)->populate_entries(&data[0]);
}

void retrieve_button_cb (GtkWidget*btn, gpointer user_data) {
  gdouble data[CONSTANTS_SIZE];

  ((INF_INPUT_VIEW*)user_data)->get_data_from_entries(&data[0]);
  runs.set_flu_manager_data(&data[0]);
}

void combo_box_cb (GtkComboBox *combo_box, gpointer user_data)
{
  if (gtk_combo_box_get_active (combo_box) != 0) {
    GtkWidget* da = (GtkWidget*) user_data;
    gint k = gtk_combo_box_get_active (combo_box);

    PlotData* data = new (PlotData);
    data->n_entries = runs.current_idx;
    for (gint i = 0; i < runs.current_idx - 1; i++) {
      //Convert from cumulative values to non-cumulative
      data->y[i] = (PLFLT)(runs.current_trends[i+1][k]-runs.current_trends[i][k]);
    }

    data->y_label = "Number of Cases";
    data->title = "Influenza Plots";
    create_drawing_window(da, data);
  }
}

gint timeout_loop(gpointer text_view) {


  //Check if already finished
  if (runs.state == RUNNING) {
    MPI_Test(&runs.recv_req, &runs.flag, &runs.mpi_status);
    if (runs.flag == 1) {
      runs.flag = 0;
      if (runs.current_trends[runs.current_idx][0] == -1) {
        runs.stop_run();
        combo_box_cb (GTK_COMBO_BOX(((INF_RESULT_VIEW*)text_view)->combo_box), (gpointer) ((INF_RESULT_VIEW*)text_view)->da);
      } else {
        update_trends((INF_RESULT_VIEW*) text_view);
        runs.get_trend_data_from_run();
      }
    }
  }
  return 1;
}

void update_trends(INF_RESULT_VIEW* text_view) {
  text_view->insert_data (runs.current_trends[runs.current_idx]);
  runs.current_idx++;
}

void exit ()
{
  runs.exit_process ();
}
