#pragma once

#include "json.h"

class Fishery
{
public :
	//constructor for the fishery object
	Fishery() 
	{
		//preinit for safety. intended to properly init inside the simulation loop
		fishStock = 0;
		carryingCapacity = 0;
		reproductionRate = 0;
		catchability = 0;
	};

	//simple model variables
	const double& getSimpleReproductionRate() { return reproductionRate; };
	void setSimpleReproductionRate(double newReproductionRate) { reproductionRate = newReproductionRate; }

	const double& getSimpleCarryingCapacity() { return carryingCapacity; };
	void setSimpleCarryingCapacity(double newCarryingCapacity) { carryingCapacity = newCarryingCapacity; }

	const double& getFishStock() { return fishStock; };
	void setFishStock(double newFishStock) { fishStock = newFishStock; }

	//delay equation model variables
	const double& getCatchability() { return catchability; };
	void setCatchability(double newCatchability) { catchability = newCatchability; }

private:
	//the current amount of fish stock in the fishery
	double fishStock;

	//Simple Model Variables

	//represented by tons of biomass
	//the long term stable stock with no fishing activity
	//used by the simple model
	double carryingCapacity;

	//a parameter representing the basic reproductive rate of the fish
	double reproductionRate;

	//Delay Equation Model Variables

	//a parameter representing how easy it is to catch fish for a given amount of effort
	double catchability;

};

