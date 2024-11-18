set(BENCHMARK_SOURCES
    run_dotmark.cpp
    dotmark.h
)

include_directories(
    ./MultiScaleOT/src
)

link_directories(
    ./MultiScaleOT/build/Common
    ./MultiScaleOT/build/LP_Lemon
    ./MultiScaleOT/build/ShortCutSolver
)

# Add the executable for the DOTmark benchmark
add_executable(run_dotmark ${BENCHMARK_SOURCES})

target_link_libraries(run_dotmark libShortCutSolver.a libLP_Lemon.a libCommon.a libemon.a)

set_target_properties(run_dotmark
    PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/benchmark
)

# Set BENCHMARK_DATA_DIRECTORY if the data directory exists
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/Data")
    set(BENCHMARK_DATA_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/Data")
    message(STATUS "Setting BENCHMARK_DATA_DIRECTORY to ${BENCHMARK_DATA_DIRECTORY}")
    target_compile_definitions(run_dotmark PRIVATE BENCHMARK_DATA_DIRECTORY="${BENCHMARK_DATA_DIRECTORY}")
endif()
