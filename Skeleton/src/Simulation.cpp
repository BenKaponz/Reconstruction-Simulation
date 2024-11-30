#include "Simulation.h"
#include "SelectionPolicy.h"
#include "Auxiliary.h"
#include "Action.h"
#include <algorithm>
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>

using namespace std;

Simulation::Simulation(const string &configFilePath)
    : isRunning(false),
      planCounter(0),
      actionsLog(),
      plans(),
      settlements(),
      facilitiesOptions() 
{
    cout << "Opening config file: " << configFilePath << endl;

    ifstream configFile(configFilePath);
    if (!configFile.is_open()) {
        throw runtime_error("Unable to open config file: " + configFilePath);
    }

    string line;
    while (getline(configFile, line)) {
        vector<string> args = Auxiliary::parseArguments(line);
        if (args.empty() || args[0][0] == '#') {
            continue;  // Skip empty or comment lines
        }

        if (args[0] == "settlement") {
            if (args.size() != 3) {
                cerr << "Invalid settlement configuration: " << line << endl;
                continue;
            }

            string name = args[1];
            int typeInt = stoi(args[2]);
            SettlementType type = static_cast<SettlementType>(typeInt);
            settlements.push_back(new Settlement(name, type));

        } else if (args[0] == "facility") {
            if (args.size() != 7) {
                cerr << "Invalid facility configuration: " << line << endl;
                continue;
            }

            string name = args[1];
            FacilityCategory category = static_cast<FacilityCategory>(stoi(args[2]));
            int price = stoi(args[3]);
            int lifeQuality = stoi(args[4]);
            int economy = stoi(args[5]);
            int environment = stoi(args[6]);

            facilitiesOptions.emplace_back(name, category, price, lifeQuality, economy, environment);

        } else if (args[0] == "plan") {
            if (args.size() != 3) {
                cerr << "Invalid plan configuration: " << line << endl;
                continue;
            }

            string settlementName = args[1];
            string policyStr = args[2];
            SelectionPolicy *policy = nullptr;

            if (policyStr == "eco") {
                policy = new EconomySelection();
            } else if (policyStr == "bal") {
                policy = new BalancedSelection(0, 0, 0);
            } else if (policyStr == "sus") {
                policy = new SustainabilitySelection();
            } else if (policyStr == "nai") {
                policy = new NaiveSelection();
            } else {
                cerr << "Invalid selection policy: " << policyStr << endl;
                continue;
            }

            auto it = find_if(settlements.begin(), settlements.end(),
                              [&settlementName](Settlement *s) { return s->getName() == settlementName; });

            if (it == settlements.end()) {
                cerr << "Settlement not found: " << settlementName << endl;
                delete policy;
                continue;
            }

            Settlement *settlement = *it;
            plans.emplace_back(planCounter++, *settlement, policy, facilitiesOptions);
        } else {
            cerr << "Unknown configuration command: " << args[0] << endl;
        }
    }

    configFile.close();
    cout << "Finished processing config file." << endl;
}

// Rule of Three - Copy Constructor
Simulation::Simulation(const Simulation &other) 

    : isRunning(other.isRunning), 
      planCounter(other.planCounter) {
    // Deep copy facilitiesOptions
    for (const auto &facility : other.facilitiesOptions) {
        facilitiesOptions.emplace_back(facility);  // Use FacilityType's constructor
    }

    // Deep copy settlements
    for (const auto *settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }

    // Deep copy plans
    for (const auto &plan : other.plans) {
        plans.emplace_back(plan);  // Use Plan's copy constructor
    }

    // Deep copy actionsLog
    for (const auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone());  // Use BaseAction's clone method
    }
}
// Rule of Three - Copy Assignment Operator
Simulation &Simulation::operator=(const Simulation &other) {
    if (this == &other) {
        return *this;  // Handle self-assignment
    }

    // Clean up existing resources
    for (auto *settlement : settlements) {
        delete settlement;
    }
    settlements.clear();

    for (auto *action : actionsLog) {
        delete action;
    }
    actionsLog.clear();

    plans.clear();

    // Copy data from other
    isRunning = other.isRunning;
    planCounter = other.planCounter;

    // Deep copy facilitiesOptions
    for (const auto &facility : other.facilitiesOptions) {
        facilitiesOptions.emplace_back(facility);  // Use FacilityType's constructor
    }

    // Deep copy settlements
    for (const auto *settlement : other.settlements) {
        settlements.push_back(new Settlement(*settlement));
    }

    // Deep copy plans
    for (const auto &plan : other.plans) {
        plans.emplace_back(plan);  // Use Plan's copy constructor
    }

    // Deep copy actionsLog
    for (const auto *action : other.actionsLog) {
        actionsLog.push_back(action->clone());  // Use BaseAction's clone method
    }

    return *this;
}
// Rule of Three - Destructor
Simulation::~Simulation() {
    for (auto settlement : settlements) {
        delete settlement;
    }
    for (auto action : actionsLog) {
        delete action;
    }
}

void Simulation::start() {
    if (isRunning) {
        throw runtime_error("Simulation is already running!");
    }
    isRunning = true;
    cout << "Simulation started." << endl;
}

void Simulation::addPlan(const Settlement *settlement, SelectionPolicy *selectionPolicy) {
    if (!settlement || !selectionPolicy) {
        cerr << "Invalid settlement or selection policy in addPlan!" << endl;
        return;
    }
    plans.emplace_back(planCounter++, *settlement, selectionPolicy, facilitiesOptions);
}

bool Simulation::addSettlement(Settlement *settlement) {
    if (any_of(settlements.begin(), settlements.end(),
               [&settlement](Settlement *s) { return s->getName() == settlement->getName(); })) {
        return false;
    }
    settlements.push_back(settlement);
    return true;
}

bool Simulation::addFacility(FacilityType facility) {
    facilitiesOptions.push_back(facility);
    return true;
}

Settlement *Simulation::getSettlement(const std::string &settlementName) {
    for (auto *settlement : settlements) {
        if (settlement->getName() == settlementName) {
            return settlement;
        }
    }
    throw std::runtime_error("Settlement not found: " + settlementName);
}

Plan &Simulation::getPlan(const int planID) {
    auto it = find_if(plans.begin(), plans.end(), [planID](const Plan &p) {
        return p.getPlanId() == planID;
    });
    if (it == plans.end()) {
        throw runtime_error("Plan ID not found: " + to_string(planID));
    }
    return *it;
}

const std::vector<BaseAction*>& Simulation::getActionsLog() const {
    return actionsLog;
}

void Simulation::step() {
    if (!isRunning) {
        throw runtime_error("Simulation is not running!");
    }
    for (auto &plan : plans) {
        plan.step();
    }
}

void Simulation::close() {
    if (!isRunning) {
        throw runtime_error("Simulation is not running!");
    }
    isRunning = false;
    cout << "Simulation closed." << endl;
}

void Simulation::open() {
    if (isRunning) {
        throw runtime_error("Simulation is already open!");
    }
    isRunning = true;
    cout << "Simulation opened." << endl;
}