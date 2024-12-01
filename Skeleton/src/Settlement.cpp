#include "Settlement.h"

// No rule of 3 needed here

// Constructor
Settlement::Settlement(const string &name, SettlementType type)
    : name(name), type(type) {
}

// SettlementType getter
SettlementType Settlement::getType() const{
    return type;
}

// Name getter
const string &Settlement::getName() const{
    return name;
}

// Converts the settlement's data to a readable string
const string Settlement::toString() const{
    string typeStr;
    switch (type) {
        case SettlementType::VILLAGE:    typeStr = "Village"; break;
        case SettlementType::CITY:       typeStr = "City"; break;
        case SettlementType::METROPOLIS: typeStr = "Metropolis"; break;                     
    }
    return "Name: " + name + ", Type: " + typeStr;
}

