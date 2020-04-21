// async_tcp_echo_server.cpp
// ~~~~~~~~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#include <cstdlib>
#include <iostream>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "session.h"
#include "server.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"

using boost::asio::ip::tcp;

int main(int argc, char* argv[]) {

  BOOST_LOG_TRIVIAL(info) << "Starting Server";

  try {  
    if (argc != 2) {
      BOOST_LOG_TRIVIAL(fatal) << "Usage: ./server ./nginx_config";
      return 1;
    }

    NginxConfig config;
    short port_num = config.GetPort(argv[1]);
    if (port_num < 0 || port_num > 65535) {
      BOOST_LOG_TRIVIAL(fatal) << "Invalid port number in config file";
      return 2;
    }

    // create and run server
    boost::asio::io_service io_service;

    server s(io_service, port_num);

    io_service.run();
  } catch (std::exception& e) {
    BOOST_LOG_TRIVIAL(error) << "Exception: " << e.what();
  }
  BOOST_LOG_TRIVIAL(info) << "Shutting Down Server";
  return 0;
}
