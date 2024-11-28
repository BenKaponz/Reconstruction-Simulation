#include "Settlement.h"
#include <iostream>
using std::string;
using std::vector;


Settlement::Settlement(const string &name, SettlementType type)
    : name(name), type(type) {
}

SettlementType Settlement::getType() const{
    return type;
}

const string &Settlement::getName() const{
    return name;
}

const string Settlement::toString() const{
    string typeStr;
    switch (type) {
        case SettlementType::VILLAGE:    typeStr = "Village"; break;
        case SettlementType::CITY:       typeStr = "City"; break;
        case SettlementType::METROPOLIS: typeStr = "Metropolis"; break;
        default:                         typeStr = "Unknown"; break;
    }
    return "Name: " + name + ", Type: " + typeStr;
}

