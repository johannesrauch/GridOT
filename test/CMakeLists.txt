set(ULMON_ALL_TESTS
smart_bpdigraph
ulm_grid_graph

ulm_network_simplex
shielded_pivot_rule

ulm_grid_solver
)

if(ULMON_COMPILE_TESTS)
foreach(ULMON_TEST IN LISTS ULMON_ALL_TESTS)
    #message(STATUS "add_executable(${ULMON_TEST} ${ULMON_TEST}.cpp)")
    add_executable(${ULMON_TEST} ${ULMON_TEST}.cpp)
    target_link_libraries(${ULMON_TEST} libemon.a)
endforeach()

enable_testing()

foreach(ULMON_TEST IN LISTS ULMON_ALL_TESTS)
    add_test(
        NAME ${ULMON_TEST} 
        COMMAND $<TARGET_FILE:${ULMON_TEST}> 
        WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}/test
    )
endforeach()

endif()

