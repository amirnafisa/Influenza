#ifndef __SIMULATION_CALLS__
#define __SIMULATION_CALLS__

#include <gtk/gtk.h>
#include <Flu_Manager.h>
#include <City.h>
#include <Multi_City_44_Templates.h>
#include <mpi_interface.h>
#include "LineGraph.h"
#include "interface_classes.h"

#define PARALLEL_OMP_INFLUENZA_RUN_PROC  0
#define GUINTERFACE_PROC 1

class SIMULATION_RUNS
{
public:
  state_types state;
  gint run_idx;
  gint current_trends[SIMULATION_DAYS_LIMIT][N_TRENDS];
  gint current_idx;
  MPI_Status mpi_status;
  gint flag;
  MPI_Request recv_req;
  gint dest_rank;

public:
  SIMULATION_RUNS ()
  {
    run_idx = 0;
    dest_rank = 0;
    state = INIT;
  }

  void get_trend_data_from_run() {
      MPI_Irecv(&current_trends[current_idx], RECV_DATA_SIZE, MPI_INT, dest_rank, DATA_TAG, MPI_COMM_WORLD, &recv_req);
  }

  void start_new_run (INF_TREE_VIEW* text_view)
  {
    if (state == RUNNING) {
      stop_run();
    }

    printf("...Starting the run...\n");
    run_idx++;
    current_idx = 0;
    text_view->clear_table();
    state = RUNNING;
    MPI_Send(&state, 1, MPI_INT, dest_rank, SIGNAL_TAG, MPI_COMM_WORLD);
    //Check for signal from run process indicating the completion of the run
    get_trend_data_from_run();
  }

  void pause_run ()
  {
    if (state == RUNNING) {
      printf("...Pausing the run...\n");
      state = PAUSED;
      MPI_Send(&state, 1, MPI_INT, dest_rank, SIGNAL_TAG, MPI_COMM_WORLD);
    }
  }

  void continue_run ()
  {
    if (state == PAUSED) {
      printf("...Continue the run...\n");
      state = RUNNING;
      MPI_Send(&state, 1, MPI_INT, dest_rank, SIGNAL_TAG, MPI_COMM_WORLD);
    }
  }
  void stop_run ()
  {
    if (state != STOPPED) {
      printf("...Stopping the run...\n");
      state = STOPPED;
      MPI_Send(&state, 1, MPI_INT, dest_rank, SIGNAL_TAG, MPI_COMM_WORLD);
    }
  }
  void exit_process ()
  {
    printf("...Exiting the application...\n");
    stop_run ();
    state = EXIT;
    MPI_Send(&state, 1, MPI_INT, dest_rank, SIGNAL_TAG, MPI_COMM_WORLD);
  }
};

gint initialise_mpi (gint argc, gchar* argv[]);
void create_run (gint cur_run_idx);
void instantiate_flu_manager ();

#endif
