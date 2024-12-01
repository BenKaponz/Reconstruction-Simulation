#pragma once
#include "Facility.h"
#include "Settlement.h"
#include "SelectionPolicy.h"
#include <sstream>
#include <iostream>
#include <algorithm>
#include <vector>

using namespace std;
using std::vector;

enum class PlanStatus {
    AVALIABLE,
    BUSY,
};

class Plan {
    public:
        Plan(const int planId, const Settlement &settlement, SelectionPolicy *selectionPolicy, const vector<FacilityType> &facilityOptions);
        Plan(const Plan &other);             
        Plan &operator=(const Plan &other) = delete;  
        Plan(Plan &&other) noexcept;
        Plan &operator=(Plan &&other) noexcept = delete;
        ~Plan();
        const Settlement& getSettlement() const;
        const int getPlanId() const;
        const int getlifeQualityScore() const;
        const int getEconomyScore() const;
        const int getEnvironmentScore() const;
        SelectionPolicy* getSelectionPolicy() const; 
        const vector<Facility*> &getFacilities() const;
        const vector<Facility *> &getFacilitiesUnderConstruction() const;
        void setSelectionPolicy(SelectionPolicy *selectionPolicy);
        void step();
        void addFacility(Facility* facility);
        void printStatus();
        const string toString() const;

    private:
        int plan_id;
        const Settlement &settlement;
        SelectionPolicy *selectionPolicy; //What happens if we change this to a reference?
        PlanStatus status;
        vector<Facility*> facilities;
        vector<Facility*> underConstruction;
        const vector<FacilityType> &facilityOptions;
        int life_quality_score, economy_score, environment_score;
};