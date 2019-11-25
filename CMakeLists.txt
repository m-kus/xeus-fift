cmake_minimum_required(VERSION 3.15.2)
project(xeus-fift)

set(XFIFT_EXE xeus-fift)
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

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/libzmq EXCLUDE_FROM_ALL)
set(ZeroMQ_DIR ${CMAKE_BINARY_DIR}/third-party/libzmq)

set(cppzmq_REQUIRED_VERSION 4.3.0)
set(CPPZMQ_BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/cppzmq EXCLUDE_FROM_ALL)
set(cppzmq_DIR ${CMAKE_BINARY_DIR}/third-party/cppzmq)

set(BUILD_TESTS OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/xtl EXCLUDE_FROM_ALL)
set(xtl_DIR ${CMAKE_BINARY_DIR}/third-party/xtl)

set(JSON_BuildTests OFF CACHE INTERNAL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/nlohmann_json EXCLUDE_FROM_ALL)
set(nlohmann_json_DIR ${CMAKE_BINARY_DIR}/third-party/nlohmann_json)

set(xeus_REQUIRED_VERSION 0.21.1)
set(BUILD_TESTING OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/xeus EXCLUDE_FROM_ALL)

set(TON_ONLY_TONLIB ON CACHE BOOL "" FORCE)
set(TON_USE_ROCKSDB OFF CACHE BOOL "" FORCE)
set(TON_USE_ABSEIL OFF CACHE BOOL "" FORCE)
set(TDDB_USE_ROCKSDB OFF CACHE BOOL "" FORCE)
add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/ton EXCLUDE_FROM_ALL)

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

# Target and link xeus-fift
# =========================

set(XFIFT_EXE xeus-fift)
set(XFIFT_SRC 
    "${CMAKE_SOURCE_DIR}/src/xfift/xfift.cpp"
    "${CMAKE_SOURCE_DIR}/src/xfift/xinterpreter.cpp"
    "${CMAKE_SOURCE_DIR}/src/xresult.cpp"
    "${CMAKE_SOURCE_DIR}/src/xutil.cpp"
)

add_executable(${XFIFT_EXE} src/xfift/main.cpp ${XFIFT_SRC})
include_directories(${XFIFT_EXE} PUBLIC
    ${CMAKE_SOURCE_DIR}/src
    ${CMAKE_SOURCE_DIR}/third-party
    ${CMAKE_SOURCE_DIR}/third-party/xeus/include
    ${CMAKE_SOURCE_DIR}/third-party/nlohmann_json/include
    ${CMAKE_SOURCE_DIR}/third-party/libzmq/include
    ${CMAKE_SOURCE_DIR}/third-party/xtl/include
    ${CMAKE_SOURCE_DIR}/third-party/cppzmq
)
target_link_libraries(${XFIFT_EXE} PUBLIC 
    libzmq-static
    xeus_static 
    fift-lib
    stdc++fs
)

if (APPLE)
    set_target_properties(${XFIFT_EXE} PROPERTIES
        MACOSX_RPATH ON
    )
else()
    set_target_properties(${XFIFT_EXE} PROPERTIES
        BUILD_WITH_INSTALL_RPATH 1
        SKIP_BUILD_RPATH FALSE
    )
endif()

set_target_properties(${XFIFT_EXE} PROPERTIES
    INSTALL_RPATH_USE_LINK_PATH TRUE
)

# Target and link xeus-func
# =========================


# set(XFUNC_EXE xeus-func)
# file(GLOB_RECURSE XFUNC_SRC "${CMAKE_SOURCE_DIR}/src/xfunc/*")

# add_executable(${XFIFT_EXE} src/main.cpp ${XFIFT_SRC})
# include_directories(${XFIFT_EXE} PUBLIC
#     ${CMAKE_SOURCE_DIR}/third-party
#     ${CMAKE_SOURCE_DIR}/third-party/xeus/include
#     ${CMAKE_SOURCE_DIR}/third-party/nlohmann_json/include
#     ${CMAKE_SOURCE_DIR}/third-party/libzmq/include
#     ${CMAKE_SOURCE_DIR}/third-party/xtl/include
#     ${CMAKE_SOURCE_DIR}/third-party/cppzmq
# )
# target_link_libraries(${XFIFT_EXE} PUBLIC 
#     libzmq-static
#     xeus_static 
#     fift-lib
#     stdc++fs
# )

# if (APPLE)
#     set_target_properties(${XFIFT_EXE} PROPERTIES
#         MACOSX_RPATH ON
#     )
# else()
#     set_target_properties(${XFIFT_EXE} PROPERTIES
#         BUILD_WITH_INSTALL_RPATH 1
#         SKIP_BUILD_RPATH FALSE
#     )
# endif()

# set_target_properties(${XFIFT_EXE} PROPERTIES
#     INSTALL_RPATH_USE_LINK_PATH TRUE
# )


# Testing
# =======

set(XFIFT_TEST_EXE "xeus-fift-test")

add_subdirectory(${CMAKE_SOURCE_DIR}/third-party/googletest EXCLUDE_FROM_ALL)

set(SRC_TESTS
    "${CMAKE_SOURCE_DIR}/src/xfunc/xfunc.cpp"
    "${CMAKE_SOURCE_DIR}/src/xfunc/codegen.cpp"
    "${CMAKE_SOURCE_DIR}/src/xfunc/symguard.cpp"
    "${CMAKE_SOURCE_DIR}/src/xfift/xfift.cpp"
    "${CMAKE_SOURCE_DIR}/src/xresult.cpp"
    "${CMAKE_SOURCE_DIR}/src/xutil.cpp"
    "${CMAKE_SOURCE_DIR}/test/test_xfift.cpp"
    "${CMAKE_SOURCE_DIR}/test/test_xfunc.cpp"
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
    libzmq-static
    xeus_static
    fift-lib
    func-lib
    stdc++fs
)

add_test(NAME "test" COMMAND test)

# Installation
# ============

set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY TRUE)

# Install my_kernel
install(TARGETS ${XFIFT_EXE}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR})

# Configuration and data directories for jupyter and my_kernel
set(XJUPYTER_DATA_DIR "share/jupyter" CACHE STRING "Jupyter data directory")

# Install Jupyter kernelspecs
set(MY_KERNELSPEC_DIR ${CMAKE_CURRENT_SOURCE_DIR}/share/jupyter/kernels)
install(DIRECTORY ${MY_KERNELSPEC_DIR}
        DESTINATION ${XJUPYTER_DATA_DIR}
        PATTERN "*.in" EXCLUDE)

# Copy Fift libraries
install(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/third-party/ton/crypto/fift/lib/ 
        DESTINATION /usr/lib/fift)