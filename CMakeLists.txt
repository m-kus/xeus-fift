cmake_minimum_required(VERSION 3.4.3)
project(xeus-fift)

set(EXECUTABLE_NAME xeus-fift)
set(BUILD_SHARED_LIBS NO CACHE BOOL "" FORCE)
set(BUILD_STATIC_LIBS YES CACHE BOOL "" FORCE)

# Configuration
# =============

include(GNUInstallDirs)

# We generate the kernel.json file, given the installation prefix and the executable name
configure_file (
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/fift/kernel.json.in"
    "${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels/fift/kernel.json"
)

# Dependencies
# ============

# Be sure to use recent versions
set(xeus_REQUIRED_VERSION 0.21.1)
set(cppzmq_REQUIRED_VERSION 4.3.0)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/libzmq)

set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/cppzmq)
set(cppzmq_DIR ${CMAKE_BINARY_DIR}/third-party/cppzmq)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/xtl)
set(xtl_DIR ${CMAKE_BINARY_DIR}/third-party/xtl)

set(JSON_BuildTests OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/nlohmann_json)
set(nlohmann_json_DIR ${CMAKE_BINARY_DIR}/third-party/nlohmann_json)

set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/xeus)

set(TON_ONLY_TONLIB ON CACHE BOOL "" FORCE)
set(TON_USE_ROCKSDB OFF CACHE BOOL "" FORCE)
set(TON_USE_ABSEIL OFF CACHE BOOL "" FORCE)
set(TDDB_USE_ROCKSDB OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/ton)

# Flags
# =====

include(CheckCXXCompilerFlag)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang" OR CMAKE_CXX_COMPILER_ID MATCHES "GNU" OR CMAKE_CXX_COMPILER_ID MATCHES "Intel")
    CHECK_CXX_COMPILER_FLAG("-std=c++14" HAS_CPP14_FLAG)

    if (HAS_CPP14_FLAG)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++14")
    else()
        message(FATAL_ERROR "Unsupported compiler -- xeus requires C++14 support!")
    endif()
endif()

# Target and link
# ===============

file(GLOB_RECURSE XFIFT_SRC "${CMAKE_SOURCE_DIR}/src/*")

add_executable(${EXECUTABLE_NAME} src/main.cpp ${XFIFT_SRC})
include_directories(${EXECUTABLE_NAME} PUBLIC
    ${CMAKE_SOURCE_DIR}/third-party
    ${CMAKE_SOURCE_DIR}/third-party/xeus/include
    ${CMAKE_SOURCE_DIR}/third-party/nlohmann_json/include
    ${CMAKE_SOURCE_DIR}/third-party/libzmq/include
    ${CMAKE_SOURCE_DIR}/third-party/xtl/include
    ${CMAKE_SOURCE_DIR}/third-party/cppzmq
)
target_link_libraries(${EXECUTABLE_NAME} PUBLIC 
    xeus_static 
    fift-lib
)

if (APPLE)
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        MACOSX_RPATH ON
    )
else()
    set_target_properties(${EXECUTABLE_NAME} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        SKIP_BUILD_RPATH FALSE
    )
endif()

set_target_properties(${EXECUTABLE_NAME} PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Testing
# =======

set(XFIFT_TEST_EXE "xeus-fift-test")

add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/googletest)

set(SRC_TESTS
    "${CMAKE_SOURCE_DIR}/src/xfift.cpp"
    "${CMAKE_SOURCE_DIR}/src/xfift.hpp"
    "${CMAKE_SOURCE_DIR}/test/test_xfift.cpp"
    "${CMAKE_SOURCE_DIR}/test/main.cpp"
)

add_executable(${XFIFT_TEST_EXE} ${SRC_TESTS})
target_include_directories(${XFIFT_TEST_EXE} PUBLIC 
    "${CMAKE_SOURCE_DIR}/test" 
    "${CMAKE_SOURCE_DIR}/src" 
    "${gtest_SOURCE_DIR}/include"
)
target_link_libraries(${XFIFT_TEST_EXE} PUBLIC
    gtest 
    gtest_main 
    xeus_static 
    fift-lib
)

add_test(NAME "test" COMMAND test)

# Installation
# ============

# Install my_kernel
install(TARGETS ${EXECUTABLE_NAME}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Configuration and data directories for jupyter and my_kernel
set(XJUPYTER_DATA_DIR "share/jupyter" CACHE STRING "Jupyter data directory")

# Install Jupyter kernelspecs
set(MY_KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
install(DIRECTORY ${MY_KERNELSPEC_DIR}
        DESTINATION ${XJUPYTER_DATA_DIR}
        PATTERN "*.in" EXCLUDE)