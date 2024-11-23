#pragma once
#include <string>
#include <vector>
using std::string;
using std::vector;


enum class SettlementType {
    VILLAGE,
    CITY,
    METROPOLIS,
};

class Settlement {
    public:
        Settlement(const string &name, SettlementType type);
        Settlement &operator=(const Settlement &other);
        const string &getName() const;
        SettlementType getType() const;
        const string toString() const;

        private:
            const string name;
            SettlementType type;
};