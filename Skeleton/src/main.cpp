
#include <iostream>
#include "Settlement.h"

using namespace std;

int main() {
    // יצירת אובייקטים מסוג Settlement
    Settlement village("KfarSP", SettlementType::VILLAGE);
    Settlement city("TelSP", SettlementType::CITY);
    Settlement metropolis("SPopolis", SettlementType::METROPOLIS);

    // הדפסת פרטי היישובים
    cout << "Settlement Details:" << endl;

    cout << "Name: " << village.getName() 
         << ", Type: " << (village.getType() == SettlementType::VILLAGE ? "Village" : "Unknown") 
         << endl;

    cout << "Name: " << city.getName() 
         << ", Type: " << (city.getType() == SettlementType::CITY ? "City" : "Unknown") 
         << endl;

    cout << "Name: " << metropolis.getName() 
         << ", Type: " << (metropolis.getType() == SettlementType::METROPOLIS ? "Metropolis" : "Unknown") 
         << endl;

cout << "hiii" << endl;
    return 0;
}