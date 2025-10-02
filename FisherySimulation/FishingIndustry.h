#pragma once
class FishingIndustry
{
public:
	FishingIndustry() 
	{
		//preinit for safety. intended to properly init inside the simulation loop
		harvestRate = 0;
		harvestingEffort = 0;
		fishMarketStock = 0;
		catchStockingRate = 0;
		stockReturnRate = 0;
		fishPrice = 0;
		fishingCost = 0;
	}

	//Harvesting rate in tons for the simplified model
	const double getSimpleHarvestRate() { return harvestRate; };
	void setSimpleHarvestRate(double newHarvestRate) { harvestRate = newHarvestRate; }

	//Harvesting effort for the delay equation model
	const double getHarvestingEffort() { return harvestingEffort; };
	void setHarvestingEffort(double newHarvestingEffort) { harvestingEffort = newHarvestingEffort; }

	//Fish market stock for the delay equation model
	const double getFishMarketStock() { return fishMarketStock; };
	void setFishMarketStock(double newFishMarketStock) { fishMarketStock = newFishMarketStock; }

	//Stocking fraction for the delay equation model
	const double getCatchStockingRate() { return catchStockingRate; };
	void setCatchStockingRate(double newCatchStockingRate) { catchStockingRate = newCatchStockingRate; }

	//Stock return rate for the delay equation model
	const double getStockReturnRate() { return stockReturnRate; };
	void setStockReturnRate(double newStockReturnRate) { stockReturnRate = newStockReturnRate; }

	//Market price of fish for the delay equation model
	const double getFishPrice() { return fishPrice; };
	void setFishPrice(double newFishPrice) { fishPrice = newFishPrice; }

	//cost per effort of fishing
	const double getFishingCost() { return fishingCost; };
	void setFishingCost(double newFishingCost) { fishingCost = newFishingCost; }

private:

	//Simple Model Variables

	//the harvesting rate in tons of fish stock
	double harvestRate;

	//Delay Equation Model Variables
	
	//The "effort" expended by fishing fleets 
	//Dimensionless composite value representing the total time spent fishing 
	//and the amount of fishing boats operating
	double harvestingEffort;

	//the amount of fish stored for later sale
	double fishMarketStock;

	//the fraction of caught fish stocked for later
	double catchStockingRate;

	//the fraction fish market stock sold
	double stockReturnRate;

	//the market price of fish
	double fishPrice;

	//the cost per effort of fishing
	double fishingCost;
};

