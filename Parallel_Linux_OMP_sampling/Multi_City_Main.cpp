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
#include "City.h"
#include "Flu_Manager.h"
#include "Multi_City_44_Templates.h"

using namespace std;

/*************functions***************/

int main()
{	
	// Declaring global variables
	int		t_now = 0; // simulation clock time in hours
	//load initialization data
	Flu_Manager::Instance()->loadConstants();
	Flu_Manager::Instance()->loadInput();

	//create _runs y_p
	//# pragma omp parallel for num_threads(Flu_Manager::Instance()->NUM_THREADS)
	for(int i=0;i<Flu_Manager::Instance()->NUM_RUNS;i++)
	{
		SimulationRun* run = new SimulationRun(i); //create new simulation run and put pointer to SimulationRun object, totally create number of NUM_RUNS object pointers
		Flu_Manager::Instance()->addRun(run); //put simulation run pointer to _runs vector
	}
	//execute outbreak
	for(int i=0;i<Flu_Manager::Instance()->NUM_RUNS;i++)
		Flu_Manager::Instance()->getRun(i)->outbreak();
	//store global data
	FILE* globalOutput_genStats=fopen("output_general_stats.txt","w");
	fprintf(globalOutput_genStats, "Day		Infected_total		Infected_p		Infected_s\n");
	//collect global data
	for(int j=0;j<Flu_Manager::Instance()->max_days; j++)
	{
		int sumOfDaysPandemic=0;
		int sumOfDaysSeasonal=0;
		int sumOfDaysTotal=0;
		for(int i=0;i<Flu_Manager::Instance()->NUM_RUNS;i++)
		{
			if(j==0)
			{
				sumOfDaysPandemic+=Flu_Manager::Instance()->getRun(i)->getDailyInfectedPandemic(j);
				sumOfDaysSeasonal+=Flu_Manager::Instance()->getRun(i)->getDailyInfectedSeasonal(j);
				sumOfDaysTotal+=Flu_Manager::Instance()->getRun(i)->getTotalInfected(j);
			}
			else
			{
				sumOfDaysPandemic+=Flu_Manager::Instance()->getRun(i)->getDailyInfectedPandemic(j)-Flu_Manager::Instance()->getRun(i)->getDailyInfectedPandemic(j-1);//DailyInfectedPandemic is cumulative daily incidence
				sumOfDaysSeasonal+=Flu_Manager::Instance()->getRun(i)->getDailyInfectedSeasonal(j)-Flu_Manager::Instance()->getRun(i)->getDailyInfectedSeasonal(j-1);
				sumOfDaysTotal+=Flu_Manager::Instance()->getRun(i)->getTotalInfected(j)-Flu_Manager::Instance()->getRun(i)->getTotalInfected(j-1);
			}
		}
		int avePandemic=sumOfDaysPandemic/Flu_Manager::Instance()->NUM_RUNS;
		int aveSeasonal=sumOfDaysSeasonal/Flu_Manager::Instance()->NUM_RUNS;
		int aveTotal=sumOfDaysTotal/Flu_Manager::Instance()->NUM_RUNS;
		fprintf(globalOutput_genStats,"%d		%d			%d			%d\n",j+1,aveTotal,avePandemic,aveSeasonal);
	}
	fclose(globalOutput_genStats);
	//printf("total_HH: %d\ntotal_WP: %d\ntotal_ER: %d\n",cities[0]->total_HH,cities[0]->total_WP,cities[0]->total_ER);*/
	//print the rn totals
	/*FILE* R_output= fopen("output_R.txt","w");
	fprintf(R_output, "Generation   Generation_size_p   Avg_R_p   Generation_size_s   Avg_R_s   \n");
	for(int i=0; i< Flu_Manager::Instance()->max_days;i++)
	{
		float ave_rn_p=0.0f;//ave gen i+1 pandemic reproduction number for all cities
		float ave_rn_s=0.0f;//ave gen i+1 seasonal reproduction number for all cities
		float sum_rn_p=0.0f;//sum of cicties pandemic rn number for gen i+1
		float sum_rn_s=0.0f;//sum of cicties seasonal rn number for gen i+1
		int sumPandemicGeneration=0;//the sum of all the cities pandemic generations i+1
		int sumSeasonalGeneration=0;//the sum of all the cities seasonal generations i+1
		int sumCities_p=0;//the sum of all cities with a pandemic rn number for this generation >0
		int sumCities_s=0;///the sum of all cities with a seasonal rn number for this generation >0
		ave_rn_p=sum_rn_p/sumCities_p;
		ave_rn_s=sum_rn_s/sumCities_s;
		fprintf(R_output,"%d			%d				%f			%d				%f\n",i+1,sumPandemicGeneration,ave_rn_p,sumSeasonalGeneration,ave_rn_s);
	}
	fclose(R_output);*/
	//test code
	time_t now;
	struct tm* timeInfo;
	time(&now);
	timeInfo=localtime(&now);
	printf("%s\n",asctime(timeInfo));
	char any;
	cin >> any;//don't close console automatically on exit, wait unitl user can read it
	return 0;
}
