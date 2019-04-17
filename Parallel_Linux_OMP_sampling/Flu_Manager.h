/****a singleton manager class for a flu simulation. It holds global varialbles, constants,
templates for creating different types of people, households and businesses,and handles random number generation***/
#pragma once
#include "SimulationRun.h"
#include <vector>
#include <string>
#ifndef FLU_MANAGER_H
#define FLU_MANAGER_H
class Flu_Manager
{
private:
	Flu_Manager(void);
	static Flu_Manager *p_inst;//pointer to the singleton
	std::vector <SimulationRun*> _runs;
	std::vector<std::string> _workFiles_scaled;
	std::vector<std::string> _workFiles_unScaled;
public:
	float asymp;// factor by which the infectiousness profile of an asymptomatic is reduced. asymp = 4.4954/20.2094.  Cite: Carrat08
	float epsilon_p;//factor by which a pandemic infectious profile is affected given the influence of a seasonal profile that is also occuring(e.g. cross-immunity created by the seasonal virus)
	float epsilon_pr;//factor by which a pandemic infectious profile is affected given the influence of a seasonal profile that has already passed(e.g. cross-immunity created by the seasonal virus)
	float epsilon_ps;//factor by which a pandemic infectious profile is affected given the influence of a seasonal profile when both are ocurring at the same time(e.g. cross-immunity created by the seasonal virus)
	float epsilon_s;//factor by which a seasonal infectious profile is affected given the influence of a pandemic profile that is also occuring(e.g. cross-immunity created by the pandemic virus)
	float epsilon_sr;//factor by which a seasonal infectious profile is affected given the influence of a pandemic profile that has already passed(e.g. cross-immunity created by the pandemic virus)
	float epsilon_sp;//factor by which a seasonal infectious profile is affected given the influence of a pandemic profile when both are acurring at the same time(e.g. cross-immunity created by the pandemic virus)
	float k_hh;// proportion of duration and closeness of a household contact with respect to the household contact  k_hh=1. Cite: Mossong06
	float k_wp;// proportion of duration and closeness of a workplace contact with respect to the household contact k_wp=0.67.  Cite: Mossong06
	float k_er;// proportion of duration and closeness of an errand contact with respect to the household contact k_er=0.44. Cite: Mossong06
	int initial_infected_pandemic;
	int initial_infected_seasonal;
	int max_days;// number of simulation days
	float reproduction_number_pandemic;
	float reproduction_number_seasonal;
	float SEVERITY;//A factor modifiying the probability that a persons will be too sick to go to work
	enum QuarantineOptions{NONE,ABSENTEEISM,HOSPITALIZATION};//type of quarantine possible in simulation
	QuarantineOptions quarantineOption; //type of quarantine allowed in this simulation
	int PANDEMIC_VACCINATION;//1=simulation includes pandemic vaccination, 0=no pandemic vaccination
	int SEASONAL_VACCINATION;//1=simulation includes seasonal vaccination, 0=no seasonal vaccination
	int NUM_CITIES;
	int NUM_RUNS;//the number of replicates of the simulation to run 
	long SEED;//random seed
	int ALLOW_TRAVEL;//Allow people to travel between cities
	int ALLOW_OOS_TRAVEL;//Allow people to travel in ad out of state
	float VACC_COVERAGE_0TO5;//percentage of people vaccinated by age
	float VACC_COVERAGE_6TO14;
	float VACC_COVERAGE_15TO17;
	float VACC_COVERAGE_18TO29;
	float VACC_COVERAGE_30TO64;
	float VACC_COVERAGE_65PLUS;
	float VACCINE_COEFF_TRANSMISSION;//modifier to the chance of transmitting virus when vaccinated 
	float VACCINE_COEFF_INFECTIOUSNESS;//modifier to the infectiousness of a virus transmitted by a vaccinated person
	float VACCINE_COEFF_SYMPTOMATIC;//addtional modifier for vaccinated symptomatic individuals
	int usingAntivirals;//Does the simulation include antiviral treatment, 1=yes 0=no
	float AV_TRANSMISSION;//modifier to the chance of transmitting virus when on antiviral treatment
	float AV_INFECTIOUSNESS;//modifier to the infectiousness of a virus transmitted by a person on antiviral treatment
	float AV_SYMPTOMATIC;//addtional modifier for symptomatic individuals on antiviral treatment
	float ANTIVIRAL_PROB;//the probability that a vulnerable person will recieve antiviral treatment
	int NUM_THREADS; //the number of threads to run in parallel where appropriate
	int SCALING;//will the region size be scaled down or not
//yuwen add seek healthcare 05152018_1 begin
	float POVERTY_PROB;//Percentage of people under poverty baseline
	float PERCEIVED_PANDEMIC_PROB;//Probablity of a person perceived to be pandemic flu
	int SAMPLING_CRITERIA;//Switch for selecting sampling criteria. 0=not activated, 1=FIFO, 2=FIFO Within Risk Group, 3=Hybric, 4=FIFO but report first week only, 5=FIFO within risk group report first week only.
	float ED_MSSS_PERCENTAGE;//Percentage of ERs in Michigan report to MSSS.
	float COLLECT_PROB;//Probability that a health care provider but ER choose to collect a case sample when case is not severe.
	int NUM_PHL;//number of public health labs in Michigan. 1 as default
	int PHL_CAPACITY;//maximum daily capacity in a PHL (<=1000)
	float OOS_TRAVEL_PROB;//Probability that a traveler traveling out of state
	float PR_ACCESSIBLITY;//Probability that a case can access a primary care
	int WORK_WEEKEND;//DOES PHL WORK IN WEEKEND?
//yuwen add seek healthcare 05152018_1 end
//Yuwen add begin week day 010819 begin
	int BEGIN_WEEKDAY;//The day that starts the simulation of a week: Mon 1, TUE 2, WED 3, THU 4, FRI 5, SAT 6, SUN 7
//Yuwen add begin week day 010819 end	
	static Flu_Manager *Instance();//create or get the singleton class object
	~Flu_Manager(void); 
	float getUniformValue(int);//get a value from _uniform[]
	void loadInput();//load initialization values from txt files
	void setCounter(int);// set _uniform_counter
	void loadConstants();
	float getAdultAgesCumProb(int) const;
	int getAdultAge(int) const;
	float getChildAgesCumProb(int) const;
	int getChildAge(int) const;
	int getChildWorkType(int) const;
	float getHouseCumPmf(int) const;
	float getGamma1CumDensity(int) const;
	float getGamma2CumDensity(int) const;
	float getLognormal1CumDensity(int) const;
	float getLognormal2CumDensity(int) const;
	float getWeibu1CumDensity(int) const;
	float getWeibu2CumDensity(int) const;
	int getHouseNumAdults(int) const;
	int getHouseNumChildren(int) const;
	float getProbTravel(int) const;
	float getProbDestination(int) const;
	int getRegionScaledPopulation(int)const;
	int getRegionUnscaledPopulation(int)const;
	int getScaledOutsideArrivals(int)const;
	int getUnscaledOutsideArrivals(int)const;
	float getZipCumProb(int) const;
	int getZipcode(int) const;
	std::string getScaledWorkFile(int);
	std::string getUnscaledWorkFile(int);
	SimulationRun* getRun(int) const;
	void addRun(SimulationRun*);
};
#endif

