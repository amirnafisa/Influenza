#include "Location.h"
#include "Person.h"

Location::Location(void)
{
}

Location::Location(int id,int cr,int infected )
{
	_id=id;
	_numInfected=infected;
	_contactRate=cr;
	_occupants.clear();
}

Location::~Location(void)
{
	//_occupants.clear();
}

int Location::getId() const
{
	return _id;
}

int Location::getContactRate() const
{
	return _contactRate;
}

int Location::getNumInfected() const
{
	return _numInfected;
}

void Location::setContactRate(int rate)
{
	_contactRate=rate;
}

void Location::setNumInfected(int infected)
{
	_numInfected=infected;
}

float Location::getLocMod() const
{
	return _locationModifier;
}

void Location::setLocMod(float w)
{
	_locationModifier=w;
}

std::vector<Person*> Location::getOccupants()const
{
	return _occupants;
}

void Location::addOccupant(Person* person) //adds the person into the occupants vector. There is occupant vector per workplace
{
	#pragma omp critical
	_occupants.push_back(person);
}

void Location::removeOccupants()
{
	_occupants.clear();
	//_occupants.shrink_to_fit();
}

bool Location::hasOccupant(Person * person)
{
	if(_occupants.empty())
		return false;
	else
	{
		for(unsigned int i=0;i<_occupants.size();i++)
		{
			if(_occupants[i]==person)
				return true;
		}
		return false;
	}
}

