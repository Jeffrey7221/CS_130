#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "session.h"
#include "http/reply.h"
#include "http/request_parser.h"
#include "http/request.h"
#include "config_parser/nginx_config_parser.h"
#include "request_handler/dispatcher.h"
#include "request_handler/request_handler.h"
#include <tuple>
#include <string.h>

using ::testing::_;
using ::testing::AtLeast;

// A mock of the dispatcher class that handles whether we are looking for static or echo handler
class MockDispatcher : public RequestHandlerDispatcher {
    public:
        MockDispatcher(NginxConfig& config) : RequestHandlerDispatcher(config) {}
        MOCK_METHOD1(dispatch, std::shared_ptr<RequestHandler>(request& req));
};

class SessionTestFix : public :: testing::Test {
    protected:
        session* sesh;
        NginxConfigParser parser;
        MockDispatcher* m_dispatcher;
        boost::asio::io_service io_service;
        void SetUp() override {
            NginxConfig config;
            parser.Parse("./example_configs/example_config", &config);
            m_dispatcher = new MockDispatcher(config);
            sesh = new session(io_service, m_dispatcher);
        }
};

// Test that the session stops when a bad error code is set
TEST_F(SessionTestFix, ErrorCodeTest) {
    boost::system::error_code ec = boost::system::errc::make_error_code(boost::system::errc::not_supported);
    int read_ret = sesh->handle_read(ec, 0);
    EXPECT_EQ(read_ret, -1);

    int write_ret = sesh->handle_write(ec, 0);
    EXPECT_EQ(write_ret, -1);
}

// // Test that preformatted input immediately gets parsed in the session
// TEST_F(SessionTestFix, PreFormattedInputTest) {
//     char incoming_request[1024] = "GET /echo HTTP/1.1\r\n";
//     strcpy(sesh->data_, incoming_request);
//     int read_ret = sesh->handle_read(boost::system::error_code(), strlen(sesh->data_));
//     EXPECT_EQ(read_ret, -1);
// }

// // Test that the session formats the input if necessary before parsing
// TEST_F(SessionTestFix, NonPreFormattedInputTest) {
//     char incoming_request[1024] = "GET /echo HTTP/1.1\n";
//     strcpy(sesh->data_, incoming_request);
//     int read_ret = sesh->handle_read(boost::system::error_code(), strlen(sesh->data_));
//     EXPECT_EQ(read_ret, 0);
// }

// Test that a bad request is properly handled
TEST_F(SessionTestFix, BadRequestTest) {
    char incoming_request[1024] = "Bad Request";
    strcpy(sesh->data_, incoming_request);
    int read_ret = sesh->handle_read(boost::system::error_code(), strlen(sesh->data_));
    EXPECT_EQ(read_ret, 1);
}

// Test that the session starts up properly
TEST_F(SessionTestFix, StartTest) {
    char incoming_request[1024] = "GET /echo HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
    strcpy(sesh->data_, incoming_request);
    sesh->start();
    EXPECT_TRUE(true);
}

// Test that the session closes the socket at the end of writing
TEST_F(SessionTestFix, CloseSocketTest) {
    int write_ret = sesh->handle_write(boost::system::error_code(), 0);
    EXPECT_EQ(write_ret, 0);
}