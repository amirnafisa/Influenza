cmake_minimum_required(VERSION 3.13)

PROJECT(InfluenzaWithSurv CXX)

ADD_EXECUTABLE( ${PROJECT_NAME} Multi_City_Main.cpp Business.cpp City.cpp Flu_Manager.cpp Household.cpp Location.cpp Person.cpp SimulationRun.cpp)

set(CMAKE_C_COMPILER "/usr/local/opt/llvm/bin/clang")
set(CMAKE_CXX_COMPILER "/usr/local/opt/llvm/bin/clang++")
set(OPENMP_LIBRARIES "/usr/local/opt/libomp/lib/")
set(OPENMP_INCLUDES "/usr/local/opt/libomp/include/")

#OPTION (USE_OpenMP "Use OpenMP to enable <omp.h>" ON)
#
## Find OpenMP
#if(APPLE AND USE_OpenMP)
#    if(CMAKE_C_COMPILER_ID MATCHES "Clang")
#        set(OpenMP_C "${CMAKE_C_COMPILER}")
#        set(OpenMP_C_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
#        set(OpenMP_C_LIB_NAMES "libomp" "libgomp" "libiomp5")
#        set(OpenMP_libomp_LIBRARY ${OpenMP_C_LIB_NAMES})
#        set(OpenMP_libgomp_LIBRARY ${OpenMP_C_LIB_NAMES})
#        set(OpenMP_libiomp5_LIBRARY ${OpenMP_C_LIB_NAMES})
#    endif()
#    if(CMAKE_CXX_COMPILER_ID MATCHES "Clang")
#      set(OpenMP_CXX "${CMAKE_CXX_COMPILER}")
#      set(OpenMP_CXX_FLAGS "-fopenmp=libomp -Wno-unused-command-line-argument")
#      set(OpenMP_CXX_LIB_NAMES "libomp" "libgomp" "libiomp5")
#      set(OpenMP_libomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#      set(OpenMP_libgomp_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#      set(OpenMP_libiomp5_LIBRARY ${OpenMP_CXX_LIB_NAMES})
#    endif()
#endif()
#
set(OpenMP_CXX_FLAGS "-fopenmp -Wno-unused-command-line-argument")
set(CMAKE_EXE_LINKER_FLAGS "/usr/local/Cellar/libomp/7.0.0/lib/libomp.dylib")

find_package(OpenMP REQUIRED)

target_link_libraries(${PROJECT_NAME} PRIVATE OpenMP::OpenMP_CXX)

