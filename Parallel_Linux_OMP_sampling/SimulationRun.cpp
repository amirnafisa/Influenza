#include <string>
#include <string.h>
#include <stdio.h>
#include "SimulationRun.h"
#include "Flu_Manager.h"
#include "Multi_City_44_Templates.h"
#include <time.h> 

SimulationRun::SimulationRun(int id)
{
	_id=id;
	//_uniform_counter = 1;
	_seed=Flu_Manager::Instance()->SEED +_id;
	_totalPopulation=0;
	total_infected_pandemic = 0;
	total_infected_seasonal = 0;
	total_sim_coinfected = 0;
	total_infected_pandemic_only = 0;
	total_infected_seasonal_only = 0;
	total_coinfected_s_p = 0;
	total_reinfected_s_p = 0;
	total_coinfected_p_s = 0;
	total_reinfected_p_s = 0;

	new_infected_today_pandemic = 0; // number of new infected entities every day
	new_infected_today_seasonal = 0; // number of new infected entities every day
	new_infected_today_pandemic_only = 0; // number of new infected entities every day
	new_infected_today_seasonal_only = 0; // number of new infected entities every day
	new_sim_coinfected = 0; // number of new infected entities every day
	new_coinfected_s_p = 0; // number of new infected entities every day
	new_reinfected_s_p = 0; // number of new infected entities every day
	new_coinfected_p_s = 0; // number of new infected entities every day
	new_reinfected_p_s = 0; // number of new infected entities every day
	new_recovered_today_pandemic = 0; // number of new recovered entities every day
	new_recovered_today_seasonal = 0; // number of new recovered entities every day

//yuwen add 0624 print
	total_PHLtested_pandemic = 0;
	total_PHLtested_seasonal = 0;
	total_PHLtested_all = 0;
	total_pandemic_submit_samples = 0;//total number of submitted pandemic samples
	total_seasonal_submit_samples = 0;//total number of submitted seasonal samples
	total_submit_samples = 0;//total number of submitted samples
	total_pandemic_uncollect_samples = 0;//number of total uncollected pandemic samples
	total_seasonal_uncollect_samples = 0;//number of total uncollected seasonal samples
	total_uncollect_samples = 0;//number of total uncollected samples
	total_pandemic_MSSS_samples = 0;//number of total MSSS pandemic samples
	total_seasonal_MSSS_samples = 0;//number of total MSSS seasonal samples
	total_MSSS_samples = 0;//number of total MSSS samples
	CAPACITY_IND = 0;//Initial status of PHL capacity is not reached.
	total_discard_pandemic = 0;//number of total discarded pandemic samples
	total_discard_seasonal = 0;//number of total discarded seasonal samples
	total_discard_all = 0;
	total_both_submit_samples = 0;

	int i,j;
	for(i=0;i<23;i++)
	{
		for(j=0;j<23;j++)
		{
			submitPandemic[i][j] = 0;//matrix to store the number of tested pandemic specimens by submission date 
			submitSeasonal[i][j] = 0;//matrix to store the number of tested seasonal specimens by submission date 
			submitTotal[i][j] = 0;
		}
	}
//yuwen add 0625 end
//yuwen add 0925 begin store the number of tested specimens by submission date in an array
//	arraySubmitPan[24]={0};//initialized all 15 elements to 0
//	arraySubmitSea[24]={0};
//	arraySubmitTotal[24]={0};
//yuwen add 0925 end store the number of tested specimens by submission date in an array
	//create an array of uniform random numbers
	//yuwen change add

	/*delete for parallel
	for (int i = 1; i <= 10000; ++i) 
	{
		_uniform[i]= (float)fluRandom();
	}
	*/

	//yuwen change end
	//generate cities
//begin yuwen delete to fix memory problems 8/21/18
/*
	for(int i=0;i<Flu_Manager::Instance()->NUM_CITIES;i++)
	{
		City* city;
		if(i==0)//outbreak starts in this city
			city =new City(i,true,this);
		else
			city =new City(i,false,this);
		_cities.push_back(city);//_cities is vector with city object pointer
		_totalPopulation+=city->getPopulationSize();
	}
	printf("Cities for run %d generated\n",_id);
*/
//end yuwen delete to fix memory problems 8/21/18
}

SimulationRun::~SimulationRun()
{
	for(unsigned int i=0;i<_cities.size();i++)
	{
		delete _cities[i];
		_cities[i]=NULL;
	}
//yuwen 0623 add
	_samplePoolPHL.clear();
	_samplePoolPHL.shrink_to_fit();
	_samplePoolRural.clear();
	_samplePoolRural.shrink_to_fit();
//	_uncollectedSample.clear();
//	_uncollectedSample.shrink_to_fit();
//	_sampleMsss.clear();
//	_sampleMsss.shrink_to_fit();
//yuwen 0623 add end
}

int SimulationRun::getId() const
{
	return _id;
}

City* SimulationRun::getCity(int index) const
{
	return _cities[index];
}

void SimulationRun::outbreak()
{
	int	t_now = 0; // simulation clock time in hours
	int day;//days of simulation
	int hr;// hour of a given day
	clock_t t;
	//begin yuwen add to fix memory problem 8/21/18

	for(int i=0;i<Flu_Manager::Instance()->NUM_CITIES;i++)
	{
		City* city;
		if(i==0)//outbreak starts in this city
			city =new City(i,true,this);
		else
			city =new City(i,false,this);
		_cities.push_back(city);//_cities is vector with city object pointer
		_totalPopulation+=city->getPopulationSize();
	}
	printf("Cities for run %d generated\n",_id);

	//generate unique name for run output
	char runId[3];
	sprintf(runId,"%d",_id);//convert id to string
	std::string RunStr="Run_no";
	RunStr+=runId;
	char statStr[34];
	strcpy(statStr,RunStr.c_str());
	char baseStr[26]="_output_general_stats.txt";
	strcat(statStr,baseStr);
//yuwen add 0625 printout
	char sampleStr[35];
	strcpy(sampleStr,RunStr.c_str());
	char bStr[25]="_number_daily_tested.txt";
	strcat(sampleStr,bStr);
//yuwen	add 0625 printout end

	FILE* globalOutput_genStats=fopen(statStr,"w");
	fprintf(globalOutput_genStats, "Day  Population  Infected_total  Infected_p  Recovered_p  Infected_s  Recovered_s  Infected_p_only  Infected_s_only coinfected_s_p reinfected_s_p coinfected_p_s reinfected_p_s coinfected_sim\n");
//yuwen add 0624 print file
	FILE* sampleOutput=fopen(sampleStr,"w");
	fprintf(sampleOutput, "day 	test_p 	test_s 	test 	submit_p 	submit_s 	submit 	sumit_b 	uncollect_p 	uncollect_s 	uncollect 	\
MSSS_p 	MSSS_s 	MSSS 	discard_p 	discard_s 	discard 	CAP_ind\n");

//end yuwen add to fix memory problem 8/21/18
//yuwen add 0624 end
	for(unsigned int i=0;i<_cities.size();i++)
		_cities[i]->openOutputFiles();
	//FILE* time_output=fopen("timer.txt","w");
	while(hasCityWithOutbreak())
	{
		day = 1+t_now/24; // A day begins ...
		if (day >=2)
		{//hr cycles from 1 to 24
			hr=(t_now-(24*(day-1)))+1; 
		}
		else {//less than 24 hours have passed
			hr=t_now+1;            
		}
		printf("\nRun %d Day %d  Hour %d\n\n",_id,day,hr);
//yuwen 0624 add update sample pool
		if(day !=1 && hr==8 && Flu_Manager::Instance()->SAMPLING_CRITERIA>0)
		{	
    		//t = clock();
			SamplingPHL(day);
			//t = clock() - t; 
    		//double time_taken = ((double)t)/CLOCKS_PER_SEC/60; // in seconds 
    		//fprintf(time_output,"Sampling on day %d took %f mins to execute \n", day, time_taken);
    	}

//yuwen 0624 add end
		//have each city updates its flu outbreak
		for(unsigned int i=0;i<_cities.size();i++)
		{
			if(_cities[i]->outbreakRunning())
			{
				_cities[i]->updateOutbreak(hr,day);
//yuwen add sample testing 05162018_3 begin
				if(hr==17 && Flu_Manager::Instance()->SAMPLING_CRITERIA>0)//At 5pm, PHL collect samples from CL from all cities
				{
					_cities[i]->collectFromCity(day);//start sampling process in the CL
//yuwen add sample testing 05162018_3 end
				}

				if(hr==23)//time to collect infection data
				{
					new_recovered_today_pandemic+=_cities[i]->getNewRecoverd(true);
					new_recovered_today_seasonal+=_cities[i]->getNewRecoverd(false);
					total_sim_coinfected += _cities[i]->getNewInfectedByType(COINFECTED_SIMULTANEOUS);
		 			total_infected_pandemic_only += _cities[i]->getNewInfectedByType(INFECTED_PANDEMIC);
		 			total_infected_seasonal_only += _cities[i]->getNewInfectedByType(INFECTED_SEASONAL);
		 			total_coinfected_s_p += _cities[i]->getNewInfectedByType(COINFECTED_SEASONAL_PANDEMIC);
		 			total_reinfected_s_p +=_cities[i]->getNewInfectedByType(REINFECTED_SEASONAL_PANDEMIC);
		 			total_coinfected_p_s += _cities[i]->getNewInfectedByType(COINFECTED_PANDEMIC_SEASONAL);
					total_reinfected_p_s += _cities[i]->getNewInfectedByType(REINFECTED_PANDEMIC_SEASONAL);
					total_infected_pandemic = total_infected_pandemic_only+total_coinfected_s_p +total_reinfected_s_p +total_sim_coinfected;
		 			total_infected_seasonal = total_infected_seasonal_only+total_coinfected_p_s+total_reinfected_p_s+ total_sim_coinfected;
					total_infected=total_infected_pandemic_only+total_infected_seasonal_only+total_sim_coinfected;
				}

			}
		}
	
		if(hr==24)
		{
			_infectedByDay_p.push_back(total_infected_pandemic);
			_infectedByDay_s.push_back(total_infected_seasonal);
			_totalInfectedByDay.push_back(total_infected);

			fprintf(globalOutput_genStats,"%d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d		%d \n",day,_totalPopulation,total_infected,total_infected_pandemic,new_recovered_today_pandemic,total_infected_seasonal,new_recovered_today_seasonal,total_infected_pandemic_only,total_infected_seasonal_only,total_coinfected_s_p,total_reinfected_s_p,total_coinfected_p_s,total_reinfected_p_s,total_sim_coinfected);
			//reset global daily variables
			new_recovered_today_pandemic = 0;
			new_recovered_today_seasonal = 0;
//yuwen add 0624 print file
			fprintf(sampleOutput, "%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d 	%d\n", day, total_PHLtested_pandemic,total_PHLtested_seasonal,\
				total_PHLtested_all,total_pandemic_submit_samples,total_seasonal_submit_samples,total_submit_samples,total_both_submit_samples,\
				total_pandemic_uncollect_samples,total_seasonal_uncollect_samples,total_uncollect_samples,\
				total_pandemic_MSSS_samples,total_seasonal_MSSS_samples,total_MSSS_samples,\
				total_discard_pandemic,total_discard_seasonal,total_discard_all,CAPACITY_IND);
//yuwen add 0624 end
		}
		t_now++;
	}
//yuwen change 0625 print
	//fclose(time_output);
	fclose(globalOutput_genStats);
	fclose(sampleOutput);
//yuwen add 1003 printout tested by submission days begin
	//Print tested results for pandemic
	char titleStr[35];
	char cStr[15];
	strcpy(titleStr,RunStr.c_str());
	strcpy(cStr, "_pandemic.txt");
	strcat(titleStr,cStr);

	FILE* fSubmitPandemic=fopen(titleStr,"w");
	
	for (int c = 0; c <23; c++)
	{
      for (int d = 0 ; d < 23; d++) 
      {
         fprintf(fSubmitPandemic, "%d\t", submitPandemic[c][d]);
      }
      fprintf(fSubmitPandemic, "\n"); 
   	}
	fclose(fSubmitPandemic);

	//Print tested results for seasonal
	titleStr[0] = 0;
	cStr[0] = 0;
	strcpy(titleStr,RunStr.c_str());
	strcpy(cStr, "_seasonal.txt");
	strcat(titleStr,cStr);

	FILE* fSubmitSeasonal=fopen(titleStr,"w");
	
	for (int c = 0; c <23; c++)
	{
      for (int d = 0 ; d < 23; d++) 
      {
         fprintf(fSubmitSeasonal, "%d\t", submitSeasonal[c][d]);
      }
      fprintf(fSubmitSeasonal, "\n");
   	}
   	fclose(fSubmitSeasonal);

   	//Print tested results for total
   	titleStr[0] = 0;
	cStr[0] = 0;
	strcpy(titleStr,RunStr.c_str());
	strcpy(cStr, "_total.txt");
	strcat(titleStr,cStr);

	FILE* fSubmitTotal=fopen(titleStr,"w");
	
	for (int c = 0; c <23; c++)
	{
      for (int d = 0 ; d < 23; d++) 
      {
         fprintf(fSubmitTotal, "%d\t", submitTotal[c][d]);
      }
      fprintf(fSubmitTotal, "\n");
   	}
   	fclose(fSubmitTotal);
//yuwen add 1003 printout tested by submission days end

//yuwen change 0625 print end
	for(unsigned int i=0;i<_cities.size();i++)
	{
		_cities[i]->closeOutputFiles();
		_cities[i]->checkRNumber();	
	}
	//print the rn totals
	char rnStr[21];
	strcpy(rnStr,RunStr.c_str());
	strcat(rnStr,"output_R.txt");

	FILE* R_output= fopen(rnStr,"w");
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
		for(int c=0;c<Flu_Manager::Instance()->NUM_CITIES;c++)
		{
			if(_cities[c]->getPandemicRn(i)>0)
			{
				sum_rn_p+=_cities[c]->getPandemicRn(i);
				sumCities_p++;
			}	
			if(_cities[c]->getSeasonalRn(i)>0)
			{
				sum_rn_s+=_cities[c]->getSeasonalRn(i);
				sumCities_s++;
			}	
			sumPandemicGeneration +=_cities[c]->getPandemicGenerations(i);
			sumSeasonalGeneration +=_cities[c]->getSeasonalGenerations(i);
		}
		ave_rn_p=sum_rn_p/sumCities_p;
		ave_rn_s=sum_rn_s/sumCities_s;
		fprintf(R_output,"%d			%d				%f			%d				%f\n",i+1,sumPandemicGeneration,ave_rn_p,sumSeasonalGeneration,ave_rn_s);
	}
	fclose(R_output);

//begin yuwen add to release memory of vectors for each simulation run
	for(unsigned int i=0;i<_cities.size();i++)
	{
		delete _cities[i];
		_cities[i]=NULL;
	}
	_samplePoolPHL.clear();
	_samplePoolPHL.shrink_to_fit();
	_samplePoolRural.clear();
	_samplePoolRural.shrink_to_fit();
	_uncollectedSample.clear();
	_uncollectedSample.shrink_to_fit();
	_sampleMsss.clear();
	_sampleMsss.shrink_to_fit();
}

bool SimulationRun::hasCityWithOutbreak()
{
	for(unsigned int i=0;i<_cities.size();i++)
	{
			if(_cities[i]->outbreakRunning())
				return true;
	}
	return false;
}

int SimulationRun::getDailyInfectedPandemic(int index)
{
	return _infectedByDay_p[index];
}
int SimulationRun::getDailyInfectedSeasonal(int index)
{
	return _infectedByDay_s[index];
}
int SimulationRun::getTotalInfected(int index)
{
	return _totalInfectedByDay[index];
}
//yuwen chang add
//return a random value between int a and int b
/*
float SimulationRun::uni(int a, int b)
{
	if (_uniform_counter < 9999) {
		++_uniform_counter;
		return a + (b-a)*_uniform[_uniform_counter];
	}
	else {
		_uniform_counter = 1;
		return a + (b-a)*_uniform[_uniform_counter];
	}
}
//yuwen change end
//yuwen change add
//  creates iid random variates uniformly distributed between 0 and 1, based on a seed input from init files
double SimulationRun::fluRandom()
{
		long i=_seed/127773;
		long j=_seed%127773;
		long k=j*16807-i*2836;
		if (k>0) _seed=k;
		else _seed=k+2147483647;

		return _seed/2147483647.0;
}
//yuwen change end
//yuwen change add 
float SimulationRun::getUniformValue(int index) 
{
	return _uniform[index];
}
//yuwen change end
*/

double SimulationRun::uni(int a, int b)
{   
	double x, y;
	do {y = rand()/(double)RAND_MAX;
		x = ((double)(a) + ((double)(b)-(double)a)*y);
		}
		while (x == 0);
	return x;
}

//yuwen 0623 add
void SimulationRun::addtoSamplePool(Person* person, int day)
{
//#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) reduction(+:total_pandemic_submit_samples,\
	total_seasonal_submit_samples, total_both_submit_samples,total_submit_samples)
//{
	struct Samples samplePoolPHL;
	samplePoolPHL._sample = person;
	samplePoolPHL._virusType = person->currentVirusType;
	samplePoolPHL._sampleClock = 0;
	samplePoolPHL._visitCityId = person->getVisitingCity()->getId();
	samplePoolPHL._PHLID = 1;
	samplePoolPHL._submitDay=day;//record the day sample is submitted.
//yuwen add parallel 1002 begin
	if(day%7==6)//sampling collected in weekends assuming no shipment on Saturday
	{
		if(samplePoolPHL._visitCityId==2)//samples are collected from rural area
		{
			samplePoolPHL._arriveDay = day+4;//earlest day for testing in PHL is in 4 days
#pragma omp critical
			_samplePoolRural.push_back(samplePoolPHL);
		}
		else
		{
			samplePoolPHL._arriveDay = day+3;//earlest day for testing in PHL is in 3 day
#pragma omp critical
			_samplePoolPHL.push_back(samplePoolPHL);
		}
	}
	else if(day%7==0)//sampling collected in weekends assuming no shipment in weekends
	{
		if(samplePoolPHL._visitCityId==2)//samples are collected from rural area
		{
			samplePoolPHL._arriveDay = day+3;//earlest day for testing in PHL is in 4 days
#pragma omp critical
			_samplePoolRural.push_back(samplePoolPHL);
		}
		else
		{
			samplePoolPHL._arriveDay = day+2;//earlest day for testing in PHL is in 3 day
#pragma omp critical
			_samplePoolPHL.push_back(samplePoolPHL);
		}
	}
	else//sample are collected in weekdays
	{
		if(samplePoolPHL._visitCityId==2)//samples are collected from rural area
		{
			samplePoolPHL._arriveDay = day+2;//earlest day for testing in PHL is in 2 days
#pragma omp critical
			_samplePoolRural.push_back(samplePoolPHL);
		}
		else
		{
			samplePoolPHL._arriveDay = day+1;//earlest day for testing in PHL is next day
#pragma omp critical
			_samplePoolPHL.push_back(samplePoolPHL);
		}
	}
	if(samplePoolPHL._virusType==1)
#pragma omp atomic
		total_pandemic_submit_samples++;
	else if (samplePoolPHL._virusType==2)
#pragma omp atomic
		total_seasonal_submit_samples++;
	else if (samplePoolPHL._virusType==3)
	{
#pragma omp atomic
		total_pandemic_submit_samples++;
#pragma omp atomic		
		total_seasonal_submit_samples++;
#pragma omp atomic		
		total_both_submit_samples++;
	}
#pragma omp atomic	
	total_submit_samples++;
//yuwen add parallel 1002 end
//}
}
//yuwen 0623 add end

//yuwen 0623 add
void SimulationRun::addtoUnCollected(Person* person, int day)
{
//#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) reduction(+:total_pandemic_uncollect_samples,\
	total_seasonal_uncollect_samples,total_uncollect_samples)
//{
	struct Samples uncollectedSample;
	uncollectedSample._sample = person;
	uncollectedSample._virusType = person->currentVirusType;
	uncollectedSample._sampleClock = -1;
	uncollectedSample._visitCityId = person->getVisitingCity()->getId();
	uncollectedSample._PHLID = -1;//
	uncollectedSample._arriveDay = day;
	uncollectedSample._submitDay = day;
//1002 yuwen delete begin
//	_uncollectedSample.push_back(uncollectedSample);
//1002 yuwen delete end
//yuwen add parallel 1002 begin
	if(uncollectedSample._virusType==1)
#pragma omp atomic
		total_pandemic_uncollect_samples++;
	else if (uncollectedSample._virusType==2)
#pragma omp atomic
		total_seasonal_uncollect_samples++;
	else if (uncollectedSample._virusType==3)
	{
#pragma omp atomic
		total_pandemic_uncollect_samples++;
#pragma omp atomic
		total_seasonal_uncollect_samples++;
	}
#pragma omp atomic
	total_uncollect_samples++;
//yuwen add parallel 1002 end
//}
}
//yuwen 0623 add end

//yuwen 0623 add
void SimulationRun::addtoMsssPool(Person* person, int day)
{
//#pragma omp parallel num_threads(Flu_Manager::Instance()->NUM_THREADS) reduction(+:total_pandemic_MSSS_samples,\
	total_seasonal_MSSS_samples,total_MSSS_samples)
//{
	struct Samples sampleMsss;
	sampleMsss._sample = person;
	sampleMsss._virusType = person->currentVirusType;
	sampleMsss._sampleClock = -1;
	sampleMsss._visitCityId = person->getVisitingCity()->getId();
	sampleMsss._PHLID = -1;
	sampleMsss._arriveDay = day;
	sampleMsss._submitDay = day;
//1002 yuwen delete begin
//	_sampleMsss.push_back(sampleMsss);
//1002 yuwen delete end
	if(sampleMsss._virusType==1)
//yuwen add parallel 1002 begin
#pragma omp atomic
		total_pandemic_MSSS_samples++;
	else if (sampleMsss._virusType==2)
#pragma omp atomic
		total_seasonal_MSSS_samples++;
	else if (sampleMsss._virusType==3)
	{
#pragma omp atomic
		total_pandemic_MSSS_samples++;
#pragma omp atomic
		total_seasonal_MSSS_samples++;
	}
#pragma omp atomic
	total_MSSS_samples++;
//yuwen add parallel 1002 end
//}
}
//yuwen 0623 add end

//yuwen 0624 add update sample pool
void SimulationRun::SamplingPHL(int day)
{
	int _testByDay=0;
	int work=0;//check whether PHL work on day
	if(Flu_Manager::Instance()->WORK_WEEKEND==1 || day%7!=0 || day%7!=6)//allow PHL work in weekend
		work=1;	

	//int samplePoolSize = _samplePoolPHL.size();
	//printf("sample size in pool before rural is %d\n",samplePoolSize);
	//int offset=0;
	int j=0;
	while(j!=_samplePoolPHL.size())
	//for(int i=0;i<samplePoolSize;i++)
	{
		//int j = i-offset;
		_samplePoolPHL[j]._sampleClock++;

		if(work==1 && _testByDay<Flu_Manager::Instance()->PHL_CAPACITY)
		{
			if(_samplePoolPHL[j]._sampleClock<=3)//life is 3 days
			{
				if(_samplePoolPHL[j]._arriveDay<=day)//arriving day (earliest available testing day) is the current day or before
				{
					if(Flu_Manager::Instance()->SAMPLING_CRITERIA==1)
					{
						sumTested(j);//aggregate the number of tested samples by tested days
						updateTestedBySubmit(j,day);//aggregate the number of tested samples by submission days
					 	_testByDay++;
					}
					else if(Flu_Manager::Instance()->SAMPLING_CRITERIA==4) //FIFO only in first week
					{
						if(_samplePoolPHL[j]._arriveDay<=7){
							sumTested(j);
							updateTestedBySubmit(j,day);
					 		_testByDay++;
					 	}
					 	else
					 		sumDiscard(j);
					}
					else if (Flu_Manager::Instance()->SAMPLING_CRITERIA==2)
					{
						if(_samplePoolPHL[j]._sample->getAge() <=5 || _samplePoolPHL[j]._sample->getAge()>=65)
						{
							sumTested(j);
							updateTestedBySubmit(j,day);
					 		_testByDay++;
						}
						else
							sumDiscard(j);
					}
					else if (Flu_Manager::Instance()->SAMPLING_CRITERIA==5) //FIFO in high risk group only in first week
					{
						if(_samplePoolPHL[j]._arriveDay<=7){
							if(_samplePoolPHL[j]._sample->getAge() <=5 || _samplePoolPHL[j]._sample->getAge()>=65)
							{
								sumTested(j);
								updateTestedBySubmit(j,day);
						 		_testByDay++;
							}
							else
							sumDiscard(j);
						}
						else
							sumDiscard(j);
					}
					else if (Flu_Manager::Instance()->SAMPLING_CRITERIA==3)//hybrid sampling 
					{
						if (CAPACITY_IND==0)//indicator = 0 for capacity not reached yet
						{
							sumTested(j);
							updateTestedBySubmit(j,day);
					 		_testByDay++;
						}
						else if(_samplePoolPHL[j]._sample->getAge() <=5 || _samplePoolPHL[j]._sample->getAge()>=65)
						{
							sumTested(j);
							updateTestedBySubmit(j,day);
					 		_testByDay++;
						}
						else
							sumDiscard(j);			
					}
					_samplePoolPHL.erase(_samplePoolPHL.begin()+j);
				}
				else//
					j++;
			}
			else
			{
				sumDiscard(j);
				_samplePoolPHL.erase(_samplePoolPHL.begin()+j);//delete the (j+1)th element in the vector.
			}
		}
		else
			j++;	
	}

//yuwen change 1002 copy rural array after testing begin
	int sampleRuralSize=_samplePoolRural.size();
	if(work==1 || day%7!=0 || day%7!=6)//weekends work
	{
		for(int i = 0; i<sampleRuralSize; ++i)
		{
			_samplePoolPHL.push_back(_samplePoolRural[i]);
		}
		_samplePoolRural.clear();
	}
	//printf("sample size in pool after rural is %d\n",samplePoolSize);
//yuwen change 1002 copy rural array after testing end

	if(_testByDay==Flu_Manager::Instance()->PHL_CAPACITY)
		CAPACITY_IND=1;
	//_samplePoolPHL.erase(_samplePoolPHL.begin(),_samplePoolPHL.begin()+_testByDay);
	//debug 
}

void SimulationRun::sumTested(int j)//aggregate the number of tested samples by tested days
{
	if(_samplePoolPHL[j]._virusType==1) //Pandemic
     	total_PHLtested_pandemic++;
	else if(_samplePoolPHL[j]._virusType==2) //seasonal
		total_PHLtested_seasonal++;
	else if(_samplePoolPHL[j]._virusType==3) //both
	{
		total_PHLtested_pandemic++;
		total_PHLtested_seasonal++;
	}
	total_PHLtested_all++;
}

//yuwen add 1002 tested by submission days begin
void SimulationRun::updateTestedBySubmit(int j,int day)//matrix for the number of tested new cases by submission days
{
	int submitDay = _samplePoolPHL[j]._submitDay;
	if(_samplePoolPHL[j]._virusType==1) //Pandemic
		submitPandemic[submitDay-1][day-1]++;
//yuwen add 0925 end collect tested specimen with submission date
	else if(_samplePoolPHL[j]._virusType==2) //seasonal
		submitSeasonal[submitDay-1][day-1]++;
	else if(_samplePoolPHL[j]._virusType==3) //both
	{
		submitPandemic[submitDay-1][day-1]++;
		submitSeasonal[submitDay-1][day-1]++;
	}
	submitTotal[submitDay-1][day-1]++;
}
//yuwen add 1002 tested by submission days end

void SimulationRun::sumDiscard(int j)
{
	if(_samplePoolPHL[j]._virusType==1) //Pandemic
		total_discard_pandemic++;
	else if(_samplePoolPHL[j]._virusType==2) //seasonal
		total_discard_seasonal++;
	else if(_samplePoolPHL[j]._virusType==3) //both
	{
		total_discard_pandemic++;
		total_discard_seasonal++;	
	}
	total_discard_all++;	
}
