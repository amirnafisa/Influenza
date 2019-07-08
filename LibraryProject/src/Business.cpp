#include "Business.h"
#include "Multi_City_44_Templates.h"
#include "Flu_Manager.h"

Business::Business(int businessType,int busID,long double perWork,float errWeekday,float errWeekend,float volQuarantine, int contactRate)
{
	_id=busID;
	_locationModifier=Flu_Manager::Instance()->k_wp;
	eType=(eBusinessType)(businessType-1); 
	_percentWeekendErrands=errWeekend; 
	_percentWeekdayErrands=errWeekday;
	_percentVoluntaryQuarantine=volQuarantine;
	_percentWorkplaces= perWork;
	_contactRate=contactRate;
	_numInfected=0;
}


Business::~Business(void)
{
}

float Business::getPerWeekendErrands() const
{
	return _percentWeekendErrands; 
}
float Business::getPerWeekdayErrands() const
{
	return _percentWeekdayErrands;
}
float Business::getPerVolQuarantine() const
{
	return _percentVoluntaryQuarantine;
}

long double Business::getPerWorkplaces() const
{
	return _percentWorkplaces;
}
int Business::getIdBus() const
{
	return _id;
}

