IDIR=../
#-mcmodel=large  gives lot of errors
flu: ${IDIR}/Business.cpp ${IDIR}/City.cpp ${IDIR}/Flu_Manager.cpp ${IDIR}/Household.cpp ${IDIR}/Location.cpp ${IDIR}/Multi_City_Main.cpp ${IDIR}/Person.cpp ${IDIR}/SimulationRun.cpp
	g++-8 -O3 -std=c++11 -fopenmp ${IDIR}/Business.cpp ${IDIR}/City.cpp ${IDIR}/Flu_Manager.cpp ${IDIR}/Household.cpp ${IDIR}/Location.cpp ${IDIR}/Multi_City_Main.cpp ${IDIR}/Person.cpp ${IDIR}/SimulationRun.cpp -o FluSimulation -lm

run:
	./FluSimulation
	python3 ${IDIR}/plot.py
