#include "simulation_calls.h"
#include "mpi.h"
#include <Flu_Manager.h>
#include <City.h>

gint initialise_mpi (gint argc, gchar* argv[]) {
	gint numprocs, rank;
	MPI_Init (&argc, &argv);
	MPI_Comm_size(MPI_COMM_WORLD, &numprocs);
	MPI_Comm_rank(MPI_COMM_WORLD, &rank);

	if (numprocs != 2) {
		return -1;
	}
	return rank;
}

void instantiate_flu_manager ()
{
  //load initialization data
  Flu_Manager::Instance()->loadConstants();
  Flu_Manager::Instance()->loadInput();
}

void create_run (gint cur_run_idx)
{
  SimulationRun* run = new SimulationRun(cur_run_idx); //create new simulation run and put pointer to SimulationRun object, totally create number of NUM_RUNS object pointers
  Flu_Manager::Instance()->addRun(run); //put simulation run pointer to _runs vector
  Flu_Manager::Instance()->NUM_RUNS++;

  Flu_Manager::Instance()->getRun(cur_run_idx)->outbreak();

  //Free up memory
  free (run);

}
