CXX = mpic++
INCDIR = ../LibraryProject/include/
SRCLIBDIR = ../LibraryProject/lib/
TARGETLIBDIR = lib/
SRCDIR = ./
SRCFILES = Multi_City_Main.cpp interface_main.cpp interface_calls.cpp simulation_calls.cpp LineGraph.cpp
lib = FluSimulation
SRCS = $(addprefix $(SRCDIR),$(SRCFILES))

CXXFLAGS = -O3 -std=c++11 -fopenmp -I $(INCDIR) -L $(TARGETLIBDIR) -l $(lib)
GTKFLAGS = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`
PLPLOTFLAGS = -I/usr/local/Cellar/plplot/5.15.0_1/include/ -L/usr/local/Cellar/plplot/5.15.0_1/lib/ -lplplot -lplplotcxx

TARGET = FluSimulation

.PHONY: FluSimulation
all: ${TARGET}
	cp -rf $(SRCLIBDIR) $(TARGETLIBDIR)
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) $(PLPLOTFLAGS) $(SRCS) -o ${TARGET}


.PHONY: clean
clean:
	rm ${TARGET}
	rm output_data/*
