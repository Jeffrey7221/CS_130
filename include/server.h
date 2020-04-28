#ifndef SERVER_H
#define SERVER_H    

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"
#include "gtest/gtest.h"
#include "request_handler/dispatcher.h"

using boost::asio::ip::tcp;

class server {
  public:
    server(boost::asio::io_service& io_service, const NginxConfig &config, short port);

    // Make our test cases a friend for access to private variables
    friend class ServerTest_HandleAcceptTest_Test;

  private:
    // create a new session, start accepting requests
    void start_accept();

    // handle error and start accept
    void handle_accept(session* new_session,
      const boost::system::error_code& error);

    boost::asio::io_service& io_service_;

    tcp::acceptor acceptor_;
    RequestHandlerDispatcher* dispatcher_;
};

#endif