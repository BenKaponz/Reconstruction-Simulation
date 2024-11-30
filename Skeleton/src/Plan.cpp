#include "Plan.h"
#include <sstream>
#include <iostream>
#include <algorithm>

using namespace std;

//Constructor
Plan::Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions)
    : plan_id(planId),
      settlement(&settlement),
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

//Copy Constructor 
Plan::Plan(const Plan &other)
    : plan_id(other.plan_id),
      settlement(other.settlement),
      selectionPolicy(other.selectionPolicy->clone()),
      status(other.status),
      facilities(),
      underConstruction(),
      facilityOptions(other.facilityOptions),
      life_quality_score(other.life_quality_score),
      economy_score(other.economy_score),
      environment_score(other.environment_score)
    {

    // Deep copy facilities
    for (auto facility : other.facilities) {
        facilities.push_back(new Facility(*facility));
    }

    // Deep copy under-construction facilities
    for (auto facility : other.underConstruction) {
        underConstruction.push_back(new Facility(*facility));
    }
}

Plan::~Plan() {
    delete selectionPolicy; ///remember
    for (auto *facility : facilities) {
        delete facility;
    }
    for (auto *facility : underConstruction) {
        delete facility;
    }
}

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

const Settlement& Plan::getSettlement() const {
    return *settlement; // Dereference the pointer to return the referenced Settlement object
}

void Plan::setSelectionPolicy(SelectionPolicy *newSelectionPolicy) {
    if (selectionPolicy) {
        delete selectionPolicy;  // Clean up old policy
    }
    selectionPolicy = newSelectionPolicy->clone();  // Clone the new policy
}

void Plan::step() {
    size_t capacity;
    switch (settlement->getType()) {
        case SettlementType::VILLAGE:    capacity = 1; break;
        case SettlementType::CITY:       capacity = 2; break;
        case SettlementType::METROPOLIS: capacity = 3; break;
    }
    while (capacity > underConstruction.size() && facilityOptions.size() != 0)  {  // להחליט אחר כך על התנאי
        FacilityType nextType = selectionPolicy->selectFacility(facilityOptions);
        Facility* nextFacility = new Facility(nextType, settlement->getName());
        addFacility(nextFacility);
    }
    
    // Process under-construction facilities
    for (auto it = underConstruction.begin(); it != underConstruction.end(); ) {
        Facility *facility = *it;
        facility->step(); // arrow is a sugar daddy for (*facility).step()
        if (facility->getStatus() == FacilityStatus::OPERATIONAL) {
            addFacility(facility);
            life_quality_score += facility->getLifeQualityScore();
            economy_score += facility->getEconomyScore();
            environment_score += facility->getEnvironmentScore();
            it = underConstruction.erase(it);  // Remove from under construction
        } else {
            ++it;
        }
    }

    if (underConstruction.size() == capacity) {
        status = PlanStatus::BUSY;
    } else {
        status = PlanStatus::AVALIABLE;
    }
}

void Plan::printStatus() {
    if(status == PlanStatus::BUSY) {
        cout << "PlanStatus: BUSY" << endl;
    } else {
        cout << "PlanStatus: AVAILABLE" << endl;
    }
}

const vector<Facility *> &Plan::getFacilities() const {
    return facilities;
}

void Plan::addFacility(Facility *facility) {
    if(facility->getStatus() == FacilityStatus::UNDER_CONSTRUCTIONS) {
        underConstruction.push_back(facility);
    } else {
        facilities.push_back(facility);
    }
}

const string Plan::toString() const {
    std::ostringstream output;

    output << "PlanID: " << plan_id << "\n";
    output << "SettlementName: " << settlement->getName() << "\n";
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
