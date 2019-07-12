#ifndef __MPI_INTERFACE_H__
#define __MPI_INTERFACE_H__

#include "mpi.h"
#include <stdio.h>

#define RECV_DATA_SIZE  18
enum mpi_tag_types{SIGNAL_TAG=1, DATA_TAG=2};
enum state_types{INIT=0, START=1, RUNNING=1, PAUSED=2, STOPPED=3, EXIT=-1};

void check_state (MPI_Request* rq, int* flag, state_types* state);

void send_data2interface(int generalTestedStats[]);

#endif
