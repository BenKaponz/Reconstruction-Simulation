#include "Facility.h"
using namespace std;

//Constructor
FacilityType::FacilityType(const string &name, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : name(name), category(category), price(price), lifeQuality_score(lifeQuality_score), economy_score(economy_score), environment_score(environment_score) {}

// Copy Constructor
FacilityType::FacilityType(const FacilityType &other) : name(other.name), category(other.category), price(other.price), lifeQuality_score(other.lifeQuality_score), economy_score(other.economy_score), environment_score(other.environment_score) {}

// Copy Assignment Operator
FacilityType &FacilityType::operator=(const FacilityType &other) {
    return *this;
}

const string &FacilityType::getName() const
{
    return name;
}

int FacilityType::getCost() const
{
    return price;
}

int FacilityType::getLifeQualityScore() const
{
    return lifeQuality_score;
}

int FacilityType::getEnvironmentScore() const
{
    return environment_score;
}

int FacilityType::getEconomyScore() const
{
    return economy_score;
}

FacilityCategory FacilityType::getCategory() const
{
    return category;
}


///////////////////////////////////////////////////////////////////////////////////////////

//1st Constructor
Facility::Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score)
    : FacilityType(name, category, price, lifeQuality_score, economy_score, environment_score), 
      settlementName(settlementName), 
      status(FacilityStatus::UNDER_CONSTRUCTIONS), 
      timeLeft(price) 
{
}
//2nd Constructor 
Facility::Facility(const FacilityType &type, const string &settlementName)
    : FacilityType(type), 
      settlementName(settlementName), 
      status(FacilityStatus::UNDER_CONSTRUCTIONS), 
      timeLeft(price) 
{
}

const string &Facility::getSettlementName() const
{
    return settlementName;
}

const int Facility::getTimeLeft() const
{
    return timeLeft;
}

FacilityStatus Facility::step()
{
    if (status == FacilityStatus::UNDER_CONSTRUCTIONS && timeLeft > 0) {
        timeLeft--;
        if (timeLeft == 0) {
            status = FacilityStatus::OPERATIONAL;
        }
    }
    return status;
}

void Facility::setStatus(FacilityStatus newStatus)
{
    status = newStatus;
}

const FacilityStatus &Facility::getStatus() const
{
    return status;
}

const string Facility::toString() const
{
    return "Facility: " + getName()  + ", Settlement: " + settlementName + 
           ", Status: " + (status == FacilityStatus::OPERATIONAL ? "Operational" : "Under Construction") + 
           ", Time Left: " + to_string(timeLeft);
}