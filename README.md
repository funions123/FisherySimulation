# FisherySimulation

# Project Status
Command-line Interface - Mostly Implemented

Core Simulation Loop - Partially Implemented

Simple Logistic Model - Mostly Implemented

Delay Equation Model - Partially Implemented

JABBA-based Model - Not yet started

JSON Parameter Parse Functionality - Not yet started

Simulation Validation and Tuning - Not yet started

More details on the Trello board.
- https://trello.com/invite/b/68dd7eea945acf70ccdc51bd/ATTI725c6fb9c662f3a8811a6feeaec16950F0B5D976/fishery-simulator-management-board

# Installation Instructions
To build and run this repository, simply clone it into a folder then use the .sln file to create a Visual Studio project. 
- You can drag-and-drop the .sln file into a Visual Studio window, and it will automatically prompt you to set up the project.
Once this is done, you will be able to compile and run the project from source using the visual studio build/debug buttons.

- To be added: A complete build and install consisting of a command-line executable and portable data folder

# Usage
To use this simulator, simply follow the command-line prompts.

# Architecture Overview
Main data classes: Fishery.h and FishingIndustry.h
- These data classes contain the parameters for the simulation, such as the fish stock, harvesting effort, reproduction rate, etc.

Core program loop: FisherySimulation.cpp
- This file contains the main() function that governs the command-line input and output.
- This file also contains the simulation algorithms, implemented in the form of growth value functions.

Simulation algorithms: FisherySimulation.cpp
- Three algorithms are implemented as growth value functions
	1. A simple model using logistic growth
	2. A model using infinite delay equations
	3. A model using the JABBA framework
- The JABBA framework repo can be found here: https://github.com/jabbamodel/JABBA