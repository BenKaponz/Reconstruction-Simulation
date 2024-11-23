#include "Settlement.h"
#include <iostream>

// Constructor
Settlement::Settlement(const string &name, SettlementType type)
    : name(name), type(type) {
    std::cout << "Settlement Constructor Called for: " << name << std::endl;
}

// Copy Constructor
Settlement::Settlement(const Settlement &other)
    : name(other.name), type(other.type) {
    std::cout << "Settlement Copy Constructor Called for: " << name << std::endl;
}

// Copy Assignment Operator
Settlement &Settlement::operator=(const Settlement &other) {
    if (this == &other) {
        return *this; // Handle self-assignment
    }
    // The `name` is const and cannot be reassigned.
    // The type is the only assignable field.
    const_cast<SettlementType&>(type) = other.type;

    std::cout << "Settlement Copy Assignment Operator Called for: " << name << std::endl;
    return *this;
}

// Destructor
Settlement::~Settlement() {
    std::cout << "Settlement Destructor Called for: " << name << std::endl;
}



SettlementType Settlement::getType() const
{
    return SettlementType();
}

const string Settlement::toString() const
{
    return string();
}

const string &Settlement::getName() const
{
    // TODO: insert return statement here
}

