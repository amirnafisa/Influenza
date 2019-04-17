#pragma once
#include "Location.h"

#ifndef BUSINESS_H
#define BUSINESS_H

class Business :
	public Location
{
private:
	float _percentWeekendErrands; // cum % of errands Sat-Sun
	float _percentWeekdayErrands;// cum % of after work errands Mon-Fri
	float _percentVoluntaryQuarantine;// cum % of voluntary quarantine
	long double _percentWorkplaces;// cum % of workplaces for this business entity
public:
	enum eBusinessType{HOME,FACTORY,OFFICE,PRE_SCHOOL,ELEMENTARY_SCHOOL,MIDDLE_SCHOOL,HIGH_SCHOOL,
		UNIVERSITY,AFTERSCHOOL_CENTER,GROCERY,RETAIL,RESTAURANT,ENTERTAINMENT,CHURCH};
	eBusinessType eType;
	Business(int,int,long double,float,float,float,int);
	~Business(void);
	float getPerWeekendErrands() const;
	float getPerWeekdayErrands() const;
	float getPerVolQuarantine() const;
	long double getPerWorkplaces() const;
	int getIdBus() const;
};
#endif

