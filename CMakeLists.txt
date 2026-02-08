cmake_minimum_required(VERSION 3.12)
project(LHCEventAnalysis VERSION 1.0 LANGUAGES CXX)

# C++17 standard required
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Optimization flags
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wextra")

# Find ROOT
find_package(ROOT REQUIRED COMPONENTS Core RIO Tree Hist)

# Handle nlohmann_json dependency (required by ROOT 6.28+)
find_package(nlohmann_json QUIET)
if(NOT nlohmann_json_FOUND)
    message(STATUS "nlohmann_json not found - attempting to install...")
    message(STATUS "Run: sudo apt install nlohmann-json3-dev")
    message(FATAL_ERROR "Please install nlohmann-json3-dev and retry")
endif()

include(${ROOT_USE_FILE})

# Find FastJet
if(DEFINED ENV{FASTJET_DIR})
    set(FASTJET_DIR $ENV{FASTJET_DIR})
else()
    set(FASTJET_DIR "$ENV{HOME}/cern-workspace/fastjet-install")
endif()

include_directories(${FASTJET_DIR}/include)
link_directories(${FASTJET_DIR}/lib)

# Find OpenMP
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
    add_definitions(-DUSE_OPENMP)
endif()

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/include)
include_directories(${ROOT_INCLUDE_DIRS})

# Source files for library
set(LIB_SOURCES
    src/Event.cpp
    src/Particle.cpp
    src/EventReader.cpp
    src/ZAnalysis.cpp
    src/TopAnalysis.cpp
    src/PerformanceTimer.cpp
    src/HistogramManager.cpp
)

# Create analysis library
add_library(AnalysisLib SHARED ${LIB_SOURCES})
target_link_libraries(AnalysisLib ${ROOT_LIBRARIES} fastjet)

# Event generator executable
add_executable(GenerateEvents src/GenerateEvents.cpp)
target_link_libraries(GenerateEvents AnalysisLib ${ROOT_LIBRARIES})

# Analysis executable
add_executable(RunAnalysis src/RunAnalysis.cpp)
target_link_libraries(RunAnalysis AnalysisLib ${ROOT_LIBRARIES} fastjet)

# Benchmark executable
add_executable(BenchmarkPerformance src/BenchmarkPerformance.cpp)
target_link_libraries(BenchmarkPerformance AnalysisLib ${ROOT_LIBRARIES} fastjet)
if(OpenMP_CXX_FOUND)
    target_link_libraries(BenchmarkPerformance OpenMP::OpenMP_CXX)
endif()

# Installation
install(TARGETS AnalysisLib GenerateEvents RunAnalysis BenchmarkPerformance
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib)