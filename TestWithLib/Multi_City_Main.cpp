/*A Flu Simulation
 coding convetions:
 private, non pointer, member variables are preceeded by an underscore.
 memeber pointers are preceeded by 'p_'
 Originally coded in C by Diana Prieto. Code modified and converted to C++ by Greg Ostroy
  *************************************************************************/

#include "interface_main.h"

/*************functions***************/

gint main( gint argc, gchar* argv[] )
{
	gint rank, dest_rank;
	MPI_Status mpi_status;
	state_types state = INIT;

	gint cur_run_idx = 0;

	rank = initialise_mpi (argc, argv);
	dest_rank = (rank + 1) % 2;
	switch (rank) {

		case PARALLEL_OMP_INFLUENZA_RUN_PROC:
			instantiate_flu_manager ();
			while (state != EXIT) {
				MPI_Recv(&state, 1, MPI_INT, dest_rank, SIGNAL_TAG, MPI_COMM_WORLD, &mpi_status);
				if (state == START) {
					create_run (cur_run_idx);
					cur_run_idx++;
				}
			}
			break;

		case GUINTERFACE_PROC:
			run_gui_interface(argc, argv);
			exit();
			break;

		default:
			printf("ERR: Call with -np 2\n");
	}

	MPI_Finalize();

	return 0;
}
