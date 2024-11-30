#include "Simulation.h"
#include "Auxiliary.h"
#include "Action.h"
#include <algorithm>
#include <fstream>
#include <iostream>
#include <sstream>
#include <unordered_map>

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

Simulation::Simulation(const Simulation &other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions(other.facilitiesOptions) {

    // העתקת היישובים
    for (size_t i = 0; i < other.settlements.size(); ++i) {
        settlements.push_back(new Settlement(*other.settlements[i])); // יצירת עותק של כל יישוב
    }

    // העתקת תוכניות ושמירת הקשר ליישובים החדשים
    for (size_t i = 0; i < other.plans.size(); ++i) {
        // מציאת היישוב המתאים ברשימה החדשה לפי אינדקס
        // מציאת האינדקס של היישוב הישן
        const Settlement *oldSettlement = &other.plans[i].getSettlement();
        size_t settlementIndex = 0;
        for (size_t j = 0; j < other.settlements.size(); ++j) {
            if (other.settlements[j] == oldSettlement) {
                settlementIndex = j;
                break;
            }

        }

        // שימוש ביישוב החדש עם אותו אינדקס
        Settlement &newSettlement = *settlements[settlementIndex];
        
        // יצירת עותק של התוכנית
        plans.emplace_back(other.plans[i].getPlanId(), 
                           newSettlement, 
                           other.plans[i].getSelectionPolicy()->clone(), 
                           facilitiesOptions);
    }

    // העתקת לוג הפעולות
    for (size_t i = 0; i < other.actionsLog.size(); ++i) {
        actionsLog.push_back(other.actionsLog[i]->clone()); // יצירת עותק
    }
}


//Assignment Operator=
Simulation &Simulation::operator=(const Simulation &other) {
    if (this == &other) return *this; // Handle self-assignment

    // Clean up current state
    for (auto *settlement : settlements) {
        delete settlement;
        settlement = nullptr;
    }
    settlements.clear();

    // Deep copy of settlements
    for (Settlement* settlement : other.settlements) {
        settlements.push_back(new Settlement(settlement->getName(), settlement->getType()));
    }

    for (auto *action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    plans.clear();
    facilitiesOptions.clear();

    // Copy new state from 'other'
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    // Deep copy of facilitiesOptions
    facilitiesOptions = vector<FacilityType>(other.facilitiesOptions);
    
     // Deep copy plans
    for (const auto& plan : other.plans) {
        plans.emplace_back(plan); // Assumes Plan has a proper copy constructor
    }

    // Deep copy of actionsLog
    for (const auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone()); // Ensure clone() is implemented correctly
    }

    return *this;
}
// Move Constructor
Simulation::Simulation(Simulation &&other) noexcept
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(std::move(other.actionsLog)),
      plans(std::move(other.plans)),
      settlements(std::move(other.settlements)),
      facilitiesOptions(std::move(other.facilitiesOptions)) {
    // Clear the state of the moved-from object
    other.isRunning = false;
    other.planCounter = 0;
}

// Move Assignment Operator
Simulation &Simulation::operator=(Simulation &&other) noexcept {
    if (this == &other) return *this; // Handle self-assignment

    // Clean up current state
    for (auto *settlement : this->settlements) {
        delete settlement;
    }
    this->settlements.clear();

    for (auto *action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    plans.clear();
    facilitiesOptions.clear();

    // Steal resources from the moved-from object
    isRunning = other.isRunning;
    planCounter = other.planCounter;
    actionsLog = std::move(other.actionsLog);
    plans = std::move(other.plans);
    settlements = std::move(other.settlements);
    facilitiesOptions = std::move(other.facilitiesOptions);

    // Reset the moved-from object
    other.isRunning = false;
    other.planCounter = 0;

    return *this;
}


// Distructor 
 Simulation::~Simulation() {
    for (auto* settlement : settlements) {
        delete settlement;
    }
    settlements.clear();


    for (auto* action : actionsLog) {
        delete action;
    }
    actionsLog.clear();


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
        cout << "PlanID: " << plan.getPlanId() << "\n";
        cout << "SettlementName: " << plan.getSettlement().getName() << "\n";
        cout << "LifeQuality_Score: " << plan.getlifeQualityScore() << "\n";
        cout << "Economy_Score: " << plan.getEconomyScore() << "\n";
        cout << "Environment_Score: " << plan.getEnvironmentScore() << "\n";
        cout << "----------------------------------------" << endl;
    }
    // Set simulation state to not running
    isRunning = false;

    // Indicate the simulation has ended
    cout << "Simulation closed successfully." << endl;
}

void Simulation::open() {
    isRunning = true;
    cout << "The simulation has started" << endl;
}

    