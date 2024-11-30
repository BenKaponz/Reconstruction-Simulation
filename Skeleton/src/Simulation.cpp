#include "Simulation.h"
#include "Auxiliary.h"
#include "Action.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>

using namespace std;

// Constructor: Initialize the simulation using a configuration file
Simulation::Simulation(const string &configFilePath) : isRunning(false), planCounter(0),actionsLog(), plans(), settlements(),
    facilitiesOptions() {
    ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw runtime_error("Unable to open configuration file");
    }

    string line;
    while (getline(configFile, line)) {
        vector<string> args = Auxiliary::parseArguments(line);

        if (args.empty() || args[0][0] == '#') {
            continue;  // Skip empty or comment lines
        }

        if (args[0] == "settlement") {
            if (args.size() != 3) throw runtime_error("Invalid settlement configuration");
            SettlementType type = static_cast<SettlementType>(stoi(args[2]));
            addSettlement(new Settlement(args[1], type));

        } else if (args[0] == "facility") {
            if (args.size() != 7) throw runtime_error("Invalid facility configuration");
            FacilityCategory category = static_cast<FacilityCategory>(stoi(args[2]));
            FacilityType facility(args[1], category, stoi(args[3]), stoi(args[4]), stoi(args[5]), stoi(args[6]));
            addFacility(facility);

        } else if (args[0] == "plan") {
            if (args.size() != 3) throw runtime_error("Invalid plan configuration");
            if (!isSettlementExists(args[1])) throw runtime_error("Settlement not found for plan");
            const Settlement *settlement = getSettlement(args[1]);
            SelectionPolicy *policy = nullptr;
            if (args[2] == "nve") policy = new NaiveSelection();
            else if (args[2] == "bal") policy = new BalancedSelection(0,0,0);
            else if (args[2] == "eco") policy = new EconomySelection();
            else if (args[2] == "env") policy = new SustainabilitySelection();
            else throw runtime_error("Unknown selection policy");
            addPlan(settlement, policy);
        }
    }

    configFile.close();
}

//Copy constuctor
Simulation::Simulation(const Simulation &other)
    : isRunning(other.isRunning), planCounter(other.planCounter), actionsLog(), plans(), settlements(),
    facilitiesOptions() {
    // Deep copy facilitiesOptions
    facilitiesOptions = vector<FacilityType>(other.facilitiesOptions);

    // Deep copy settlements
    for (const auto *settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }

    // Deep copy plans
    for (const auto &plan : other.plans) {
        plans.emplace_back(plan); // Plan צריך לתמוך ב-Copy Constructor
    }

    // Deep copy actionsLog
    for (const auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone()); // Assume BaseAction::clone() is implemented
    }
}

// Copy assignment operator
Simulation &Simulation::operator=(const Simulation &other) {
    if (this != &other) { // בדיקה אם זה לא אותו אובייקט (Self-assignment)
        // שחרר את המשאבים הנוכחיים
        for (auto *settlement : settlements) {
            delete settlement;
        }
        settlements.clear();

        for (auto *action : actionsLog) {
            delete action;
        }
        actionsLog.clear();

        plans.clear();

        isRunning = other.isRunning;
        planCounter = other.planCounter;
        facilitiesOptions = vector<FacilityType>(other.facilitiesOptions);

        for (const auto *settlement : other.settlements) {
            settlements.push_back(new Settlement(*settlement)); 
        }

        for (const auto &plan : other.plans) {
            plans.emplace_back(plan); 
        }

        for (const auto *action : other.actionsLog) {
            actionsLog.push_back(action->clone()); 
        }
    }
    return *this; 
}

// Distructor 
 Simulation::~Simulation() {
    for (auto* settlement : settlements) {
        delete settlement;
    }

    for (auto* action : actionsLog) {
        delete action;
    }

}

void Simulation::start() {
    open(); // Indicates that the simulation is running

    while (isRunning) {
        string line;
        cout << "Enter an action: ";
        getline(cin, line);

        vector<string> tokens = Auxiliary::parseArguments(line);
        if (tokens.empty()) continue; // Skip empty input

        BaseAction *action = nullptr;

        try {
            if (tokens[0] == "settlement") {
                if (tokens.size() != 3) throw runtime_error("Invalid settlement command");
                action = new AddSettlement(tokens[1], static_cast<SettlementType>(stoi(tokens[2])));
            } 
            else if (tokens[0] == "facility") {
                if (tokens.size() != 7) throw runtime_error("Invalid facility command");
                action = new AddFacility(tokens[1], static_cast<FacilityCategory>(stoi(tokens[2])), stoi(tokens[3]),
                                         stoi(tokens[4]), stoi(tokens[5]), stoi(tokens[6]));
            } 
            else if (tokens[0] == "plan") {
                if (tokens.size() != 3) throw runtime_error("Invalid plan command");
                action = new AddPlan(tokens[1], tokens[2]);
            } 
            else if (tokens[0] == "step") {
                if (tokens.size() != 2) throw runtime_error("Invalid step command");
                action = new SimulateStep(stoi(tokens[1]));
            } 
            else if (tokens[0] == "planStatus") {
                if (tokens.size() != 2) throw runtime_error("Invalid planStatus command");
                action = new PrintPlanStatus(stoi(tokens[1]));
            } 
            else if (tokens[0] == "changePolicy") {
                if (tokens.size() != 3) throw runtime_error("Invalid changePolicy command");
                action = new ChangePlanPolicy(stoi(tokens[1]), tokens[2]);
            } 
            else if (tokens[0] == "log") {
                action = new PrintActionsLog();
            } 
            else if (tokens[0] == "close") {
                action = new Close();
            } 
            else if (tokens[0] == "backup") {
                action = new BackupSimulation();
            } 
            else if (tokens[0] == "restore") {
                action = new RestoreSimulation();
            } 
            else {
                throw runtime_error("Unknown command");
            }

            // If action was created, execute it and add it to the log
            if (action) {
                action->act(*this);
                addAction(action);
            }
        } 
        catch (const exception &e) {
            // Print error message
            if (action) delete action; // Clean up memory if an action was created but not added
            cout << "Error: " << e.what() << endl;
        }
    }
}

// Add a plan
void Simulation::addPlan(const Settlement *settlement, SelectionPolicy *selectionPolicy) {
    plans.emplace_back(planCounter++, *settlement, selectionPolicy, facilitiesOptions);
}

// Add a new action to the log
void Simulation::addAction(BaseAction *action) {
    actionsLog.push_back(action);
}

// Add a settlement
bool Simulation::addSettlement(Settlement *settlement) {
    if (isSettlementExists(settlement->getName())) {
        return false; // Settlement already exists
    }
    settlements.push_back(settlement);
    return true;
}

// Add a facility type
bool Simulation::addFacility(FacilityType facility) {
    for (const auto &existingFacility : facilitiesOptions) {
        if (existingFacility.getName() == facility.getName()) {
            return false; // Facility already exists
        }
    }
    facilitiesOptions.push_back(facility);
    return true;
}

// Check if a settlement exists
bool Simulation::isSettlementExists(const string &settlementName) {
    return any_of(settlements.begin(), settlements.end(),
                  [&](const Settlement *s) { return s->getName() == settlementName; });
}

// Check if a type of facility exists
bool Simulation::isFacilityExists(const string &facilityName) {
    return any_of(facilitiesOptions.begin(), facilitiesOptions.end(),
                       [&](const FacilityType &facility) { return facility.getName() == facilityName; });
}     

// Check if a plan exists
bool Simulation::isPlanExists(const int planId) {
    return std::any_of(plans.begin(), plans.end(),
                       [&](const Plan &plan) { return plan.getPlanId() == planId; });
}

// Get a settlement by name
Settlement *Simulation::getSettlement(const string &settlementName) {
    for (auto *s : settlements) {
        if (s->getName() == settlementName) {
            return s;
        }
    }
    return nullptr;
}

// Get a plan by ID
Plan &Simulation::getPlan(const int planID) {
    for (auto &plan : plans) {
        if (plan.getPlanId() == planID) {
            return plan;
        }
    }
    throw runtime_error("Plan not found");
}

const std::vector<BaseAction*>& Simulation::getActionsLog() const {
    return actionsLog;
}

// Execute a simulation step
void Simulation::step() {
    for (auto &plan : plans) {
        plan.step();
    }
}

void Simulation::close() {
    // Print all plans and their accumulated results
    for (const auto& plan : plans) {
        std::cout << "PlanID: " << plan.getPlanId() << "\n";
        std::cout << "SettlementName: " << plan.getSettlement().getName() << "\n";
        std::cout << "LifeQuality_Score: " << plan.getlifeQualityScore() << "\n";
        std::cout << "Economy_Score: " << plan.getEconomyScore() << "\n";
        std::cout << "Environment_Score: " << plan.getEnvironmentScore() << "\n";
        std::cout << "----------------------------------------" << std::endl;
    }
    // Set simulation state to not running
    isRunning = false;

    // Indicate the simulation has ended
    std::cout << "Simulation closed successfully." << std::endl;
}

void Simulation::open() {
    isRunning = true;
    cout << "The simulation has started" << endl;
}

    