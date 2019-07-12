CXX = mpic++
SRCDIR = src/
INCDIR = include/
LIBDIR = lib/
SRCFILES =  Business.cpp City.cpp Flu_Manager.cpp Household.cpp Location.cpp Person.cpp SimulationRun.cpp mpi_interface.cpp
SRCS = $(addprefix $(SRCDIR),$(SRCFILES))

OBJS = $(SRCFILES:.cpp=.o)

CXXFLAGS = -O3 -std=c++11 -fopenmp -I $(INCDIR) -c

LDFLAGS = -O3 -std=c++11 -fopenmp -I $(INCDIR) -dynamiclib

TARGET_LIB = $(LIBDIR)libFluSimulation.dylib

.PHONY: all
all: ${TARGET_LIB}
	$(CXX) $(CXXFLAGS) $(SRCS)
	$(CXX) $(LDFLAGS) $(OBJS) -o ${TARGET_LIB}
	rm *.o



.PHONY: clean
clean:
	rm -rf lib
