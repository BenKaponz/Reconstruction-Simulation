#include "Simulation.h"
#include "Auxiliary.h"
#include "Settlement.h"
#include "Facility.h"
#include "SelectionPolicy.h"
#include "Plan.h"
#include "Action.h"
#include <iostream>
#include <sstream>
#include <stdexcept>

using namespace std;

extern Simulation *backup = nullptr; // Global variable

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************* BaseAction ******************************************************* //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BaseAction::BaseAction() : status(ActionStatus::ERROR), errorMsg("") {}

ActionStatus BaseAction::getStatus() const {
    return status;
}

void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
}

void BaseAction::error(string errorMsg) {
    this->errorMsg = move(errorMsg); // Store the error message -- *move חוסך את עלות ההעתקה במקום שכפול 
    cout << "Error: " << this->errorMsg << endl; // Print the error message
}

const string &BaseAction::getErrorMsg() const {
    return errorMsg;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************* SimulateStep ***************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {}

void SimulateStep::act(Simulation &simulation) {
    try {
        for (int i = 0; i < numOfSteps; i++) {
            simulation.step();
        }
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
}

const string SimulateStep::toString() const {
    ostringstream oss;
    oss << "step " << numOfSteps << " " 
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *************************************************** AddPlan ******************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy)
    : settlementName(settlementName), selectionPolicy(selectionPolicy) {}

void AddPlan::act(Simulation &simulation) {
    try {
        // Check if the settlement exists
        if (!simulation.isSettlementExists(settlementName)) {
            throw runtime_error("Cannot create this plan: Settlement not found");
        }
         // Determine the appropriate SelectionPolicy based on the input string
        SelectionPolicy *policy = nullptr;
        if (selectionPolicy == "eco") {
            policy = new EconomySelection();
        } else if (selectionPolicy == "bal") {
            policy = new BalancedSelection(0, 0, 0);
        } else if (selectionPolicy == "sus") {
            policy = new SustainabilitySelection();
        } else if (selectionPolicy == "nve") {
            policy = new NaiveSelection();
        } else {
            throw invalid_argument("Invalid selection policy: " + selectionPolicy);
        }
        simulation.addPlan(simulation.getSettlement(settlementName), policy);
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

AddPlan *AddPlan::clone() const {
    return new AddPlan(*this);
}

const string AddPlan::toString() const {
    ostringstream oss;
    oss << "plan " << settlementName << " " << selectionPolicy << " "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************ AddSettlement ***************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType)
    : settlementName(settlementName), settlementType(settlementType) {}

void AddSettlement::act(Simulation &simulation) {
    try {
        // Check if the settlement already exists
        if (simulation.isSettlementExists(settlementName)) {
            throw runtime_error("Settlement already exists");
        }
        simulation.addSettlement(new Settlement(settlementName, settlementType));
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

AddSettlement *AddSettlement::clone() const {
    return new AddSettlement(*this);
}

const string AddSettlement::toString() const {
    ostringstream oss;
    oss << "settlement " << settlementName << " " << static_cast<int>(settlementType) << " "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************* AddFacility ****************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

AddFacility::AddFacility(const string &facilityName, const FacilityCategory facilityCategory,
                         const int price, const int lifeQualityScore,
                         const int economyScore, const int environmentScore)
    : facilityName(facilityName), facilityCategory(facilityCategory), price(price),
      lifeQualityScore(lifeQualityScore), economyScore(economyScore), environmentScore(environmentScore) {}

void AddFacility::act(Simulation &simulation) {
    try {
        // Check if the facility already exists
        if (simulation.isFacilityExists(facilityName)) {
            throw runtime_error("Facility already exists");
        }
        simulation.addFacility(FacilityType(facilityName, facilityCategory, price,
                                            lifeQualityScore, economyScore, environmentScore));
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

AddFacility *AddFacility::clone() const {
    return new AddFacility(*this);
}

const string AddFacility::toString() const {
    ostringstream oss;
    oss << "facility " << facilityName << " " 
        << static_cast<int>(facilityCategory) << " "
        << price << " " << lifeQualityScore << " " 
        << economyScore << " " << environmentScore << " "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *********************************************** PrintPlanStatus **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

void PrintPlanStatus::act(Simulation &simulation) {
    try {
        if (!simulation.isPlanExists(planId)) {
            throw runtime_error("Plan doesn't exists");
        }
        cout << simulation.getPlan(planId).toString();
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

PrintPlanStatus *PrintPlanStatus::clone() const {
    return new PrintPlanStatus(*this);
}

const string PrintPlanStatus::toString() const {
    ostringstream oss;
    oss << "planStatus " << planId << " "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** ChangePlanPolicy **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy)
    : planId(planId), newPolicy(newPolicy) {}

void ChangePlanPolicy::act(Simulation &simulation) {
    try {
        if (!simulation.isPlanExists(planId)) {
            throw runtime_error("Cannot change selection policy");
        }
        Plan &plan = simulation.getPlan(planId);

        // Determine the appropriate SelectionPolicy based on the input string
        SelectionPolicy *policy = nullptr;
        if (newPolicy == "eco") {
            policy = new EconomySelection();
        } else if (newPolicy == "bal") {
            policy = new BalancedSelection(0, 0, 0); // to check -------------------------------------------------!!!!!!!!!!
        } else if (newPolicy == "sus") {
            policy = new SustainabilitySelection();
        } else if (newPolicy == "nai") {
            policy = new NaiveSelection();
        } else {
            throw runtime_error("Cannot change selection policy");
        }

        plan.setSelectionPolicy(policy);
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

ChangePlanPolicy *ChangePlanPolicy::clone() const {
    return new ChangePlanPolicy(*this);
}

const string ChangePlanPolicy::toString() const {
    ostringstream oss;
    oss << "changePolicy " << planId << " " << newPolicy << " " <<((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *********************************************** PrintActionsLog **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

PrintActionsLog::PrintActionsLog() {}

void PrintActionsLog::act(Simulation &simulation) {
    try {
        for (const auto &action : simulation.getActionsLog()) {
            cout << action->toString() << endl;
        }
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

PrintActionsLog *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

const string PrintActionsLog::toString() const {
    ostringstream oss;
    oss << "log "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *************************************************** Close ********************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

Close::Close() {}

void Close::act(Simulation &simulation) {
    simulation.close();
    complete();
}

Close *Close::clone() const {
    return new Close(*this);
}

const string Close::toString() const {
    return "close COMPLETED";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *********************************************** BackupSimulation *************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BackupSimulation::BackupSimulation() {}

void BackupSimulation::act(Simulation &simulation) {
    if (backup != nullptr) {
        delete backup;
    }
    backup = new Simulation(simulation);
    complete();
}

BackupSimulation *BackupSimulation::clone() const {
    return new BackupSimulation(*this);
}

const string BackupSimulation::toString() const {
    return "backup COMPLETED";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** RestoreSimulation *************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////


RestoreSimulation::RestoreSimulation() {}

void RestoreSimulation::act(Simulation &simulation) {
    try {
        // Check if a backup exists
        if (backup == nullptr) {
            throw runtime_error("No backup available");
        }
        
        // Restore the simulation by overwriting the current state with the backup
        simulation = *backup; 
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

RestoreSimulation *RestoreSimulation::clone() const {
    return new RestoreSimulation(*this);
}

const string RestoreSimulation::toString() const {
    ostringstream oss;
    oss << "restore "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
}