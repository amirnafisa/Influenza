/*implimentation for the city class, uses algoritms developed by Diana Prieto -Greg Ostroy*/
#include <string>
#include <string.h>
#include <omp.h>
#include <iostream>
#include <fstream>
#include "City.h"
#include "Flu_Manager.h"
#include <math.h>

//using namespace std

City::City(int id,bool outbreak,SimulationRun* sim)
{
	_id=id;
	p_run=sim;
	_population=0;
	new_sim_coinfected = 0;
	new_infected_today_pandemic = 0;
	new_infected_today_pandemic_only = 0;
	new_coinfected_s_p = 0;
	new_reinfected_s_p = 0;
	new_recovered_today_pandemic = 0;
	new_infected_today_seasonal = 0;
	new_infected_today_seasonal_only = 0;
	new_coinfected_p_s = 0;
	new_reinfected_p_s = 0;
	new_recovered_today_seasonal = 0;

	total_infected_pandemic=0;
	total_infected_seasonal=0;
	total_sim_coinfected=0;
	total_infected_pandemic_only=0;
	total_infected_seasonal_only=0;
	total_coinfected_s_p=0;
	total_reinfected_s_p=0;
	total_coinfected_p_s=0;
	total_reinfected_p_s=0;

	_probablityInfectedArrival_p=0.0f;
	_probablityInfectedArrival_s=0.0f;
	//set number of households according to region
	if(Flu_Manager::Instance()->SCALING==0)
	{
		NUM_HOUSEHOLDS=Flu_Manager::Instance()->getRegionUnscaledPopulation(id);
		_numOutsideArrivals=Flu_Manager::Instance()->getUnscaledOutsideArrivals(id);
	}
	else
	{
		NUM_HOUSEHOLDS=Flu_Manager::Instance()->getRegionScaledPopulation(id);
		_numOutsideArrivals=Flu_Manager::Instance()->getScaledOutsideArrivals(id);
	}
	//initialize arrays of repro numbers and generation sizes
	for(int i=0;i<Flu_Manager::Instance()->max_days;i++)
	{
		averages_rn_p.push_back(0.0f);
		averages_rn_s.push_back(0.0f);
		generationsPandemic.push_back(0);
		generationsSeasonal.push_back(0);
	}
	//get business data for this region
	std::string filename;

	if(Flu_Manager::Instance()->SCALING==0)
		filename=Flu_Manager::Instance()->getUnscaledWorkFile(id);
	else
		filename=Flu_Manager::Instance()->getScaledWorkFile(id);

	std::ifstream workfile(filename);

	if(workfile.is_open())
	{
		for (int i = 0; i <= number_business_type; i++)
		{
			struct WorkPlaceTemplate work;
			workfile >> work._type >> work._numberOfType >> work._percentWork >> work._errandsWeekday >> work._errandsWeekend >> work._errandQuarantine >> \
			work._numContacts >> work._cumBusinesses >> work._cumPerWork >> work._cumWeekday >> work._cumWeekend >> work._cumVoluntaryQuarantine;
			_workplaceTypes[i]=work;
		}
		workfile.close();
	}
	else
		std::cout<<"couldn't open file";
	//create a hospital in which to hold hosptalized people
	p_Hospital=new Location(0,0,0);
	//creates the cities people and businesses
	//double start_t=omp_get_wtime();
	generateBusinesses();
	//double eclapse1=omp_get_wtime()-start_t;
	//printf("time used for generate business%f\n",eclapse1);
	//printf("pass businesses\n");
	//start_t=omp_get_wtime();
	generateHousehold();
	//double eclapse2=omp_get_wtime()-start_t;
	//printf("time used for generate household%f\n",eclapse2);
	//printf("pass household\n");
	//start_t=omp_get_wtime();
	assignWorkplaces();
	//double eclapse3=omp_get_wtime()-start_t;
	//printf("time used for assign workplaces%f\n",eclapse3);
	//printf("total time used for generate entities%f\n",eclapse1+eclapse2+eclapse3);
	//assignZipcodes(); //imcomplete function
	//set travel probabilities
	_probTravel=Flu_Manager::Instance()->getProbTravel(id);
	_probDestination = Flu_Manager::Instance()->getProbDestination(id);
	//start the outbreak if this city starts with infected
	if(outbreak==true)
	{
		_daysOutbreak=1;
		setPatientZeros();
//yuwen add print day 0 infection results 010719 begin
		total_sim_coinfected = getNewInfectedByType(COINFECTED_SIMULTANEOUS);
		total_infected_pandemic_only = getNewInfectedByType(INFECTED_PANDEMIC);
		total_infected_seasonal_only = getNewInfectedByType(INFECTED_SEASONAL);
		total_coinfected_s_p = getNewInfectedByType(COINFECTED_SEASONAL_PANDEMIC);
		total_reinfected_s_p = getNewInfectedByType(REINFECTED_SEASONAL_PANDEMIC);
		total_coinfected_p_s = getNewInfectedByType(COINFECTED_PANDEMIC_SEASONAL);
		total_reinfected_p_s = getNewInfectedByType(REINFECTED_PANDEMIC_SEASONAL);
		total_infected_pandemic = total_infected_pandemic_only+total_coinfected_s_p +total_reinfected_s_p; //for initial pandemic, do not add total_sim_co
		total_infected_seasonal = total_infected_seasonal_only+total_coinfected_p_s+total_reinfected_p_s+ total_sim_coinfected;
		total_infected=total_infected_pandemic_only+total_infected_seasonal_only+total_sim_coinfected;
		char runId[3];
//yuwen delete correct id 012219 begin
		//sprintf(runId,"%d",_id);//convert id to string
//yuwen delete correct id 012219 end
//yuwen add correct id 012219 begin
		sprintf(runId,"%d",p_run->getId());//convert id to string
//yuwen add correct id 012219 end
		std::string RunStr="output_data/Run_no";
		RunStr+=runId;
		char statStr[34];
		strcpy(statStr,RunStr.c_str());
		char baseStr[26]="_output_general_stats.txt";
		strcat(statStr,baseStr);

		//printf("total_infected is %d\ntotal_infected_pandemic_only is %d\ntotal_infected_seasonal_only is %d\n", total_infected,total_infected_pandemic_only,total_infected_seasonal_only);
		FILE* globalOutput_genStats_city=fopen(statStr,"w");
		fprintf(globalOutput_genStats_city, "Day  Population  Infected_total  Infected_p  Recovered_p  Infected_s  Recovered_s  Infected_p_only  Infected_s_only coinfected_s_p reinfected_s_p coinfected_p_s reinfected_p_s coinfected_sim\n");
		fprintf(globalOutput_genStats_city,	"0		0		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d \n",total_infected,total_infected_pandemic,new_recovered_today_pandemic,total_infected_seasonal,new_recovered_today_seasonal,total_infected_pandemic_only,total_infected_seasonal_only,total_coinfected_s_p,total_reinfected_s_p,total_coinfected_p_s,total_reinfected_p_s,total_sim_coinfected);
		fclose(globalOutput_genStats_city);
//yuwen add print day 0 infection results 010719 end
	}
	else
	{
		_daysOutbreak=0;
		total_HH=0;
		total_WP=0;
		total_ER=0;
		total_infectedByZeros=0;
	}
}


City::~City(void)
{
	//free household memory
	for(int i=0;i<NUM_HOUSEHOLDS;i++)//loop through the array of housholds and delete them
	{
		delete _households[i];
		_households[i]=NULL;
	}
	_businesses.clear();
	//_households.clear();
	//community.clear();
	//_businesses.shrink_to_fit();
	//_households.shrink_to_fit();
	//community.shrink_to_fit();
}

void City::updateOutbreak(int hour, int day)//only for city with ongoing outbreak
{
	//double start_t=omp_get_wtime();
	disease_progress(hour);//number of recovered people hourly
	//double eclapse=omp_get_wtime()-start_t;
	//printf("time used for disease progress%f\n",eclapse);
	if(hour==1)
	{	//determine the number of out of state visitors for today
		//start_t=omp_get_wtime();
		if(Flu_Manager::Instance()->ALLOW_OOS_TRAVEL>0 )
			updateArrivals(day);
		//eclapse=omp_get_wtime()-start_t;
		//printf("time used for update arrivals%f\n",eclapse);

		//start_t=omp_get_wtime();
		setSchedule(day);
		//eclapse=omp_get_wtime()-start_t;
		//printf("time used for set Schedules%f\n",eclapse);
		if(Flu_Manager::Instance()->ALLOW_OOS_TRAVEL>0  && !_sickArrivals.empty())
		{
			//# pragma omp parallel for num_threads(Flu_Manager::Instance()->NUM_THREADS)
			//start_t=omp_get_wtime();
			for(int i=0;i<(int)_sickArrivals.size();i++)
				_sickArrivals[i]->setArrivalSchedule(day);
			//eclapse=omp_get_wtime()-start_t;
			//printf("time used for set Arrival Schedule%f\n",eclapse);
		}
	}
	if(hour<22 && hour>7)
	{
		//start_t=omp_get_wtime();
		declareLocations(hour);
		//eclapse=omp_get_wtime()-start_t;
		//printf("time used for declare locations%f\n",eclapse);
		//start_t=omp_get_wtime();
		spreadDisease(hour,day);
		//eclapse=omp_get_wtime()-start_t;
		//printf("time used for spread disease%f\n",eclapse);
	}

	if (hour == 22)
	{
		//start_t=omp_get_wtime();
		#pragma omp parallel for num_threads(Flu_Manager::Instance()->NUM_THREADS) shared(community)
		for(int i=0;i<community.size();i++)
		{
			community[i]->setContactCounterHH(0);
			community[i]->setContactCounterWP(0);
			community[i]->setContactCounterER(0);
		}
		//eclapse=omp_get_wtime()-start_t;
		//printf("time used for set community contact zero%f\n",eclapse);
	}

//yuwen 102718 debugging the hourly location begin
/*	total_household_people = 0;
	total_business_people = 0;

	for(int h=0;h < NUM_HOUSEHOLDS;h++)
		total_household_people+= _households[h]->getOccupants().size();
		fprintf(_output, "There are %d household people in the hour %d and day %d \n",total_household_people,hour,day);
	}

	for(unsigned int b=1;b<_businesses.size();b++)
		total_business_people+=_businesses[b].getOccupants().size();
		fprintf(_output, "There are %d business people in the hour %d and day %d \n",total_business_people,hour,day);
*/
//yuwen 102718 	debugging the hourly location end
	if (hour==24)
	{
		//update probabilities of out of state arrivals being infected
		_probablityInfectedArrival_p=(float)new_infected_today_pandemic/_population;
		_probablityInfectedArrival_s=(float)new_infected_today_seasonal/_population;
		fprintf(_output_GeneralStats,"%d    %d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d \n",_daysOutbreak,_population,total_infected_pandemic,new_recovered_today_pandemic,total_infected_seasonal,new_recovered_today_seasonal,total_infected_pandemic_only,total_infected_seasonal_only,total_coinfected_s_p,total_reinfected_s_p,total_coinfected_p_s,total_reinfected_p_s,total_sim_coinfected);
		//reset daily values
		new_sim_coinfected = 0;
		new_infected_today_pandemic = 0;
		new_infected_today_pandemic_only = 0;
		new_coinfected_s_p = 0;
		new_reinfected_s_p = 0;
		new_recovered_today_pandemic = 0;
		new_infected_today_seasonal = 0;
		new_infected_today_seasonal_only = 0;
		new_coinfected_p_s = 0;
		new_reinfected_p_s = 0;
		new_recovered_today_seasonal = 0;
		_daysOutbreak++;
	}
}
/*creates businesses based on an array of templates. For each template the
appropriate number of businesses is created and initialized*/
//yuwen change start delete
/*
void  City::generateBusinesses()
{
	int s=0;
	for (int i = 1; i <= number_business_type; ++i)//loop through business templates
	{//set prev values to zero for the first business created
		long double prevWork=0.0;//percentage of peolpe working in business n-1
		float prevWeekday=0.0f;//% errands run at business n-1 during week
		float prevWeekend=0.0f;//% errands run at business n-1 on the weekends
		float prevQuarantine=0.0f;// % volunatry quarantine errands run business n-1
		for (int k =s+1; k <=s+_workplaceTypes[i]._numberOfType; ++k)
		{
			if(k>1)//after first business is created pevious values come from business n-1
			{
				prevWork=_businesses[k-2].getPerWorkplaces();
				prevWeekday=_businesses[k-2].getPerWeekdayErrands();
				prevWeekend =_businesses[k-2].getPerWeekendErrands();
				prevQuarantine=_businesses[k-2].getPerVolQuarantine();
			}
			//assign the above values for business n
			long double percentWorkplaces=prevWork+(long double)_workplaceTypes[i]._percentWork/_workplaceTypes[i]._numberOfType;
			float percentWeekdayErrands= prevWeekday+(float)_workplaceTypes[i]._errandsWeekday/_workplaceTypes[i]._numberOfType;
			float percentWeekendErrands= prevWeekend+(float)_workplaceTypes[i]._errandsWeekend/_workplaceTypes[i]._numberOfType;
			float percentVoluntaryQuarantine= prevQuarantine+(float)_workplaceTypes[i]._errandQuarantine/_workplaceTypes[i]._numberOfType;;
			//assign business contact rate
			int contactRate=_workplaceTypes[i]._numContacts;
			//create business and add to the businesses array
			Business bus(i,k,percentWorkplaces,percentWeekdayErrands,percentWeekendErrands,percentVoluntaryQuarantine,contactRate);
			_businesses.push_back(bus);
		}
		s+=_workplaceTypes[i]._numberOfType;//advance to the next group of businesses
	}
}
*/
//parallel algr replace the generateBusinesses
void  City::generateBusinesses()
{
	for (int i = 1; i <= _workplaceTypes[14]._cumBusinesses; ++i)
	{
		Business bus(0,i,0,0,0,0,0);
		_businesses.push_back(bus); //initiate _businesses vector
	}

	#pragma omp parallel for num_threads(Flu_Manager::Instance()->NUM_THREADS) //schedule(static,int((_workplaceTypes[14]._cumBusinesses)/omp_get_num_threads())+1)
	for (int i = 1; i <= _workplaceTypes[14]._cumBusinesses; ++i)//loop through business templates
	{
		int j = 1;
		while (i > _workplaceTypes[j]._cumBusinesses){
			j++;
		}
		//assign the above values for business n
		long double percentWorkplaces = (i - _workplaceTypes[j-1]._cumBusinesses)*_workplaceTypes[j]._percentWork / _workplaceTypes[j]._numberOfType + _workplaceTypes[j-1]._cumPerWork;
		float percentWeekdayErrands = (i - _workplaceTypes[j-1]._cumBusinesses)*_workplaceTypes[j]._errandsWeekday / _workplaceTypes[j]._numberOfType + _workplaceTypes[j-1]._cumWeekday;
		float percentWeekendErrands = (i - _workplaceTypes[j-1]._cumBusinesses)* _workplaceTypes[j]._errandsWeekend / _workplaceTypes[j]._numberOfType + _workplaceTypes[j-1]._cumWeekend;
		float percentVoluntaryQuarantine = (i - _workplaceTypes[j-1]._cumBusinesses)* _workplaceTypes[j]._errandQuarantine / _workplaceTypes[j]._numberOfType + _workplaceTypes[j-1]._cumVoluntaryQuarantine;
			//assign business contact rate
		int contactRate=_workplaceTypes[j]._numContacts;
			//create business and add to the businesses array

		Business bus(j,i,percentWorkplaces,percentWeekdayErrands,percentWeekendErrands,percentVoluntaryQuarantine,contactRate);
		_businesses[i] = bus; //save bus object in to _businesses vector in order
		//printf("_business i is %d\n", _businesses[i].getIdBus());
	}
}

/******
for (i = 1; i <= number_businesses; i++) {
		j = 1;
		while (i > workplaces[j][8]) { //Yuwen delete "and i < 20000"
			j++;
		}
		business[i].id = i;
		business[i].type = workplaces[j][1];
		business[i].work = (long double)(i - workplaces[j-1][8]) * workplaces[j][3] / workplaces[j][2] + (long double)workplaces[j-1][9];
		business[i].weekday =(float)(i - workplaces[j-1][8]) * workplaces[j][4] / workplaces[j][2] + (float)workplaces[j-1][10];
		business[i].weekend = (float)(i - workplaces[j-1][8]) * workplaces[j][5] / workplaces[j][2] + (float)workplaces[j-1][11];
		business[i].vol_quarantine = ((float)i - workplaces[j-1][8]) * workplaces[j][6] / workplaces[j][2] + (float)workplaces[j-1][12];
		business[i].contact_rate = workplaces[j][7];
	}
*/
//yuwen change end

/*create the cities households and people number of adults and children are based
on a random sampling of an array of husehold templates*/
//yuwen change start
void  City::generateHousehold()
{
	/*int n1=0; int n2=0;
	int i, s;
	float y;*/

#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) reduction(+:_population)
	{
		srand((unsigned int)(time(NULL)) ^ omp_get_thread_num());
		//srand((unsigned)(time(NULL)));
		//yuwen add parallize vector push_pack 12/19/18 begin1
		std::vector<class Household*> _households_private;
		std::vector<Person*> community_private;
//yuwen add parallize vector push_pack 12/19/18 end1
		#pragma omp for nowait //schedule(guided,int(NUM_HOUSEHOLDS/omp_get_num_threads())+1)
		for (int i=0;i<NUM_HOUSEHOLDS;++i)
		{
			float y = p_run->uni(0,1); // n1 = #adults, n2 = # kids, randomly genereated
			int s = 0;
			while (y > Flu_Manager::Instance()->getHouseCumPmf(s))
			{
				s++;
			}
			int n1=  Flu_Manager::Instance()->getHouseNumAdults(s);
			int n2=  Flu_Manager::Instance()->getHouseNumChildren(s);
			int n=n1+n2;

		//yuwen delete 102618 begin
		//#pragma omp atomic
		//yuwen delete 102618 end
			_population+=n;
//yuwen delete push back 122218 begin
			//Household* household=new Household(n1,n2,this);//create the household, the household will create its own people
//yuwen delete push back 122218 end
//yuwen add push back 122218 begin
			Household* household=new Household(n1,n2,this,community_private);//create the household, the household will create its own people
//yuwen add push back 122218 end
//yuwen delete parallize vector push_pack 12/19/18 begin
			/*#pragma omp critical
			{
				_households.push_back(household);//add it to the array of households
			}*/
//yuwen delete parallize vector push_pack 12/19/18 end

//yuwen add parallize vector push_pack 12/19/18 begin2
			_households_private.push_back(household);
		}
		#pragma omp critical
		{
			_households.insert(_households.end(), _households_private.begin(), _households_private.end());
			community.insert(community.end(), community_private.begin(),community_private.end());
		}
//yuwen add parallize vector push_pack 12/19/18 end2
//yuwen delete omp optimization 12/19/18 begin
/*		#pragma omp for
		for (int i=0;i<NUM_HOUSEHOLDS;++i)
		{
			_households[i]->setFamiliyPtrs();//people reference their household
			//printf("household %d has %d adults and %d children\n", i, _households[i]->getNumAdults(), _households[i]->getNumChildren());
		}
*/
//yuwen delete omp optimization 12/19/18 end
	}
}
//yuwen change end
//paral alg
/**/
void City::addToCommunity(Person* person, std::vector<Person*> &community_private)
{
//yuwen delete push back begin 122218
	/*#pragma omp critical
	community.push_back(person);*/
//yuwen delete push back end 122218
//yuwen add push back begin 122218
	community_private.push_back(person);
//yuwen add push back end 122218
}
//end

/*assign each household a zipcode*/
/*
void City::assignZipcodes()
{
	int total_per_zipcode;
	int householdCtr=0;
	for (int i = 1; i <= number_zipcodes; ++i)
	{
		total_per_zipcode=(int)(Flu_Manager::Instance()->getZipCumProb(i) *_population);
		int j=0;
		//assign households to zip code until total per zip code is reached or all households are assigned
		while(j< total_per_zipcode && householdCtr<NUM_HOUSEHOLDS)
		{
			int size=_households[householdCtr]->getSize();
			if(total_per_zipcode-j<size)//let a house with more people than zip spaces left be assigned to the next zipcode
				break;
			_households[householdCtr]->setZipCode(Flu_Manager::Instance()->getZipcode(i));
			j+=size;//j increases by size of household
			householdCtr++;//advance to the next household
		}
	}
	while(householdCtr<NUM_HOUSEHOLDS)//assign any remaining households the last zipcode
	{
		_households[householdCtr]->setZipCode(Flu_Manager::Instance()->getZipcode(number_zipcodes));
		householdCtr++;//advance to the next household
	}
}
*/
/*have each household assign workplaces to it's members*/
//yuwen change add, comment: only household assigned with workplaces? assignworkplaces include school?
void City::assignWorkplaces()
{
	# pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS)
{
	srand((unsigned int)(time(NULL)) ^ omp_get_thread_num());
	# pragma omp for schedule(dynamic)
	for(int i=0;i<NUM_HOUSEHOLDS;i++)
	{
//yuwen add omp optimization 12/19/18 begin
		_households[i]->setFamiliyPtrs();
//yuwen add omp optimization 12/19/18 end
		_households[i]->assignWorkplaces(_businesses);
	}
}
}
SimulationRun* City::getSimRun() const
{
	return p_run;
}

Household* City::getHousehold(int index) const
{
	return _households[index];
}

int City::getPopulationSize() const
{
	return _population;
}

int City::getBusinessSize() const
{
	return _businesses.size();
}

int City::getId() const
{
	return _id;
}

Business * City::getBusiness(int index)
{
	Business * bPtr=&_businesses[index];
	return bPtr;
}

Location* City::getHospital() const
{
	return p_Hospital;
}

float City::getPandemicRn(int index) const
{
	return averages_rn_p[index];
}

float City::getSeasonalRn(int index) const
{
	return averages_rn_s[index];
}

int City::getPandemicGenerations(int index) const
{
	return generationsPandemic[index];
}

int City::getSeasonalGenerations(int index) const
{
	return generationsSeasonal[index];
}

float City::getProbTravel() const
{
	return _probTravel;
}

float City::getProbDestination() const
{
	return _probDestination;
}

int City::getDaysOutbreak() const
{
	return _daysOutbreak;
}

int City::getNumHouseholds()const
{
	return NUM_HOUSEHOLDS;
}

int City::getBusinessNumbers(int index) const
{
	return _workplaceTypes[index]._numberOfType;
}
/*
void City::addToInfectedList(Person* person)
{
	#pragma omp critical
	{
		_infected.insert(std::pair<int,Person*>(person->getId(),person));
		_infectIds.push_back(person->getId());
	}
}

void City::remmoveFromInfectedList(int id)
{
	if(_infected.count(id)>0)
	{
		_infected.erase(id);
	}
}
*/
//chooses random households which choose a random person to start out as infected
void City::setPatientZeros()
{	//loop from 0 to n-1 where n is the initial number of infected people set by a config file
//yuwen comented parallel 102618 begin
//#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS)
//{
	//srand((unsigned int)(time(NULL))^omp_get_thread_num());
	srand((unsigned int)(time(NULL)));
	//#pragma omp for
	for(int i=0;i<Flu_Manager::Instance()->initial_infected_pandemic;i++)
	{	//choose a random household
		int infected=(int)p_run->uni(0,NUM_HOUSEHOLDS);
		_households[infected]->pickPatientZero(true);//household chooses a person among its members
	}
	//repeat for seasonal virus
	//#pragma omp for
	srand((unsigned int)(time(NULL)));
	for(int i=0;i<Flu_Manager::Instance()->initial_infected_seasonal;i++)
	{
		int infected=(int)p_run->uni(0,NUM_HOUSEHOLDS);
		_households[infected]->pickPatientZero(false);
	}
//yuwen comented parallel 102618 end
//}
}
/*gathers the number of people infected this hour and calls functions to update the hours and days each infected person has been
sick */
void City::disease_progress(int hour)
{
#pragma omp parallel for num_threads(Flu_Manager::Instance()->NUM_THREADS) reduction(+:new_recovered_today_pandemic, new_recovered_today_seasonal) shared(community)
	for(int i=0;i<community.size();i++)
	{
		new_recovered_today_pandemic+= community[i]->setDiseaseClockPandemic(hour);
		new_recovered_today_seasonal+= community[i]->setDiseaseClockSeasonal(hour);
	}
}
/*sets each persons daily schedule. Takes the day to be scheduled as an argument*/
void City::setSchedule(int day)
{	//loop through each household
	#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) shared(community)
{
	srand((unsigned int)(time(NULL)) ^ omp_get_thread_num());
	#pragma omp for schedule(dynamic)
	for(int i=0;i<community.size();i++)
	{
	//schedule adults day
			community[i]->setSchedule(day);
	}
}
}
//put peoples ids into the array of occupants at their location at hour
//yuwen change add
void City::declareLocations(int hour)
{	//clear last hour's business location data
#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) shared(_businesses, _households, community)
{
	#pragma omp for nowait
	for( int bus=0;bus< (int)_businesses.size();bus++)
	{
		_businesses[bus].setNumInfected(0);
		_businesses[bus].removeOccupants();
		//if(_businesses[bus].getOccupants().size()!=0)
			//printf("business %d has %d people.\n", bus, _businesses[bus].getOccupants().size());
	}

	#pragma omp for
	for(int i=0;i<NUM_HOUSEHOLDS;i++)
	{//clear last hour's household location data
		_households[i]->setNumInfected(0);
		_households[i]->removeOccupants();
		//if(_households[i]->getOccupants().size()!=0)
				//printf("household %d has %d people.\n", i, _households[i]->getOccupants().size());
	}

	#pragma omp for
	for(int i=0;i<community.size();i++)
	{
		community[i]->checkOccupants(hour);
	}
	#pragma omp for
	for(int i=0;i<community.size();i++)
	{
		community[i]->declareLocation(hour); //declare adult locations
	}
}
	if(Flu_Manager::Instance()->ALLOW_TRAVEL>0  && !_sickArrivals.empty())
		{
			for(int i=0;i<(int)_sickArrivals.size();i++)
				_sickArrivals[i]->declareLocation_unpall(hour);
		}
}
//yuwen change end
//yuwen change add
void City::spreadDisease(int hour,int day)
{
	#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) shared(community)
	{
		srand((unsigned int)(time(NULL))^omp_get_thread_num());
		#pragma omp for
		for(int i=0;i<community.size();i++)
			community[i]->processInfections(hour,day);
	}
/*	if(Flu_Manager::Instance()->ALLOW_TRAVEL>0 &&!_sickArrivals.empty())
	{
		for(unsigned int i=0;i<_sickArrivals.size();i++)
			_sickArrivals[i]->processInfections(hour,day);
	}*/

	if (hour == 21)
	{
	#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) shared(community)
		{
		srand((unsigned int)(time(NULL))^omp_get_thread_num());
		#pragma omp for
			for(int i=0;i<community.size();i++)
			{
				community[i]->infectContacts(day);
			}
		}
	}
}
//yuwen change end

void City::addInfected(Person* person)
{
/*#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) reduction(+:new_infected_today_pandemic_only,\
		new_infected_today_pandemic,total_infected_pandemic,total_infected_pandemic_only,\
		new_infected_today_seasonal_only,new_infected_today_seasonal,total_infected_seasonal,total_infected_seasonal_only,\
		new_coinfected_p_s,total_coinfected_p_s,new_coinfected_s_p,total_coinfected_s_p,new_reinfected_p_s,total_reinfected_p_s,\
		new_reinfected_s_p,total_reinfected_s_p,new_sim_coinfected,total_sim_coinfected)*/
//{
//	addToInfectedList(person);
	switch(person->infectionType)
	{
		case INFECTED_PANDEMIC:
		{
			#pragma omp atomic
			new_infected_today_pandemic_only++;
			#pragma omp atomic
			new_infected_today_pandemic++;
			#pragma omp atomic
			total_infected_pandemic++;
			#pragma omp atomic
			total_infected_pandemic_only++;
			break;
		}
		case INFECTED_SEASONAL:
		{
			#pragma omp atomic
			new_infected_today_seasonal_only++;
			#pragma omp atomic
			new_infected_today_seasonal++;
			#pragma omp atomic
			total_infected_seasonal++;
			#pragma omp atomic
			total_infected_seasonal_only++;
			break;
		}
		case COINFECTED_PANDEMIC_SEASONAL:
		{
			#pragma omp atomic
			new_coinfected_p_s++;
			#pragma omp atomic
			new_infected_today_seasonal++;
			#pragma omp atomic
			total_infected_seasonal++;
			#pragma omp atomic
			total_coinfected_p_s++;
			break;
		}
		case COINFECTED_SEASONAL_PANDEMIC:
		{
			#pragma omp atomic
			new_coinfected_s_p++;
			#pragma omp atomic
			new_infected_today_pandemic++;
			#pragma omp atomic
			total_infected_pandemic++;
			#pragma omp atomic
			total_coinfected_s_p++;
			break;
		}
		case REINFECTED_PANDEMIC_SEASONAL:
		{
			#pragma omp atomic
			new_reinfected_p_s++;
			#pragma omp atomic
			new_infected_today_seasonal++;
			#pragma omp atomic
			total_infected_seasonal++;
			#pragma omp atomic
			total_reinfected_p_s++;
			break;
		}
		case REINFECTED_SEASONAL_PANDEMIC:
		{
			#pragma omp atomic
			new_reinfected_s_p++;
			#pragma omp atomic
			new_infected_today_pandemic++;
			#pragma omp atomic
			total_infected_pandemic++;
			#pragma omp atomic
			total_reinfected_s_p++;
			break;
		}
		case  COINFECTED_SIMULTANEOUS:
		{
			#pragma omp atomic
			new_infected_today_seasonal++;
			#pragma omp atomic
			total_infected_seasonal++;
			#pragma omp atomic
			new_infected_today_pandemic++;
			#pragma omp atomic
			total_infected_pandemic++;
			#pragma omp atomic
			new_sim_coinfected++;
			#pragma omp atomic
			total_sim_coinfected++;
			break;
		}
	}
//}
}

int City::getInfectedByType(int infectionType)
{
	switch(infectionType)
	{
		case INFECTED_PANDEMIC:
			return total_infected_pandemic_only;
		case INFECTED_SEASONAL:
			return total_infected_seasonal_only;
		case COINFECTED_PANDEMIC_SEASONAL:
			return total_coinfected_p_s;
		case COINFECTED_SEASONAL_PANDEMIC:
			return total_coinfected_s_p;
		case REINFECTED_PANDEMIC_SEASONAL:
			return total_reinfected_p_s;
		case REINFECTED_SEASONAL_PANDEMIC:
			return total_reinfected_s_p;
		case  COINFECTED_SIMULTANEOUS:
			return total_sim_coinfected;
		default:
			return 0;
	}
}

int City::getNewInfectedByType(int infectionType)
{
	switch(infectionType)
	{
		case INFECTED_PANDEMIC:
			return new_infected_today_pandemic_only;
		case INFECTED_SEASONAL:
			return new_infected_today_seasonal_only;
		case COINFECTED_PANDEMIC_SEASONAL:
			return new_coinfected_p_s;
		case COINFECTED_SEASONAL_PANDEMIC:
			return new_coinfected_s_p;
		case REINFECTED_PANDEMIC_SEASONAL:
			return new_reinfected_p_s;
		case REINFECTED_SEASONAL_PANDEMIC:
			return new_reinfected_s_p;
		case  COINFECTED_SIMULTANEOUS:
			return new_sim_coinfected;
		default:
			return 0;
	}
}

int City::getNewRecoverd(bool pandemic)
{
	if(pandemic==true)
		return new_recovered_today_pandemic;
	return new_recovered_today_seasonal;
}
//make a textfiles for storing output data and open them
void City::openOutputFiles()
{
	//make a city id string
	char runId[4];
	sprintf(runId,"%d",p_run->getId());
	char idStr[4];//a string representation of the cities id
	sprintf(idStr,"%d",_id);//convert id to string
	std::string rId="output_data/Run_";
	std::string cityId="_City_no";
	rId += runId+cityId+idStr;
	//make an output file for daily general stats
	char outputGenStats_cStr[42];
	strcpy(outputGenStats_cStr,rId.c_str());
	char dailyStats_cStr[21]="_dailyStatistics.txt";
	strcat(outputGenStats_cStr,dailyStats_cStr);
	//open the textflie for output
	_output_GeneralStats=fopen(outputGenStats_cStr,"w");
	_output=fopen("output_data/locationCheck.txt","w");
	//print general stats header
	fprintf(_output_GeneralStats, "Day  Population  Infected_p  Recovered_p  Infected_s  Recovered_s  Infected_p_only  Infected_s_only coinfected_s_p reinfected_s_p coinfected_p_s reinfected_p_s coinfected_sim\n");
	//checkContacts=fopen("checkContacts.txt","w");
}

void City::closeOutputFiles()
{
	fclose(_output);
	fclose(_output_GeneralStats);
	//fclose(checkContacts);
}

void City::checkRNumber()
{
	//make a city id string
	char runId[4];
	sprintf(runId,"%d",p_run->getId());
	char idStr[4];//a string representation of the cities id
	sprintf(idStr,"%d",_id);//convert id to string
	std::string rId="output_data/Run_";
	std::string cityId="_City_no";
	rId += runId+cityId+idStr;

	char output_cStr[36];//a c-string to hold the file name
	//make an output file for hourly locations of people
	strcpy(output_cStr,rId.c_str());
	char R_cStr[14]="_R_Output.txt";//append name of specific file to city's id string
	strcat(output_cStr,R_cStr);
	//open file and print header
	FILE* output_R = fopen(output_cStr,"w");
	fprintf(output_R, "Generation   Generation_size_p   Avg_R_p   Generation_size_s   Avg_R_s\n");

	//use that list of people to check reproduction numbers over time
	for(int aux=1; aux <= Flu_Manager::Instance()->max_days; ++ aux)
	{
		int aux_size_p = 0;
		int aux_sumrn_p = 0;
		int aux_size_s = 0;
		int aux_sumrn_s = 0;
		float aux_average_rn_p= 0.0f;
		float aux_average_rn_s= 0.0f;

		for(unsigned int p=0;p<community.size();p++)
		{
			if(community[p]->getGenerationPandemic()==aux){
				++aux_size_p;
				aux_sumrn_p = aux_sumrn_p + community[p]->getReproNumber(true);
			}
			if(community[p]->getGenerationSeasonal()==aux){
				++aux_size_s;
				aux_sumrn_s = aux_sumrn_s + community[p]->getReproNumber(false);
			}
		}
		//get the average repro numbers
		if(aux_size_p >0)//don't divide by zero
			aux_average_rn_p=  (float)aux_sumrn_p/aux_size_p;
		if(aux_size_s >0)//don't divide by zero
			aux_average_rn_s=  (float)aux_sumrn_s/aux_size_s;
		//store values for retrieval
		generationsPandemic[aux-1]=aux_size_p;
		generationsSeasonal[aux-1]=aux_size_s;
		averages_rn_p[aux-1]=aux_average_rn_p;
		averages_rn_s[aux-1]=aux_average_rn_s;
		fprintf(output_R,"	%d			%d			%f			%d			%f \n", aux, aux_size_p, aux_average_rn_p, aux_size_s, aux_average_rn_s);
	}
	fclose(output_R);
}

bool City::outbreakRunning()
{
	if(_daysOutbreak > 0 && _daysOutbreak <=Flu_Manager::Instance()->max_days)
		return true;
	return false;
}

void City::startOutbreak()
{
	_daysOutbreak=1;//start the clock for this city
}
//determines the number of sick arrivals from out of state for this day
void City::updateArrivals(int day)
{
	_sickArrivals.clear();//remove previous days arrivals
	bool infectedPandemic=false;
	bool infectedSeasonal=false;
	//determine how many arrivals will be sick
	for(int i=0; i < _numOutsideArrivals;i++)
	{
		float diceRoll=p_run->uni(0,1);
		if(diceRoll<=_probablityInfectedArrival_p)
			infectedPandemic=true;
		diceRoll=p_run->uni(0,1);
		if(diceRoll<=_probablityInfectedArrival_s)
			infectedSeasonal=true;
		if(infectedPandemic==true || infectedSeasonal==true)//person is infected add them to a list of infected arrivals
		{
			int age=21;
			//set the age, I will assume all travelers are adults
			float y=p_run->uni(0,1);//the random value
			if (y <= Flu_Manager::Instance()->getAdultAgesCumProb(1)) {//person is 22-29
				age = (int)p_run->uni(22, Flu_Manager::Instance()->getAdultAge(1));
			}
			else if (y >Flu_Manager::Instance()->getAdultAgesCumProb(1)&& y <= Flu_Manager::Instance()->getAdultAgesCumProb(2)) {//person is 29-64
				age= (int)p_run->uni(Flu_Manager::Instance()->getAdultAge(1), Flu_Manager::Instance()->getAdultAge(2));
			}
			else {//person is over 64
				age = (int)p_run->uni( Flu_Manager::Instance()->getAdultAge(2), Flu_Manager::Instance()->getAdultAge(3));
			}
			Person* person= new Person(age,this); //set city to this one, actual city of origin doesn't matter
			person->pickRestingPlace();//find a place for them to stay, either home or entertainment
			//infect the person
			if(infectedPandemic==true)
				person->infected(true,NULL,day);//treat as 1st generation infected (infector==null)
			if(infectedSeasonal==true)
				person->infected(false,NULL,day);
			person->setTraveling(true);
			_sickArrivals.push_back(person);
		}
		infectedPandemic=false;
		infectedSeasonal=false;
	}
}

//yuwen add 05312018_2 sample criteria begin
void City::collectFromCity(int day)
{
//yuwen add 1002 parallel begin
	//double start_t=omp_get_wtime();
	#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) shared(community)
	{
		srand((unsigned int)(time(NULL))^omp_get_thread_num());
		#pragma omp for
//yuwen add 1002 parallel end
		for(int i=0;i<community.size();i++)
		{
			community[i]->collectSample(day);
		}
	}
	//double eclapse=omp_get_wtime()-start_t;
	//printf("time used for collect samples%f\n",eclapse);
}
