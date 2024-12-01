#include "SelectionPolicy.h"

// No rule of 3 needed in any

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ************************************************ NaiveSelection **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor: Initializes the NaiveSelection with no previous selection
NaiveSelection::NaiveSelection() : lastSelectedIndex(-1) {}

// Selects the next facility one by one
const FacilityType& NaiveSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    if (facilitiesOptions.empty()) {
        throw runtime_error("No facilities available to select");
    }

    lastSelectedIndex = (lastSelectedIndex + 1) % facilitiesOptions.size();
    return facilitiesOptions[lastSelectedIndex];
}

// Returns the string representation of NaiveSelection
const string NaiveSelection::toString() const {
    return "nve";
}

// Clone
NaiveSelection* NaiveSelection::clone() const {
    return new NaiveSelection(*this);
}

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** BalancedSelection *************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor: Initializes BalancedSelection with current scores
BalancedSelection::BalancedSelection(int lifeQualityScore, int economyScore, int environmentScore)
    : LifeQualityScore(lifeQualityScore), EconomyScore(economyScore), EnvironmentScore(environmentScore) {}

// Selects the facility with the most balanced scores (smallest range between scores)
const FacilityType& BalancedSelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    const FacilityType* bestFacility = nullptr;
    int smallestRange = INT_MAX; 

    for (const auto& facility : facilitiesOptions) {
        int adjustedLifeQuality = facility.getLifeQualityScore() + LifeQualityScore;
        int adjustedEconomy = facility.getEconomyScore() + EconomyScore;
        int adjustedEnvironment = facility.getEnvironmentScore() + EnvironmentScore;

        // Calculate the range between max and min scores
        int maxScore = max({adjustedLifeQuality, adjustedEconomy, adjustedEnvironment});
        int minScore = min({adjustedLifeQuality, adjustedEconomy, adjustedEnvironment});
        int range = maxScore - minScore;

        if (range < smallestRange) {
            smallestRange = range;
            bestFacility = &facility;
        }
    }
    
    // Update the accumulated scores
    LifeQualityScore += bestFacility->getLifeQualityScore();
    EconomyScore += bestFacility->getEconomyScore();
    EnvironmentScore += bestFacility->getEnvironmentScore();

    return *bestFacility;
}

// Returns the string representation of BalancedSelection
const string BalancedSelection::toString() const {
    return "bal";
}

// Clone
BalancedSelection* BalancedSelection::clone() const {
    return new BalancedSelection(*this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ********************************************** EconomySelection **************************************************** //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor: Initializes EconomySelection with no previous selection
EconomySelection::EconomySelection() : lastSelectedIndex(-1) {}

// Selects the next facility with an ECONOMY category
const FacilityType& EconomySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    for (size_t i = 1; i <= facilitiesOptions.size(); i++) {
        size_t curr = (lastSelectedIndex + i) % facilitiesOptions.size(); 
        if (FacilityCategory::ECONOMY == facilitiesOptions[curr].getCategory()) {
            lastSelectedIndex = curr; 
            return facilitiesOptions[curr]; 
        }
    }
    throw runtime_error("No suitable facility found for EconomySelection");
}

// Returns the string representation of EconomySelection
const string EconomySelection::toString() const {
    return "eco";
}

// Clone
EconomySelection* EconomySelection::clone() const {
    return new EconomySelection(*this);
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// ******************************************* SustainabilitySelection ************************************************ //
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Constructor: Initializes SustainabilitySelection with no previous selection
SustainabilitySelection::SustainabilitySelection() : lastSelectedIndex(-1) {}

// Selects the next facility with an ENVIRONMENT category
const FacilityType& SustainabilitySelection::selectFacility(const vector<FacilityType>& facilitiesOptions) {
    for (size_t i = 1; i <= facilitiesOptions.size(); i++) {
        size_t curr = (lastSelectedIndex + i) % facilitiesOptions.size(); 
        if (FacilityCategory::ENVIRONMENT == facilitiesOptions[curr].getCategory()) {
            lastSelectedIndex = curr; 
            return facilitiesOptions[curr]; 
        }
    }
    throw runtime_error("No suitable facility found for SustainabilitySelection");
}

// Returns the string representation of SustainabilitySelection
const string SustainabilitySelection::toString() const {
    return "sus";
}

// Clone
SustainabilitySelection* SustainabilitySelection::clone() const {
    return new SustainabilitySelection(*this);
}