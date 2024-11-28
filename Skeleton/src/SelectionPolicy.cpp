#include "SelectionPolicy.h"

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************ NaiveSelection **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

// Selects the next facility in a round-robin fashion
const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    lastSelectedIndex = (lastSelectedIndex + 1) % facilitiesOptions.size();
    return facilitiesOptions[lastSelectedIndex];
}

// Returns the type of selection as a string
const string NaiveSelection::toString() const {
    return "NaiveSelection";
}

// Creates a copy of the current object
NaiveSelection* NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** BalancedSelection *************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
BalancedSelection::BalancedSelection(int lifeQualityScore, int economyScore, int environmentScore)
    : LifeQualityScore(lifeQualityScore), EconomyScore(economyScore), EnvironmentScore(environmentScore) {}

// Selects the most balanced facility based on weights
const FacilityType& BalancedSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    const FacilityType* bestFacility = &facilitiesOptions[0];
    int bestBalanceScore = 0;

    for (const auto& facility : facilitiesOptions) {
        int balanceScore = LifeQualityScore * facility.getLifeQualityScore() +
                           EconomyScore * facility.getEconomyScore() +
                           EnvironmentScore * facility.getEnvironmentScore();

        if (balanceScore > bestBalanceScore) {
            bestBalanceScore = balanceScore;
            bestFacility = &facility;
        }
    }

    return *bestFacility;
}

// Returns the type of selection as a string
const string BalancedSelection::toString() const {
    return "BalancedSelection";
}

// Creates a copy of the current object
BalancedSelection* BalancedSelection::clone() const {
    return new BalancedSelection(*this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** EconomySelection **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
EconomySelection::EconomySelection() : lastSelectedIndex(-1) {}

// Selects the facility with the highest economy score
const FacilityType& EconomySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    int bestEconomy = 0;
    const FacilityType* bestFacility = nullptr;

    for (const auto& facility : facilitiesOptions) {
        if (facility.getEconomyScore() > bestEconomy) {
            bestEconomy = facility.getEconomyScore();
            bestFacility = &facility;
        }
    }

    return *bestFacility;
}

// Returns the type of selection as a string
const string EconomySelection::toString() const {
    return "EconomySelection";
}

// Creates a copy of the current object
EconomySelection* EconomySelection::clone() const {
    return new EconomySelection(*this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ******************************************* SustainabilitySelection ************************************************ //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor
SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(-1) {}

// Selects the facility with the highest environment score
const FacilityType& SustainabilitySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    int bestEnvironment = 0;
    const FacilityType* bestFacility = nullptr;

    for (const auto& facility : facilitiesOptions) {
        if (facility.getEnvironmentScore() > bestEnvironment) {
            bestEnvironment = facility.getEnvironmentScore();
            bestFacility = &facility;
        }
    }

    return *bestFacility;
}

// Returns the type of selection as a string
const string SustainabilitySelection::toString() const {
    return "SustainabilitySelection";
}

// Creates a copy of the current object
SustainabilitySelection* SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}