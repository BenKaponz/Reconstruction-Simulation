# All Targets
all: program

# Tool invocations
# Executable "program" depends on the object files main.o, Settlement.o, Facility.o, Plan.o, SelectionPolicy.o, Auxiliary.o, Simulation.o, and Action.o.
program: bin/main.o bin/Settlement.o bin/Facility.o bin/Plan.o bin/SelectionPolicy.o bin/Auxiliary.o bin/Simulation.o bin/Action.o
	@echo 'Building target: program'
	@echo 'Invoking: C++ Linker'
	g++ -o bin/program bin/main.o bin/Settlement.o bin/Facility.o bin/Plan.o bin/SelectionPolicy.o bin/Auxiliary.o bin/Simulation.o bin/Action.o
	@echo 'Finished building target: program'
	@echo ' '

# Compile main.cpp into an object file
bin/main.o: src/main.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/main.o src/main.cpp

# Compile Settlement.cpp into an object file
bin/Settlement.o: src/Settlement.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Settlement.o src/Settlement.cpp

# Compile Facility.cpp into an object file
bin/Facility.o: src/Facility.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Facility.o src/Facility.cpp

# Compile Plan.cpp into an object file
bin/Plan.o: src/Plan.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Plan.o src/Plan.cpp

# Compile SelectionPolicy.cpp into an object file
bin/SelectionPolicy.o: src/SelectionPolicy.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/SelectionPolicy.o src/SelectionPolicy.cpp

# Compile Auxiliary.cpp into an object file
bin/Auxiliary.o: src/Auxiliary.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Auxiliary.o src/Auxiliary.cpp

# Compile Simulation.cpp into an object file
bin/Simulation.o: src/Simulation.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Simulation.o src/Simulation.cpp

# Compile Action.cpp into an object file
bin/Action.o: src/Action.cpp
	g++ -g -Wall -Weffc++ -std=c++11 -c -Iinclude -o bin/Action.o src/Action.cpp

# Clean the build directory
clean:
	rm -f bin/*