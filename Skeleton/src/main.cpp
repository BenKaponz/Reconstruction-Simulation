#include "Simulation.h"
#include "Facility.h"
#include "Plan.h"
#include "SelectionPolicy.h"
#include <iostream>
#include <vector>

using namespace std;

int main() {
    // Test Settlement class
    cout << "Testing Settlement Class:" << endl;
    Settlement settlement("Sample Town", SettlementType::CITY);
    cout << settlement.toString() << endl;

    // Test FacilityType class
    cout << "\nTesting FacilityType Class:" << endl;
    FacilityType school("School", FacilityCategory::LIFE_QUALITY, 100, 50, 10, 20);
    FacilityType factory("Factory", FacilityCategory::ECONOMY, 200, 20, 70, 30);
    FacilityType park("Park", FacilityCategory::ENVIRONMENT, 150, 60, 20, 80);

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

    // Test Plan class
    cout << "\nTesting Plan Class:" << endl;

    // Define facility options
    vector<FacilityType> facilityOptions = {school, factory, park};

    // Create selection policy
    BalancedSelection policy(10, 20, 30);

    // Create Plan
    Plan plan(1, settlement, &policy, facilityOptions);
    cout << plan.toString() << endl;
    cout << "\nInitial Plan Status:" << endl;
    plan.printStatus();
    plan.step();
    cout << plan.toString() << endl;
    // Simulate construction steps
    cout << "\nSimulating Plan Construction Steps:" << endl;
    for (int i = 0; i < 100; ++i) {
        cout << "\nStep " << i + 1 << ":" << endl;
        plan.step();
        plan.printStatus();
    }
    cout << plan.toString() << endl;
    cout << "\nFinal Plan Status:" << endl;
    plan.printStatus();

    return 0;
}