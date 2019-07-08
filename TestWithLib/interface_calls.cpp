#include "interface_calls.h"

int current_state;
int *constants;

MPI_Status *mpi_status;
int flag;
MPI_Request recv_req;

void start_run () {
  if(!is_finished()) {
    stop_run();
  }

  if (current_state != RUNNING) {
    constants = (int*) calloc (40, sizeof(int));

  	printf("...Starting the run...\n");
    current_state = RUNNING;
		constants[0] = current_state;
		MPI_Send(constants, 40, MPI_INT, 0, 1, MPI_COMM_WORLD);
    //Check for signal from run process indicating the completion of the run
		MPI_Irecv(constants, 40, MPI_INT, 0, 2, MPI_COMM_WORLD, &recv_req);
	}
}

void pause_run () {
  is_finished();
	if (current_state == RUNNING) {
  	printf("...Pausing the run...\n");
    current_state = PAUSED;
		constants[0] = current_state;
		MPI_Send(constants, 40, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
}

bool is_finished () {
  if (current_state == STOPPED) {
    return 1;
  }
  //Check if already finished
  if (current_state == RUNNING) {
    MPI_Test(&recv_req, &flag, mpi_status);
    if (flag == 1) {
      flag = 0;
      stop_run();
      return 1;
    }
  }
  return 0;
}

void continue_run () {
	if (current_state == PAUSED) {
    printf("...Continue the run...\n");
    current_state = RUNNING;
		constants[0] = current_state;
		MPI_Send(constants, 40, MPI_INT, 0, 1, MPI_COMM_WORLD);
	}
}

void stop_run () {
  //Else send stop command
  if (current_state != STOPPED) {
  	printf("...Stopping the run...\n");
    current_state = STOPPED;
		constants[0] = current_state;
    MPI_Send(constants, 40, MPI_INT, 0, 1, MPI_COMM_WORLD);

    free(constants);
  }
}

void exit_process () {
  printf("...Exiting the application...\n");

  stop_run ();

  if (current_state == STOPPED) {
    constants = (int*) calloc (40, sizeof(int));
  }
  constants[0] = 4;
  MPI_Send(constants, 40, MPI_INT, 0, 1, MPI_COMM_WORLD);
  free(constants);

}
