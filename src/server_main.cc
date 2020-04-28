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
#include "logger/logger.h"

using boost::asio::ip::tcp;

void handleClose(int s) {
  Logger& logger = Logger::getInstance();
  logger.log("Server shut down from SIGINT", NORMAL);
  exit(1);
}

int main(int argc, char* argv[]) {

  Logger& logger = Logger::getInstance();
  logger.log("Server start up", NORMAL);

  // Handler to detect server shutdown
  struct sigaction sigIntHandler;
  sigIntHandler.sa_handler = handleClose;
  sigemptyset(&sigIntHandler.sa_mask);
  sigIntHandler.sa_flags = 0;
  sigaction(SIGINT, &sigIntHandler, NULL);

  try {  
    if (argc != 2) {
      logger.log("Server started with improper arguements. Usage: ./server ./nginx_config", CRITICAL);
      return 1;
    }

    NginxConfig config;
    short port_num = config.GetPort(argv[1]);
    if (port_num < 0 || port_num > 65535) {
      logger.log("Invalid port provided: " + std::to_string(port_num), CRITICAL);
      return 2;
    }
    logger.log("Valid configuration received. Server initialization sequence started.", NORMAL);
    // create and run server
    boost::asio::io_service io_service;

    server s(io_service, config, port_num);
    logger.log("Server initialized and listening on port: " + std::to_string(port_num), NORMAL);
    io_service.run();
  } catch (std::exception& e) {
    logger.log("Port binding issue. Address already in use", ERROR);
  }
  return 0;
}
