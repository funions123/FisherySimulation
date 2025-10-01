
#include <iostream>
#include "Fishery.h"
#include "FishingIndustry.h"

int main()
{
    //hard-coded simulation setup for testing purposes
    Fishery myFishery(12000.0, 9000.0, 1.0);
    FishingIndustry myFishingIndustry(2000, 0.4);

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
        printf("Year | Fish Stock | Harvest Rate\n");
        printf("--------------------------------------------------\n");
        printf("%4d | %14.4f | %10.4f\n", 0, myFishery.getFishStock(), myFishingIndustry.getSimpleHarvestRate());
    }
    else if (choice == 2)
    {
        printf("Year | Fish Stock | Effort\n");
        printf("--------------------------------------------------\n");
        printf("%4d | %14.4f | %10.4f\n", 0, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort());
    }

    std::cout << "\nSimulation finished. Press Enter to exit." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); //press any key

    //exit the program
    return 0;
}

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
*/
double DelayEquationModelGrowthAmount(Fishery& fishery, FishingIndustry& fishingindustry)
{

}
