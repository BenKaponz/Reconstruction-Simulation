#include "Facility.h"

Facility::Facility(const string &name, const string &settlementName, const FacilityCategory category, const int price, const int lifeQuality_score, const int economy_score, const int environment_score) {
}

Facility::Facility(const FacilityType &type, const string &settlementName) {
}

const string &Facility::getSettlementName() const
{
    // TODO: insert return statement here
}

const int Facility::getTimeLeft() const
{
    return 0;
}

FacilityStatus Facility::step()
{
    return FacilityStatus();
}

void Facility::setStatus(FacilityStatus status)
{
}

const FacilityStatus &Facility::getStatus() const
{
    // TODO: insert return statement here
}

const string Facility::toString() const
{
    return string();
}
