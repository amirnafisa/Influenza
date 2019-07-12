#include "interface_calls.h"

SIMULATION_RUNS runs;

void start_button_cb (GtkWidget*btn, gpointer user_data) {
  create_drawing_window(&runs.current_trends);
  //runs.start_new_run((INF_TEXT*) user_data);
}

void pause_button_cb (GtkWidget*btn, gpointer user_data) {
  runs.pause_run();
}

void continue_button_cb (GtkWidget*btn, gpointer user_data) {
  runs.continue_run();
}

gint timeout_loop(gpointer text_view) {
  //Check if already finished
  if (runs.state == RUNNING) {
    MPI_Test(&runs.recv_req, &runs.flag, &runs.mpi_status);
    if (runs.flag == 1) {
      runs.flag = 0;
      if (runs.recv_data[0] == -1) {
        runs.stop_run();
        create_drawing_window(&runs.current_trends);
      } else {
        update_trends((INF_TEXT*) text_view);
        runs.get_trend_data_from_run();
      }
    }
  }
  return 1;
}

void update_trends(INF_TEXT* text_view) {

  GtkTextBuffer* buf;
  GtkTextIter    iter;

  text_view->add_text ("\n", PLAIN_OUTPUT);
  for (gint i: runs.recv_data) {
    text_view->add_text (g_strdup_printf("%i\t", i), PLAIN_OUTPUT);
  }
  text_view->add_text ("\n", PLAIN_OUTPUT);

  runs.current_trends.data[runs.current_trends.idx].day         = runs.recv_data[0];
  runs.current_trends.data[runs.current_trends.idx].test_p      = runs.recv_data[1];
  runs.current_trends.data[runs.current_trends.idx].test_s      = runs.recv_data[2];
  runs.current_trends.data[runs.current_trends.idx].test        = runs.recv_data[3];
  runs.current_trends.data[runs.current_trends.idx].submit_p    = runs.recv_data[4];
  runs.current_trends.data[runs.current_trends.idx].submit_s    = runs.recv_data[5];
  runs.current_trends.data[runs.current_trends.idx].submit      = runs.recv_data[6];
  runs.current_trends.data[runs.current_trends.idx].submit_b    = runs.recv_data[7];
  runs.current_trends.data[runs.current_trends.idx].uncollect_p = runs.recv_data[8];
  runs.current_trends.data[runs.current_trends.idx].uncollect_s = runs.recv_data[9];
  runs.current_trends.data[runs.current_trends.idx].uncollect   = runs.recv_data[10];
  runs.current_trends.data[runs.current_trends.idx].MSSS_p      = runs.recv_data[11];
  runs.current_trends.data[runs.current_trends.idx].MSSS_s      = runs.recv_data[12];
  runs.current_trends.data[runs.current_trends.idx].MSSS        = runs.recv_data[13];
  runs.current_trends.data[runs.current_trends.idx].discard_p   = runs.recv_data[14];
  runs.current_trends.data[runs.current_trends.idx].discard_s   = runs.recv_data[15];
  runs.current_trends.data[runs.current_trends.idx].discard     = runs.recv_data[16];
  runs.current_trends.data[runs.current_trends.idx].CAP_ind     = runs.recv_data[17];

  runs.current_trends.idx++;

}

void exit ()
{
  runs.exit_process ();
}
