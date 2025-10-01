#pragma once
class FishingIndustry
{
public:
	FishingIndustry(const double harvestRate,
					const double harvestingEffort) 
					: harvestRate(harvestRate), harvestingEffort(harvestingEffort) {}

	//Harvesting rate in tons for the simplified model
	const double getSimpleHarvestRate() { return harvestRate; };

	//Harvesting effort for the delay equation model
	const double getHarvestingEffort() { return harvestingEffort; };

private:

	//Simple Model Variables

	//the harvesting rate in tons of fish stock
	double harvestRate;

	//Delay Equation Model Variables
	
	//The "effort" expended by fishing fleets 
	//Dimensionless composite value representing the total time spent fishing 
	//and the amount of fishing boats operating
	double harvestingEffort;
};

