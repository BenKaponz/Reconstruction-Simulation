#include "Simulation.h"
#include "Action.h"

// Rule of 5 used here - Class contains resources.

// Constructor: Initialize the simulation using a configuration file
Simulation::Simulation(const string &configFilePath) : isRunning(false), planCounter(0),actionsLog(), plans(), settlements(),
    facilitiesOptions() {

    // Open the configuration file for reading
    ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw runtime_error("Unable to open configuration file");
    }

    string line;
    // Parse each line in the configuration file
    while (getline(configFile, line)) {
        vector<string> args = Auxiliary::parseArguments(line);

        // Skip empty or commented lines
        if (args.empty() || args[0][0] == '#') {
            continue; 
        }

        // Handle "settlement" configuration
        if (args[0] == "settlement") {
            if (args.size() != 3) throw runtime_error("Invalid settlement configuration");
            if(!isSettlementExists(args[1])){
                SettlementType type = static_cast<SettlementType>(stoi(args[2]));
                addSettlement(new Settlement(args[1], type));
            }
        // Handle "facility" configuration
        } else if (args[0] == "facility") {
            if (args.size() != 7) throw runtime_error("Invalid facility configuration");
            if (!isFacilityExists(args[1])){
                FacilityCategory category = static_cast<FacilityCategory>(stoi(args[2]));
                FacilityType facility(args[1], category, stoi(args[3]), stoi(args[4]), stoi(args[5]), stoi(args[6]));
                addFacility(facility);
            }
        // Handle "plan" configuration
        } else if (args[0] == "plan") {
            if (args.size() != 3) throw runtime_error("Invalid plan configuration");
            if (!isSettlementExists(args[1])) throw runtime_error("Settlement not found for plan");
            const Settlement &settlement = getSettlement(args[1]);
            SelectionPolicy *policy = nullptr;

            // Determine the selection policy
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

// Copy Constructor
Simulation::Simulation(const Simulation &other)
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions(other.facilitiesOptions) {

    // Copy settlements
    for (const auto *settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement)); 
    }

    // Copy plans and match them with the new settlements
    for (const auto &plan : other.plans) {
        const Settlement &oldSettlement = plan.getSettlement();
        Settlement *newSettlement = nullptr;
        
        // Find the corresponding settlement in the new list
        for (auto *settlement : settlements) {
            if (settlement->getName() == oldSettlement.getName()) {
                newSettlement = settlement; // Found the corresponding settlement
                break;
            }
        }

        // Create a copy of the plan
        plans.emplace_back(plan.getPlanId(), 
                           *newSettlement, 
                           plan.getSelectionPolicy()->clone(), 
                           facilitiesOptions);
    }

    // Copy the action log
    for (const auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone()); 
    }
}

// Assignment Operator
Simulation &Simulation::operator=(const Simulation &other) {
    if (this == &other) return *this; // Handle self-assignment

    // Clean up current state
    for (auto *settlement : settlements) {
        delete settlement;
        settlement = nullptr;
    }
    settlements.clear();

    for (auto *action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    plans.clear();
    facilitiesOptions.clear();

    // Copy other fields
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    // Deep copy of settlements
    for (Settlement* settlement : other.settlements) {
        settlements.push_back(new Settlement(settlement->getName(), settlement->getType()));
    }

    // Deep copy of facilitiesOptions
    facilitiesOptions = vector<FacilityType>(other.facilitiesOptions);
    
     // Deep copy plans
    for (const auto& plan : other.plans) {
        plans.emplace_back(plan); // Calls Plan's copy constructor
    }

    // Deep copy of actionsLog
    for (const auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone()); 
    }

    return *this;
}

// Move Constructor
Simulation::Simulation(Simulation &&other) noexcept
    : isRunning(other.isRunning),
      planCounter(other.planCounter),
      actionsLog(move(other.actionsLog)),
      plans(move(other.plans)),
      settlements(move(other.settlements)),
      facilitiesOptions(move(other.facilitiesOptions)) {
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
    actionsLog = move(other.actionsLog);
    plans = move(other.plans);
    settlements = move(other.settlements);
    facilitiesOptions = move(other.facilitiesOptions);

    // Reset the moved-from object
    other.isRunning = false;
    other.planCounter = 0;

    return *this;
}

// Destructor 
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


// Start the simulation loop
void Simulation::start() {
    open(); // Indicates that the simulation is running

    while (isRunning) { // As long as we didn't command 'close'
        string line;
        cout << "Enter an action: ";
        getline(cin, line); // Read the entire line of input from the user

        vector<string> args = Auxiliary::parseArguments(line);
        if (args.empty()) continue; // Skip empty input

        BaseAction *action = nullptr;

        // Match the first argument (command) with its corresponding action
        try {
            if (args[0] == "settlement") {
                if (args.size() != 3) throw runtime_error("Invalid settlement command");
                action = new AddSettlement(args[1], static_cast<SettlementType>(stoi(args[2])));
            } 
            else if (args[0] == "facility") {
                if (args.size() != 7) throw runtime_error("Invalid facility command");
                action = new AddFacility(args[1], static_cast<FacilityCategory>(stoi(args[2])), stoi(args[3]),
                                         stoi(args[4]), stoi(args[5]), stoi(args[6]));
            } 
            else if (args[0] == "plan") {
                if (args.size() != 3) throw runtime_error("Invalid plan command");
                action = new AddPlan(args[1], args[2]);
            } 
            else if (args[0] == "step") {
                if (args.size() != 2) throw runtime_error("Invalid step command");
                action = new SimulateStep(stoi(args[1]));
            } 
            else if (args[0] == "planStatus") {
                if (args.size() != 2) throw runtime_error("Invalid planStatus command");
                action = new PrintPlanStatus(stoi(args[1]));
            } 
            else if (args[0] == "changePolicy") {
                if (args.size() != 3) throw runtime_error("Invalid changePolicy command");
                action = new ChangePlanPolicy(stoi(args[1]), args[2]);
            } 
            else if (args[0] == "log") {
                action = new PrintActionsLog();
            } 
            else if (args[0] == "close") {
                action = new Close();
            } 
            else if (args[0] == "backup") {
                action = new BackupSimulation();
            } 
            else if (args[0] == "restore") {
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

// Add a plan to the simulation
void Simulation::addPlan(const Settlement &settlement, SelectionPolicy *selectionPolicy) {
    plans.emplace_back(planCounter++, settlement, selectionPolicy, facilitiesOptions);
}

// Add a new action to the log
void Simulation::addAction(BaseAction *action) {
    actionsLog.push_back(action);
}

// Add a settlement to the simulation
bool Simulation::addSettlement(Settlement *settlement) {
    settlements.push_back(settlement);
    return true;
}

// Add a facility type to the simulations
bool Simulation::addFacility(FacilityType facility) {
    facilitiesOptions.push_back(facility);
    return true;
}

// Check if a settlement exists in the simulation
bool Simulation::isSettlementExists(const string &settlementName) {
    for (size_t i = 0; i < settlements.size(); i++) {
        if (settlements[i]->getName() == settlementName) {
            return true; 
        }
    }
    return false; 
}

// Check if a type of facility exists in the simulation
bool Simulation::isFacilityExists(const string &facilityName) {
    for (size_t i = 0; i < facilitiesOptions.size(); ++i) {
        if (facilitiesOptions[i].getName() == facilityName) {
            return true; 
        }
    }
    return false; 
}

// Check if a plan exists in the simulation
bool Simulation::isPlanExists(const int planId) {
    for (size_t i = 0; i < plans.size(); ++i) {
        if (plans[i].getPlanId() == planId) {
            return true; 
        }
    }
    return false; 
}

// Get a settlement by name
Settlement &Simulation::getSettlement(const string &settlementName) {
    for (auto *s : settlements) {
        if (s->getName() == settlementName) {
            return *s;
        }
    }
    throw runtime_error("Settlement not found");
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

// Get the action log (read-only).
const std::vector<BaseAction*>& Simulation::getActionsLog() const {
    return actionsLog;
}

// Perform one simulation step by advancing all plans.
void Simulation::step() {
    for (auto &plan : plans) {
        plan.step();
    }
}

// Print results of all plans and stop the simulation.
void Simulation::close() {
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

// Start the simulation.
void Simulation::open() {
    isRunning = true;
    cout << "The simulation has started" << endl;
}

    