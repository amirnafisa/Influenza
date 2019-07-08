#ifndef __INTERFACE_CALLS__
#define __INTERFACE_CALLS__

#include <stdio.h>
#include "mpi.h"
#include <unistd.h>
#include <Flu_Manager.h>
#include <City.h>

#define RUNNING 1
#define PAUSED  2
#define STOPPED 3
#define EXIT 4



void start_run();
void pause_run ();
void continue_run ();
void stop_run ();
void exit_process ();
bool is_finished ();

#endif
