cmake_minimum_required(VERSION 3.10) # Or a more recent version if needed

project(NeQuickJRC VERSION 1.0.0 LANGUAGES C)


set(CMAKE_COMPILER_WARNING_FLAGS "-Wall -Wno-comment -Werror=implicit-function-declaration -Wunused-function -Wunused-variable -Wunreachable-code")
set(CMAKE_COMPILER_COVERAGE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
set(CMAKE_LINKER_COVERAGE_FLAGS  "--coverage -lgcov")


set(CMAKE_C_STANDARD 11) # Or a more recent standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra") # Add compiler warnings
set(CMAKE_C_FLAGS_DEBUG "-g -O0  ${CMAKE_COMPILER_WARNING_FLAGS} ${CMAKE_COMPILER_COVERAGE_FLAGS}")
SET(CMAKE_C_FLAGS_PROFILER "${CMAKE_COMPILER_COVERAGE_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -pg" CACHE STRING
    "Flags used by the C compiler to generate gmon.out profiler files."
    FORCE )

# Make compilation with preloaded MODIP grid and CCIR data
add_definitions(-DFTR_MODIP_CCIR_AS_CONSTANTS)

# Source files
set(NEQUICK_LIB_NAME nequick)

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_CCIR.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir11.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir12.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir13.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir14.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir15.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir16.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir17.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir18.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir19.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir20.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir21.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/CCIR/NeQuickG_JRC_ccir22.c

    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/ITU_R_P_371_8.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_coordinates.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_electron_density.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_Gauss_Kronrod_integration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_geometry.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_input_data.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_interpolate.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_iono_E_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_iono_F1_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_iono_F2_layer_fourier_coefficients.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_iono_F2_layer.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_iono_profile.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_math_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_MODIP_grid.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_MODIP.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_ray_slant.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_ray_vertical.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_ray.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_solar_activity.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_solar.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_TEC_integration.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC_time.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/NeQuickG_JRC.c
)

# Include directories
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src/lib/public/ ${CMAKE_CURRENT_SOURCE_DIR}/src/lib/private/)

# Executable
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/src)

add_executable(NeQuickJRC
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeQuickG_JRC_exception.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeQuickG_JRC_exception.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeQuickG_JRC_input_data_std_stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeQuickG_JRC_input_data_std_stream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeQuickG_JRC_input_data_stream.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeQuickG_JRC_input_data_stream.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/NeQuickG_JRC_main.c
)

target_link_libraries(NeQuickJRC ${NEQUICK_LIB_NAME} m)

# Installation (optional)
#install(TARGETS NeQuickJRC DESTINATION bin)
#install(DIRECTORY include/ DESTINATION include)

# Optional: Add tests (if you have them)
# enable_testing()
# add_executable(NeQuickJRCTests test/NeQuickJRCTests.cpp src/NeQuickJRC.cpp) # Adjust paths
# target_link_libraries(NeQuickJRCTests NeQuickJRC)
# add_test(NAME NeQuickJRCTests COMMAND NeQuickJRCTests)

# For usage as a library
add_library(${NEQUICK_LIB_NAME} ${SOURCES})
target_include_directories(${NEQUICK_LIB_NAME} PUBLIC include)
install(TARGETS ${NEQUICK_LIB_NAME} DESTINATION lib)
install(DIRECTORY include/ DESTINATION include)

# Optional: Add a configuration option for a static or shared library build.
# option(BUILD_SHARED_LIBS "Build shared libraries" ON)
# if(BUILD_SHARED_LIBS)
#   add_library(${NEQUICK_LIB_NAME} SHARED ${SOURCES})
# else()
#   add_library(${NEQUICK_LIB_NAME} STATIC ${SOURCES})
# endif()
# target_include_directories(${NEQUICK_LIB_NAME} PUBLIC include)
# install(TARGETS ${NEQUICK_LIB_NAME} DESTINATION lib)
# install(DIRECTORY include/ DESTINATION include)

# Optional: Find and link external libraries (if any)
# find_package(SomeLibrary REQUIRED)
# if(SomeLibrary_FOUND)
#   target_link_libraries(NeQuickJRC PRIVATE SomeLibrary::SomeLibrary)
# endif()

# Optional: Configure installation prefix
# set(CMAKE_INSTALL_PREFIX /usr/local CACHE PATH "Installation directory")

# Optional: Add a version file if you want to.
# configure_file(
#     "${PROJECT_SOURCE_DIR}/include/version.h.in"
#     "${PROJECT_BINARY_DIR}/include/version.h"
# )
# target_include_directories(NeQuickJRC PRIVATE "${PROJECT_BINARY_DIR}/include")