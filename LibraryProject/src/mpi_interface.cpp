#include "mpi_interface.h"
static const int dest_rank = 1;
void check_state (MPI_Request* rq, int* flag, state_types* state)
{
	MPI_Status mpi_status;
	if (!*flag) {
		MPI_Test(rq, flag, &mpi_status);
	}
	if (*flag == 1) {
		*flag = 0;
		if (*state != STOPPED && *state != EXIT)
			MPI_Irecv(state, 1, MPI_INT, dest_rank, SIGNAL_TAG, MPI_COMM_WORLD,rq);
	}
}

void send_data2interface(int generalTestedStats[])
{
	MPI_Send(&generalTestedStats[0], RECV_DATA_SIZE, MPI_INT, dest_rank, DATA_TAG, MPI_COMM_WORLD);
}
