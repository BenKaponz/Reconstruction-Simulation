# Please implement your Makefile rules and targets below.
# Customize this file to define how to build your project.
# Please implement your Makefile rules and targets below.
# Customize this file to define how to build your project.
# Compiler
CXX = g++

# Compiler flags
CXXFLAGS = -std=c++11 -Wall -Wextra -pedantic

# Target executable
TARGET = settlement_test

# Source files
SRCDIR = src
SOURCES = $(SRCDIR)/main.cpp $(SRCDIR)/Settlement.cpp

# Object files
OBJECTS = $(SOURCES:.cpp=.o)

# Header files directory
INCDIR = include

# Build the target executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -o $@ $^

# Build object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) -I$(INCDIR) -c $< -o $@

# Clean build files
clean:
	rm -f $(SRCDIR)/*.o $(TARGET)
