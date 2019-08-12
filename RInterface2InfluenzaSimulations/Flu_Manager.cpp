#include <stdio.h>
#include "Flu_Manager.h"
#include "City.h"
#include "Multi_City_44_Templates.h"

/*This class follows a singleton pattern, because there should be exactly one instance of the class created in the program */
Flu_Manager* Flu_Manager::p_inst=NULL;//no instance has been created yet
/*****arrays of structures use as templates***********************/
struct Age_AdultTemplate _ageAdultRanges[number_adult_age_type +1 ];//array of adult age ranges
struct Age_ChildTemplate _ageChildRanges[number_child_age_type +1];//array of child age ranges
struct  HouseHoldTemplate _householdTypes[number_household_type +1];// an array of different household types
struct ZipcodeTemplate _zipCodes[number_zipcodes +1];
struct Gamma1 _gammaCurves1[DISTRIBUTION_COLUMNS +1];//gamma CDF profile of the symptomatic individuals
struct Gamma2 _gammaCurves2[DISTRIBUTION_COLUMNS +1];
struct Lognorm1 _lognormCurves1[DISTRIBUTION_COLUMNS +1];//lognormal CDF profile of the symptomatic indiMviduals
struct Lognorm2 _lognormCurves2[DISTRIBUTION_COLUMNS +1];
struct Weib1 _weibCurves1[DISTRIBUTION_COLUMNS +1]; //weibul CDF profile of the symptomatic individuals
struct Weib2 _weibCurves2[DISTRIBUTION_COLUMNS +1];
struct TravelStats _travelStats[NUMBER_REGIONS];

/*Creates an instance of Flu_Manager if none exists, returns a pointer to the instance*/
Flu_Manager* Flu_Manager::Instance()
{
	if(!p_inst)//no flu manager object created yet
		p_inst=new Flu_Manager;
	return p_inst;
}
Flu_Manager::Flu_Manager(void)
{
	//SEVERITY=.5f;
}

Flu_Manager::~Flu_Manager(void)
{
	for(unsigned int i=0;i<_runs.size();i++)
	{
		delete _runs[i];
		_runs[i]=NULL;
	}
}

//load values for class templates from txt files
void Flu_Manager::loadInput()
{
	FILE  *gamma1File, *gamma2File, *lognorm1File, *lognorm2File, *weib1File, *weib2File;
	//create an array of household types
	FILE* houseFile = fopen("households.txt","r");
	for(int i=0;i<=number_household_type;i++)// parse the values on each line of a text file to a structure then add that structure to an array
	{
		struct HouseHoldTemplate householdType;
		fscanf(houseFile ,"%d",&householdType._numAdults);
		fscanf(houseFile ,"%d",&householdType._numChildren);
		fscanf(houseFile ,"%f",&householdType._cumPmf);
		_householdTypes[i]=householdType;
	}
	fclose(houseFile);
	//create an array of child age ranges
	FILE* childAgeFile = fopen("age_child.txt","r");
	for (int i = 0; i <= number_child_age_type; ++i)
	{
		struct Age_ChildTemplate childAgeRange;
		fscanf(childAgeFile, "%d", &childAgeRange._age);
		fscanf(childAgeFile, "%f", &childAgeRange._cumulativeProb);
		fscanf(childAgeFile, "%d", &childAgeRange._workType);
		_ageChildRanges[i]=childAgeRange;
		printf("Type %d child has age %d and a school %d\n",i,_ageChildRanges[i]._age,_ageChildRanges[i]._workType);
	}
	fclose(childAgeFile);
	//create an array of adult age ranges
	FILE* adultAgeFile = fopen("age_adult.txt","r");
	for (int i = 0; i <= number_adult_age_type; ++i)
	{
		struct Age_AdultTemplate adultAgeRange;
		fscanf(adultAgeFile, "%d", & adultAgeRange._age);
		fscanf(adultAgeFile, "%f", & adultAgeRange._cumulativeProb);
		_ageAdultRanges[i]=adultAgeRange;
	}
	fclose(adultAgeFile);
	//create an array of zipcodes
	FILE* zipCodeFile = fopen("zipcode.txt","r");
	for (int i = 0; i <= number_zipcodes; ++i)
	{
		struct ZipcodeTemplate zipcode;
		fscanf(zipCodeFile, "%d", &zipcode._number);
		fscanf(zipCodeFile, "%f", &zipcode._cumProb);
		_zipCodes[i]=zipcode;
		//printf("zipcode %g has a proportion %g \n",zipcode[i][1],zipcode[i][2]);
	}
	fclose(zipCodeFile);
	//create an array of travel probabilities
	FILE* travelFile = fopen("regions.txt","r");
	for (int i=0;i < NUMBER_REGIONS; i++)
	{
		struct TravelStats travel;
		fscanf(travelFile,"%f",&travel._probTravel);
		fscanf(travelFile,"%f",&travel._probDest);
		fscanf(travelFile,"%d",&travel._numHouseholds_scaled);
		fscanf(travelFile,"%d",&travel._numHouseholds_unscaled);
		fscanf(travelFile,"%d",&travel._numArrivals_unscaled);
		fscanf(travelFile,"%d",&travel._numArrivals_scaled);
		_travelStats[i]=travel;
	}
	fclose(travelFile);
	//create an array of gamma distribution columns
	gamma1File = fopen("gamma1.txt","r");
	for (int i = 0; i <= DISTRIBUTION_COLUMNS; ++i)
	{
			int dump;//integer to hold the first column value. It will be discarded. The index of gamaCurves = days of infectiousness
			struct Gamma1 gammaOne;
			fscanf(gamma1File, "%d", &dump);//dump the first column value
			fscanf(gamma1File, "%f", &gammaOne._cumDensity);
			_gammaCurves1[i]=gammaOne;
	}
	fclose(gamma1File);
	//printf("the gamma 1 cdf is %f \n", _gammaCurves1[10+1]._cumDensity);
	//create an array of gamma distribution columns
	gamma2File = fopen("gamma2.txt","r");
	for (int i = 0; i <= DISTRIBUTION_COLUMNS; ++i)
	{
		int dump;//integer to hold the first column value. It will be discarded. The index of gamaCurves = days of infectiousness
			struct Gamma2 gammaTwo;
			fscanf(gamma2File, "%d", &dump);//dump the first column value
			fscanf(gamma2File, "%f", &gammaTwo._cumDensity);
			_gammaCurves2[i]=gammaTwo;
	}
	fclose(gamma2File);
	//printf("the gamma 2 cdf is %f \n", _gammaCurves1[10+1]._cumDensity);
	//create an array of lognormal distribution columns
	lognorm1File = fopen("lognorm1.txt","r");
	for (int i = 0; i <= DISTRIBUTION_COLUMNS; ++i)
	{
		int dump;//integer to hold the first column value. It will be discarded. The index of lognormalCurves = days of infectiousness
			struct Lognorm1 logOne;
			fscanf(lognorm1File, "%d", &dump);//dump the first column value
			fscanf(lognorm1File, "%f", &logOne._cumDensity);
			_lognormCurves1[i]=logOne;
	}
	fclose(lognorm1File);
	//printf("the lognormal 1 cdf is %f \n", _lognormCurves1[10+1]._cumDensity);
	//create an array of lognormal distribution columns
	lognorm2File = fopen("lognorm2.txt","r");
	for (int i = 0; i <= DISTRIBUTION_COLUMNS; ++i)
	{
		int dump;//integer to hold the first column value. It will be discarded. The index of lognormalCurves = days of infectiousness
			struct Lognorm2 logTwo;
			fscanf(lognorm2File, "%d", &dump);//dump the first column value
			fscanf(lognorm2File, "%f", &logTwo._cumDensity);
			_lognormCurves2[i]=logTwo;
	}
	fclose(lognorm2File);
	//printf("the lognormal 2 cdf is %f \n",_lognormCurves2[10+1]._cumDensity);
	//create an array of weib distribution columns
	weib1File = fopen("weib1.txt","r");
	for (int i = 0; i <= DISTRIBUTION_COLUMNS; ++i)
	{
		int dump;//integer to hold the first column value. It will be discarded. The index of weibCurves = days of infectiousness
			struct Weib1 weibOne;
			fscanf(weib1File, "%d", &dump);//dump the first column value
			fscanf(weib1File, "%f", &weibOne._cumDensity);
			_weibCurves1[i]=weibOne;
	}
	fclose(weib1File);
	//printf("the weibull 1 cdf is %f \n", ,_weibCurves1[10+1]._cumDensity);
	//create an array of weib distribution columns
	weib2File = fopen("weib2.txt","r");
	for (int i = 0; i <= DISTRIBUTION_COLUMNS; ++i)
	{
		int dump;//integer to hold the first column value. It will be discarded. The index of weibCurves = days of infectiousness
			struct Weib2 weibTwo;
			fscanf(weib2File, "%d", &dump);//dump the first column value
			fscanf(weib2File, "%f", &weibTwo._cumDensity);
			_weibCurves2[i]=weibTwo;
	}
	fclose(weib2File);
}
//loads inputs from text files once initalized, the values will not change
void Flu_Manager::loadConstants()
{
	int quarantine=0;//integer for grabbing the quaratine option chosen
	ALLOW_TRAVEL=0;
	FILE *input_constants=fopen("constants.txt","r");
	fscanf(input_constants, "%d", &SEED);
	fscanf(input_constants, "%d", &max_days);
	fscanf(input_constants, "%f", &reproduction_number_pandemic);
    fscanf(input_constants, "%f", &reproduction_number_seasonal);
	fscanf(input_constants, "%d", &initial_infected_pandemic);
    fscanf(input_constants, "%d", &initial_infected_seasonal);
	fscanf(input_constants, "%f", &asymp);
	fscanf(input_constants, "%f", &k_hh);
	fscanf(input_constants, "%f", &k_wp);
	fscanf(input_constants, "%f", &k_er);
	fscanf(input_constants, "%f", &epsilon_p);
	fscanf(input_constants, "%f", &epsilon_pr);
	fscanf(input_constants, "%f", &epsilon_ps);
	fscanf(input_constants, "%f", &epsilon_s);
	fscanf(input_constants, "%f", &epsilon_sr);
	fscanf(input_constants, "%f", &epsilon_sp);
	fscanf(input_constants, "%d", &quarantine);
	fscanf(input_constants, "%d", &PANDEMIC_VACCINATION);
	fscanf(input_constants, "%d", &SEASONAL_VACCINATION);
	fscanf(input_constants, "%d", &usingAntivirals);
	fscanf(input_constants, "%d", &NUM_THREADS);
	fscanf(input_constants, "%d", &NUM_CITIES);
	fscanf(input_constants, "%d", &NUM_RUNS);
	fscanf(input_constants, "%d", &SCALING);
	fscanf(input_constants, "%d", &ALLOW_OOS_TRAVEL);
	fscanf(input_constants, "%f", &SEVERITY);
    fscanf(input_constants, "%f", &ANTIVIRAL_PROB);
//yuwen add sampling criteria begin
    fscanf(input_constants, "%f", &POVERTY_PROB);
    fscanf(input_constants, "%f", &ED_MSSS_PERCENTAGE);
    fscanf(input_constants, "%f", &PERCEIVED_PANDEMIC_PROB);
    fscanf(input_constants, "%f", &COLLECT_PROB);
    fscanf(input_constants, "%d", &SAMPLING_CRITERIA);
    fscanf(input_constants, "%d", &NUM_PHL);
    for(int i=0;i<NUM_PHL;i++)//read lab capacities from as many as PHLS
    {
    	fscanf(input_constants, "%d", &PHL_CAPACITY);
    }
    fscanf(input_constants, "%f", &PR_ACCESSIBLITY);
    fscanf(input_constants, "%d", &WORK_WEEKEND);
//yuwen add sampling criteria end
	if(NUM_CITIES>1)//Travel is only possible if there is more than one city
	{
		fscanf(input_constants, "%d", &ALLOW_TRAVEL);
	}
	fscanf(input_constants, "%f", &OOS_TRAVEL_PROB);
	fscanf(input_constants, "%f", &BEGIN_WEEKDAY);
	fclose(input_constants);
	quarantineOption=(QuarantineOptions)quarantine;//convert quaratine from int enumerated type and assign the value to quaratineOption
	//set % vaccine coverage
	VACC_COVERAGE_0TO5=.434f;
	VACC_COVERAGE_6TO14=.277f;
	VACC_COVERAGE_15TO17=.199f;
	VACC_COVERAGE_18TO29=.1085f;
	VACC_COVERAGE_30TO64=.2315f;
	VACC_COVERAGE_65PLUS=.3985f;
	//set vaccination factors
	VACCINE_COEFF_TRANSMISSION =.6f;
	VACCINE_COEFF_INFECTIOUSNESS =.5f;
	VACCINE_COEFF_SYMPTOMATIC =.17f;
	//set antiviral factors
	AV_TRANSMISSION=.7f;
	AV_INFECTIOUSNESS=.2f;
	AV_SYMPTOMATIC=.4f;
	//Populate the list of workplace templates
	_workFiles_scaled.push_back("workplaces_MI_region1_scaled_Jan14.txt");
	_workFiles_unScaled.push_back("workplaces_MI_region1_unscaled_Jan14.txt");
	_workFiles_scaled.push_back("workplaces_MI_region2_scaled_Jan14.txt");
	_workFiles_unScaled.push_back("workplaces_MI_region2_unscaled_Jan14.txt");
	_workFiles_scaled.push_back("workplaces_MI_region3_scaled_Jan14.txt");
	_workFiles_unScaled.push_back("workplaces_MI_region3_unscaled_Jan14.txt");
	_workFiles_scaled.push_back("workplaces_MI_region4_scaled_Jan14.txt");
	_workFiles_unScaled.push_back("workplaces_MI_region4_unscaled_Jan14.txt");
}

/*functions for accessing the array of runs*/
SimulationRun* Flu_Manager::getRun(int index) const
{
	return _runs[index];
}

void Flu_Manager::addRun(SimulationRun* run)
{
	_runs.push_back(run);
}
/*These functions get specific values from specific class templates in arrays of templates. they are used to initialize classes used in this simulation*/
float Flu_Manager::getAdultAgesCumProb(int index) const
{
	return  _ageAdultRanges[index]._cumulativeProb;
}
float Flu_Manager::getChildAgesCumProb(int index) const
{
	return _ageChildRanges[index]._cumulativeProb;
}
float Flu_Manager::getHouseCumPmf(int index) const
{
	return _householdTypes[index]._cumPmf;
}
int Flu_Manager::getHouseNumAdults(int index) const
{
	return _householdTypes[index]._numAdults;
}
int Flu_Manager::getHouseNumChildren(int index) const
{
	return _householdTypes[index]._numChildren;
}
float Flu_Manager::getZipCumProb(int index) const
{
	return _zipCodes[index]._cumProb;
}
int Flu_Manager::getZipcode(int index) const
{
	return _zipCodes[index]._number;
}
int Flu_Manager::getAdultAge(int index) const
{
	return  _ageAdultRanges[index]._age;
}
int Flu_Manager::getChildAge(int index) const
{
	return _ageChildRanges[index]._age;
}
int Flu_Manager::getChildWorkType(int index) const
{
	return _ageChildRanges[index]._workType;
}
float Flu_Manager::getGamma1CumDensity(int index) const
{
	return _gammaCurves1[index]._cumDensity;
}
float Flu_Manager::getGamma2CumDensity(int index) const
{
	return _gammaCurves2[index]._cumDensity;
}
float Flu_Manager::getLognormal1CumDensity(int index) const
{
	return _lognormCurves1[index]._cumDensity;
}
float Flu_Manager::getLognormal2CumDensity(int index) const
{
	return _lognormCurves2[index]._cumDensity;
}
float Flu_Manager::getWeibu1CumDensity(int index) const
{
	return _weibCurves1[index]._cumDensity;
}
float Flu_Manager::getWeibu2CumDensity(int index) const
{
	return _weibCurves2[index]._cumDensity;
}
float Flu_Manager::getProbTravel(int index) const
{
	return _travelStats[index]._probTravel;
}
float Flu_Manager::getProbDestination(int index) const
{
	return _travelStats[index]._probDest;
}
int Flu_Manager::getRegionScaledPopulation(int index)const
{
	return _travelStats[index]._numHouseholds_scaled;
}
int Flu_Manager::getRegionUnscaledPopulation(int index)const
{
	return _travelStats[index]._numHouseholds_unscaled;
}
std::string Flu_Manager::getScaledWorkFile(int index)
{
	return _workFiles_scaled[index];
}
std::string Flu_Manager::getUnscaledWorkFile(int index)
{
	return _workFiles_unScaled[index];
}
int Flu_Manager::getScaledOutsideArrivals(int index)const
{
	return _travelStats[index]._numArrivals_scaled;
}
int Flu_Manager::getUnscaledOutsideArrivals(int index)const
{
	return _travelStats[index]._numArrivals_unscaled;
}
