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
	MPI_Send(&generalTestedStats[0], DATA_SIZE, MPI_INT, dest_rank, DATA_TAG, MPI_COMM_WORLD);
}

void send_constants2interface (double constants[])
{
	MPI_Send(&constants[0], CONSTANTS_SIZE, MPI_DOUBLE, dest_rank, CONSTANTS_TAG, MPI_COMM_WORLD);
}

void receive_constants_from_interface (double constants[])
{
	MPI_Status mpi_status;
	MPI_Recv(&constants[0], CONSTANTS_SIZE, MPI_DOUBLE, dest_rank, CONSTANTS_TAG, MPI_COMM_WORLD, &mpi_status);
}
