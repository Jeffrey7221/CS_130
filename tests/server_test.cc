#include "gtest/gtest.h"
#include "http/reply.h"
#include "session.h"
#include "gmock/gmock.h"
#include "server.h"
#include <boost/asio.hpp>
#include <stdio.h>

using boost::asio::ip::tcp;
using http::server::reply;
using ::testing::AtLeast; 
using ::testing::_;

// A mock session object so we don't deal with actual session functionality
class MockSession : public session {
    public:
        boost::asio::io_service& io_service;
        MockSession(boost::asio::io_service& m_io_service) : session(m_io_service), io_service(m_io_service) {}
        MOCK_METHOD0(start, void());
};

// A mock of the io_service since we do not test io_service since it is from a library
class MockIOService : public boost::asio::io_service {
    public:
        MockIOService() : io_service() {}
        MOCK_METHOD0(run, void());
};

// Test that Handle Accept properly starts a new session
TEST(ServerTest, HandleAcceptTest){
    MockIOService m_io_service;
    MockSession m_session(m_io_service);
    EXPECT_CALL(m_session, start()).Times(AtLeast(1));
    short port = 8080;
    server m_server(m_session.io_service, port);
    m_server.handle_accept(&m_session, boost::system::error_code());
}