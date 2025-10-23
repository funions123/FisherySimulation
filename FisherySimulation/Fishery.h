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
		maxAge = 0;
		naturalMortality = 0;
		vb_Linf = 0; vb_k = 0; vb_t0 = 0;
		lw_a = 0; lw_b = 0;
		maturity_A50 = 0; maturity_k = 0;
		constantRecruitment = 0;
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

	//----- Age Structured Operating Model Functions ------

	/**
	 * @brief Sets the core biological parameters for the age-structured model.
	 */
	void setAgeModelParams(int maxAge, double M, double linf, double k, double t0, double lwa, double lwb, double mat50, double matk, double constR) 
	{
		this->maxAge = maxAge;
		naturalMortality = M;
		vb_Linf = linf;
		vb_k = k;
		vb_t0 = t0;
		lw_a = lwa;
		lw_b = lwb;
		maturity_A50 = mat50;
		maturity_k = matk;
		constantRecruitment = constR;
		numbersAtAge.resize(maxAge + 1, 0.0);
	}

	/**
	 * @brief Sets the initial population numbers for each age class.
	 */
	void setInitialNumbers(const std::vector<double>& numbers) 
	{
		if (numbers.size() == maxAge + 1) 
		{
			numbersAtAge = numbers;
		}
		else 
		{
			std::cout << "Error: Initial numbers vector size mismatch." << std::endl;
			numbersAtAge.assign(maxAge + 1, 0.0); 
		}
	}

	const std::vector<double>& getNumbersAtAge() const { return numbersAtAge; }
	void setNumbersAtAge(const std::vector<double>& numbers) { numbersAtAge = numbers; }

	/**
	 * @brief Calculates the weight of a fish at a given age (von Bertalanffy + Length-Weight).
	 */
	double getWeightAtAge(int age) const 
	{
		double length = vb_Linf * (1.0 - std::exp(-vb_k * (age - vb_t0)));
		return lw_a * std::pow(length, lw_b);
	}

	/**
	 * @brief Calculates the proportion of mature fish at a given age (logistic curve).
	 */
	double getMaturityAtAge(int age) const 
	{
		return 1.0 / (1.0 + std::exp(-maturity_k * (age - maturity_A50)));
	}

	/**
	 * @brief Calculates the total biomass (sum of N[age] * W[age]).
	 */
	double getTotalBiomass() const 
	{
		double totalBiomass = 0.0;
		for (int age = 0; age <= maxAge; ++age) 
		{
			totalBiomass += numbersAtAge[age] * getWeightAtAge(age);
		}
		return totalBiomass;
	}

	/**
	 * @brief Calculates the spawning stock biomass (sum of N[age] * W[age] * M[age]).
	 */
	double getSpawningStockBiomass() const 
	{
		double ssb = 0.0;
		for (int age = 0; age <= maxAge; ++age) 
		{
			ssb += numbersAtAge[age] * getWeightAtAge(age) * getMaturityAtAge(age);
		}
		return ssb;
	}

	int getMaxAge() const { return maxAge; }
	double getNaturalMortality() const { return naturalMortality; }
	double getConstantRecruitment() const { return constantRecruitment; }

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

	//Age-structured Operating Model Variables

	//the array of fish at each age
	std::vector<double> numbersAtAge;

	//the maximum age class
	int maxAge;

	//the annual rate at which fish die of natural causes
	double naturalMortality;

	//the theoretical max length a fish can reach
	double vb_Linf;

	//the growth rate coefficient
	double vb_k;

	//the theoretical age of 0 length (usually negative)
	double vb_t0;

	//the scaling coefficient of the length-weight relationship
	double lw_a;

	//the growth exponent of the length-weight relationship
	double lw_b;

	//the age at which half of the fish population can reproduce
	double maturity_A50; 

	//the steepness of the curve maturity-age
	//a high k means fish rapidly go from 0% to 50% mature around the maturity_A50
	double maturity_k; 

	//the number of new fish that are born each year
	double constantRecruitment;

};

