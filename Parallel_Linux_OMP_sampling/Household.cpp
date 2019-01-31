#include "Household.h"
#include "Flu_Manager.h"
#include "Multi_City_44_Templates.h"
#include "Business.h"

//yuwen change 102718 add index begin
Household::Household(int adults,int children, City* city, std::vector<Person*> &community_private)
//yuwen change 102718 add index end
{
	_id=1;
	p_run=city->getSimRun();
	p_city=city;
	_contactRate=3;
	_locationModifier=Flu_Manager::Instance()->k_hh;
	_numInfected=0;
//yuwen add seek healthcare 05132018_13 begin
	homePovertyState=setHomePovertyState();
//yuwen add seek healthcare 05132018_13 end
	for(int i=0;i<adults;i++)
		addAdult(city,community_private);
	for(int i=0;i<children;i++)
		addChild(city,community_private);
}


Household::~Household(void)
{
	//free memory fom person arrays
	for(unsigned int i=0;i<_adults.size();i++)
	{
		delete _adults[i];
		_adults[i]=NULL;
	}
	for(unsigned int i=0;i<_children.size();i++)
	{
		delete _children[i];
		_children[i]=NULL;
	}
}

void Household::addAdult(City* city, std::vector<Person*> &community_private)
{	//persons age is based on where in the range of cumulative probabilities of being a cretain age a random value falls
	int age, mapPerson;
	float y=p_run->uni(0,1);//the random value
	if (y <= Flu_Manager::Instance()->getAdultAgesCumProb(1)) {//person is 22-29
			age = (int)p_run->uni(22, Flu_Manager::Instance()->getAdultAge(1));
	}
	else if (y >Flu_Manager::Instance()->getAdultAgesCumProb(1)&& y <= Flu_Manager::Instance()->getAdultAgesCumProb(2)) {//person is 29-64
			age= (int)p_run->uni( Flu_Manager::Instance()->getAdultAge(1), Flu_Manager::Instance()->getAdultAge(2));
	}
	else {//person is over 64
			age = (int)p_run->uni( Flu_Manager::Instance()->getAdultAge(2), Flu_Manager::Instance()->getAdultAge(3));
	}
	Person* adult=new Person(age,city);
	_adults.push_back(adult);
	p_city->addToCommunity(adult,community_private);
}

void Household::addChild(City* city,std::vector<Person*> &community_private)
{	//persons age is based on where in the the range of cumulative probabilities of being a cretain age a random value falls
	int age;
	float y=p_run->uni(0,1);//the randome value
	for (int j = 1; j <= number_child_age_type; ++j) {//iterate through each age range category and find where y lies, thats the childs age range
			if (y >= Flu_Manager::Instance()->getChildAgesCumProb(j-1) && y <  Flu_Manager::Instance()->getChildAgesCumProb(j)) {
				age = (int)p_run->uni(Flu_Manager::Instance()->getChildAge(j-1), Flu_Manager::Instance()->getChildAge(j));
			}
	}
	Person* child =new Person(age,city);
	_children.push_back(child);
	p_city->addToCommunity(child,community_private);
}
//yuwen change add
void Household::assignWorkplaces(std::vector<Business> & workplaces)
{
	for(unsigned int i=0;i<_adults.size();i++)
	{
		//for each person get a random value ,y, and do a binary search to locate a workplace within 5 places of where the probalbility of working at a given place=y 
		int low;
		int high;
		int mid;

		float y = p_run->uni(0,1);
		low = 0; 
		high = workplaces.size()-1;
		mid = (low + high)/2;
		while ( high - low >= 5) {
			if( y <=workplaces[mid].getPerWorkplaces()){
				high = mid;
				mid = (low + high)/2;
			}
			if( y >workplaces[mid].getPerWorkplaces()){
				low = mid;
				mid = (low + high)/2;
				}
		}

		for (int j = low; j <= high; ++j) {
			long double prevWork=0.0;//previous workplace in the lis of templates, if it does not exist, value is 0
			if(j>0)
				prevWork=workplaces[j-1].getPerWorkplaces();
			//assign adult the found workplace
			if (y >= prevWork && y <workplaces[j].getPerWorkplaces()) {
				_adults[i]->setWorkplace(&workplaces[j]);
				break;
			}
			else if(j==high)// businesses may have such a small difference in probability of being the workplaace that it rounds off to the same value, making the previous if statement always false , under that condition the person will be assigned to the last possible workplace
				_adults[i]->setWorkplace(&workplaces[j]);
		}
	}

	for(unsigned int i=0;i<_children.size();i++)
	{
		int age1=_children[i]->getAge();
		int  t,z;
		// fish out the type of school based on the age
		for (int j = 1; j <= number_child_age_type; ++j) {
			if (age1 >=Flu_Manager::Instance()->getChildAge(j-1) && age1 < Flu_Manager::Instance()->getChildAge(j))
				t = Flu_Manager::Instance()->getChildWorkType(j);
		}

		z = (int)p_run->uni(0,p_city->getBusinessNumbers(t+1)); //for the workplace array.  In notepad t = _ageChildRanges[i]._workType is correct but in C t+1 = _ageChildRanges[i]._workType gives the correct position for the school type in the workplace array.  Print  _workplaceTypes[i]._numberOfType to understand
		// send the child to business(school) of type t
		for (unsigned int j = 1; j <= workplaces.size()-1; ++j) {
			if (workplaces[j].eType == (Business::eBusinessType)t) {
				_children[i]->setWorkplace(&workplaces[j+z-1]);
				break;
			}
		}
	}
}
//yuwen change end
//yuwen add seek healthcare 05132018_12 begin
//set the poverty state to a household randomely
Household::ePovertyState Household::setHomePovertyState()
{
	float prob;
	float randNum=p_city->getSimRun()->uni(0,1);
	prob=Flu_Manager::Instance()->POVERTY_PROB;//State baseline poverty probability
	if(randNum<=prob)//The household is poor
		return HIGH;
	return LOW;
}
//yuwen add seek healthcare 05132018_12 end
void Household::setZipCode(int zipcode)
{
	_zipcode=zipcode;
}

int Household::getZipcode() const
{
	return _zipcode;
}
int Household::getSize() const
{
	return _adults.size()+_children.size();
}

int Household::getNumAdults() const
{
	return _adults.size();
}

int Household::getNumChildren() const
{
	return _children.size();
}

Person* Household::getAdult(int index) const
{
	return _adults[index];
}

Person* Household::getChild(int index) const
{
	return _children[index];
}
//set a pointer to a persons home and workplace
void Household::setFamiliyPtrs()
{
	for(unsigned int i=0;i<_adults.size();i++)
		_adults[i]->setHousehold(this);
	for(unsigned int i=0;i<_children.size();i++)
		_children[i]->setHousehold(this);
}
//randomly choose a person to start out infected takes a bool that s true if the virus is pandemic, false if the virus is seasonal
void Household::pickPatientZero(bool pandemic)
{//randomly choose a person in the household
	int householdSize=(int)(_adults.size()+_children.size());
	unsigned int pZero=(unsigned int)p_run->uni(0,householdSize);
	if(pZero<_adults.size())//choice is an adult
	{
		_adults[pZero]->infected(pandemic,NULL,1);
		//p_city->addToInfectedList(_adults[pZero]);
	}
	else //choice is a child
	{
		_children[pZero-_adults.size()]->infected(pandemic,NULL,1);
		//p_city->addToInfectedList(_children[pZero-_adults.size()]);
	}
}

