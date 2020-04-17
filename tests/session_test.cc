#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "session.h"
#include "http/reply.h"
#include "http/request_parser.h"
#include "http/request.h"
#include <tuple>

using ::testing::_;
using ::testing::AtLeast;

class SessionTestFix : public :: testing::Test {
    protected:
        boost::asio::io_service io_service;
        session* sesh = new session(io_service);
};

// Test the echo response for proper inputs
TEST_F(SessionTestFix, EchoResponseTest) {
    char incoming_request[1024] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
    strcpy(sesh->data_, incoming_request);
    reply r = sesh->echo_response();
    EXPECT_TRUE(r.status == reply::ok);

    for (int i = 0; i < r.headers.size(); i++){
        if (r.headers[i].name == "Content-Length"){
            EXPECT_TRUE(r.headers[i].value == std::to_string(strlen(incoming_request)-2));
        }
        if (r.headers[i].name == "Content-Type"){
            EXPECT_TRUE(r.headers[i].value == "text/plain");
        }
    }

    EXPECT_TRUE(std::string(incoming_request, strlen(incoming_request)-2) == r.content);
}

// Test the echo response for bad inputs
TEST_F(SessionTestFix, EchoBadResponseTest) {
    char incoming_request[1024] = "BAD RESPONSE";
    strcpy(sesh->data_, incoming_request);
    reply r = sesh->echo_bad_response();
    EXPECT_TRUE(r.status == reply::bad_request);

    for (int i = 0; i < r.headers.size(); i++){
        if (r.headers[i].name == "Content-Length"){
            EXPECT_TRUE(r.headers[i].value == std::to_string(strlen(incoming_request)-2));
        }
        if (r.headers[i].name == "Content-Type"){
            EXPECT_TRUE(r.headers[i].value == "text/plain");
        }
    }

    EXPECT_TRUE(std::string(incoming_request, strlen(incoming_request)-2) == r.content);
}

// Test that the session stops when a bad error code is set
TEST_F(SessionTestFix, ErrorCodeTest) {
    boost::system::error_code ec = boost::system::errc::make_error_code(boost::system::errc::not_supported);
    int read_ret = sesh->handle_read(ec, 0);
    EXPECT_EQ(read_ret, -1);

    int write_ret = sesh->handle_write(ec, 0);
    EXPECT_EQ(write_ret, -1);
}

// Test that preformatted input immediately gets parsed in the session
TEST_F(SessionTestFix, PreFormattedInputTest) {
    char incoming_request[1024] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
    strcpy(sesh->data_, incoming_request);
    int read_ret = sesh->handle_read(boost::system::error_code(), strlen(sesh->data_));
    EXPECT_EQ(read_ret, 0);
}

// Test that the session formats the input if necessary before parsing
TEST_F(SessionTestFix, NonPreFormattedInputTest) {
    char incoming_request[1024] = "GET / HTTP/1.1\nHost: www.w3.org/pub/WWW/TheProject.html\n\n";
    strcpy(sesh->data_, incoming_request);
    int read_ret = sesh->handle_read(boost::system::error_code(), strlen(sesh->data_));
    EXPECT_EQ(read_ret, 0);
}

// Test that a bad request is properly handled
TEST_F(SessionTestFix, BadRequestTest) {
    char incoming_request[1024] = "Bad Request";
    strcpy(sesh->data_, incoming_request);
    int read_ret = sesh->handle_read(boost::system::error_code(), strlen(sesh->data_));
    EXPECT_EQ(read_ret, 1);
}

// Test that the session starts up properly
TEST_F(SessionTestFix, StartTest) {
    char incoming_request[1024] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
    strcpy(sesh->data_, incoming_request);
    sesh->start();
    EXPECT_TRUE(true);
}

// Test that the session closes the socket at the end of writing
TEST_F(SessionTestFix, CloseSocketTest) {
    int write_ret = sesh->handle_write(boost::system::error_code(), 0);
    EXPECT_EQ(write_ret, 0);
}