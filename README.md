# 🏗️ Reconstruction Simulation
**C++ OOP Simulation – SPL 2025 @ BGU**

A modular and extensible C++ simulation designed to model strategic recovery planning in post-war SPLand. Developed as part of the **Systems Programming Lab (SPL)** course at Ben-Gurion University, the system simulates settlements, facilities, and development plans using object-oriented principles and strategic selection policies.

---

## 🔧 Technologies Used
- **C++11** – Classes, enums, STL containers, Rule of Five
- **UNIX/Linux** – Required execution environment
- **Valgrind** – Memory leak detection
- **Makefile** – Builds the project and generates the final binary in `bin/`

---

## 💡 Project Structure
```
Reconstruction-Simulation/
├── bin/                      # Output binary placed here
├── include/                  # Header files
│   ├── Action.h
│   ├── Auxiliary.h
│   ├── Facility.h
│   ├── Plan.h
│   ├── SelectionPolicy.h
│   ├── Settlement.h
│   └── Simulation.h
├── src/                      # Implementation files (.cpp)
│   ├── Action.cpp
│   ├── Auxiliary.cpp
│   ├── Facility.cpp
│   ├── Plan.cpp
│   ├── SelectionPolicy.cpp
│   ├── Settlement.cpp
│   ├── Simulation.cpp
│   └── main.cpp
├── config_file.txt           # Sample configuration file
├── commands.txt              # Sample automated command sequence
├── makefile                  # Build script
└── README.md                 # Project documentation
```

A file named `commands.txt` with a valid example is already included in the root directory.

---

## ✨ Features
- Object-oriented design with support for inheritance and polymorphism
- Support for multiple settlements and reconstruction plans
- Four distinct selection strategies: naive, balanced, economy, sustainability
- Real-time simulation steps controlled by user-defined actions
- In-memory simulation snapshot and recovery via backup/restore commands
- Final report generation on termination

---

## 🔄 Simulation Flow & Concepts

The simulation models the recovery of post-war SPLand by assigning **plans** to **settlements** and constructing **facilities** based on selected **policies**.

### 🧱 Core Concepts

| Concept         | Description |
|----------------|-------------|
| **Settlement** | A city, village, or metropolis (type 0/1/2). Each plan is attached to a settlement. |
| **Facility**   | A buildable structure (e.g. Market, Road, SolarFarm) that improves life quality, economy, and environment scores. |
| **Plan**       | A strategic development assigned to a settlement. It builds facilities over time according to a selection policy. |
| **Selection Policy** | Defines how a plan chooses which facility to build next: `nve` (naive), `eco` (economy-focused), `env` (environment-focused), `bal` (balanced). |
| **Step**       | A time unit in which plans attempt to build a facility. Triggered by the `step` command. |
| **Backup / Restore** | Allows saving and reverting the simulation state. Useful for branching scenarios. |
| **Log**        | A chronological list of executed actions. Can be printed using the `log` command. |

### 🔁 Simulation Flow

1. **Initialization**
   - Simulation reads `config_file.txt` which defines settlements, facilities, and plans **in that order**.
2. **Execution**
   - The user interacts with the simulation via commands (manually or via `commands.txt`).
   - Plans use their policy to select and build facilities step-by-step.
3. **Finalization**
   - Upon issuing the `close` command, a final report with scores and statuses is printed.

---

## 🚀 How to Run the Simulation

### 1. Build
```bash
make
```
Compiles the project into `bin/simulation`.

### 2. Run with Configuration File
```bash
./bin/simulation config_file.txt
```
Launches the simulation interactively with predefined data from `config_file.txt`.

### 3. Run with Automated Commands
You may also provide a sequence of commands using a text file (e.g., `commands.txt`) for automatic execution:
```bash
./bin/simulation config_file.txt < commands.txt
```
To save the output to a file:
```bash
./bin/simulation config_file.txt < commands.txt > output.txt
```

Example `commands.txt` content:
```txt
step 1
planStatus 0
planStatus 1
settlement anotherVillage 0
backup
log
plan anotherVillage bal
planStatus 2
restore
planStatus 2
facility road 1 2 1 1 1
plan anotherVillage bal
step 2
planStatus 2
close
```

---

## 🧭 Available Commands

Once the simulation is started (either interactively or via a batch file), you may enter the following commands:

| Command                          | Description |
|----------------------------------|-------------|
| `settlement <name> <type>`       | Adds a new settlement with the given name and type (`0`: Village, `1`: City, `2`: Metropolis) |
| `facility <name> <category> <life> <eco> <env> <cost>` | Adds a new facility option (category is an int enum) |
| `plan <settlement> <policy>`    | Assigns a new development plan to a settlement. Policies: `nve`, `bal`, `eco`, `env` |
| `step <n>`                       | Simulates `n` time steps |
| `planStatus <id>`               | Displays the status of plan with given ID |
| `changePolicy <id> <policy>`    | Changes the selection policy of an existing plan |
| `log`                            | Prints a history of all executed actions |
| `backup`                         | Saves the current state of the simulation |
| `restore`                        | Reverts to the last saved state |
| `close`                          | Terminates the simulation and prints final summary |

---

## 🗂️ Configuration File Format (`config_file.txt`)

The configuration file initializes the simulation with a predefined setup of settlements, facilities, and plans. It is passed as the first argument to the simulator.

⚠️ **Important:** The lines in this file **must appear in the following order**:
1. All `settlement` definitions
2. All `facility` definitions
3. All `plan` definitions

If the order is not respected (e.g., a plan references a settlement that has not been defined yet), the simulator will throw an error.

Each line in the file defines one of the following:

- A settlement:
  ```
  settlement <name> <type>
  ```
  Example:
  ```
  settlement KfarSPL 0
  ```

- A facility:
  ```
  facility <name> <category> <lifeScore> <ecoScore> <envScore> <cost>
  ```
  Example:
  ```
  facility Market 1 2 1 2 1
  ```

- A plan (must reference a known settlement):
  ```
  plan <settlement> <policy>
  ```
  Example:
  ```
  plan KfarSPL eco
  ```

Lines beginning with `#` are treated as comments.

---

## 📄 Output Description

Throughout the simulation, the user can:
- Print the status of any plan
- Advance simulation steps
- Change plan policies
- Track action history
- Save and restore state
- Terminate the simulation with a final report

Sample output includes:
- PlanID, settlement name
- Current scores (life quality, economy, sustainability)
- Facility statuses (under construction / operational)

---

## 🧪 Testing
To validate memory safety:
```bash
valgrind --leak-check=full --show-reachable=yes ./bin/simulation config_file.txt
```
Expected output:
```
All heap blocks were freed -- no leaks are possible
ERROR SUMMARY: 0 errors from 0 contexts
```

⚠️ Make sure to compile with `-g` flag to enable line-level debugging with valgrind.

Ensure compilation includes the following flags:
```bash
-g -Wall -Weffc++ -std=c++11 -Iinclude
```

---

## 📚 Course Information
- **Course:** SPL – Systems Programming Lab  
- **Institution:** Ben-Gurion University of the Negev  
- **Year:** 2025  
- **Instructor:** Gil Einziger  
- **TAs:** Nir Sorani  
- **Environment:** Linux CS Lab, Docker-compatible

---

## 🧑‍💻 Authors

**Ben Kapon**  
Student at BGU  
[LinkedIn](https://www.linkedin.com/in/ben-kapon1/)

**Itay Shaul**  
Student at BGU  
[LinkedIn](https://www.linkedin.com/in/itay-shaul/)

---

## 📝 Important Note
This project was developed and tested on a **Docker-compatible Linux environment**.  
Ensure your submission compiles and runs with no warnings or memory leaks on CS Lab machines.

> This project demonstrates dynamic simulation of strategic development plans using OOP, memory-safe design, and user-driven commands in C++.

