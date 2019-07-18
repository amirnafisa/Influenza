#ifndef __MPI_INTERFACE_H__
#define __MPI_INTERFACE_H__

#include "mpi.h"
#include <stdio.h>

#define DATA_SIZE  18


enum mpi_tag_types{SIGNAL_TAG=1, DATA_TAG=2, COMMAND_TAG=3, CONSTANTS_TAG=4};
enum state_types{INIT=0, START=1, RUNNING=1, PAUSED=2, STOPPED=3, EXIT=-1, GET_COMMAND=4, SET_COMMAND=5};
enum constants_types{SIM_DAYS, R_PANDEMIC, R_SEASONAL, SCALED, OOS_TRAVEL, SAMPLING, N_PHL, PHL_CAPACITY, PHL_WEEKEND, TRAVEL_CITIES, CONSTANTS_SIZE};

void check_state (MPI_Request* rq, int* flag, state_types* state);
void send_constants2interface (double constants[]);
void send_data2interface(int generalTestedStats[]);
void receive_constants_from_interface (double constants[]);

#endif
