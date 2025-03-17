cmake_minimum_required(VERSION 3.10) # Or a more recent version if needed

project(NeQuickJRC VERSION 1.0.0 LANGUAGES C)

include(CTest)
enable_testing()


set(CMAKE_COMPILER_WARNING_FLAGS "-Wall -Wno-comment -Werror=implicit-function-declaration -Wunused-function -Wunused-variable -Wunreachable-code")
set(CMAKE_COMPILER_COVERAGE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
set(CMAKE_LINKER_COVERAGE_FLAGS  "--coverage -lgcov")


set(CMAKE_C_STANDARD 11) # Or a more recent standard
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wmissing-prototypes -Wfloat-equal -Wshadow -Wpedantic -Wconversion -Wcast-qual")
set(CMAKE_C_FLAGS_DEBUG "-g -O0  ${CMAKE_COMPILER_WARNING_FLAGS} ${CMAKE_COMPILER_COVERAGE_FLAGS}")
SET(CMAKE_C_FLAGS_PROFILER "${CMAKE_COMPILER_COVERAGE_FLAGS} ${CMAKE_C_FLAGS_DEBUG} -pg" CACHE STRING
    "Flags used by the C compiler to generate gmon.out profiler files."
    FORCE )

# Make compilation with preloaded MODIP grid and CCIR data
add_definitions(-DFTR_MODIP_CCIR_AS_CONSTANTS)

# Source files
set(NEQUICK_LIB_NAME nequick)

# Library
add_subdirectory(lib)

# Executables
add_subdirectory(app)

# Installation (optional)
#install(TARGETS NeQuickJRC DESTINATION bin)
#install(DIRECTORY include/ DESTINATION include)

# Test
add_subdirectory("test/unit_test")

# add_executable(NeQuickJRCTests test/NeQuickJRCTests.cpp src/NeQuickJRC.cpp) # Adjust paths
# target_link_libraries(NeQuickJRCTests NeQuickJRC)
# add_test(NAME NeQuickJRCTests COMMAND NeQuickJRCTests)


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