
#include <iostream>
#include "Fishery.h"
#include "FishingIndustry.h"

/*  @brief Simulates a growth and harvesting step in the fishery
*   This function utilizes a basic model with a logistic growth simulation
*   Calculates the natural growth of the fish population for a step, then
*   subtracts the harvested fish stock.
*   @param fishery The fishery being simulated
*   @param fishingindustry The fishing industry harvesting from the fishery
*/
double SimpleModelGrowthAmount(Fishery& fishery, FishingIndustry& fishingindustry)
{
    //calculate the natural growth of the fish stock
    double naturalGrowth = fishery.getSimpleReproductionRate() * fishery.getFishStock() * (1 - fishery.getFishStock() / fishery.getSimpleCarryingCapacity());

    //calculate the impact of harvesting on the fish stock
    //this number CAN be negative
    double finalGrowth = naturalGrowth - fishingindustry.getSimpleHarvestRate();

    return finalGrowth;
}

/*  @brief Simulates a growth and harvesting step in the fishery
*   This function implements a system of delay equations
*   @param fishery The fishery being simulated
*   @param fishingindustry The fishing industry harvesting from the fishery
*/
void DelayEquationModelStep(Fishery& fishery, FishingIndustry& fishingindustry, double timeStep)
{
    double n = fishery.getFishStock();
    double E = fishingindustry.getHarvestingEffort();
    double S = fishingindustry.getFishMarketStock();

    //step catch - equation 1
    double currentCatch = fishery.getCatchability() * n * E;

    //calculate the rate of change for each variable
    //dn/dt = rn(1-n) - qnE
    double dn_dt = fishery.getSimpleReproductionRate() * n * (1 - n) - currentCatch;

    //dE/dt = p((1-η)qnE + δS) - cE
    double dE_dt = fishingindustry.getFishPrice() * ((1 - fishingindustry.getCatchStockingRate()) * currentCatch + fishingindustry.getStockReturnRate() * S) - fishingindustry.getFishingCost() * E;

    //dS/dt = ηqnE - δS
    double dS_dt = fishingindustry.getCatchStockingRate() * currentCatch - fishingindustry.getStockReturnRate() * S;

    //update the state variables using a simple forward Euler step (assuming Δt = 1).
    fishery.setFishStock(std::max(0.0, n + dn_dt * timeStep));
    fishingindustry.setHarvestingEffort(std::max(0.0, E + dE_dt * timeStep));
    fishingindustry.setFishMarketStock(std::max(0.0, S + dS_dt * timeStep));

    return;
}

int main()
{
    int choice = 0;

    while (choice != 1 && choice != 2)
    {
        //ask the user which model to use
        std::cout << "Select a fishery simulation model:" << std::endl;
        std::cout << "1. Simple Logistic Model" << std::endl;
        std::cout << "2. Delay Equation Model" << std::endl;
        std::cout << "Enter your choice (1 or 2): ";
        std::cin >> choice;

        if (std::cin.fail() || (choice != 1 && choice != 2))
        {
            std::cout << "\nInvalid choice. Please enter 1 or 2.\n" << std::endl;
            std::cin.clear(); //clear the error flag on cin.
            //discard the rest of the line to handle invalid input.
            std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
            choice = 0; //reset choice to continue the loop.
        }
    }

    std::cout << "\n";

    if (choice == 1)
    {
        // --- Simple Model Simulation ---
        Fishery myFishery = Fishery();
        FishingIndustry myFishingIndustry = FishingIndustry();
        int simulationYears = 15;
        
        //test init
        myFishery.setSimpleCarryingCapacity(12000.0); //12k tons
        myFishery.setSimpleReproductionRate(1.0);
        myFishery.setFishStock(10000.0); //10k tons
        myFishingIndustry.setSimpleHarvestRate(2000.0); //2k tons

        std::cout << "--- Simple Logistic Model Simulation ---" << std::endl;
        printf("Year | Fish Stock (tons)\n");
        printf("--------------------------------------\n");
        printf("%4d | %f\n", 0, myFishery.getFishStock());

        //run the simulation loop
        for (int year = 1; year <= simulationYears; ++year) {
            double growth = SimpleModelGrowthAmount(myFishery, myFishingIndustry);
            myFishery.setFishStock(std::max(0.0, myFishery.getFishStock() + growth));
            printf("%4d | %f\n", year, myFishery.getFishStock());
        }
    }
    else if (choice == 2)
    {
        // --- Delay Equation Model Simulation ---
        Fishery myFishery = Fishery();
        FishingIndustry myFishingIndustry = FishingIndustry();
        int simulationYears = 20;
        int stepsPerYear = 100; //using sub-year steps
        double timeStep = 1.0 / stepsPerYear;

        //test init based on sample parameters from the paper
        myFishery.setSimpleReproductionRate(1.0);
        myFishery.setCatchability(2.0);
        myFishery.setFishStock(0.4);

        myFishingIndustry.setFishPrice(7.0);
        myFishingIndustry.setFishingCost(1.0);
        myFishingIndustry.setStockReturnRate(2.0);      //delta
        myFishingIndustry.setCatchStockingRate(0.5);    //eta
        myFishingIndustry.setHarvestingEffort(0.4);     //effort (E)
        myFishingIndustry.setFishMarketStock(0.1);      //market stock (S)

        std::cout << "--- Delay Equation Model Simulation ---" << std::endl;
        printf("Year | Population (n) | Effort (E) | Market Stock (S)\n");
        printf("----------------------------------------------------------\n");
        printf("%4d | %14.4f | %10.4f | %16.4f\n", 0, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock());

        //run the simulation loop
        for (int year = 1; year <= simulationYears; ++year) {
            for (int i = 0; i < stepsPerYear; ++i) {
                DelayEquationModelStep(myFishery, myFishingIndustry, timeStep);
            }
            printf("%4d | %14.4f | %10.4f | %16.4f\n", year, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock());
        }
    }

    std::cout << "\nSimulation finished. Press Enter to exit." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); //press any key

    //exit the program
    return 0;
}