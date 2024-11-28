#include "Settlement.h"
#include <iostream>
using std::string;
using std::vector;


// Constructor
Settlement::Settlement(const string &name, SettlementType type)
    : name(name), type(type) {
}

// Copy Constructor
Settlement::Settlement(const Settlement &other)
    : name(other.name), type(other.type) {
}

SettlementType Settlement::getType() const{
    return SettlementType;
}

const string Settlement::toString() const{
    return "Settlement: " + name;
}

const string &Settlement::getName() const{
    return name;
}
