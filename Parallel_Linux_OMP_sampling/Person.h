#pragma once
#include <map>
#include "City.h"

#ifndef PERSON_H
#define PERSON_H

const float PERCENT_SYMPTOMATIC=.669f; // 66.9% of people present symptomatic infection Carrat08

class Person
{
public:
	enum eVirusMode{NONE,PANDEMIC,SEASONAL,BOTH};
	enum eProfileType{UNDEFINED,GAMMA_1,LOGNORMAL_1,WEIBULL_1,GAMMA_2,LOGNORMAL_2,WEIBULL_2};
	enum eSymptomState{NOT_INFECTED,SYMPTOMATIC,ASYMPTOMATIC};
	enum eQuarantineType{NO_QUARANTINE,AT_HOME,HOSPITALIZED};
//yuwen add seek healthcare 05132018_1 begin
	enum eHealthcareType{NOCARE,ER,PRIMARYCARE,URGENTCARE}; //ER=Emergency Room=1, PRIMARYCARE=Primary Care=2, URGENTCARE=Urgentcare=3, NOCARE=No professional care=0
	enum eSeverityState{SEVERE,NOTSEVERE};
//yuwen add seek healthcare 05132018_1 end
	int infectionType;//used to determine which group the infected person will belong to.
	eVirusMode currentVirusType;//current type of infection
	eVirusMode prevVirusType;//previous type of infection
	eProfileType profilePandemic;
	eProfileType profileSeasonal;
	eSymptomState symptomatic;
	eQuarantineType quaratineState;//Is the person sick at home or hosptialized
//yuwen add seek healthcare 05132018_2 begin
	eHealthcareType healthcareType;//The type of healthcare that a case/person has decided to go.
	eSeverityState severityState;//Disease severity is assigned to a case/person.
//yuwen add seek healthcare 05132018_2 end
	Person(int,class City*);
	~Person(void);
	void setWorkplace(class Business *ptr);
	void setHousehold(class Household *ptr);
	int getAge() const;
	int getId() const;
	class City* getCity() const;
//yuwen add sample testing 05312018_1 begin
	class City* getVisitingCity() const;
//yuwen add sample testing 05322018_1 end
	void setCity(class City*);
	int getGenerationPandemic() const;
	int getGenerationSeasonal() const;
	Household* getHousehold() const;
	class Location* getLocation(int) const;
	void setLocation(int,class Location*);
	eQuarantineType isAbsent();//determins if the person will be absent from work/errands due to illness
	eQuarantineType checkHospitalized();
//yuwen add seek healthcare 05132018_3 begin
	eHealthcareType selectHealthcare(int);//determine what type of healthcare a case/person decides to go.
	eHealthcareType decideProvider(int);
	eSeverityState setSeverityState();//determine if a case/person symptom is severe or not.
	void collectSample(int);//Begin the sample collection process in each city
//yuwen add seek healthcare 05132018_3 end
	void infected(bool,Person*,int);
	int setDiseaseClockPandemic(int);
	int setDiseaseClockSeasonal(int);
	void setReproNumber(bool);
	int getReproNumber(bool)const;
	bool getVaccinationStatusPandemic()const;
	bool getVaccinationStatusSeasonal()const;
	bool getAntiviralStatus()const;
	void setSchedule(int);
	void setTraveling(bool);
	void declareLocation(int);
	void processInfections(int,int);
//yuwen add for parallelization 11/01/18 begin
	void checkOccupants(int);
	void declareLocation_unpall(int);
//yuwen add for parallelization 11/01/18 end
	//void setTraveling(City*,Location*,int);
	void stopTraveling();
	void setArrivalSchedule(int);//schedual out of state arrivals
	void pickRestingPlace();//choose a resting location for the traveler
	void addtoContacts(Person*, float); //a pointer to person*, add an infector to the list of people infecting this person
	int  getDaysInfectionPandemic(); //get the number of days that the person has been infected with pandemic flu
	int  getDaysInfectionSeasonal(); //get the number of days that the person has been infected with seasonal flu
	void infectContacts(int);
	int getContactCounterHH()const;
	int getContactCounterWP()const;
	int getContactCounterER()const;
	void setContactCounterHH(int);
	void setContactCounterWP(int);
	void setContactCounterER(int);
	float getRpreNumberInit(bool) const;
private:
	int _id;//a unique id  for each person
	int _age;
	int  _day_begin_infection_pandemic; // day of the outbreak the infection begins
	int  _day_begin_infection_seasonal; // day of the outbreak the infection begins
	int	 _disease_clock_pandemic; //0 = alive and well, -1 = dead, -3 = recover
	int _disease_clock_seasonal;
	int _daysOfPandemicInfection;// >0 = number of days since infection began; -6 = alive and well; -2 = infected; -5 = recovered
	int _daysOfSeasonalInfection;// >0 = number of days since infection began; -6 = alive and well; -2 = infected; -5 = recovered
	float _rn_init_pandemic; // initial reproduction number for panedmic virus
	float _rn_init_seasonal;//sam as above for seasonal virus
	int _rn_pandemic;// final reproduction number pandemic
	int _rn_seasonal;// final reproduction number seasonsl
	int  _contactCounterHH;//To accumulate the number of contacts in the household per day
	int _contactCounterWP;//To accumulate the number of contacts in the workplace/school per day
	int _contactCounterER;//To accumulate the number of contacts in the errands per day
	int  _generation_pandemic;//Level of pandemic infection tree where peson resides
    int  _generation_seasonal;
	bool _vaccinated_pandemic;//is the person vaccinated for pandemic virus
	bool _vaccinated_seasonal;//is the person vaccinated for seasonal virus
	bool _onAntivirals;//is the person recieving antiviral treatmet
	bool _traveling;//is the person visiting another city or going out of state
	int _maxTravelDays;//maximum number of days person will be out of town
	int _daysTraveled;//number of days person has been out of town
	class Business *p_workplace;//pointer to the person's place of business
	class Household *p_home;//pointer to the person's household
	Location *p_restPlace;//pointer to where a person is staying for traveling purposes
	Location *p_Outside;//out of state location
	City* p_city;//pointer to the city person is from
	City* p_visitingCity;//pointer to the city the person is visiting
	class Location *_schedule[24];//array of pointers to locations were the person is located throughout the day
	struct Errand{ //a struct to hold the hour, business, and number of contacts for a given errand
		int _errandHour;
		int _businessId;
		int _numContacts;
	};
	struct Contact{
		Person* _contactee;//the person being contacted by an infected person
		float _locModifier;
	};
	std::vector<struct Contact> _contacts;//a map of contacts 
	struct Errand _errandsUnemployed[3];//array of errand structs for weekday errands
	struct Errand _errandsWeekend[3]; //same as above for weekend errands
	static int _idCounter;//A value that increases with each pearson created, giving them a unique id number;
	void setAdultSchedule();
	void setChildSchedule();
	void setWeekendSchedule();
	void profile_assignment(eVirusMode);
	int makeContacts(int,float,int,int);
	void exposeContact(Person*,float,int);
	float setProbability(float,eProfileType,int, bool,Person *,float);
	bool checkIds(int,std::vector<int>&);
	void setVaccinationStatus();
	void setAntiviralStatus();
	void setTravelState();
//yuwen add sample testing 05172018_7 begin
	void sampleCollectCL(int);//Decide from what type of healthcare a sample is eollected and the virus type.
	bool _sampleCollected;//Is the person sample collected already?
//yuwen add sample testing 05172018_7 end
//yuwen add for parallelization 11/01/18 begin
	bool _inOccupants;//If the person is already in _occupants of a location in a particular hour
//yuwen add for parallelization 11/01/18 end
};
#endif
