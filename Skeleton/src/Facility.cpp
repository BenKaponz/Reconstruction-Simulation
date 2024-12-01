#include "Facility.h"

// No rule of 3 needed

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************* FacilityType ***************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
FacilityType::FacilityType(const string &name, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : name(name), category(category), price(price), lifeQuality_score(lifeQuality_score), economy_score(economy_score), environment_score(environment_score) {}

// Field's getters
const string &FacilityType::getName() const {
    return name;
}

int FacilityType::getCost() const {
    return price;
}

int FacilityType::getLifeQualityScore() const {
    return lifeQuality_score;
}

int FacilityType::getEnvironmentScore() const {
    return environment_score;
}

int FacilityType::getEconomyScore() const {
    return economy_score;
}

FacilityCategory FacilityType::getCategory() const {
    return category;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************** Facility ******************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// No rule of 3 needed here

// Constructor: creates a Facility from detailed fields
Facility::Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score), 
      settlementName(settlementName), 
      status(FacilityStatus::UNDER_CONSTRUCTIONS), 
      timeLeft(price) 
{
}

// Constructor: creates a Facility from an existing FacilityType
Facility::Facility(const FacilityType &type, const string &settlementName)
    : FacilityType(type), 
      settlementName(settlementName), 
      status(FacilityStatus::UNDER_CONSTRUCTIONS), 
      timeLeft(price) 
{
}

// Field's getters and setters
const string &Facility::getSettlementName() const {
    return settlementName;
}

const int Facility::getTimeLeft() const {
    return timeLeft;
}

const FacilityStatus &Facility::getStatus() const {
    return status;
}

void Facility::setStatus(FacilityStatus newStatus) {
    status = newStatus;
}

// Updates the status of the facility by simulating one time step
FacilityStatus Facility::step() {
    if (status == FacilityStatus::UNDER_CONSTRUCTIONS && timeLeft > 0) {
        timeLeft--;
        if (timeLeft == 0) {
            status = FacilityStatus::OPERATIONAL;
        }
    }
    return status;
}

// Converts the facility's data to a readable string
const string Facility::toString() const {
    return "Facility: " + getName()  + ", Settlement: " + settlementName + 
           ", Status: " + (status == FacilityStatus::OPERATIONAL ? "Operational" : "Under Construction") + 
           ", Time Left: " + to_string(timeLeft);
}