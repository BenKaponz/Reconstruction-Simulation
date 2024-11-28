#include "Simulation.h"
#include "Facility.h"
#include <iostream>

using namespace std;


int main() {
    // Test Settlement class
    cout << "Testing Settlement Class:" << endl;
    Settlement settlement("Sample Town", SettlementType::CITY);
    cout << settlement.toString() << endl;

    // Test FacilityType class
    cout << "\nTesting FacilityType Class:" << endl;
    FacilityType school("School", FacilityCategory::LIFE_QUALITY, 100, 50, 10, 20);
    cout << "Facility Type Name: " << school.getName() << endl;
    cout << "Category: " << (school.getCategory() == FacilityCategory::LIFE_QUALITY ? "Life Quality" : "Other") << endl;
    cout << "Cost: " << school.getCost() << endl;
    cout << "Life Quality Score: " << school.getLifeQualityScore() << endl;
    cout << "Economy Score: " << school.getEconomyScore() << endl;
    cout << "Environment Score: " << school.getEnvironmentScore() << endl;

    // Test Facility class
    cout << "\nTesting Facility Class:" << endl;
    Facility facility(school, "Sample Town");
    cout << facility.toString() << endl;

    cout << "\nSimulating Construction:" << endl;
    while (facility.getStatus() != FacilityStatus::OPERATIONAL) {
        facility.step();
        cout << facility.toString() << endl;
    }

    cout << "\nFinal Facility State:" << endl;
    cout << facility.toString() << endl;

    return 0;
    }
    