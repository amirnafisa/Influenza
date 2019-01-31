#pragma once
#include <vector>
#include "Location.h"
#include "Person.h"

#ifndef HOUSEHOLD_H
#define HOUSEHOLD_H
class Household :
	public Location
{
private:
	std::vector<class Person*> _adults;
	std::vector<class Person*> _children;
	class SimulationRun* p_run;
	class City* p_city;
	int _zipcode;
public:
	Household(int,int, class City*, std::vector<Person*> &);
	~Household(void);
	void addAdult(class City*,std::vector<Person*> &);
	void addChild(class City*,std::vector<Person*> &);
	void setZipCode(int);
	int getSize() const;
	int getNumAdults() const;
	int getNumChildren() const;
	int getZipcode() const;
	Person* getAdult(int) const;
	Person* getChild(int) const;
	void assignWorkplaces(std::vector<class Business> &);
	void setFamiliyPtrs();
	void pickPatientZero(bool);
//yuwen add seek healthcare 05132018_11 begin
	enum ePovertyState{HIGH,LOW};//High=cannot afford ER, Low=can afford ER
//yuwen add seek healthcare 05132018_11 end	
//yuwen add seek healthcare 05132018_10 begin
	ePovertyState setHomePovertyState();
	ePovertyState homePovertyState;//the poverty state of a household
//yuwen add seek healthcare 05132018_10 end
};
#endif

