/*This contains constants and sctructures used in the flu simulation Single_city_43 - Greg Ostroy 2/12/2016*/
#pragma once
#ifndef MULTI_CITY_44_TEMPLATES_H
#define MULTI_CITY_44_TEMPLATES_H

/*********constants*************/
#define NUM_BUSINESSES 12000//number of businesses
#define culmination_period 240 // after these many hrs the person dies or recover
#define business_capacity 4000 // max business capacity (# of people)
#define household_capacity 10 // max household capacity (# of people)
#define contact_capacity 50 // max number of contacts per person per day (# of people)
#define number_business_type 14 // # of business types (including home)
#define number_adult_age_type 3 // # of age types for adults
#define number_child_age_type 5 // # of age types for kids
#define number_household_type 9 // # of household types
#define number_population_center 1 //# of population centers in the community
#define number_zipcodes 57 //# of population centers in the community
#define DISTRIBUTION_COLUMNS 10//number of x plots for the gamma, lognormal, and weib curves
#define NUMBER_REGIONS 4
/*infection profiles*/
#define INFECTED_SEASONAL 0//person is infected with seasonal virus 
#define INFECTED_PANDEMIC 1//person is infected with pandemic virus 
#define COINFECTED_PANDEMIC_SEASONAL 2//person with pandemic virus is infected with seasonal virus
#define COINFECTED_SEASONAL_PANDEMIC 3// person with seasonal virus is infected with panmeic virus
#define REINFECTED_PANDEMIC_SEASONAL 4//person recovered from pandemic virus is infected with seasonal virus
#define REINFECTED_SEASONAL_PANDEMIC 5//person recovered from seasonal virus is infected with pandemic virus
#define COINFECTED_SIMULTANEOUS 6//person is infected with both seasonal virus and pandemic virus on the same day
/********Templates*******/
struct WorkPlaceTemplate //represents a workplace type
{
	int _type;/*0 = home, 1 = factory, 2 = office, 3 = pre-school, 4 = elementary school, 5 = middle school, 6 = high school, 
7 = university, 8 = afterschool center,  9 = grocery store, 10 = other store, 11 = restaurant, 12 = entertainment, 13 = church*/
	int _numberOfType;
	float _percentWork;//percentage of peolpe working in places of ths type
	float _errandsWeekday;//% errands run at this type during week
	float _errandsWeekend;//% errands run at the type on the weekends
	float _errandQuarantine; // % volunatry quarantine errands run at this type
	int _numContacts;// number of people contacted at this type
	int _cumBusinesses;//cumulative number of businesses up to this tyoe
	float _cumPerWork;//cumulative number of people employed up to this type
	float _cumWeekday;//cumulative % errands run up to this type during week
	float _cumWeekend;//cumulative % errands run up to this type on the weekends
	float _cumVoluntaryQuarantine;//cumulative % volunatry quarantine errands run up to this type
};
struct Age_AdultTemplate //represents an adlut age group
{
	int _age; //maximum age of group
	float _cumulativeProb; //cumulative probability of someone being in that age group
};
struct Age_ChildTemplate// represents a child age group
{
	int _age; //maximum age of group
	float _cumulativeProb; //probability of someone being in that age group or younger
	int _workType;//workplace type child will be found
};
struct HouseHoldTemplate // represents a household type
{
	int _numAdults;//number of adults in house
	int _numChildren;//number of children in house
	float _cumPmf;//percentage of households matching this type or those preceeding it the file list
};
struct ZipcodeTemplate
{
	int _number;//the zip code number
	float _cumProb;//the cumulative probability of someone living in that area
};
struct Gamma1
{
	float _cumDensity;// gamma cumulative density F(x)-F(x-1) for symptomatic cases
};
struct Gamma2
{
	float _cumDensity;
};
struct Lognorm1
{
	float _cumDensity;// lognormal cumulative density F(x)-F(x-1) for symptomatic cases
};
struct Lognorm2
{
	float _cumDensity;
};
struct Weib1
{
	float _cumDensity;// weibull cumulative density F(x)-F(x-1) for symptomatic cases
};
struct Weib2
{
	float _cumDensity;
};

struct contact
{
	int _contactId;//id of the contact
	float _locationModifier;//modifier based on the closeness and duration of the contact
};

struct TravelStats
{
	float _probTravel;
	float _probDest;
	int _numHouseholds_scaled;
	int _numHouseholds_unscaled;
	int _numArrivals_scaled;
	int _numArrivals_unscaled;
};
#endif



