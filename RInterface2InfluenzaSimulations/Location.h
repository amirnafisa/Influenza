#pragma once
#include<vector>

#ifndef LOCATION_H
#define LOCATION_H
class Location
{
protected:
	int _id;
	int _contactRate;
	int _numInfected;
	float _locationModifier;//==k_wp,k_hh,or k_err depending on time and location type
	std::vector<class Person*> _occupants;//array of occupants for each hour of the day
public:
	Location(void);
	Location(int,int,int);
	~Location(void);
	int getId() const;
	int getContactRate() const;
	int getNumInfected() const;
	void setContactRate(int);
	void setNumInfected(int);
	float getLocMod() const;
	void setLocMod(float);
	std::vector<class Person*> getOccupants()const;
	void addOccupant(class Person*);
	void removeOccupants();
	bool hasOccupant(class Person*);
};
#endif

