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

// Copy Assignment Operator
Settlement &Settlement::operator=(const Settlement &other) {
    if (this != &other) {
        name = other.name;
        type = other.type;
    }
    return *this;
}

// Destructor
Settlement::~Settlement() {
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
cout<<s<<endl;