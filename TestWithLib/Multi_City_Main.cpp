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

				if (state == GET_COMMAND) {
					double data[CONSTANTS_SIZE];
					data[SIM_DAYS] = Flu_Manager::Instance()->max_days;
					data[R_PANDEMIC] = Flu_Manager::Instance()->reproduction_number_pandemic;
					data[R_SEASONAL] = Flu_Manager::Instance()->reproduction_number_seasonal;
					data[SCALED] = Flu_Manager::Instance()->SCALING;
					data[OOS_TRAVEL] = Flu_Manager::Instance()->ALLOW_OOS_TRAVEL;
					data[SAMPLING] = Flu_Manager::Instance()->SAMPLING_CRITERIA;
					data[N_PHL] = Flu_Manager::Instance()->NUM_PHL;
					data[PHL_CAPACITY] = Flu_Manager::Instance()->PHL_CAPACITY;
					data[PHL_WEEKEND] = Flu_Manager::Instance()->WORK_WEEKEND;
					data[TRAVEL_CITIES] = Flu_Manager::Instance()->ALLOW_TRAVEL;

					send_constants2interface (&data[0]);

				} else if (state == SET_COMMAND) {
					double data[CONSTANTS_SIZE];
					MPI_Recv(&data[0], CONSTANTS_SIZE, MPI_DOUBLE, dest_rank, CONSTANTS_TAG, MPI_COMM_WORLD, &mpi_status);
					Flu_Manager::Instance()->max_days = data[SIM_DAYS];
					Flu_Manager::Instance()->reproduction_number_pandemic = data[R_PANDEMIC];
					Flu_Manager::Instance()->reproduction_number_seasonal = data[R_SEASONAL];
					Flu_Manager::Instance()->SCALING = data[SCALED];
					Flu_Manager::Instance()->ALLOW_OOS_TRAVEL = data[OOS_TRAVEL];
					Flu_Manager::Instance()->SAMPLING_CRITERIA = data[SAMPLING];
					Flu_Manager::Instance()->NUM_PHL = data[N_PHL];
					Flu_Manager::Instance()->PHL_CAPACITY = data[PHL_CAPACITY];
					Flu_Manager::Instance()->WORK_WEEKEND = data[PHL_WEEKEND];
					Flu_Manager::Instance()->ALLOW_TRAVEL = data[TRAVEL_CITIES];

				} else if (state == START) {
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
