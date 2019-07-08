/*A Flu Simulation
 coding convetions:
 private, non pointer, member variables are preceeded by an underscore.
 memeber pointers are preceeded by 'p_'
 Originally coded in C by Diana Prieto. Code modified and converted to C++ by Greg Ostroy
  *************************************************************************/
#include <vector>
#include <iostream>
#include <omp.h>
#include <time.h>
#include <Multi_City_44_Templates.h>
#include "mpi.h"
#include <unistd.h>
#include "interface_main.h"

using namespace std;
/*************functions***************/


int main( int argc, char* argv[] )
{

	int numprocs, rank, namelen;
	MPI_Status *mpi_status;

	MPI_Init(&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (numprocs != 2) {
		printf("ERR: Call with -np 2\n");
		MPI_Finalize();
		exit(0);
	}

	if (rank == 0) {
			//load initialization data
			Flu_Manager::Instance()->loadConstants();
			Flu_Manager::Instance()->loadInput();
			Flu_Manager::Instance()->NUM_RUNS = 0;
			int *constants = (int*) calloc (40, sizeof(int));

			int cur_run_idx = 0;
			while (constants[0] != EXIT) {
				MPI_Recv(constants, 40, MPI_INT, 1, 1, MPI_COMM_WORLD, mpi_status);
				if (constants[0] == RUNNING) {

					SimulationRun* run = new SimulationRun(cur_run_idx); //create new simulation run and put pointer to SimulationRun object, totally create number of NUM_RUNS object pointers
					Flu_Manager::Instance()->addRun(run); //put simulation run pointer to _runs vector
					Flu_Manager::Instance()->NUM_RUNS++;

					constants[0] = 1;
					Flu_Manager::Instance()->getRun(cur_run_idx)->outbreak(constants);

					//test code
					time_t now;
					struct tm* timeInfo;
					time(&now);
					timeInfo=localtime(&now);
					printf("%s\n",asctime(timeInfo));

					free (run);

					constants[0] = STOPPED;
					MPI_Send(constants, 40, MPI_INT, 1, 2, MPI_COMM_WORLD);
					cur_run_idx++;
				}
			}

			free (constants);

	} else if (rank == 1) {

		run_gui_interface(argc, argv);

		exit_process();

	}

	MPI_Finalize();

	char any;
	cin >> any;//don't close console automatically on exit, wait unitl user can read it

	return 0;
}
