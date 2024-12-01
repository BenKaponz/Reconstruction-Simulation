#include "Action.h"

// No rule of 3 needed.

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************* BaseAction ******************************************************* //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Default constructor for BaseAction
BaseAction::BaseAction() : errorMsg(""), status(ActionStatus::ERROR) {}

// Get the status of the action
ActionStatus BaseAction::getStatus() const {
    return status;
}

// Mark the action as completed
void BaseAction::complete() {
    status = ActionStatus::COMPLETED;
}

// Set an error message and print it
void BaseAction::error(string errorMsg) {
    this->errorMsg = move(errorMsg); 
    cout << "Error: " << this->errorMsg << endl; 
}

// Get the error message
const string &BaseAction::getErrorMsg() const {
    return errorMsg;
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************* SimulateStep ***************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
SimulateStep::SimulateStep(const int numOfSteps) : numOfSteps(numOfSteps) {}

// Execute the SimulateStep action
void SimulateStep::act(Simulation &simulation) {
    for (int i = 0; i < numOfSteps; i++) {
        simulation.step();
    }
    complete();
}

// Clone
SimulateStep *SimulateStep::clone() const {
    return new SimulateStep(*this);
}

// Convert the SimulateStep action to string
const string SimulateStep::toString() const {
    ostringstream oss;
    oss << "step " << numOfSteps << " " 
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *************************************************** AddPlan ******************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor 
AddPlan::AddPlan(const string &settlementName, const string &selectionPolicy)
    : settlementName(settlementName), selectionPolicy(selectionPolicy) {}

// Execute the AddPlan action
void AddPlan::act(Simulation &simulation) {
    try {
        // Check if the settlement exists
        if (!simulation.isSettlementExists(settlementName)) {
            throw runtime_error("Cannot create this plan");
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
            throw invalid_argument("Cannot create this plan");
        }
        simulation.addPlan(simulation.getSettlement(settlementName), policy);
        complete();
    } catch (const exception &e) {
        error(e.what()); 
    }
}

// Clone
AddPlan *AddPlan::clone() const {
    return new AddPlan(*this);
}

// Convert the AddPlan action to string
const string AddPlan::toString() const {
    ostringstream oss;
    oss << "plan " << settlementName << " " << selectionPolicy << " "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************ AddSettlement ***************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
AddSettlement::AddSettlement(const string &settlementName, SettlementType settlementType)
    : settlementName(settlementName), settlementType(settlementType) {}

// Execute the AddSettlement action
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

// Clone
AddSettlement *AddSettlement::clone() const {
    return new AddSettlement(*this);
}

// Convert the AddSettlement action to string
const string AddSettlement::toString() const {
    ostringstream oss;
    oss << "settlement " << settlementName << " " << static_cast<int>(settlementType) << " "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************* AddFacility ****************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
AddFacility::AddFacility(const string &facilityName, const FacilityCategory facilityCategory,
                         const int price, const int lifeQualityScore,
                         const int economyScore, const int environmentScore)
    : facilityName(facilityName), facilityCategory(facilityCategory), price(price),
      lifeQualityScore(lifeQualityScore), economyScore(economyScore), environmentScore(environmentScore) {}

// Execute the AddFacility action
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

// Clone
AddFacility *AddFacility::clone() const {
    return new AddFacility(*this);
}

// Convert the AddFacility action to string
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

// Constructor
PrintPlanStatus::PrintPlanStatus(int planId) : planId(planId) {}

// Execute the PrintPlanStatus action
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

// Clone
PrintPlanStatus *PrintPlanStatus::clone() const {
    return new PrintPlanStatus(*this);
}

// Convert the PrintPlanStatus action to string
const string PrintPlanStatus::toString() const {
    ostringstream oss;
    oss << "planStatus " << planId << " "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** ChangePlanPolicy **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//Constructor
ChangePlanPolicy::ChangePlanPolicy(const int planId, const string &newPolicy)
    : planId(planId), newPolicy(newPolicy) {}

// Execute the ChangePlanPolicy action
void ChangePlanPolicy::act(Simulation &simulation) {
    try {
        if (!simulation.isPlanExists(planId)) {
            throw runtime_error("Cannot change selection policy");
        }
        Plan &plan = simulation.getPlan(planId);

        // Check if the desired policy is the same as the current policy
        if (plan.getSelectionPolicy()->toString() == newPolicy) {
            throw runtime_error("Cannot change selection policy");
        }

        // Determine the appropriate SelectionPolicy based on the input string
        SelectionPolicy *policy = nullptr;
        if (newPolicy == "eco") {
            policy = new EconomySelection();
        } else if (newPolicy == "bal") {
            // Use existing city scores
            int lifeQualityScore = plan.getlifeQualityScore();
            int economyScore = plan.getEconomyScore();
            int environmentScore = plan.getEnvironmentScore();
            // Add scores for under-construction facilities
            const auto &underConstruction = plan.getFacilitiesUnderConstruction();
            for (const Facility *facility : underConstruction) {
                lifeQualityScore += facility->getLifeQualityScore();
                economyScore += facility->getEconomyScore();
                environmentScore += facility->getEnvironmentScore();
            }   
            // Set to a bal selectionPolicy according to the current stats.
            policy = new BalancedSelection(lifeQualityScore, economyScore, environmentScore); 
        } else if (newPolicy == "sus") {
            policy = new SustainabilitySelection();
        } else if (newPolicy == "nve") {
            policy = new NaiveSelection();
        } else {
            throw runtime_error("Cannot change selection policy");
        }
        cout << "planID: " << planId << "\npreviousPolicy: " 
                 << plan.getSelectionPolicy()->toString() << "\nnewPolicy: " 
                 << policy->toString() << endl;
        plan.setSelectionPolicy(policy);
        complete();
    } catch (const exception &e) {
        error(e.what());
    }
}

// Clone
ChangePlanPolicy *ChangePlanPolicy::clone() const {
    return new ChangePlanPolicy(*this);
}

// Convert ChangePlanPolicy action to a string
const string ChangePlanPolicy::toString() const {
    ostringstream oss;
    oss << "changePolicy " << planId << " " << newPolicy << " " <<((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *********************************************** PrintActionsLog **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
PrintActionsLog::PrintActionsLog() {}

// Execute the PrintActionsLog action
void PrintActionsLog::act(Simulation &simulation) {
    for (const auto &action : simulation.getActionsLog()) {
        cout << action->toString() << endl;
    }
    complete();
}

// Clone
PrintActionsLog *PrintActionsLog::clone() const {
    return new PrintActionsLog(*this);
}

// Convert PrintActionsLog action to a string
const string PrintActionsLog::toString() const {
    ostringstream oss;
    oss << "log "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *************************************************** Close ********************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
Close::Close() {}

// Execute the Close action
void Close::act(Simulation &simulation) {
    simulation.close();
    complete();
}

// Clone
Close *Close::clone() const {
    return new Close(*this);
}

// Convert Close action to a string
const string Close::toString() const {
    return "close COMPLETED";
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// *********************************************** BackupSimulation *************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor 
BackupSimulation::BackupSimulation() {}

// Execute the BackupSimulation action
void BackupSimulation::act(Simulation &simulation) {
    if (backup != nullptr) {
        delete backup;
    }
    backup = new Simulation(simulation);
    complete();
}

// Clone
BackupSimulation *BackupSimulation::clone() const {
    return new BackupSimulation(*this);
}

// Convert BackupSimulation action to a string
const string BackupSimulation::toString() const {
    return "backup COMPLETED";
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** RestoreSimulation *************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
RestoreSimulation::RestoreSimulation() {}

// Execute the RestoreSimulation action
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

// Clone
RestoreSimulation *RestoreSimulation::clone() const {
    return new RestoreSimulation(*this);
}

// Convert RestoreSimulation action to a string
const string RestoreSimulation::toString() const {
    ostringstream oss;
    oss << "restore "
        << ((getStatus() == ActionStatus::COMPLETED) ? "COMPLETED" : "ERROR");
    return oss.str();
}