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
		fishingMortality = 0;
		selectivity_A50 = 0;
		selectivity_k = 0;
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

	/**
	 * @brief Sets the core fishing parameters for the age-structured model.
	 */
	void setAgeModelParams(double F, double sel50, double selk) 
	{
		fishingMortality = F;
		selectivity_A50 = sel50;
		selectivity_k = selk;
	}

	/**
	 * @brief Calculates the fishing selectivity at a given age (logistic curve).
	 */
	double getSelectivityAtAge(int age) const
	{
		return 1.0 / (1.0 + std::exp(-selectivity_k * (age - selectivity_A50)));
	}

	double getFishingMortality() const { return fishingMortality; }

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

	//the total mortality from fishing activity
	double fishingMortality;

	//the age at 50 percent selectivity
	//the age at which half of all fish are caught by the fishing gear
	//fishing nets are specifically designed to allow small/young fish to escape
	double selectivity_A50;

	//a parameter representing the relationship between fishing gear selection and fish length
	//specificially, a high k means the fishing gear sharply goes from catching no fish at lower ages to catching
	//all fish at the A_50 age
	double selectivity_k;
};

