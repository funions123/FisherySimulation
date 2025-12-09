
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
 * @param modelChoice 1 for Simple Model, 2 for Delay Model, 3 for Age-Structured Model.
 * @param outSimYears (Output) The number of simulation years.
 * @param outStepsPerYear (Output) The number of steps per year (for delay model).
 * @return True if parameters were loaded successfully, false otherwise.
 */
bool loadParametersFromJSON(const json& params, Fishery& fishery, FishingIndustry& industry, int modelChoice, int& outSimYears, int& outStepsPerYear)
{
    try {
        if (modelChoice == 1) 
        {
            auto modelParams = params.at("simpleModel");
            outSimYears = modelParams.at("simulationYears").get<int>();
            fishery.setSimpleCarryingCapacity(modelParams.at("carryingCapacity").get<double>());
            fishery.setSimpleReproductionRate(modelParams.at("reproductionRate").get<double>());
            fishery.setFishStock(modelParams.at("initialFishStock").get<double>());
            industry.setSimpleHarvestRate(modelParams.at("harvestRate").get<double>());
            fishery.setReproductionStdDev(modelParams.at("reproductionStdDev").get<double>());
        }
        else if (modelChoice == 2) 
        {
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
            fishery.setCatchabilityStdDev(modelParams.at("catchabilityStdDev").get<double>());
        }
        else if (modelChoice == 3) 
        {
            auto modelParams = params.at("ageStructuredModel");
            outSimYears = modelParams.at("simulationYears").get<int>();
            int maxAge = modelParams.at("maxAge").get<int>();

            fishery.setAgeModelParams(
                maxAge,
                modelParams.at("naturalMortality").get<double>(),
                modelParams.at("vb_Linf").get<double>(),
                modelParams.at("vb_k").get<double>(),
                modelParams.at("vb_t0").get<double>(),
                modelParams.at("lw_a").get<double>(),
                modelParams.at("lw_b").get<double>(),
                modelParams.at("maturity_A50").get<double>(),
                modelParams.at("maturity_k").get<double>(),
                modelParams.at("constantRecruitment").get<double>()
            );

            fishery.setRecruitmentStdDev(modelParams.at("recruitmentStdDev").get<double>());

            industry.setAgeModelParams(
                modelParams.at("fishingMortality").get<double>(),
                modelParams.at("selectivity_A50").get<double>(),
                modelParams.at("selectivity_k").get<double>()
            );

            std::vector<double> initialNumbers = modelParams.at("initialNumbers").get<std::vector<double>>();
            if (initialNumbers.size() != maxAge + 1)
            {
                std::cout << "Error: 'initialNumbers' array size in JSON (" << initialNumbers.size()
                    << ") does not match 'maxAge' + 1 (" << (maxAge + 1) << ")." << std::endl;
                return false;
            }
            fishery.setInitialNumbers(initialNumbers);
        }
        return true;
    }
    catch (json::parse_error& e) 
    {
        std::cout << "Error: Failed to parse JSON file:\n" << e.what() << std::endl;
        return false;
    }
    catch (json::exception& e) 
    {
        std::cout << "Error: Missing parameter in JSON file:\n" << e.what() << std::endl;
        return false;
    }
    catch (std::exception& e) 
    {
        std::cout << "An unexpected error occurred: " << e.what() << std::endl;
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

/**
 * @brief Gets the current date and time as a human-readable string for logging.
 * @return A string formatted as YYYY-MM-DD HH:MM:SS
 */
std::string getReadableTimestamp() 
{
    auto now = std::chrono::system_clock::now();
    auto in_time_t = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    std::tm buf;

#ifdef _WIN32
    localtime_s(&buf, &in_time_t); //windows
#else
    localtime_r(&in_time_t, &buf); //posix
#endif

    ss << std::put_time(&buf, "%Y-%m-%d %H:%M:%S");
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
    double noise = fishery.getNoisyMultiplier(fishery.getReproductionStdDev());

    //apply noise to reproduction rate
    double noisyRate = fishery.getSimpleReproductionRate() * noise;

    //calculate the natural growth of the fish stock
    double naturalGrowth = noisyRate * fishery.getFishStock() * (1 - fishery.getFishStock() / fishery.getSimpleCarryingCapacity());

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
    double noise = fishery.getNoisyMultiplier(fishery.getCatchabilityStdDev());

    double n = fishery.getFishStock();
    double E = fishingindustry.getHarvestingEffort();
    double S = fishingindustry.getFishMarketStock();

    //step catch - equation 1
    double currentCatch = (fishery.getCatchability() * noise) * n * E;

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

/**
 * @brief Simulates one year step of the Age-Structured Model.
 * @param fishery The fishery object (contains state and bio params).
 * @param industry The industry object (contains fishing params).
 * @return The total catch in biomass for the year.
 */
double AgeStructuredModelStep(Fishery& fishery, const FishingIndustry& industry) 
{
    int maxAge = fishery.getMaxAge();
    std::vector<double> N_start = fishery.getNumbersAtAge(); //numbers at start of year
    std::vector<double> N_end(maxAge + 1); //numbers at end of year
    double totalCatchBiomass = 0.0;
    double M = fishery.getNaturalMortality();
    double F_max = industry.getFishingMortality();

    // Loop from age 1 to maxAge - 1
    for (int age = 1; age < maxAge; ++age) 
    {
        double sel = industry.getSelectivityAtAge(age - 1); //selectivity of the cohort from the previous age
        double F = F_max * sel;
        double Z = M + F; //total mortality
        N_end[age] = N_start[age - 1] * std::exp(-Z);

        //baranov catch equation (biomass)
        totalCatchBiomass += (F / Z) * (1.0 - std::exp(-Z)) * N_start[age - 1] * fishery.getWeightAtAge(age - 1);
    }

    //handle the plus group (age maxAge)
    double sel_recruit = industry.getSelectivityAtAge(maxAge - 1);
    double F_recruit = F_max * sel_recruit;
    double Z_recruit = M + F_recruit;
    double recruits_to_plus_group = N_start[maxAge - 1] * std::exp(-Z_recruit);

    double sel_plus = industry.getSelectivityAtAge(maxAge);
    double F_plus = F_max * sel_plus;
    double Z_plus = M + F_plus;
    double survivors_from_plus_group = N_start[maxAge] * std::exp(-Z_plus);

    //total fish in maxAge
    N_end[maxAge] = recruits_to_plus_group + survivors_from_plus_group;

    totalCatchBiomass += (F_recruit / Z_recruit) * (1.0 - std::exp(-Z_recruit)) * N_start[maxAge - 1] * fishery.getWeightAtAge(maxAge - 1);
    totalCatchBiomass += (F_plus / Z_plus) * (1.0 - std::exp(-Z_plus)) * N_start[maxAge] * fishery.getWeightAtAge(maxAge);

    //fish reproduction (new log-normal noisy recruitment)
    N_end[0] = fishery.getNoisyRecruitment();
    fishery.setNumbersAtAge(N_end);

    return totalCatchBiomass;
}

int main()
{
    int choice = 0;
    const std::string paramFilename = "parameters.json";
    json params;

    std::ifstream f(paramFilename);
    if (!f.is_open()) 
    {
        std::cout << "Error: Could not open parameter file: " << paramFilename << std::endl;
        std::cout << "Please ensure 'parameters.json' exists in the same directory." << std::endl;
        return 1;
    }

    try 
    {
        params = json::parse(f);
        f.close();
    }
    catch (json::parse_error& e) 
    {
        std::cout << "Error: Failed to parse JSON file: " << paramFilename << "\n" << e.what() << std::endl;
        f.close();
        return 1;
    }
    catch (json::exception& e)
    {
        std::cout << "Error: Failed to parse JSON file: " << paramFilename << "\n" << e.what() << std::endl;
        f.close();
        return 1;
    }

    while (choice != 1 && choice != 2 && choice != 3)
    {
        //ask the user which model to use
        std::cout << "Select a fishery simulation model:" << std::endl;
        std::cout << "1. Simple Logistic Model" << std::endl;
        std::cout << "2. Delay Equation Model" << std::endl;
        std::cout << "3. Age-Structured Model" << std::endl;
        std::cout << "Enter your choice (1, 2, or 3): ";
        std::cin >> choice;

        if (std::cin.fail() || (choice != 1 && choice != 2 && choice != 3))
        {
            std::cout << "\nInvalid choice. Please enter 1, 2, or 3.\n" << std::endl;
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
        int stepsPerYear = 0;

        if (!loadParametersFromJSON(params, myFishery, myFishingIndustry, 1, simulationYears, stepsPerYear)) 
        {
            std::cout << "Error loading simple model parameters. Exiting." << std::endl;
            return 1;
        }

        //data logging
        std::string timestamp = getCurrentTimestamp();
        std::string filename = "simple_model_simulation_" + timestamp + ".csv";
        CSVManager logger;
        logger.open(filename);

        logger.writeComment("Simulation Log");
        logger.writeComment("Model: Simple Logistic Model");
        logger.writeComment("Timestamp: " + getReadableTimestamp());
        logger.writeComment("Parameters: ");
        std::stringstream ss;
        ss << params.at("simpleModel").dump(4);
        std::string line;
        while (std::getline(ss, line)) 
        {
            logger.writeComment("  " + line);
        }
        logger.writeComment(""); 

        logger.writeHeader("Year,FishStock_tons");

        auto start = std::chrono::high_resolution_clock::now();

        std::cout << "--- Simple Logistic Model Simulation ---" << std::endl;
        printf("Year | Fish Stock (tons)\n");
        printf("--------------------------------------\n");
        printf("%4d | %f\n", 0, myFishery.getFishStock());
        logger.writeRow(0, myFishery.getFishStock()); //log initial state

        //run the simulation loop
        for (int year = 1; year <= simulationYears; ++year) 
        {
            double growth = SimpleModelGrowthAmount(myFishery, myFishingIndustry);
            myFishery.setFishStock(std::max(0.0, myFishery.getFishStock() + growth));
            printf("%4d | %f\n", year, myFishery.getFishStock());
            logger.writeRow(year, myFishery.getFishStock()); //log step
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::string durationString = "Simulation duration (ms): " + std::to_string(duration.count());

        printf("%s\n", durationString.c_str());

        logger.writeComment("");
        logger.writeComment(durationString);

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

        double fishStockAccum = 0.0;

        if (!loadParametersFromJSON(params, myFishery, myFishingIndustry, 2, simulationYears, stepsPerYear)) 
        {
            std::cout << "Error loading delay model parameters. Exiting." << std::endl;
            return 1;
        }

        //data logging
        std::string timestamp = getCurrentTimestamp();
        std::string filename = "delay_model_simulation" + timestamp + ".csv"; // Store filename
        CSVManager logger;
        logger.open(filename);

        logger.writeComment("Simulation Log");
        logger.writeComment("Model: Delay Equation Model");
        logger.writeComment("Timestamp: " + getReadableTimestamp());
        logger.writeComment("Parameters: ");
        std::stringstream ss;
        ss << params.at("delayModel").dump(4); 
        std::string line;
        while (std::getline(ss, line)) 
        {
            logger.writeComment("  " + line);
        }
        logger.writeComment("");
        logger.writeHeader("Time_Year,Population_n,Effort_E,MarketStock_S");

        auto start = std::chrono::high_resolution_clock::now();

        std::cout << "--- Delay Equation Model Simulation ---" << std::endl;
        printf("Year | Population (n) | Effort (E) | Market Stock (S)\n");
        printf("----------------------------------------------------------\n");
        printf("%4d | %14.4f | %10.4f | %16.4f\n", 0, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock());
        logger.writeRow(currentTime, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock()); // Log initial state

        //run the simulation loop
        for (int year = 1; year <= simulationYears; ++year) 
        {
            for (int i = 0; i < stepsPerYear; ++i) 
            {
                DelayEquationModelStep(myFishery, myFishingIndustry, timeStep);
                currentTime += timeStep;
                logger.writeRow(currentTime, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock());
            }
            printf("%4d | %14.4f | %10.4f | %16.4f\n", year, myFishery.getFishStock(), myFishingIndustry.getHarvestingEffort(), myFishingIndustry.getFishMarketStock());
            fishStockAccum += myFishery.getFishStock();
        }

        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::string durationString = "Simulation duration (ms): " + std::to_string(duration.count());

        std::string averageFishStockString = "Average fish stock level: " + std::to_string(fishStockAccum / simulationYears);

        printf("%s\n", averageFishStockString.c_str());

        printf("%s\n", durationString.c_str());

        logger.writeComment("");
        logger.writeComment(averageFishStockString);
        logger.writeComment("");
        logger.writeComment(durationString);

        logger.close();
        std::cout << "\nSimulation results saved to:\n" << getCurrentWorkingDirectory() << "/" << filename << std::endl;
    }
    else if (choice == 3)
    {
        // --- Age-Structured Model Simulation ---
        Fishery myFishery = Fishery();
        FishingIndustry myFishingIndustry = FishingIndustry();
        int simulationYears = 0;
        int stepsPerYear = 0;

        if (!loadParametersFromJSON(params, myFishery, myFishingIndustry, 3, simulationYears, stepsPerYear)) 
        {
            std::cout << "Error loading age-structured model parameters. Exiting." << std::endl;
            return 1;
        }

        std::string timestamp = getCurrentTimestamp();
        std::string filename = "age_structured_simulation" + timestamp + ".csv";
        CSVManager logger;
        logger.open(filename);

        logger.writeComment("Simulation Log");
        logger.writeComment("Model: Age-Structured Model");
        logger.writeComment("Timestamp: " + getReadableTimestamp());
        logger.writeComment("Parameters: ");
        std::stringstream ss;
        ss << params.at("ageStructuredModel").dump(4);
        std::string line;
        while (std::getline(ss, line)) 
        {
            logger.writeComment("  " + line);
        }
        logger.writeComment("");

        logger.writeHeader("Year,TotalBiomass,SpawningStockBiomass,TotalCatch");

        auto start = std::chrono::high_resolution_clock::now();

        std::cout << "--- Age-Structured Model Simulation ---" << std::endl;
        printf("Year | Total Biomass | Spawning Biomass | Total Catch (Biomass)\n");
        printf("----------------------------------------------------------------------\n");

        double initialTotalBiomass = myFishery.getTotalBiomass();
        double initialSSB = myFishery.getSpawningStockBiomass();
        printf("%4d | %15.2f | %18.2f | %20.2f\n", 0, initialTotalBiomass, initialSSB, 0.0);
        logger.writeRow(0, initialTotalBiomass, initialSSB, 0.0);

        for (int year = 1; year <= simulationYears; ++year) 
        {
            double totalCatch = AgeStructuredModelStep(myFishery, myFishingIndustry);

            double totalBiomass = myFishery.getTotalBiomass();
            double ssb = myFishery.getSpawningStockBiomass();

            printf("%4d | %15.2f | %18.2f | %20.2f\n", year, totalBiomass, ssb, totalCatch);
            logger.writeRow(year, totalBiomass, ssb, totalCatch);
        }
        auto end = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end - start;
        std::string durationString = "Simulation duration (ms): " + std::to_string(duration.count());

        printf("%s\n", durationString.c_str());

        logger.writeComment("");
        logger.writeComment(durationString);

        logger.close();

        std::cout << "\nSimulation results saved to:\n" << getCurrentWorkingDirectory() << "/" << filename << std::endl;
    }

    std::cout << "\nSimulation finished. Press Enter to exit." << std::endl;
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    std::cin.get(); //press any key

    //exit the program
    return 0;
}