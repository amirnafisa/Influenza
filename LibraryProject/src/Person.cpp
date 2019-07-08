/*Implemetation of the Person class */
#include<set>
#include "Person.h"
#include "Multi_City_44_Templates.h"
#include "Flu_Manager.h"
#include <omp.h>
#include <math.h>
#include <string.h>

#define WEEKDAY 0
#define WEEKEND 1
#define UNEMPLOYED 2


using namespace std;

int Person::_idCounter=0;//first person created will have id 0

Person::Person(int age,City* city)
{
	#pragma omp critical
	{
		_id=_idCounter;//increments with each person created
		_idCounter++;
	}
	_day_begin_infection_pandemic=0; 
	_day_begin_infection_seasonal=0; 
	_disease_clock_pandemic=0;
	_disease_clock_seasonal=0;
	_daysOfPandemicInfection=-6;//all people start out alive and well
	_daysOfSeasonalInfection=-6;
	_rn_init_pandemic=0.0f;
	_rn_init_seasonal=0.0f;
	_rn_pandemic=0;
	_rn_seasonal=0;
	_generation_pandemic=0;
    _generation_seasonal=0;
	_contactCounterWP=0;
	_contactCounterER=0;
	_contactCounterHH=0;
	_age=age;
	_inOccupants=true;
	p_city=city;
	p_visitingCity=city;
	currentVirusType=NONE;
	prevVirusType=NONE;
	profilePandemic=UNDEFINED;
	profileSeasonal=UNDEFINED;
	symptomatic=NOT_INFECTED;
	quaratineState=NO_QUARANTINE;//no one starts out too sick to work
//yuwen add seek healthcare 05132018_4 begin
	healthcareType=NOCARE;//A person is initially set to no healthcare providers needed.
	severityState=NOTSEVERE;//A person is initially set to no disease.
	_sampleCollected=false;//A person is not collected sample yet.
//yuwen add seek healthcare 05132018_4 end
	_traveling=false;
	_maxTravelDays=0;
	_daysTraveled=0;
	//_contacts.clear();
	//_contacts.shrink_to_fit();
	//populate the errand arrays with Errand Structs
	for(int i=0;i<3;i++)
	{
		struct Errand uErrand; //weekday errands for the unemployed
		uErrand._errandHour=8;
		uErrand._businessId=1;
		uErrand._numContacts=0;
		_errandsUnemployed[i]=uErrand;
		struct Errand wErrand;//weekend errands
		wErrand._errandHour=10;
		wErrand._businessId=1;
		wErrand._numContacts=0;
		_errandsWeekend[i]=wErrand;
	}
	setVaccinationStatus();
	setAntiviralStatus();
}


Person::~Person(void)
{
	//_contacts.clear();
}

void Person::setWorkplace(Business* work)
{	
	p_workplace=work;
}

void Person::setVaccinationStatus()
{
	if(Flu_Manager::Instance()->PANDEMIC_VACCINATION==1 || Flu_Manager::Instance()->SEASONAL_VACCINATION==1)
	{
		//assign % coverage based on age
		float vaccineCoverage;//% of people that have been vaccinated for pandemic flu
		if(_age<=5)
			vaccineCoverage= Flu_Manager::Instance()->VACC_COVERAGE_0TO5;
		else if(_age>5 && _age<=14)
			vaccineCoverage= Flu_Manager::Instance()->VACC_COVERAGE_6TO14;
		else if(_age>14 && _age<=17)
			vaccineCoverage= Flu_Manager::Instance()->VACC_COVERAGE_15TO17;
		else if(_age>17 && _age<=29)
			vaccineCoverage= Flu_Manager::Instance()->VACC_COVERAGE_18TO29;
		else if(_age>29 && _age<=64)
			vaccineCoverage= Flu_Manager::Instance()->VACC_COVERAGE_30TO64;
		else //person is 64+ years old
			vaccineCoverage = Flu_Manager::Instance()->VACC_COVERAGE_65PLUS;
		if(Flu_Manager::Instance()->PANDEMIC_VACCINATION==1)//Pandemic vaccinations being used in simulation
		{  
			float diceRoll=p_city->getSimRun()->uni(0,1);//%chance that person has been vaccinated for pandemic flu
			if(diceRoll<=vaccineCoverage)
				_vaccinated_pandemic=true;
			else
				_vaccinated_pandemic=false;
		}
		else
			_vaccinated_pandemic=false;
		if(Flu_Manager::Instance()->SEASONAL_VACCINATION==1)//Seasonal vaccinations being used in simulation
		{
			float diceRoll=p_city->getSimRun()->uni(0,1);//%chance that person has been vaccinated for seasonal flu
			if(diceRoll<=vaccineCoverage)
				_vaccinated_seasonal=true;
			else
				_vaccinated_seasonal=false;
		}
		else
			_vaccinated_seasonal=false;
	}
	else //vaccination is not being used in simulation
	{
		_vaccinated_pandemic=false;
		_vaccinated_seasonal=false;
	}	
}

void Person::setAntiviralStatus()
{
	if(Flu_Manager::Instance()->usingAntivirals==1)
	{
		if(_age<=5 || _age>=65)//antivirals will be given only to the most vulnerable people
		{
			float diceRoll=p_city->getSimRun()->uni(0,1);
			if(diceRoll <= Flu_Manager::Instance()->ANTIVIRAL_PROB)
				_onAntivirals=true;
			else
				_onAntivirals=false;
		}
		else
			_onAntivirals=false;
	}
	else
		_onAntivirals=false;
}

void Person::setHousehold(Household* home)
{
	p_home= home;
	p_home->addOccupant(this);//homes start out occupied at hour 1;
	p_restPlace=home;//home is the persons defaul resting place
}

int Person::getAge() const
{
	return _age;
}

Household* Person::getHousehold() const
{
	return p_home;
}

int Person::getId() const
{
	return _id;
}

City* Person::getCity() const
{
	return p_city;
}

void Person::setCity(City* city)
{
	p_city=city;
}

//yuwen add sample testing 05172018_7 begin
City* Person::getVisitingCity() const
{
	return p_visitingCity;
}
//yuwen add sample testing 05172018_7 end

Location* Person::getLocation(int hr) const
{
	return _schedule[hr];
}

int Person::getGenerationPandemic() const
{
	return _generation_pandemic;
}
int Person::getGenerationSeasonal() const
{
	return _generation_seasonal;
}
//switch persons current location
void Person::setLocation(int hr,Location* newLoc)
{
	_schedule[hr]=newLoc;
}

void Person::setReproNumber(bool pandemic)
{
	if(pandemic==true)
		_rn_pandemic++;
	else
		_rn_seasonal++;

}

int Person::getReproNumber(bool pandemic) const
{
	if(pandemic==true)
		return _rn_pandemic;
	return _rn_seasonal;
}

bool Person::getVaccinationStatusPandemic()const
{
	return _vaccinated_pandemic;
}

bool Person::getVaccinationStatusSeasonal()const
{
	return _vaccinated_seasonal;
}

bool Person::getAntiviralStatus()const
{
	return _onAntivirals;
}
//yuwen add sampling testing 05162018_4 begin
//set the severity of a symptomatic case
Person::eSeverityState Person::setSeverityState()
{
	float prob,randNum;
	if(symptomatic==SYMPTOMATIC)
	{
		if (_daysOfPandemicInfection>0)
		{
			randNum = p_city->getSimRun()->uni(0,1);
			prob = Flu_Manager::Instance()->getGamma1CumDensity(_daysOfPandemicInfection) * Flu_Manager::Instance()->SEVERITY;
			if(randNum <= prob)
			{
				return SEVERE;//The case/person has severe syptom.
			}
		}
		if (_daysOfSeasonalInfection>0)
		{
			randNum = p_city->getSimRun()->uni(0,1);
			prob = Flu_Manager::Instance()->getGamma1CumDensity(_daysOfSeasonalInfection)  * Flu_Manager::Instance()->SEVERITY;
			if(randNum <= prob)
			{	
				return SEVERE;//The case/person has severe syptom.
			}
		}
	}
	return NOTSEVERE;		
}
//yuwen add sampling testing 05162018_4 end

//determines if the person will be absent from work due to illness
Person::eQuarantineType Person::isAbsent()
{
//yuwen add seek healthcare 05162018_2 begin
	if(severityState==NOTSEVERE)
		return NO_QUARANTINE;//person is not infected or has not had the virus long enough to show symptoms
	return checkHospitalized();
//yuwen add seek healthcare 05162018_2 end
//yuwen delete seek healthcare 05162018_1 begin
/*
	float prob;
	if(_daysOfPandemicInfection<=0 && _daysOfSeasonalInfection<=0)
		return NO_QUARANTINE;//person is not infected or has not had the virus long enough to show symptoms
	float randNum = p_city->getSimRun()->uni(0,1);
	if(_daysOfPandemicInfection>0)// check pandemic virus
	{
		prob = Flu_Manager::Instance()->getGamma1CumDensity(_daysOfPandemicInfection) * Flu_Manager::Instance()->SEVERITY;
		if(randNum <= prob)
		{
			return checkHospitalized();//person will either stay home or go to the hospital
		}
	}
	if(_daysOfSeasonalInfection>0)//check seasonal virus
	{
		prob = Flu_Manager::Instance()->getGamma1CumDensity(_daysOfSeasonalInfection)  * Flu_Manager::Instance()->SEVERITY;
		if(randNum <= prob)
		{	
			return checkHospitalized();
		}
	}
	return NO_QUARANTINE;//person feels well enough to work
*/
}

//Determine if a person too ill too work will be hospitalized or just stay home
Person::eQuarantineType Person::checkHospitalized()
{	
	if(Flu_Manager::Instance()->quarantineOption==Flu_Manager::Instance()->HOSPITALIZATION)//hospitalization option is being used
	{
		float prob=0.0f;//percent chance that a person will be hospitalized
		if(_age<=5 ||(_age>29 &&_age<64))
			prob=.001f;// unit is %
		else if(_age>5 && _age<=29)
			prob=.000429f;
		else //person is 64+ years old
			prob=.005f;
		prob/=100;//normalize percentage
		float diceRoll=p_city->getSimRun()->uni(0,1);
		if(diceRoll<=prob)
			return HOSPITALIZED;//person will be hospitalized
		return AT_HOME;//person will stay home
	}
	return AT_HOME;//No hosptalization option, everyone too sick to work just stays home
}

//yuwen add seek healthcare 05172018_6 begin
void Person::collectSample(int day)
{
	healthcareType=selectHealthcare(day);
	if(!(healthcareType==NOCARE))
		sampleCollectCL(day);
}
//yuwen add seek healthcare 05172018_6 end

//yuwen add seek healthcare 05132018_9 begin
//determine what type of healthcare a person needs to seek.
Person::eHealthcareType Person::selectHealthcare(int day)
{
	float prob, randNum;
	if((_daysOfPandemicInfection<0 && _daysOfSeasonalInfection<0) || _sampleCollected==true)//The case is healthy, or the sample has been collected
	{
		return NOCARE;//The case doesn't need healthcare since the case is healthy.
	}
	else if(severityState==NOTSEVERE && _sampleCollected==false)//The case is sick but not severe.
	{
		randNum=p_city->getSimRun()->uni(0,1);
		prob=Flu_Manager::Instance()->PERCEIVED_PANDEMIC_PROB;//probability that people are perceived as pandemic flu 
		if(randNum<=prob)
			return decideProvider(day);
		return NOCARE;
	}
	else if(severityState==SEVERE && _sampleCollected==false)//The case is sick and severe.
	{
		return decideProvider(day);
	}
    return NOCARE; //Nafisa changed
}

Person::eHealthcareType Person::decideProvider(int day)
{
	float prob, randNum;
//yuwen add begin day in week 010819 begin
	int _offsetDays = day + Flu_Manager::Instance()->BEGIN_WEEKDAY - 1;
	if(_offsetDays%7 != 0 && _offsetDays%7 != 6)
//yuwen add begin day in week 010819 end
//yuwen delete begin day in week 010819 begin
	//if(day%7 != 0 && day%7 != 6)//The case is a child that can be seen by pediatricians immediately on weekdays
//yuwen delete begin day in week 010819 end
	{
		randNum=p_city->getSimRun()->uni(0,1);
		prob=Flu_Manager::Instance()->PR_ACCESSIBLITY;//The probability that a person can access primary care
		if(randNum<=prob)
			return PRIMARYCARE;//The case decides to go to primary care(pediatrician).
		if(p_home->homePovertyState==Household::LOW)//The case can afford ER medical expenses.
			return ER;//The case desides to go to ER right away.
		return URGENTCARE;//The case desides to go to urgent care right away.
	}
	else //No pediatricians on weekends, so a child can only go to ER or Urgent care on weekends
	{
		if(p_home->homePovertyState==Household::LOW)//The case can afford ER medical expenses.
			return ER;//The case desides to go to ER right away.
		return URGENTCARE;//The case desides to go to urgent care right away.
	}	
}

//yuwen add seek healthcare 05132018_9 end

//yuwen add test in clinic labs 05152018_1 begin
/*take the sample from the person for flu test in clinic labs*/
void Person::sampleCollectCL(int day)
{
	float prob, randNum;
	randNum=p_city->getSimRun()->uni(0,1);
	if(healthcareType==ER)
	{
		prob=Flu_Manager::Instance()->ED_MSSS_PERCENTAGE;
		if(randNum<=prob)
		{
			p_city->getSimRun()->addtoMsssPool(this,day);
		}
		p_city->getSimRun()->addtoSamplePool(this,day);
		_sampleCollected=true;
	}
	else if(healthcareType==PRIMARYCARE || healthcareType==URGENTCARE)
	{
		prob=Flu_Manager::Instance()->COLLECT_PROB;
		if(randNum<=prob)
		{
			p_city->getSimRun()->addtoSamplePool(this,day);
			_sampleCollected=true;
		}
		else
			p_city->getSimRun()->addtoUnCollected(this,day);
	}
}
//yuwen add test in clinic labs 05152018_1 end

/*yuwen comment sampling 062518
void samplingCriteriaFIFO()
{
	//by sample clock day
}

void samplingCriteriaRisk()
{
	//by sample age,hospitalized*/
//yuwen comment sampling 062518 end

/*when a person is first infected this function sets the appropriate parameters. Takes a boolean argument;
true if the infecting virus is pandemic, false if it's seasonal*/
void Person::infected(bool pandemic,Person* infector,int day)
{
	if(pandemic==true)
	{	
		//contactee has pandemic virus
		if(infector==NULL)//initial infected
		{	_generation_pandemic=1;
//yuwen add 0107 initial begin
			_day_begin_infection_pandemic=0;
//yuwen add 0107 initial end
		}
		//printf("_generation_pandemic is %d", _generation_pandemic);
		else //infected contactee is one level down from infector on the tree
		{	
			_generation_pandemic=infector->getGenerationPandemic()+1;
			infector->setReproNumber(true);
//yuwen add 0107 initial begin
			_day_begin_infection_pandemic=day;
//yuwen add 0107 initial end
		}
		_disease_clock_pandemic=0;	
		_daysOfPandemicInfection = -2;//set to "just infected"
//yuwen delete 0107 initial begin
		//_day_begin_infection_pandemic=day;
//yuwen delete 0107 initial end
		if(prevVirusType==NONE)
		{
			_rn_init_pandemic= Flu_Manager::Instance()->reproduction_number_pandemic;
			currentVirusType=PANDEMIC;
			//printf("currentvirus type is %d\n", currentVirusType);
			prevVirusType=PANDEMIC;
			infectionType=INFECTED_PANDEMIC;

		}
		else if(prevVirusType==SEASONAL)
		{
			if(_disease_clock_seasonal==-3) // recovered from seasonal
			{
				_rn_init_pandemic= Flu_Manager::Instance()->reproduction_number_pandemic*Flu_Manager::Instance()->epsilon_pr;
				currentVirusType=PANDEMIC;
				prevVirusType=BOTH;//person was already picked for seasonal, now he/she has both
				infectionType=REINFECTED_SEASONAL_PANDEMIC;
			}
				
			else//person is now infected with both viruses
			{
				_rn_init_pandemic= Flu_Manager::Instance()->reproduction_number_pandemic*Flu_Manager::Instance()->epsilon_p;
				currentVirusType=BOTH;
				prevVirusType=BOTH;//person was already picked for seasonal, now he/she has both
				if(_day_begin_infection_seasonal==day)//person has simultaneously been infected with pandemic and seasonal virus
					infectionType=COINFECTED_SIMULTANEOUS;
				else//infections were not simultaneous
					infectionType=COINFECTED_SEASONAL_PANDEMIC;
			}		
		}
		//printf("person with pandemic virus is %d with disease clock %d \n",_id, _disease_clock_pandemic);
	}
	else //contactee has seasonal virus
	{
		if(infector==NULL)//initial infected
		{
			_generation_seasonal=1;
//yuwen add 0107 initial begin
			_day_begin_infection_seasonal=0;
//yuwen add 0107 initial end
		}
		else
		{
			_generation_seasonal=infector->getGenerationSeasonal()+1; //infected is one level down from infector on the tree
			infector->setReproNumber(false);
//yuwen add 0107 initial begin
			_day_begin_infection_seasonal=day;
//yuwen add 0107 initial end
		}
		_disease_clock_seasonal=0;
		_daysOfSeasonalInfection = -2;
//yuwen delete 0107 initial begin
		//_day_begin_infection_seasonal=day;
//yuwen delete 0107 initial end
		if(prevVirusType==NONE)
		{
			_rn_init_seasonal= Flu_Manager::Instance()->reproduction_number_seasonal;
			currentVirusType=SEASONAL;
			//printf("seasonal virus\n");
			prevVirusType=SEASONAL;
			infectionType=INFECTED_SEASONAL;
		}	
		else if(prevVirusType==PANDEMIC)
		{
			if(_disease_clock_pandemic==-3)
			{
					_rn_init_seasonal= Flu_Manager::Instance()->reproduction_number_seasonal*Flu_Manager::Instance()->epsilon_sr;
					currentVirusType=SEASONAL;
					prevVirusType=BOTH;
					infectionType=REINFECTED_PANDEMIC_SEASONAL;
			}	
			else
			{
					_rn_init_seasonal= Flu_Manager::Instance()->reproduction_number_seasonal*Flu_Manager::Instance()->epsilon_s;
					currentVirusType=BOTH;
					prevVirusType=BOTH;
					if(_day_begin_infection_pandemic==day)//person has simultaneously been infected with pandemic and seasonal virus
						infectionType=COINFECTED_SIMULTANEOUS;
					else
						infectionType=COINFECTED_PANDEMIC_SEASONAL;
			}		
		}
		//printf("person with seasonal virus is %d with disease clock %d \n",_id, _disease_clock_pandemic);
	}
	//int n = omp_get_thread_num();
	//printf("thread number is %d, current virus type is %d\n", n, omp_get_thread_num(), currentVirusType);
	profile_assignment(currentVirusType);//set symptom profile
}
/*update the number of days a sick person has been infected, or set them as recovered if the period of infection
is over. Takes the hour of the day as an argument. Returns 1 if person is recovered 0 otherwise*/
int Person::setDiseaseClockPandemic(int hour)
{
	int recovered=0;
	if(_disease_clock_pandemic>=0 && _disease_clock_pandemic< culmination_period)//the person is not in recovery
	{  
		if(_daysOfPandemicInfection>0 || (_daysOfPandemicInfection==-2 && hour==8)) //the person has an ongoing infection
		{
			_disease_clock_pandemic++;
			_daysOfPandemicInfection = (_disease_clock_pandemic/(24+1))+1;
		}
	}
	else if(_disease_clock_pandemic >= culmination_period) //the period of infection is over, the person is marked as recovered
	{
		_disease_clock_pandemic=-3;
		_daysOfPandemicInfection=-5;
		if(currentVirusType==BOTH)
			currentVirusType=SEASONAL;
		else
		{
			currentVirusType=NONE;
			//p_city->remmoveFromInfectedList(_id);
		}
		recovered=1;
	}
	return recovered;//added to city's list of number of newly recovered this day
}
/*same as setDiseaseClockPandemic, but for seasonal virus*/
int Person::setDiseaseClockSeasonal(int hour)
{
	int recovered=0;
	if(_disease_clock_seasonal>=0 && _disease_clock_seasonal< culmination_period)
	{
		if(_daysOfSeasonalInfection>0 || (_daysOfSeasonalInfection==-2 && hour==8))
			{
				_disease_clock_seasonal++;
				_daysOfSeasonalInfection = (_disease_clock_seasonal/(24+1))+1;
			}
	}
	else if(_disease_clock_seasonal >= culmination_period)
	{
		_disease_clock_seasonal=-3;
		_daysOfSeasonalInfection=-5;
		if(currentVirusType==BOTH)
			currentVirusType=PANDEMIC;
		else
		{
			currentVirusType=NONE;
			//p_city->remmoveFromInfectedList(_id);
		}
		recovered=1;
	}
	return recovered;
}
/*set a persons daily schedule.Takes as arguments the day to be scheduled and a reference to the array of businesses
in the city. 
_outSick is false by default and will only change when the Absenteeism option is set to on(1)*/
void Person::setSchedule(int day)
{
//yuwen add seek healthcare 05162018_3 begin
//set disease severity for only symptomatic cases
	severityState=setSeverityState();
//yuwen add seek healthcare 05162018_3 end
	if(Flu_Manager::Instance()->quarantineOption!=Flu_Manager::Instance()->NONE)//A quarantine option is being used, check if person will be out sick or in the hosptial
		quaratineState=isAbsent();
	if((Flu_Manager::Instance()->ALLOW_TRAVEL>0 ||Flu_Manager::Instance()->ALLOW_OOS_TRAVEL>0) && quaratineState==NO_QUARANTINE)
	{
		//check who is travelling
		if((_daysOfPandemicInfection>=0 && _daysOfPandemicInfection<10) ||( _daysOfSeasonalInfection>=0 &&  _daysOfSeasonalInfection < 10) || _traveling==true)
			setTravelState();
	}
	if(quaratineState==HOSPITALIZED)//put person in hospital
	{
		for(int i=1;i<=24;i++)
			_schedule[i-1]=p_city->getHospital();
	}
	else if (p_restPlace == p_Outside || quaratineState==AT_HOME)//person is out of state schedule him to the out of state location all day
	{
		for(int i=1;i<=24;i++)
			_schedule[i-1]=p_restPlace;
	}
	else if(quaratineState==NO_QUARANTINE)//person feels fine, is in state, and will have their regular schedule
	{
		for(int i=1;i<=24;i++)//reset all schedule hours to resting place, people too sick to work will not be scheduled anywhere else
			_schedule[i-1]=p_restPlace;
//yuwen add begin day in a week 010819 begin
		int _offsetDays = day + Flu_Manager::Instance()->BEGIN_WEEKDAY - 1;
		if (_offsetDays%7 != 0 && _offsetDays%7 != 6)
//yuwen add begin day in a week 010819 end
//yuwen delete begin day in a week 010819 begin
		//if (day%7 != 0 && day%7 != 6)  //a weekday begins ...
//yuwen delete begin day in a week 010819 end
		{
				if(_age<=22)//person gets a child schedule
					setChildSchedule();
				else
					setAdultSchedule();
		}
		else//a weekend day begins ...
		{
			setWeekendSchedule();
		}
	}
}
//yuwen change add
void Person::setAdultSchedule()
{
	//printf("this is %d thread.\n", omp_get_thread_num());
	if(p_workplace->eType!=Business::HOME && _traveling==false)//person works outside the home and is not traveling
	{//send person to work from 8:00-17:00

		for(int hr=8;hr<=17;hr++)
			_schedule[hr-1]=p_workplace;
		//after work errands
		for(int hr=18;hr<=19;hr++)
		{
			float y=p_city->getSimRun()->uni(0,1);

			int low=0;
			int high=p_visitingCity->getBusinessSize()-1;
			int mid = (low + high)/2;
			while ( high - low >= 5) {
				if( y <= p_visitingCity->getBusiness(mid)->getPerWeekdayErrands() ){
					high = mid;
					mid = (low + high)/2;
				}
				if( y >p_visitingCity->getBusiness(mid)->getPerWeekdayErrands() ){
					low = mid;
					mid = (low + high)/2;
				}
			}
			// fish out y in the cum % of weekday errands in business entities
			for (int i = low; i <= high; i++) {
				float prevWeekErrPer=p_visitingCity->getBusiness(i-1)->getPerWeekdayErrands();
				if (y >= prevWeekErrPer && y < p_visitingCity->getBusiness(i)->getPerWeekdayErrands()) {
					_schedule[hr-1] = p_visitingCity->getBusiness(i);
				}
			}
		}
	}
	else //person is unemployed,traveling, or works at home
	{
		// select 3 different time slots between 8:00 and 19:00
		set<int> errandHours; //a set ensures each number hour will be a unique value
		while(errandHours.size()<3)
		{
			int hr=(int)p_city->getSimRun()->uni(8,20);
			errandHours.insert(hr);
		}
		int slot=0;
		//set the errand hours
		for(set<int>::iterator it=errandHours.begin();it!=errandHours.end();it++)
		{
					_errandsUnemployed[slot]._numContacts=0;//clear yesterdays contacts
					_errandsUnemployed[slot]._errandHour=*it;
					slot++;
		}
		//put 2 contacts into the schedule

		for(int i=0;i<2;i++)
		{
			int slot=(int)p_city->getSimRun()->uni(0,3);
			_errandsUnemployed[slot]._numContacts++;
		}
		//set the business location of the errands and the daily schedule
		for (int err= 0; err<3 ;err++) {
			float y = p_city->getSimRun()->uni(0, 1);
			// fish out y in the cum % of weekday errands in business entities
            int low = 1; 
			int high = p_visitingCity->getBusinessSize()-1;
			int mid = (low + high)/2;
			while ( high - low >= 5) {
				if( y <= p_visitingCity->getBusiness(mid)->getPerWeekdayErrands() ){
					high = mid;
					mid = (low + high)/2;
				}
				if( y > p_visitingCity->getBusiness(mid)->getPerWeekdayErrands() ){
					low = mid;
					mid = (low + high)/2;
				}
			}
			for (int i = low; i <= high; i++) {
				float prevWeekErrPer=p_visitingCity->getBusiness(i-1)->getPerWeekdayErrands();
				if (y >= prevWeekErrPer && y < p_visitingCity->getBusiness(i)->getPerWeekdayErrands()) {
					_schedule[_errandsUnemployed[err]._errandHour-1] = p_visitingCity->getBusiness(i);
					_errandsUnemployed[err]._businessId=p_visitingCity->getBusiness(i)->getId(); //record the associated business
				}
			}
		}
	}
}
//yuwen change end
//yuwen change add
void Person::setChildSchedule()
{
	//printf("this is %d thread.\n", omp_get_thread_num());
	if(_traveling==true || _age <3)//child is out of town or too young for school, assign the child weekday errands
	{
		// select 3 different time slots between 8:00 and 19:00
		set<int> errandHours; //a set ensures each number hour will be a unique value
		while(errandHours.size()<3)
		{
			int hr=(int)p_city->getSimRun()->uni(8,20);
			errandHours.insert(hr); 
		}
		int slot=0;
		//set the errand hours
		for(set<int>::iterator it=errandHours.begin();it!=errandHours.end();it++)
		{
					_errandsUnemployed[slot]._numContacts=0;//clear yesterdays contacts
					_errandsUnemployed[slot]._errandHour=*it;
					slot++;
		}
		//put 2 contacts into the schedule
		for(int i=0;i<2;i++)
		{
			int slot=(int)p_city->getSimRun()->uni(0,3);
			_errandsUnemployed[slot]._numContacts++;
		}
		//set the business location of the errands and the daily schedule
		for (int err= 0; err<3 ;err++) {
			float y = p_city->getSimRun()->uni(0, 1);
			// fish out y in the cum % of weekday errands in business entities
            int low = 1; 
			int high = p_visitingCity->getBusinessSize()-1;
			int mid = (low + high)/2;
			while ( high - low >= 5) {
				if( y <= p_visitingCity->getBusiness(mid)->getPerWeekdayErrands() ){
					high = mid;
					mid = (low + high)/2;
				}
				if( y > p_visitingCity->getBusiness(mid)->getPerWeekdayErrands() ){
					low = mid;
					mid = (low + high)/2;
				}
			}
			for (int i = low; i <= high; i++) {
				float prevWeekErrPer=p_visitingCity->getBusiness(i-1)->getPerWeekdayErrands();
				if (y >= prevWeekErrPer && y < p_visitingCity->getBusiness(i)->getPerWeekdayErrands()) {
					_schedule[_errandsUnemployed[err]._errandHour-1] = p_visitingCity->getBusiness(i);
					_errandsUnemployed[err]._businessId=p_visitingCity->getBusiness(i)->getId(); //record the associated business
				}
			}
		}
	}
	else 
	{
	// send the child to his school from 8:00 to 15:00
		for (int hr = 8; hr <= 15; hr++) 
			_schedule[hr-1]=p_workplace;

	// look for the afterschool center
		int center = (int)p_city->getSimRun()->uni(0,p_city->getBusinessNumbers(9));
		for (int i = 1; i <= p_city->getBusinessSize(); ++i)
		{
			if(p_city->getBusiness(i)->eType==Business::AFTERSCHOOL_CENTER)
			{	// put the afterschool center on the child's schedule
				for(int hr=16; hr<=19;hr++)
				{
					_schedule[hr-1]=p_city->getBusiness(i+center);
				}	
				break;
			}
		}
	}
}
//yuwen change end
//yuwen change add
void Person::setWeekendSchedule()
{
	// select 3 different time slots between 10:00 and 20:00
	//printf("this is %d thread.\n", omp_get_thread_num());
	set<int> errandHours; //a set ensures each number hour will be a unique value
	while(errandHours.size()<3)
	{
		int hr=(int)p_city->getSimRun()->uni(10,21);
		errandHours.insert(hr);
	}
	int slot=0;
	//set the errand hours
	for(set<int>::iterator it=errandHours.begin();it!=errandHours.end();it++)
	{
		_errandsWeekend[slot]._numContacts=0;//clear yesterdays contacts
		_errandsWeekend[slot]._errandHour=*it;
		slot++;
	}
	//put 2 contacts into the schedule
	for(int i=0;i<2;i++)
	{
		int slot=(int)p_city->getSimRun()->uni(0,3);
		_errandsWeekend[slot]._numContacts++;
	}
	//set the business location of the errands and the daily schedule
	for (int err= 0; err<3 ;err++) {
		float y = p_city->getSimRun()->uni(0, 1);
		// fish out y in the cum % of weekday errands in business entities
        int low = 1; 
		int high = p_visitingCity->getBusinessSize()-1;
		int mid = (low + high)/2;
		while ( high - low >= 5) {
			if( y <= p_visitingCity->getBusiness(mid)->getPerWeekendErrands() ){
				high = mid;
				mid = (low + high)/2;
		}
			if( y > p_visitingCity->getBusiness(mid)->getPerWeekendErrands() ){
				low = mid;
				mid = (low + high)/2;
			}
		}
		for (int i = low; i <= high; i++) {
			float prevWeekErrPer=p_visitingCity->getBusiness(i-1)->getPerWeekendErrands();
			if (y >= prevWeekErrPer && y < p_visitingCity->getBusiness(i)->getPerWeekendErrands()) {
				_schedule[_errandsWeekend[err]._errandHour-1] = p_visitingCity->getBusiness(i);
				_errandsWeekend[err]._businessId=p_visitingCity->getBusiness(i)->getId(); //record the associated business
			}
		}
	}
}
//yuwen change end
//yuwen add 102618 separate declaralocation for persons end

//yuwen add for parallelization 11/01/18 begin
//adds the person to the list of occupants at his/her location at the hour passed as an argument
void Person::checkOccupants(int hr)
{
	_inOccupants=true;
	if(quaratineState!=HOSPITALIZED && p_restPlace !=p_Outside)// hospital and out of state locations are null pointers
		_inOccupants=_schedule[hr-1]->hasOccupant(this);
}

void Person::declareLocation(int hr)
{
	if(_inOccupants==false)
		_schedule[hr-1]->addOccupant(this);
}

void Person::declareLocation_unpall(int hr)
{
	if(quaratineState!=HOSPITALIZED && p_restPlace !=p_Outside)// hospital and out of state locations are null pointers
	{
		if(!_schedule[hr-1]->hasOccupant(this))
			_schedule[hr-1]->addOccupant(this);
	}
}
//yuwen add for parallelization 11/01/18 end

void Person::processInfections(int hr, int day)
{
	if((_disease_clock_pandemic>0 && _disease_clock_pandemic<culmination_period)||(_disease_clock_seasonal>0 && _disease_clock_seasonal<culmination_period))
	{
			
			if(quaratineState!=HOSPITALIZED && p_restPlace !=p_Outside)//person is in state, out of the Hospital and can make contacts
			{
				float locMod=Flu_Manager::Instance()->k_hh;//currentlocationModifier
				//std::vector<int> checkHours;//the key hours of the day to accumulate contacts
//yuwen add begin day in a week 010819 begin
				int _offsetDays = day + Flu_Manager::Instance()->BEGIN_WEEKDAY - 1;
				if (_offsetDays%7 != 0 && _offsetDays%7 != 6)
//yuwen add begin day in a week 010819 end
//yuwen delete begin day in a week 010819 begin
				//if(day%7!=0 && day%7 !=6)//weekday
//yuwen delete begin day in a week 010819 end
				{
					if(hr==20)//everyone goes home at this time
					{
						_contactCounterHH=makeContacts(hr,locMod,WEEKDAY,0);
						#pragma omp atomic
						p_visitingCity->total_HH +=_contactCounterHH;
						//infectContacts(day);//at the end of the day all contacts are made, time to infect them//yuwen gu parallel pull out loop
					}		
					else if(p_workplace->eType!=Business::HOME && quaratineState==NO_QUARANTINE && _traveling==false)
					{

						if(hr==8)
						{
							locMod=Flu_Manager::Instance()->k_wp;
							_contactCounterWP=makeContacts(hr,locMod,WEEKDAY,0);
							#pragma omp atomic
							p_city->total_WP +=_contactCounterWP;
						}	
						else if(_age>22 && hr==18)//adult
						{
							locMod=Flu_Manager::Instance()->k_er;
							_contactCounterER=makeContacts(hr,locMod,WEEKDAY,0);
							#pragma omp atomic
							p_city->total_ER +=_contactCounterER;
						}
						else if(_age<=22 && hr==16)//child
						{
							locMod=Flu_Manager::Instance()->k_er;
							_contactCounterER=makeContacts(hr,locMod,WEEKDAY,0);
							#pragma omp atomic
							p_city->total_ER +=_contactCounterER;
						}
					}
					else if(quaratineState==NO_QUARANTINE )//unemployed or traveling
					{
						for(int err=0;err<3;err++)
						{
							if(_errandsUnemployed[err]._errandHour==hr)
							{
								locMod=Flu_Manager::Instance()->k_er;
								_contactCounterER+=makeContacts(hr,locMod,UNEMPLOYED,err);
								#pragma omp atomic
								p_visitingCity->total_ER +=_contactCounterER;
							}
						}
					}
				}
				else//weekend
				{
					if(hr==21)//everyone goes home at this time
					{
						_contactCounterHH=makeContacts(hr,locMod,0,0);
						#pragma omp atomic
						p_visitingCity->total_HH +=_contactCounterHH;
						//infectContacts(day);//at the end of the day all contacts are made, time to infect them//yuwen gu parallel pull out loop
					}
					else if(quaratineState==NO_QUARANTINE)
					{
						for(int err=0;err<3;err++)
						{
							if(_errandsWeekend[err]._errandHour==hr)
							{
								locMod=Flu_Manager::Instance()->k_er;
								_contactCounterER+=makeContacts(hr,locMod,WEEKEND,err);
								#pragma omp atomic
								p_visitingCity->total_ER +=_contactCounterER;
							}
						}
					}	
				}
			}
			//printf("contactor counter HH %d, WP %d, ER %d\n", _contactCounterHH,_contactCounterWP, _contactCounterER);
	}
}
//sets the disease profile of newly infected individuals
void Person::profile_assignment(eVirusMode virusType)
{
	int prof;
		
	switch(virusType)
	{
		case PANDEMIC:
		{
			if (p_city->getSimRun()->uni(0,1)<=PERCENT_SYMPTOMATIC){
				symptomatic = SYMPTOMATIC;
				prof=(int)ceil(p_city->getSimRun()->uni(0,3));
			}
			else
			{
				symptomatic = ASYMPTOMATIC;
				prof=(int)ceil(p_city->getSimRun()->uni(3,6));
			}
			profilePandemic= (eProfileType)prof;
			break;
		}
		case SEASONAL:
		{
			if (p_city->getSimRun()->uni(0,1)<=PERCENT_SYMPTOMATIC){
				symptomatic = SYMPTOMATIC;
				prof=(int)ceil(p_city->getSimRun()->uni(0,3));
			}
			else
			{
				symptomatic = ASYMPTOMATIC;
				prof=(int)ceil(p_city->getSimRun()->uni(3,6));
			}
			profileSeasonal= (eProfileType)prof;
			break;
		}
		case BOTH:
		{
			if (p_city->getSimRun()->uni(0,1)<=PERCENT_SYMPTOMATIC){
				symptomatic = SYMPTOMATIC;
				prof=(int)ceil(p_city->getSimRun()->uni(0,3));
			}
			else
			{
				symptomatic = ASYMPTOMATIC;
				prof=(int)ceil(p_city->getSimRun()->uni(3,6));
			}
			profilePandemic= (eProfileType)prof;
			if(symptomatic==ASYMPTOMATIC)
			{
					if (p_city->getSimRun()->uni(0,1)<=PERCENT_SYMPTOMATIC){
						symptomatic = SYMPTOMATIC;
						prof=(int)ceil(p_city->getSimRun()->uni(0,3));
					}
					else
						prof=(int)ceil(p_city->getSimRun()->uni(3,6));
			}
			else
					prof=(int)ceil(p_city->getSimRun()->uni(0,3));
			profileSeasonal= (eProfileType)prof;
			break;
		}
	}
	//int n = omp_get_thread_num();
	//printf("thread number is %d, random number for profile is %d\n", n, prof);
}

int Person::makeContacts(int hr,float locMod,int scheduleType,int errandHour)
{
	unsigned int cr;
	if(scheduleType==UNEMPLOYED)//unemployed and on an errand
		cr=_errandsUnemployed[errandHour]._numContacts;
	else if(scheduleType==WEEKEND)//on a weekend errand
		cr=_errandsWeekend[errandHour]._numContacts;
	else//at work or home
		cr=(unsigned int)_schedule[hr-1]->getContactRate();//maximum number of contacts
	if(_schedule[hr-1]->getOccupants().size()<=cr)//can't have more contacts than people present -infector
			cr=_schedule[hr-1]->getOccupants().size()-1;
	if (cr > 0)//infector does not work/live alone
	{	
		int ct=0;
		while(ct < cr)
		{
			int found=0;
			while(found==0)
			{
				int randContact=(int)p_city->getSimRun()->uni(0,_schedule[hr-1]->getOccupants().size());
				if(_id !=_schedule[hr-1]->getOccupants().at(randContact)->getId())//make sure contact isn't infector
				{	
					//for parall this is only for list of contactees of a contactor, may be used later
					//struct Contact contact;
					//contact._contactee=_schedule[hr-1]->getOccupants().at(randContact);
					//contact._locModifier=locMod;
					Person* contactee=_schedule[hr-1]->getOccupants().at(randContact);
					//printf("contactee id is %d\n", contactee->getId());
					found=1;
					ct++;
					//for parallel 4/21 delete
					//_contacts.push_back(contact);
					//for parallel 4/21 delete end
					//for pa add begin
					//printf("the contactor id is %d\n",this->getId());
					contactee->addtoContacts(this, locMod);
					//for pa add end
					//printf("contactor %d is added to contacts\n", this->getId());
					//int id=contact._contactee->getId();
					//fprintf(p_visitingCity->checkContacts,"person %d contacted person %d\n",_id,id);
				}
			}
		}
	}
	return cr;
}

bool Person::checkIds(int id, std::vector<int>& used)
{
	if(!used.empty())
	{
		for(unsigned int i=0;i<used.size();i++)
		{
			if(id==used[i])
				return false;
		}
	}	
	return true;
}

//race condition
void Person::infectContacts(int day)
{
	if (_contacts.size()>0)
	{
		//loop through contacts and try to infect them
		for(unsigned int i=0;i<_contacts.size();i++){
			exposeContact(_contacts[i]._contactee, _contacts[i]._locModifier, day);
		}
		//clear contact list for the next day
		_contacts.clear();
//begin yuwen add for memory release 8/22/18
		_contacts.shrink_to_fit();
//end yuwen add for memory release 8/22/18
	}
}
//A contact is exposed to a virus. If the probability of infection is met, the person is infected.
void Person::exposeContact(Person* contactor,float locationModifier,int day)
{
	float prob=0.0f;//probability of catching a virus
	float diceRoll=p_city->getSimRun()->uni(0,1);//compared to probability
       //printf("diceroll is %f ", diceRoll);
	switch(contactor->currentVirusType)
	{
		case BOTH: //contactor can transmit either or both viruses
		{
			bool isInfected=false;
			float prob_p=0.0f;
			float prob_s=0.0f;
			if(prevVirusType!=PANDEMIC && prevVirusType!=BOTH)//pesron has no prev infection with pandemic virus
			{
				if(contactor->getDaysInfectionPandemic()>=0)
				{
					float diceRoll=p_city->getSimRun()->uni(0,1);//compared to probability
					prob_p=setProbability(contactor->getRpreNumberInit(true),contactor->profilePandemic,contactor->getDaysInfectionPandemic(),true,contactor, locationModifier);
					if(diceRoll<=prob_p)
					{
						//contactee->infected(true,this,day);
						infected(true,contactor,day);
						isInfected=true;
					}		
				}
			}
			if(prevVirusType!=SEASONAL && prevVirusType!=BOTH )//person has no previous infection with seasonal virus
			{
				if(contactor->getDaysInfectionSeasonal()>=0)
				{
					float diceRoll=p_city->getSimRun()->uni(0,1);//compared to probability
					prob_s=setProbability(contactor->getRpreNumberInit(false),contactor->profileSeasonal, contactor->getDaysInfectionSeasonal(),false,contactor, locationModifier);
				//	if (isinf(prob_s)) 
					//	printf("both diceroll is %f, prob is %f\n", diceRoll, prob_p);
					if(diceRoll<=prob_s)
					{
						infected(false,contactor,day);
						isInfected=true;
					}		
				}
			}
			if(isInfected==true && p_home!=NULL) {//dont include infected out of state arrivals
				getCity()->addInfected(this);
				//printf("contactee id is %d\n", _id);
			}
			break;
		}
		case PANDEMIC://contactor can transmit pandemic virus
		{
			if(prevVirusType!=PANDEMIC && prevVirusType!=BOTH)//person has no prev infection with pandemic virus
			{
				if(contactor->getDaysInfectionPandemic()>=0)
				{
					prob=setProbability(contactor->getRpreNumberInit(true),contactor->profilePandemic,contactor->getDaysInfectionPandemic(),true,contactor, locationModifier);//current probability to be used
				//	if (isinf(prob)) 
					//	printf("pandemic diceroll is %f, prob is %f\n", diceRoll, prob);
					if(diceRoll<=prob)
					{
						infected(true,contactor,day);
						if(p_home !=NULL)//dont include infected out of state arrivals
						{
							getCity()->addInfected(this);
						}	
					}
				}
			}	
			break;
		}
		case SEASONAL://contactor can transmit seasonal virus
		{
			if(prevVirusType!=SEASONAL && prevVirusType!=BOTH)//person has no previous infection with seasonal virus
			{
				if(contactor->getDaysInfectionSeasonal()>=0)
				{
					prob=setProbability(contactor->getRpreNumberInit(false),contactor->profileSeasonal,contactor->getDaysInfectionSeasonal(),false,contactor, locationModifier);
				//	if (isinf(prob)) 
					//	printf("seasonal diceroll is %f, prob is %f\n", diceRoll, prob);
					if(diceRoll<=prob)
					{
						infected(false,contactor,day);
						if(p_home !=NULL)//dont include infected out of state arrivals
						{
							getCity()->addInfected(this);
						}			
					}
				}
			}
			break;
		}
	}
}
//Determine the probabilit of a person being infected based on the reproduction number(reproNUm), disease profile(profile), and number of days infected(daysInfected)
float Person::setProbability(float reproNum,eProfileType profile, int daysInfected, bool pandemic, Person* contactor, float locMOd)
{
	float vaccineFactor=1.0f;//factor by which probability of infection is reduced due to vaccination of inffector, 1 = not vaccinated
	float vaccineOther=1.0f;//factor by which probability of infection is reduced due to vaccination of contact, 1 = not vaccinated
	float antiviralFactor=1.0f;//factor by which probability of infection is reduced due to antiviral treatment of the infector, 1= not given antivirals
	float antiviralOther=1.0f;//factor by which probability of infection is reduced due to antiviral treatment of the contact, 1= not given antivirals
	bool vaccinated;//is the infector vaccinated
	bool vaccinatedOther;//is the contactee vaccinated
	//assign the above bools to the appropriate virus type
	if(pandemic==true)
	{
		vaccinated=_vaccinated_pandemic;
		vaccinatedOther=contactor->getVaccinationStatusPandemic();
	}
	else
	{
		vaccinated=_vaccinated_seasonal;
		vaccinatedOther=contactor->getVaccinationStatusSeasonal();
	}
	if(vaccinatedOther==true)
	{	//vaccinating the infector reduces the infectiousness of the virus
		vaccineFactor=Flu_Manager::Instance()->Flu_Manager::Instance()->VACCINE_COEFF_INFECTIOUSNESS;
		if(contactor->symptomatic == SYMPTOMATIC)//additional reduction occurs when the infector is symptomatic
			vaccineOther*=Flu_Manager::Instance()->VACCINE_COEFF_SYMPTOMATIC;
	}
	if(vaccinated==true)//vaccinating the contactee reduces the likelyhood of transmission of virus
		vaccineFactor=Flu_Manager::Instance()->VACCINE_COEFF_TRANSMISSION;
	if(contactor->getAntiviralStatus()==true)
	{
		antiviralOther = Flu_Manager::Instance()->AV_INFECTIOUSNESS;
		if(contactor->symptomatic == SYMPTOMATIC)
			antiviralOther *= Flu_Manager::Instance()->AV_SYMPTOMATIC;
	}
	if(_onAntivirals==true)
		antiviralFactor=Flu_Manager::Instance()->AV_TRANSMISSION;
	float profileType,prob;
	switch(profile)
	{
	case GAMMA_1:
		{
			profileType=Flu_Manager::Instance()->getGamma1CumDensity(daysInfected);
			break;
		}
	case LOGNORMAL_1:
		{
			profileType=Flu_Manager::Instance()->getLognormal1CumDensity(daysInfected);
			break;
		}
	case WEIBULL_1:
		{
			profileType=Flu_Manager::Instance()->getWeibu1CumDensity(daysInfected);
			break;		
		}
	case GAMMA_2:
		{
			profileType=Flu_Manager::Instance()->getGamma2CumDensity(daysInfected);
			break;
		}
	case LOGNORMAL_2:
		{
			profileType=Flu_Manager::Instance()->getLognormal2CumDensity(daysInfected);
			break;
		}
	case WEIBULL_2:
		{
			profileType=Flu_Manager::Instance()->getWeibu2CumDensity(daysInfected);
			break;
		}
	}
	if((int)profile<4)//a symptomatic profile curve
		prob=((reproNum/(((1-Flu_Manager::Instance()->asymp)*PERCENT_SYMPTOMATIC)+Flu_Manager::Instance()->asymp))*profileType)/((contactor->getContactCounterHH()*Flu_Manager::Instance()->k_hh)+(contactor->getContactCounterWP()*Flu_Manager::Instance()->k_wp)+(contactor->getContactCounterER()*Flu_Manager::Instance()->k_er));	
	else//an asymptomatic profile curve
		prob=(Flu_Manager::Instance()->asymp*(reproNum/(((1-Flu_Manager::Instance()->asymp)*PERCENT_SYMPTOMATIC)+Flu_Manager::Instance()->asymp))*profileType)/((contactor->getContactCounterHH()*Flu_Manager::Instance()->k_hh)+(contactor->getContactCounterWP()*Flu_Manager::Instance()->k_wp)+(contactor->getContactCounterER()*Flu_Manager::Instance()->k_er));
	//printf("reproNum %f, Flu_Manager::Instance()->asymp %f, PERCENT_SYMPTOMATIC %f, profileType %f, HH %d\n", reproNum, Flu_Manager::Instance()->asymp, PERCENT_SYMPTOMATIC, profileType, contactor->getContactCounterHH());
	prob*=vaccineFactor*vaccineOther*antiviralFactor*antiviralOther*locMOd;//probability reduced if vaccinated or on antivirals
	//printf("prob is %f, hh is %d, wp is %d, er is %d\n", prob, contactor->getContactCounterHH(), contactor->getContactCounterWP(), contactor->getContactCounterER());
	//if (!isnan(prob)) printf("Nan prob, hh is %d, wp is %d, er is %d\n", contactor->getContactCounterHH(), contactor->getContactCounterWP(), contactor->getContactCounterER());
	return prob;
}

/*Check if travelers should return home and which infected individuals will travel*/
void Person::setTravelState()
{
	//check who will be traveling
	if(_traveling==false)
	{
		float diceRoll=p_city->getSimRun()->uni(0,1);
		if(Flu_Manager::Instance()->ALLOW_TRAVEL==0)//only out of state travel being used
		{
//yuwen change sampling 062518
			if(diceRoll<=p_city->getProbTravel()*Flu_Manager::Instance()->OOS_TRAVEL_PROB)
//yuwen change sampling 062518 end
			{
				p_restPlace=p_Outside;
				p_visitingCity=p_city;
				_traveling =true;
			}
		}
		else
		{
			if(diceRoll<=p_city->getProbTravel())//person is traveling
			{
				_traveling =true;
				//create a list of destination regions and a matching list of cumulative probabilites
				std::vector<float> cumDestProb;
				std::vector<City*> destCities;
				float sumOfprob=0.0f;
				//populate the lists
				for(int i=0;i<Flu_Manager::Instance()->NUM_CITIES;i++)
				{
					if(i != p_city->getId())//not the home region
					{
						destCities.push_back(p_city->getSimRun()->getCity(i));
						float prob=p_city->getSimRun()->getCity(i)->getProbDestination();
						cumDestProb.push_back(sumOfprob+prob);//make each prob cumulative with the previous
						sumOfprob+=prob;		
					}	
				}
				//if there is no out of state travel, normalize the cumulative probabilities of regional travel
				if(Flu_Manager::Instance()->ALLOW_OOS_TRAVEL==0)
				{
					for(unsigned int i=0;i<cumDestProb.size();i++)
					{
						cumDestProb[i]=cumDestProb[i]/cumDestProb[cumDestProb.size()-1];
					}
				}
				//find the destination
				diceRoll=p_city->getSimRun()->uni(0,1);
				for(unsigned int i=0;i<cumDestProb.size();i++)
				{
					if(diceRoll<=cumDestProb[i])
					{
						p_visitingCity=destCities[i];
						break;
					}
				}
				if(diceRoll>cumDestProb[cumDestProb.size()-1])//person is leaving the state
				{
					p_restPlace=p_Outside;
					p_visitingCity=p_city;
				}
				if(p_visitingCity !=p_city)//person is traveling within state
				{
					//start the destinations outbreak if it has not already been started
					if(p_visitingCity->getDaysOutbreak()<=0)
					p_visitingCity->startOutbreak();
					//where is traveler staying
					pickRestingPlace();
				}
			}
			//how long will the traveler stay?
			int longestPossible=0;//longest period traveler will be gone = days left of disease
			if(_daysOfPandemicInfection < _daysOfSeasonalInfection && _daysOfPandemicInfection>=0)
				longestPossible=10-_daysOfPandemicInfection;
			else
				longestPossible=10-_daysOfSeasonalInfection;
			if(longestPossible >1)
				_maxTravelDays=(int)p_city->getSimRun()->uni(1,longestPossible);
			else
				_maxTravelDays=1;
			/*is this a family trip?
			float probFamilyTrip=.25f;
			diceRoll=Flu_Manager::Instance()->uni(0,1);
			if(diceRoll<=probFamilyTrip)
			{
				//add the other adults in the household
				for(int i=0;i < p_home->getNumAdults();i++)
				{
					if(p_home->getAdult(i)->getId()!=_id)
						p_home->getAdult(i)->setTraveling(p_visitingCity,p_restPlace,_maxTravelDays);
				}
				if(p_home->getNumChildren()>0)//add kids to travelers
				{
					for(int i=0;i < p_home->getNumChildren();i++)
					{
						if(p_home->getChild(i)->getAge()<18)
							p_home->getChild(i)->setTraveling(p_visitingCity,p_restPlace,_maxTravelDays);
					}
				}
			}*/
		}
	}
	else //check how long travelers have been away, if = max travel days end travel for them
	{
		if(_daysTraveled>=_maxTravelDays)//done with trip
		{
			stopTraveling();
		}
		else//add a day of travel
			_daysTraveled++;
	}	
}
//set a person as a traveler
/*void Person::setTraveling(City* city,Location* loc,int daysGone)
{
	_traveling=true;
	p_visitingCity=city;
	p_restPlace=loc;
	_maxTravelDays=daysGone;
}*/

void Person::stopTraveling()
{
	_traveling = false;
	p_visitingCity=p_city;
	p_restPlace=p_home;
	_maxTravelDays=0;
	_daysTraveled =0;
}

void Person::pickRestingPlace()
{
	//where is traveler staying
	float probHouse=.5f;//as a coin toss due no available data on where travelers stay
	float diceRoll=p_city->getSimRun()->uni(0,1);
	if(diceRoll<=probHouse)
	{
		//pick a houshold at random
		int randomHouseId=(int)p_city->getSimRun()->uni(0,p_visitingCity->getNumHouseholds());
		p_restPlace=p_visitingCity->getHousehold(randomHouseId);
	}
	else
	{
		//pick an entertainment business at random
		int randHotel=(int)p_city->getSimRun()->uni(0,p_visitingCity->getBusinessNumbers(13));
		for (int i = 1; i <= p_visitingCity->getBusinessSize(); ++i)
		{
			if(p_visitingCity->getBusiness(i)->eType==Business::ENTERTAINMENT)
			{
				p_restPlace=p_visitingCity->getBusiness(i+randHotel);
				break;
			}
		}
	}
}
//yuwen change add
void Person::setArrivalSchedule(int day)
{
	for(int i=1;i<=24;i++)
		_schedule[i-1]=p_restPlace;
//yuwen add begin day in week 010819 begin
		int _offsetDays = day + Flu_Manager::Instance()->BEGIN_WEEKDAY - 1;
		if (_offsetDays%7 != 0 && _offsetDays%7 != 6)
//yuwen add begin day in week 010819 end
//yuwen delete begin day in week 010819 begin
	//if(day % 7==0 || day % 7==6) //weekend
//yuwen delete begin day in week 010819 end
			setWeekendSchedule();
	else
	{	// select 3 different time slots between 8:00 and 19:00
		set<int> errandHours; //a set ensures each number hour will be a unique value
		while(errandHours.size()<3)
		{
			int hr=(int)p_city->getSimRun()->uni(8,20);
			errandHours.insert(hr); 
		}
		int slot=0;
		//set the errand hours
		for(set<int>::iterator it=errandHours.begin();it!=errandHours.end();it++)
		{
					_errandsUnemployed[slot]._numContacts=0;//clear yesterdays contacts
					_errandsUnemployed[slot]._errandHour=*it;
					slot++;
		}
		//put 2 contacts into the schedule
		for(int i=0;i<2;i++)
		{
			int slot=(int)p_city->getSimRun()->uni(0,3);
			_errandsUnemployed[slot]._numContacts++;
		}
		//set the business location of the errands and the daily schedule
		for (int err= 0; err<3 ;err++) {
			float y = p_city->getSimRun()->uni(0, 1);
			// fish out y in the cum % of weekday errands in business entities
            int low = 1; 
			int high = p_city->getBusinessSize()-1;
			int mid = (low + high)/2;
			while ( high - low >= 5) {
				if( y <= p_city->getBusiness(mid)->getPerWeekdayErrands() ){
					high = mid;
					mid = (low + high)/2;
				}
				if( y > p_city->getBusiness(mid)->getPerWeekdayErrands() ){
					low = mid;
					mid = (low + high)/2;
				}
			}
			for (int i = low; i <= high; i++) {
				float prevWeekErrPer=p_city->getBusiness(i-1)->getPerWeekdayErrands();
				if (y >= prevWeekErrPer && y < p_city->getBusiness(i)->getPerWeekdayErrands()) {
					_schedule[_errandsUnemployed[err]._errandHour-1] = p_city->getBusiness(i);
					_errandsUnemployed[err]._businessId=p_city->getBusiness(i)->getId(); //record the associated business
				}
			}
		}
	}
}
//yuwen change end

void Person::setTraveling(bool travel)
{
	_traveling=travel;
}

//for parall add begin
void Person::addtoContacts(Person* infector, float locmodifier)
{
	struct Contact contact;
	contact._contactee=infector;
	contact._locModifier=locmodifier;
	#pragma omp critical
	_contacts.push_back(contact);
}

int  Person::getDaysInfectionPandemic() 
{
	 return _daysOfPandemicInfection;
}

int  Person::getDaysInfectionSeasonal()
{
	return _daysOfSeasonalInfection;
}

int Person::getContactCounterHH() const
{
	return _contactCounterHH;
}

int Person::getContactCounterWP() const
{
	return _contactCounterWP;
}

int Person::getContactCounterER() const
{
	return _contactCounterER;
}

float Person::getRpreNumberInit(bool pandemic) const
{
	if (pandemic == true) return _rn_init_pandemic;
	return _rn_init_seasonal;
}
void Person::setContactCounterHH(int num)
{
	_contactCounterHH = num;
}
void Person::setContactCounterWP(int num)
{
	_contactCounterWP = num;
}
void Person::setContactCounterER(int num)
{
	_contactCounterER = num;
}
