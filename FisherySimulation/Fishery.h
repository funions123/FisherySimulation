#pragma once

#include "json.h"

class Fishery
{
public :
	//constructor for the fishery object
	Fishery(const double carryingCapacity,
			const double fishStock,
			const double reproductionRate) :
			carryingCapacity(carryingCapacity), fishStock(fishStock), reproductionRate(reproductionRate) {};

	//simple model variables
	const double& getSimpleReproductionRate() { return reproductionRate; };
	const double& getSimpleCarryingCapacity() { return carryingCapacity; };
	const double& getFishStock() { return fishStock; };

private:
	//the current amount of fish stock in the fishery
	double fishStock;

	//Simple Model Variables

	//represented by tons of biomass
	//the long term stable stock with no fishing activity
	//used by the simple model
	const double carryingCapacity;

	//a parameter representing the basic reproductive rate of the fish
	double reproductionRate;

};

