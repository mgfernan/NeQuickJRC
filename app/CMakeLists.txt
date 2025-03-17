set(TARGET NeQuickJRC)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src/lib/public/ ${CMAKE_SOURCE_DIR}/src/lib/private/)

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
