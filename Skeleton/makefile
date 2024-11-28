# Please implement your Makefile rules and targets below.
# Customize this file to define how to build your project.



# All Targets
all: program

# Tool invocations
# Executable "program" depends on the object files main.o, Settlement.o, and Facility.o.
program: bin/main.o bin/Settlement.o bin/Facility.o
	@echo 'Building target: program'
	@echo 'Invoking: C++ Linker'
	g++ -o bin/program bin/main.o bin/Settlement.o bin/Facility.o
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

# Clean the build directory
clean:
	rm -f bin/*