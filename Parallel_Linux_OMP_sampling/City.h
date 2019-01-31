/*A City class. It actually represents a region, and holds people and businessess and runs through a flu outbreak-Greg Ostroy*/
#pragma once
#include <vector>
#include <map>
#include <fstream>
#include "Business.h"
#include "Household.h"
#include "SimulationRun.h"
#include "Multi_City_44_Templates.h"
#ifndef CITY_H
#define CITY_H

class City
{
private:
	int _id;
	int  NUM_HOUSEHOLDS;//number of households in region
	int _numOutsideArrivals;//dailt out of state arrivals
	int _population;//total city population
	std::map<int,Person*> _infected;
	std::vector<int> _infectIds; //keys to the map of infectors	
	std::vector<Person*> _sickArrivals;//array of out of state arrivals who are infected with flu
	int _daysOutbreak; //days of the outbreak for this city 
	int	    new_infected_today_pandemic; // number of new infected entities every day
	int	    new_infected_today_seasonal; // number of new infected entities every day
	int	    new_infected_today_pandemic_only; // number of new infected entities every day
	int	    new_infected_today_seasonal_only; // number of new infected entities every day
	int	    new_sim_coinfected ; // number of new infected entities every day
	int	    new_coinfected_s_p ; // number of new infected entities every day
	int	    new_reinfected_s_p ; // number of new infected entities every day
	int	    new_coinfected_p_s ; // number of new infected entities every day
	int	    new_reinfected_p_s ; // number of new infected entities every day
	int	    new_recovered_today_pandemic ; // number of new recovered entities every day
	int	    new_recovered_today_seasonal ; // number of new recovered entities every day
	int 	total_infected_pandemic;
	int 	total_infected_seasonal;
	int     total_sim_coinfected;
	int 	total_infected_pandemic_only;
	int 	total_infected_seasonal_only;
	int 	total_coinfected_s_p;
	int 	total_reinfected_s_p;
	int 	total_coinfected_p_s;
	int 	total_reinfected_p_s;
	int		total_household_people;//number of people at home in a given hour
	int     total_business_people;//number of people at businesses in a given hour
	std::vector<float>	averages_rn_p;//the average daily pandemic reproduction numbers
	std::vector<float>  averages_rn_s;//the average daily seasonal reproduction numbers
	std::vector<int> generationsPandemic;//array of pandemic generation sizes
	std::vector<int> generationsSeasonal;//array of seasonal generation sizes
	std::vector<Person*> community; //array of total population
	float _probTravel; //probablity of a person traveling in this region
	float _probDestination;//probability of this reagion being a destination for a traveler
	float _probablityInfectedArrival_p;//probability of an out of state visitor being infected with pandemic flu
	float _probablityInfectedArrival_s;//probability of an out of state visitor being infected with seasonal flu
	std::vector<Business> _businesses;//an array of all businesses in the city
	std::vector<class Household*> _households;//an array of all households in the city
	struct WorkPlaceTemplate _workplaceTypes[number_business_type +1];//array of workplace types
	FILE* _output;//hourly update on location of people
	FILE* _output_R;//statistics on flu outbreak
	FILE* _output_GeneralStats;
	Location* p_Hospital; //The location where hospitalized people are scheduled
	Location* p_Outside;//The location to where people traveling out of state are sent
	class SimulationRun* p_run; //A pointer to the running simulation to which this city is a part
	void generateBusinesses();//creates all the cities businesses
	void generateHousehold();//creates all the cities households
	void assignZipcodes();//assigns zipcides to each households
	void assignWorkplaces();//assigns workplaces to each person
	void disease_progress(int);
	void setSchedule(int);
	void declareLocations(int);
	void spreadDisease(int,int);
	void updateArrivals(int);
public:

	City(int,bool,SimulationRun*);
	~City(void);
	void setPatientZeros();//pick initially infected people
	void updateOutbreak(int,int);//executes a flu outbreak
	class Household* getHousehold(int) const;
	int getPopulationSize()const;
	//void addToInfectedList(Person*);
	//void remmoveFromInfectedList(int);
	int getBusinessSize() const;
	int getId() const;
	Business * getBusiness(int);
	void addInfected(Person*);
	class SimulationRun* getSimRun() const;
	int getPeopleAtHome() const;
	int getPeopleAtWork() const;
	float getPandemicRn(int) const;
	float getSeasonalRn(int) const;
	int getPandemicGenerations(int) const;
	int getSeasonalGenerations(int) const;
	float getProbTravel() const;
	float getProbDestination() const;
	int getDaysOutbreak() const;
	void startOutbreak();
	void openOutputFiles();
	void closeOutputFiles();
	int getNewlyInfectedByType(int);
	int getInfectedByType(int);
	int getNewInfectedByType(int);
	int getNewRecoverd(bool);
	int getNumHouseholds()const;
	int getBusinessNumbers(int) const;
	Location* getHospital() const;
	void checkRNumber();
	int total_HH;
	int total_WP;
	int total_ER;
	int total_infectedByZeros;
	FILE* checkContacts;
	bool outbreakRunning();//has the outbreak still running(max run time 100 days)
//para alg
	void addToCommunity(Person*,std::vector<Person*> &);
//end
//yuwen add sample testing 05172018_5 begin
	void collectFromCity(int);
//yuwen add sample testing 05172018_5 end
};
#endif

