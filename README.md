# FisherySimulation

# Project Status
Command-line Interface - Fully Implemented
Core Simulation Loop - Fully Implemented
Simple Logistic Model - Fully Implemented
Delay Equation Model - Fully Implemented
Age-structured operating model - Fully Implemented
JSON Parameter Parse Functionality - Fully Implemented
CSV Data logging - Fully Implemented

MAJOR TODO:
JABBA-based Validation Testing
Simulation Validation and Tuning

More details on the Trello board.
- https://trello.com/b/SfdKqYDU/fishery-simulator-management-board

# Installation Instructions
To build and run this repository, simply clone it into a folder then use the .sln file to create a Visual Studio project. 
- You can drag-and-drop the .sln file into a Visual Studio window, and it will automatically prompt you to set up the project.
Once this is done, you will be able to compile and run the project from source using the visual studio build/debug buttons.

Alternatively, you can compile the raw source using any C++ compiler if you are proficient at that.

- To be added: A complete build and install consisting of a command-line executable and portable data folder

# Usage
To use this simulator, simply follow the command-line prompts.
To edit run parameters, edit the values inside parameters.json.

# Architecture Overview
Main data classes: Fishery.h and FishingIndustry.h
- These data classes contain the parameters for the simulation, such as the fish stock, harvesting effort, reproduction rate, etc.

Core program loop: FisherySimulation.cpp
- This file contains the main() function that governs the command-line input and output.
- This file also contains the simulation algorithms, implemented in the form of growth value functions.

Simulation algorithms: FisherySimulation.cpp
- Three algorithms are implemented as
	1. A simple model using logistic growth
	2. A model using infinite delay equations
	3. An age-structured operating model
- The JABBA framework repo can be found here: https://github.com/jabbamodel/JABBA

Auxilliary class: CSVManager.h
- Helper class to handle CSV data logging.

json.h
- Slightly modified version of the nlohmann all-in-one header JSON library.
