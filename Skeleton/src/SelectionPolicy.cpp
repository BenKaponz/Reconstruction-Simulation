#include "SelectionPolicy.h"
#include <algorithm>
#include <climits>
#include <stdexcept> 

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************ NaiveSelection **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw std::runtime_error("No facilities available to select");
    }

    lastSelectedIndex = (lastSelectedIndex + 1) % facilitiesOptions.size();
    return facilitiesOptions[lastSelectedIndex];
}

const string NaiveSelection::toString() const {
    return "NaiveSelection";
}

NaiveSelection* NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** BalancedSelection *************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BalancedSelection::BalancedSelection(int lifeQualityScore, int economyScore, int environmentScore)
    : LifeQualityScore(lifeQualityScore), EconomyScore(economyScore), EnvironmentScore(environmentScore) {}

const FacilityType& BalancedSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    const FacilityType* bestFacility = nullptr;
    int smallestRange = INT_MAX; 

    for (const auto& facility : facilitiesOptions) {
        int adjustedLifeQuality = facility.getLifeQualityScore() + LifeQualityScore;
        int adjustedEconomy = facility.getEconomyScore() + EconomyScore;
        int adjustedEnvironment = facility.getEnvironmentScore() + EnvironmentScore;

        int maxScore = std::max({adjustedLifeQuality, adjustedEconomy, adjustedEnvironment});
        int minScore = std::min({adjustedLifeQuality, adjustedEconomy, adjustedEnvironment});
        int range = maxScore - minScore;

        if (range < smallestRange) {
            smallestRange = range;
            bestFacility = &facility;
        }
    }
    
    LifeQualityScore += bestFacility->getLifeQualityScore();
    EconomyScore += bestFacility->getEconomyScore();
    EnvironmentScore += bestFacility->getEnvironmentScore();

    return *bestFacility;
}

const string BalancedSelection::toString() const {
    return "BalancedSelection";
}

BalancedSelection* BalancedSelection::clone() const {
    return new BalancedSelection(*this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** EconomySelection **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

EconomySelection::EconomySelection() : lastSelectedIndex(-1) {}

const FacilityType& EconomySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    for (size_t i = 1; i <= facilitiesOptions.size(); i++) {
        size_t curr = (lastSelectedIndex + i) % facilitiesOptions.size(); 
        if (FacilityCategory::ECONOMY == facilitiesOptions[curr].getCategory()) {
            lastSelectedIndex = curr; 
            return facilitiesOptions[curr]; 
        }
    }
    throw std::runtime_error("No suitable facility found for EconomySelection");
}

const string EconomySelection::toString() const {
    return "EconomySelection";
}

EconomySelection* EconomySelection::clone() const {
    return new EconomySelection(*this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ******************************************* SustainabilitySelection ************************************************ //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(-1) {}

const FacilityType& SustainabilitySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    for (size_t i = 1; i <= facilitiesOptions.size(); i++) {
        size_t curr = (lastSelectedIndex + i) % facilitiesOptions.size(); 
        if (FacilityCategory::ENVIRONMENT == facilitiesOptions[curr].getCategory()) {
            lastSelectedIndex = curr; 
            return facilitiesOptions[curr]; 
        }
    }
    throw std::runtime_error("No suitable facility found for SustainabilitySelection");
}

const string SustainabilitySelection::toString() const {
    return "SustainabilitySelection";
}

SustainabilitySelection* SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}