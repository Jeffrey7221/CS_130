cmake_minimum_required(VERSION 3.10.0)
project(wfh-web-server)

# Make sure developers do not run cmake in the main project directory, to keep
# build artifacts from becoming clutter
if (${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed.
    Please make a new directory (called a build directory) and run CMake from there.
    You may need to remove CMakeCache.txt." )
endif()

# Turn on debug builds if we are building in a devel shell
if (CMAKE_BUILD_TYPE STREQUAL "" AND "$ENV{DEVEL_SHELL}" STREQUAL "1")
    message(STATUS "Setting debug build type by default in devel shell")
    set(CMAKE_BUILD_TYPE Debug)
endif()

# Output binaries to a sub directory "bin"
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# Enable cmake testing
include(CTest)
enable_testing()

# Enable GoogleTest
include(GoogleTest)
add_subdirectory(/usr/src/googletest googletest)

# Enable Boost
# Use static libraries so binaries can be deployed without a full boost install
set(Boost_USE_STATIC_LIBS ON)
find_package(Boost 1.50 REQUIRED COMPONENTS system)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Update name and srcs
add_library(session_lib src/session.cc)
add_library(server_lib src/server.cc)
add_library(request_parser_lib src/http/request_parser.cc src/http/reply.cc)
add_library(reply_lib src/http/reply.cc)
# add_library(config_parser_lib src/config_parser/config_parser.cc)
add_library(nginx_config src/config_parser/nginx_config.cc)
add_library(nginx_config_parser src/config_parser/nginx_config_parser.cc)
add_library(nginx_config_statement src/config_parser/nginx_config_statement.cc)
# target_link_libraries(server_lib session_lib request_parser_lib)


# Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
# target_link_libraries(server server_lib session_lib request_parser_lib config_parser_lib Boost::system)
target_link_libraries(server server_lib session_lib request_parser_lib nginx_config nginx_config_parser nginx_config_statement Boost::system)

# Update test executable name, srcs, and deps
add_executable(request_parser_test tests/request_parser_test.cc)
add_executable(reply_test tests/reply_test.cc)
target_link_libraries(request_parser_test request_parser_lib gtest_main)
target_link_libraries(reply_test reply_lib gtest_main Boost::system)
add_executable(config_parser_test tests/config_parser_test.cc)
# target_link_libraries(config_parser_test config_parser_lib gtest_main gmock_main)
target_link_libraries(config_parser_test nginx_config nginx_config_parser nginx_config_statement gtest_main gmock_main)

# Update with test binary
# TODO(!) --> update with session, server, reply, request parser tests
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Update with target/test targets
# TODO(!) --> update with session, server, reply tests
include(cmake/CodeCoverageReportConfig.cmake)
# generate_coverage_report(TARGETS server request_parser_lib config_parser_lib TESTS request_parser_test config_parser_test)
generate_coverage_report(TARGETS server reply_lib request_parser_lib nginx_config nginx_config_parser nginx_config_statement TESTS request_parser_test reply_test config_parser_test)
