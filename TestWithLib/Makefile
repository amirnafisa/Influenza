CXX = mpic++
INCDIR = ../LibraryProject/include/
SRCLIBDIR = ../LibraryProject/lib/
TARGETLIBDIR = lib/
SRCDIR = ./
SRCFILES = Multi_City_Main.cpp interface_main.cpp interface_calls.cpp
lib = FluSimulation
SRCS = $(addprefix $(SRCDIR),$(SRCFILES))

CXXFLAGS = -O3 -std=c++11 -fopenmp -I $(INCDIR) -L $(TARGETLIBDIR) -l $(lib)
GTKFLAGS = `pkg-config --cflags gtk+-3.0` `pkg-config --libs gtk+-3.0`

TARGET = FluSimulation

.PHONY: FluSimulation
all: ${TARGET}
	cp -rf $(SRCLIBDIR) $(TARGETLIBDIR)
	$(CXX) $(CXXFLAGS) $(GTKFLAGS) $(SRCS) -o ${TARGET}


.PHONY: clean
clean:
	rm ${TARGET}
