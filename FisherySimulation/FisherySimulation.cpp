
#include <iostream>
#include "Fishery.h"
#include "FishingIndustry.h"
#include "CSVManager.h"
#include <chrono>
#include <sstream> 
#include "json.h" //slightly modified nlohmann json all-in-one header

#ifdef _WIN32 //windows
#include <direct.h>
#define GetCurrentDir _getcwd
#else
#include <unistd.h> //linux
#define GetCurrentDir getcwd
#endif

using json = nlohmann::json;

/**
 * @brief Loads simulation parameters from a JSON file.
 * @param filename The name of the JSON configuration file.
 * @param fishery The fishery object to populate.
 * @param industry The fishing industry object to populate.
 * @param modelChoice 1 for Simple Model, 2 for Delay Model.
 * @param outSimYears (Output) The number of simulation years.
 * @param outStepsPerYear (Output) The number of steps per year (for delay model).
 * @return True if parameters were loaded successfully, false otherwise.
 */
bool loadParametersFromJSON(const std::string& filename, Fishery& fishery, FishingIndustry& industry, int modelChoice, int& outSimYears, int& outStepsPerYear)
{
    std::ifstream f(filename);
    if (!f.is_open()) {
        std::cerr << "Error: Could not open parameter file: " << filename << std::endl;
        return false;
    }

    try {
        json params = json::parse(f);

        if (modelChoice == 1) {
            auto modelParams = params.at("simpleModel");
            outSimYears = modelParams.at("simulationYears").get<int>();
            fishery.setSimpleCarryingCapacity(modelParams.at("carryingCapacity").get<double>());
            fishery.setSimpleReproductionRate(modelParams.at("reproductionRate").get<double>());
            fishery.setFishStock(modelParams.at("initialFishStock").get<double>());
            industry.setSimpleHarvestRate(modelParams.at("harvestRate").get<double>());
        }
        else if (modelChoice == 2) {
            auto modelParams = params.at("delayModel");
            outSimYears = modelParams.at("simulationYears").get<int>();
            outStepsPerYear = modelParams.at("stepsPerYear").get<int>();
            fishery.setSimpleReproductionRate(modelParams.at("reproductionRate").get<double>());
            fishery.setCatchability(modelParams.at("catchability").get<double>());
            fishery.setFishStock(modelParams.at("initialFishStock").get<double>());
            industry.setFishPrice(modelParams.at("fishPrice").get<double>());
            industry.setFishingCost(modelParams.at("fishingCost").get<double>());
            industry.setStockReturnRate(modelParams.at("stockReturnRate").get<double>());
            industry.setCatchStockingRate(modelParams.at("catchStockingRate").get<double>());
            industry.setHarvestingEffort(modelParams.at("initialHarvestingEffort").get<double>());
            industry.setFishMarketStock(modelParams.at("initialFishMarketStock").get<double>());
        }
        f.close();
        return true;
    }
    catch (json::parse_error& e) {
        std::cerr << "Error: Failed to parse JSON file: " << filename << "\n" << e.what() << std::endl;
        f.close();
        return false;
    }
    catch (json::exception& e) {
        std::cerr << "Error: Missing parameter in JSON file: " << filename << "\n" << e.what() << std::endl;
        f.close();
        return false;
    }
    catch (std::exception& e) {
        std::cerr << "An unexpected error occurred: " << e.what() << std::endl;
        f.close();
        return false;
    }
}

/**
 * @brief Gets the current working directory.
 * @return A string with the path to the current working directory.
 */
std::string getCurrentWorkingDirectory() 
{
    char buff[FILENAME_MAX];
    if (GetCurrentDir(buff, FILENAME_MAX)) 
    {
        std::string current_working_dir(buff);
        return current_working_dir;
    }
    //return a fallback message if CWD can't be found
    return "[current application directory]";
}

/**
 * @brief Gets the current date and time as a string for filenames.
 * @return A string formatted as YYYYMMDD_HHMMSS.
 */
std::string getCurrentTimestamp() 
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    std::tm buf;

#ifdef _WIN32
    localtime_s(&buf, &in_time_t); //windows
#else
    localtime_r(&in_time_t, &buf); //POSIX
#endif

    ss << std::put_time(&buf, "%Y%m%d_%H%M%S");
    return ss.str();
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


        //data logging
        std::string timestamp = getCurrentTimestamp();
        std::string filename = "simple_model_simulation_" + timestamp + ".csv";
        CSVManager logger;
        logger.open("simple_model_simulation.csv");
        logger.writeHeader("Year,FishStock_tons"); 
        
        //test init
        myFishery.setSimpleCarryingCapacity(12000.0); //12k tons
        myFishery.setSimpleReproductionRate(1.0);
        myFishery.setFishStock(10000.0); //10k tons
        myFishingIndustry.setSimpleHarvestRate(2000.0); //2k tons

        std::cout << "--- Simple Logistic Model Simulation ---" << std::endl;
        printf("Year | Fish Stock (tons)\n");
        printf("--------------------------------------\n");
        printf("%4d | %f\n", 0, myFishery.getFishStock());
        logger.writeRow(0, myFishery.getFishStock()); //log initial state

        //run the simulation loop
        for (int year = 1; year <= simulationYears; ++year) {
            double growth = SimpleModelGrowthAmount(myFishery, myFishingIndustry);
            myFishery.setFishStock(std::max(0.0, myFishery.getFishStock() + growth));
            printf("%4d | %f\n", year, myFishery.getFishStock());
            logger.writeRow(year, myFishery.getFishStock()); //log step
        }
        logger.close();
        std::cout << "\nSimulation results saved to:\n" << getCurrentWorkingDirectory() << "/" << filename << std::endl;
    }
    else if (choice == 2)
    {
        // --- Delay Equation Model Simulation ---
        Fishery myFishery = Fishery();
        FishingIndustry myFishingIndustry = FishingIndustry();
        int simulationYears = 20;
        int stepsPerYear = 100; //using sub-year steps
        double timeStep = 1.0 / stepsPerYear;
        double currentTime = 0.0;

        //data logging
        std::string timestamp = getCurrentTimestamp();
        std::string filename = "delay_model_simulation_" + timestamp + ".csv"; // Store filename
        CSVManager logger;
        logger.open("delay_model_simulation.csv");
        logger.writeHeader("Time_Year,Population_n,Effort_E,MarketStock_S");

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
        logger.writeRow(currentTime, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock()); // Log initial state

        //run the simulation loop
        for (int year = 1; year <= simulationYears; ++year) {
            for (int i = 0; i < stepsPerYear; ++i) {
                DelayEquationModelStep(myFishery, myFishingIndustry, timeStep);
                currentTime += timeStep;
                logger.writeRow(currentTime, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock());
            }
            printf("%4d | %14.4f | %10.4f | %16.4f\n", year, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock());
        }
        logger.close();
        std::cout << "\nSimulation results saved to:\n" << getCurrentWorkingDirectory() << "/" << filename << std::endl;
    }

    std::cout << "\nSimulation finished. Press Enter to exit." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); //press any key

    //exit the program
    return 0;
}