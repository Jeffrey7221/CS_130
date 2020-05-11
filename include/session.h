// session header based off 
// https://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio/example/cpp11/http/server/connection.hpp

#ifndef SESSION_H
#define SESSION_H  

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "gtest/gtest.h"

#include "http/request.h"
#include "http/request_parser.h"
#include "http/header.h"
#include "http/reply.h"

#include "request_handler/dispatcher.h"
#include "request_handler/echo_request_handler.h"
#include "request_handler/static_request_handler.h"

using boost::asio::ip::tcp;
using http::server::request;
using http::server::request_parser;
using http::server::reply;

class session {
  public:
    session(boost::asio::io_service& io_service, RequestHandlerDispatcher* dispatcher)
      : socket_(io_service), dispatcher_(dispatcher) {};

    tcp::socket& socket();

    virtual void start();

    // Make our tests class a friend so we can access private methods
    friend class SessionTestFix_EchoResponseTest_Test;
    friend class SessionTestFix_EchoBadResponseTest_Test;
    friend class SessionTestFix_ErrorCodeTest_Test;
    friend class SessionTestFix_PreFormattedInputTest_Test;
    friend class SessionTestFix_NonPreFormattedInputTest_Test;
    friend class SessionTestFix_BadRequestTest_Test;
    friend class SessionTestFix_CloseSocketTest_Test;
    friend class SessionTestFix_StartTest_Test;


    enum { max_length = 1024 };
    char data_[1024];


  private:

    tcp::socket socket_;
    
    //Return value for handle_read and handle_write: 
    // 0 if good
    // 1 if bad input
    // -1 if socket connection error

    // reading in of bytes, handles HTTP formatting
    int handle_read(const boost::system::error_code& error,size_t bytes_transferred);

    // writing out of message
    int handle_write(const boost::system::error_code& error,size_t bytes_transferred);

    // The incoming request.
    request request_;

    // The parser for the incoming request.
    request_parser request_parser_;

    // The reply to be sent back to the client.
    std::shared_ptr<reply> rep;

    std::shared_ptr<RequestHandler> handler;

    RequestHandlerDispatcher* dispatcher_;

};

#endif