/* A class that runs a simulation of a flu outbreak. It is intended to run in a thread parallel to other runs*/
#pragma once

#include "Person.h"
#include <vector>
#ifndef SIMULATIONRUN_H
#define SIMULATIONRUN_H

#define INF_MIN(a,b) a<b?a:b


class SimulationRun
{
	private:
		int _id;
		//int _uniform_counter;//counter to iterate through _uniform[]
		long _seed;
		//float _uniform[10001];//an array of uniformly distributed random numbers between 0 and 1
		int 	total_infected_pandemic ;
		int 	total_infected_seasonal ;
		int     total_sim_coinfected ;
		int 	total_infected_pandemic_only ;
		int 	total_infected_seasonal_only ;
		int 	total_coinfected_s_p;
		int 	total_reinfected_s_p ;
		int 	total_coinfected_p_s ;
		int 	total_reinfected_p_s ;
		int		total_infected; //total_infected_pandemic+total_infected_seasonal

		int	    new_infected_today_pandemic ; // number of new infected entities every day
		int	    new_infected_today_seasonal ; // number of new infected entities every day
		int	    new_infected_today_pandemic_only ; // number of new infected entities every day
		int	    new_infected_today_seasonal_only ; // number of new infected entities every day
		int	    new_sim_coinfected ; // number of new infected entities every day
		int	    new_coinfected_s_p ; // number of new infected entities every day
		int	    new_reinfected_s_p ; // number of new infected entities every day
		int	    new_coinfected_p_s ; // number of new infected entities every day
		int	    new_reinfected_p_s ; // number of new infected entities every day
		int	    new_recovered_today_pandemic ; // number of new recovered entities every day
		int	    new_recovered_today_seasonal ; // number of new recovered entities every day
		int		_totalPopulation;
//yuwen add 0624 print
		int 	total_PHLtested_pandemic;//number of total cases with pandemic influenza tested by PHL
		int 	total_PHLtested_seasonal;//number of total cases with seasonal influenza tested by PHL
		int 	total_PHLtested_all;//number of total cases tested by PHL
		int     total_pandemic_submit_samples;//number of total submitted pandemic samples
		int 	total_seasonal_submit_samples;//number of total submitted seasonal samples
		int 	total_submit_samples;//number of total submitted samples
		int     total_pandemic_uncollect_samples;//number of total uncollected pandemic samples
		int     total_seasonal_uncollect_samples;//number of total uncollected seasonal samples
		int     total_uncollect_samples;//number of total uncollected samples
		int 	total_pandemic_MSSS_samples;//number of total MSSS pandemic samples
		int 	total_seasonal_MSSS_samples;//number of total MSSS seasonal samples
		int 	total_MSSS_samples;//number of total MSSS samples
		int     CAPACITY_IND;//indicator to check if PHL has been reached
		int     total_discard_pandemic;//number of total discarded pandemic samples
		int     total_discard_seasonal;//number of total discarded seasonal samples
		int     total_discard_all;//number of total discarded samples
		int 	total_both_submit_samples;
//yuwen add 0624 end
		std::vector<class City*> _cities;
		std::vector<int> _infectedByDay_p;
		std::vector<int> _infectedByDay_s;
		std::vector<int> _totalInfectedByDay;
//yuwen add sample testing 05172018_1 begin
	/*put vector and structure for sample storage in PHL*/
		struct Samples
		{
			Person* _sample;//the person that is taken sample for testing with seasonal influenza
			int _virusType;//The types of the virus that the sample contains. can be none, seasonal, pandemic and both
			int _sampleClock;//life in days of a collected sample
			int _visitCityId;//get the id of visiting city
			int _PHLID;//The sample is sent to number ID PHL.
			int _arriveDay;//the day sample arrive in PHL
			int _submitDay;//The day sample is submitted.
		};
		std::vector<struct Samples> _samplePoolPHL;//a vector to store influenza samples submitted to PHL
		std::vector<struct Samples> _samplePoolRural;//a vector to store samples from rural regions.
		std::vector<struct Samples> _uncollectedSample;//a vector to store uncollected samples from providers to PHL
		std::vector<struct Samples> _sampleMsss;//a vector to store sample infomation reported to MSSS
//yuwen add sample testing 05172018_4 end

//yuwen add 1002 begin store the number of tested specimens by submission date in a matrix
		int submitPandemic[23][23];//matrix to store the number of tested pandemic specimens by submission date
		int submitSeasonal[23][23];//matrix to store the number of tested seasonal specimens by submission date
		int submitTotal[23][23];//matrix to store the number of tested total specimens by submission date
//yuwen add 1002 end store the number of tested specimens by submission date in a matrix

	public:
		SimulationRun(int);
		~SimulationRun();
		int getId() const;
		City* getCity(int) const;
		//float getUniformValue(int);
		bool outbreak(int* status);
		bool hasCityWithOutbreak();
		int getDailyInfectedPandemic(int);
		int getDailyInfectedSeasonal(int);
		int getTotalInfected(int);
		//double fluRandom();//random number generator from 0 to 1
		//float uni(int,int);//random number generator from int a to int b
		double uni(int, int); //random number generator for openmp parallelizations
//yuwen add sample testing 05172018_2 begin
//		int SAMPLELIFE;//Life of a sample before expiration
		void addtoSamplePool(Person*,int);//add a sample to PHL sample pool
//yuwen delete 1002 begin
		void addtoUnCollected(Person*,int);//add unsample person to uncollected pool
		void addtoMsssPool(Person*,int);//add a person info to MSSS pool
		void updateTestedBySubmit(int,int);//update the number of tested samples by submission days
//yuwen delete 1002 end
		void SamplingPHL(int);//PHL choolse samping methods to test samples
		void sumTested(int);//aggregate the total number of tested samples
		void sumDiscard(int);//aggregate the total number of discarded samples
//yuwen add sample testing 05172018_2 end
};
#endif
