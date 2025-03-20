set(TARGET NeQuickJRC)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/lib)

# Executable
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(${TARGET}
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_exception.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_exception.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_input_data_std_stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_input_data_std_stream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_input_data_stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_input_data_stream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/NeQuickG_JRC_main.c
)

target_link_libraries(${TARGET} ${NEQUICK_LIB_NAME} m)

set(EXECUTABLE ${CMAKE_CURRENT_BINARY_DIR}/${TARGET})
install(PROGRAMS ${EXECUTABLE} DESTINATION bin)


# Tests
set(TESTNAME bin_command_line_ok)
add_test(NAME ${TESTNAME} COMMAND  ${EXECUTABLE} -c 236.831641 -0.39362878 0.00402826613 1 0 297.66 82.49 78.11 8.23 54.29 20281546.18)
set_tests_properties (${TESTNAME}
                      PROPERTIES PASS_REGULAR_EXPRESSION
                         "TEC =     22.33570")

set(TESTNAME bin_command_line_ko_wrong_month)
add_test(NAME ${TESTNAME} COMMAND  ${EXECUTABLE} -c 236.831641 -0.39362878 0.00402826613 13 0 297.66 82.49 78.11 8.23 54.29 20281546.18)
set_tests_properties (${TESTNAME}
                    PROPERTIES PASS_REGULAR_EXPRESSION
                        "Invalid month 13")
