cmake_minimum_required(VERSION 3.21)

# Conditional compiler setting
if(APPLE)
    # Looking for gcc installed by Homebrew
    find_program(GCC NAMES gcc-14 gcc-13 gcc-12 gcc-11)
    find_program(GXX NAMES g++-14 g++-13 g++-12 g++-11)
    if(GCC AND GXX)
        set(CMAKE_C_COMPILER ${GCC})
        set(CMAKE_CXX_COMPILER ${GXX})
    else()
        message(FATAL_ERROR "No suitable gcc/g++ compiler found. Please install gcc and g++.")
    endif()
elseif(UNIX)
    set(CMAKE_C_COMPILER gcc)
    set(CMAKE_CXX_COMPILER g++)
endif()

# Project name and version
project(APImageFilters VERSION 1.0)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Include directories
include_directories(${CMAKE_SOURCE_DIR}/src)

# Add the executable
file(GLOB_RECURSE HEADER_FILES ${CMAKE_SOURCE_DIR}/src/*.h)

add_library(APImageLib
    src/stb_impl.cpp
    src/Image.cpp
    src/Volume.cpp
    src/Filters3D.cpp
    src/Projections3D.cpp
    src/Slicing3D.cpp
    src/CommandLine.cpp
    src/Filters2D.cpp
    ${HEADER_FILES}
)

add_executable(APImageFilters
    src/main.cpp
)
target_link_libraries(APImageFilters APImageLib)

# Enable testing
enable_testing()
include(CTest)

# Define TestRunner executable
add_executable(TestRunner
    tests/main.cpp
    tests/TestRunner.cpp
    tests/Projections3DTests.cpp
    tests/ImageTests.cpp
    tests/stb_impl.cpp
    tests/Filters2DTests.cpp
    tests/Filters3DTests.cpp
    tests/Slicing3DTests.cpp
    ${HEADER_FILES}
)

# Link test executable with library
target_link_libraries(TestRunner PRIVATE APImageLib)

# Register the test
add_test(NAME RunCustomUnitTests COMMAND TestRunner)

# Include the tests
include(${CMAKE_SOURCE_DIR}/cmdtests.cmake)
