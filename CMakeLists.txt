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
find_package(Boost 1.50 REQUIRED COMPONENTS system filesystem log regex)
message(STATUS "Boost version: ${Boost_VERSION}")

include_directories(include)

# Update name and srcs
add_library(session_lib src/session.cc)
add_library(server_lib src/server.cc)
add_library(request_parser_lib src/http/request_parser.cc src/http/reply.cc)
add_library(reply_lib src/http/reply.cc)
add_library(nginx_config src/config_parser/nginx_config.cc)
add_library(nginx_config_parser src/config_parser/nginx_config_parser.cc)
add_library(nginx_config_statement src/config_parser/nginx_config_statement.cc)
add_library(logger_lib src/logger/logger.cc)

# Update executable name, srcs, and deps
add_executable(server src/server_main.cc)
target_link_libraries(server server_lib session_lib request_parser_lib nginx_config nginx_config_parser nginx_config_statement logger_lib
  Boost::system Boost::filesystem Boost::regex Boost::log_setup Boost::log)

# Update test executable name, srcs, and deps
add_executable(request_parser_test tests/request_parser_test.cc)
#add_executable(request_handler_test tests/request_handler_test.cc)
add_executable(reply_test tests/reply_test.cc)
add_executable(nginx_config_test tests/nginx_config_test.cc)
add_executable(nginx_config_parser_test tests/nginx_config_parser_test.cc)
add_executable(nginx_config_statement_test tests/nginx_config_statement_test.cc src/config_parser/nginx_config)
add_executable(session_test tests/session_test.cc)
add_executable(server_test tests/server_test.cc)
add_executable(logger_test tests/logger_test.cc)
#target_link_libraries(request_handler_test request_handler_lib request_parser_lib
#  nginx_config nginx_config_parser nginx_config_statement gtest_main Boost::log_setup Boost::log)
target_link_libraries(request_parser_test request_parser_lib logger_lib gtest_main Boost::log_setup Boost::log)
target_link_libraries(reply_test reply_lib gtest_main Boost::system Boost::log_setup Boost::log)
target_link_libraries(nginx_config_test nginx_config nginx_config_parser nginx_config_statement logger_lib gtest_main Boost::log_setup Boost::log)
target_link_libraries(nginx_config_parser_test nginx_config nginx_config_parser nginx_config_statement logger_lib gtest_main Boost::log_setup Boost::log)
target_link_libraries(nginx_config_statement_test nginx_config nginx_config_parser nginx_config_statement logger_lib gtest_main Boost::log_setup Boost::log)
target_link_libraries(session_test session_lib request_parser_lib logger_lib gtest_main gmock_main Boost::system Boost::log_setup Boost::log)
target_link_libraries(server_test server_lib session_lib request_parser_lib logger_lib gtest_main gmock_main Boost::system Boost::log_setup Boost::log)
target_link_libraries(logger_test logger_lib gtest_main gmock_main Boost::system Boost::log_setup Boost::log)

# Update with test binary
gtest_discover_tests(request_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(nginx_config_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(nginx_config_parser_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(nginx_config_statement_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(reply_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(session_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(server_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)
gtest_discover_tests(logger_test WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/tests)

# Update with target/test targets
include(cmake/CodeCoverageReportConfig.cmake)
generate_coverage_report(TARGETS server server_lib reply_lib request_parser_lib nginx_config nginx_config_parser nginx_config_statement session_lib logger_lib
  TESTS request_parser_test nginx_config_test nginx_config_parser_test nginx_config_statement_test reply_test session_test server_test logger_test)

# Update with integration tests
add_test(NAME integration_test COMMAND ${BASH_PROGRAM} ${CMAKE_CURRENT_SOURCE_DIR}/tests/integration_test.sh)
