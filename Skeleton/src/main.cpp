#include "Simulation.h"
#include "Auxiliary.h"
#include "Settlement.h"
#include "Facility.h"
#include "SelectionPolicy.h"
#include "Plan.h"
#include "Action.h"
#include <iostream>
#include <fstream>

using namespace std;

#include "Simulation.h"
#include "Action.h"
#include <iostream>
#include <fstream>

using namespace std;

int main() {
    // Create the configuration file
    ofstream configFile("config.txt");
    configFile << "# settlement <settlement_name> <settlement_type>\n"
               << "settlement KfarSPL 0\n"
               << "settlement KiryatSPL 2\n"
               << "settlement BeitSPL 1\n"
               << "# facility <facility_name> <category> <price> <lifeq_impact> <eco_impact> <env_impact>\n"
               << "facility Hospital 0 5 5 3 2\n"
               << "facility School 0 4 4 2 2\n"
               << "facility Park 0 3 3 1 3\n"
               << "facility CommunityCenter 0 4 5 2 3\n"
               << "facility Factory 1 5 2 5 1\n"
               << "facility Market 1 4 3 3 2\n"
               << "facility Warehouse 1 3 1 3 1\n"
               << "facility Bank 1 4 2 5 0\n"
               << "facility RecyclingPlant 2 5 3 1 5\n"
               << "facility SolarFarm 2 4 2 2 4\n"
               << "facility WaterTreatmentPlant 2 3 1 1 3\n"
               << "facility WildlifeReserve 2 4 2 1 4\n"
               << "# plan <settlement_name> <selection_policy>\n"
               << "plan KfarSPL eco\n"
               << "plan KiryatSPL bal\n";
    configFile.close();

    // Initialize the simulation
    cout << "The simulation has started" << endl;
    Simulation simulation("config.txt");
    simulation.start();

    // Step 1
    SimulateStep step1(1);
    cout << "step 1" << endl;
    step1.act(simulation);

    // Display plan status for PlanID 0 and PlanID 1
    PrintPlanStatus planStatus0(0);
    cout << "planStatus 0" << endl;
    planStatus0.act(simulation);

    PrintPlanStatus planStatus1(1);
    cout << "planStatus 1" << endl;
    planStatus1.act(simulation);

    // Add a new settlement
    AddSettlement anotherVillage("anotherVillage", SettlementType::VILLAGE);
    cout << "settlement anotherVillage 0" << endl;
    anotherVillage.act(simulation);

    // Backup the simulation
    BackupSimulation backup;
    cout << "backup" << endl;
    backup.act(simulation);

    // Log all actions
    PrintActionsLog log;
    cout << "log" << endl;
    log.act(simulation);

    // Step 2
    SimulateStep step2(2);
    cout << "step 2" << endl;
    step2.act(simulation);

    // Display plan status for PlanID 0 and PlanID 1
    cout << "planStatus 0 COMPLETED" << endl;
    cout << simulation.getPlan(0).toString() << endl;
    cout << "planStatus 1 COMPLETED" << endl;
    cout << simulation.getPlan(1).toString() << endl;

    // Add a new plan for the new settlement
    AddPlan newPlan("anotherVillage", "bal");
    cout << "plan anotherVillage bal" << endl;
    newPlan.act(simulation);

    // Display plan status for PlanID 2
    PrintPlanStatus planStatus2(2);
    cout << "planStatus 2" << endl;
    planStatus2.act(simulation);
    cout << simulation.getPlan(2).toString() << endl;

    // Restore simulation
    RestoreSimulation restore;
    cout << "restore" << endl;
    restore.act(simulation);

    // Attempt to display plan status for PlanID 2 after restore
    cout << "planStatus 2" << endl;
    try {
        planStatus2.act(simulation);
        cout << simulation.getPlan(2).toString() << endl;
    } catch (const exception &e) {
        cout << "Error: Plan doesn't exist" << endl;
    }

    // Add a new facility
    AddFacility newFacility("road", FacilityCategory::ENVIRONMENT, 2, 1, 1, 1);
    cout << "facility road 1 2 1 1 1" << endl;
    newFacility.act(simulation);

    // Add the plan again for the restored simulation
    cout << "plan anotherVillage bal" << endl;
    newPlan.act(simulation);

    // Step 2
    cout << "step 2" << endl;
    step2.act(simulation);

    // Display plan status for PlanID 2
    cout << "planStatus 2" << endl;
    planStatus2.act(simulation);
    cout << simulation.getPlan(2).toString() << endl;

    // Close the simulation
    Close closeSimulation;
    cout << "close" << endl;
    closeSimulation.act(simulation);

    return 0;
}