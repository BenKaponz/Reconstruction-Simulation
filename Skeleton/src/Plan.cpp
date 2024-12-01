#include "Plan.h"

// Rule of 5 used here, Class contains resources.
 // But without Assignment operator= and Move Assigment operator=


// Constructor
Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId),
      settlement(settlement),
      selectionPolicy(selectionPolicy),
      status(PlanStatus::AVALIABLE),
      facilities(),
      underConstruction(),
      facilityOptions(facilityOptions),
      life_quality_score(0),
      economy_score(0),
      environment_score(0)
      {
}

// Copy Constructor
Plan::Plan(const Plan &other)
    : plan_id(other.plan_id),
      settlement(other.settlement), // References the same settlement object.
      selectionPolicy(other.selectionPolicy->clone()), // Deep copy of selection policy.
      status(other.status),
      facilities(),
      underConstruction(),
      facilityOptions(other.facilityOptions), // References the same facility options.
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score)
    {

    // Deep copy facilities
    for (Facility* facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }

    // Deep copy under-construction facilities
    for (Facility* facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }
}

// Move Constructor
Plan::Plan(Plan &&other) noexcept
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy),
      status(other.status),
      facilities(move(other.facilities)),   // Transfers ownership
      underConstruction(move(other.underConstruction)),   // Transfers ownership
      facilityOptions(move(other.facilityOptions)), // References the same facility options.
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score) {

    // Reset pointers from the source object to ensure no double free
    other.selectionPolicy = nullptr;
}

// Destructor
Plan::~Plan() {
    delete selectionPolicy; 
    for (Facility *facility : facilities) {
        delete facility;
    }
    for (Facility *facility : underConstruction) {
        delete facility;
    }
}

// Field's getters and setters
const int Plan::getPlanId() const {
    return plan_id;
}

const int Plan::getlifeQualityScore() const {
    return life_quality_score;
}

const int Plan::getEconomyScore() const {
    return economy_score;
}

const int Plan::getEnvironmentScore() const {
    return environment_score;
}

SelectionPolicy* Plan::getSelectionPolicy() const {
    return selectionPolicy;
}

const Settlement& Plan::getSettlement() const {
    return settlement; 
}

const vector<Facility *> &Plan::getFacilities() const {
    return facilities;
}

const std::vector<Facility *> &Plan::getFacilitiesUnderConstruction() const {
    return underConstruction;
}

void Plan::setSelectionPolicy(SelectionPolicy *newSelectionPolicy) {
    if (selectionPolicy) {
        delete selectionPolicy;  // Clean up old policy
    }
    selectionPolicy = newSelectionPolicy; 
}

// Executes a single step of the plan, managing facility construction and scores.
void Plan::step() {
    size_t capacity;
    // Determines the facility capacity based on the settlement type.
    switch (settlement.getType()) {
        case SettlementType::VILLAGE:    capacity = 1; break;
        case SettlementType::CITY:       capacity = 2; break;
        case SettlementType::METROPOLIS: capacity = 3; break;
    }
    // Adds new facilities to under-construction if there's capacity and available options.
    while (capacity > underConstruction.size() && facilityOptions.size() != 0)  {  
        FacilityType nextType = selectionPolicy->selectFacility(facilityOptions);
        Facility* nextFacility = new Facility(nextType, settlement.getName());
        addFacility(nextFacility);
    }
    
    // Process under-construction facilities
    for (size_t i = 0; i < underConstruction.size(); ) {
        Facility *facility = underConstruction[i];
        facility->step(); 
        if (facility->getStatus() == FacilityStatus::OPERATIONAL) {
            addFacility(facility);
            life_quality_score += facility->getLifeQualityScore();
            economy_score += facility->getEconomyScore();
            environment_score += facility->getEnvironmentScore();
            underConstruction.erase(underConstruction.begin() + i); // Remove from under construction
        } else {
            ++i;
        }
    }

    if (underConstruction.size() == capacity) {
        status = PlanStatus::BUSY;
    } else {
        status = PlanStatus::AVALIABLE;
    }
}

// Adds a facility to either the operational or under-construction list.
void Plan::addFacility(Facility *facility) {
    if(facility->getStatus() == FacilityStatus::UNDER_CONSTRUCTIONS) {
        underConstruction.push_back(facility);
    } else {
        facilities.push_back(facility);
    }
}

// Prints the status of the plan (BUSY or AVAILABLE).
void Plan::printStatus() {
    if(status == PlanStatus::BUSY) {
        cout << "PlanStatus: BUSY" << endl;
    } else {
        cout << "PlanStatus: AVAILABLE" << endl;
    }
}

// Returns a string representation of the plan, including its details and facilities.
const string Plan::toString() const {
    std::ostringstream output;

    output << "PlanID: " << plan_id << "\n";
    output << "SettlementName: " << settlement.getName() << "\n";
    output << "PlanStatus: " << (status == PlanStatus::AVALIABLE ? "AVAILABLE" : "BUSY") << "\n";
    output << "SelectionPolicy: " << selectionPolicy->toString() << "\n";
    output << "LifeQualityScore: " << life_quality_score << "\n";
    output << "EconomyScore: " << economy_score << "\n";
    output << "EnvironmentScore: " << environment_score << "\n";

    for (const auto &facility : underConstruction) {
        output << "FacilityName: " << facility->getName() << "\n";
        output << "FacilityStatus: UNDER_CONSTRUCTION\n";
    }

    for (const auto &facility : facilities) {
        output << "FacilityName: " << facility->getName() << "\n";
        output << "FacilityStatus: OPERATIONAL\n";
    }

    return output.str();
}
