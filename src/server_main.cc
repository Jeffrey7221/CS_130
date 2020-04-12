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
#include "session.h"
#include "server.h"
#include "config_parser/config_parser.h"

using boost::asio::ip::tcp;

// UPDATE: may need to change later to handle more configurations
int get_config_settings(char* config_file) {
  int port_num = -1;

  // parse out server configuration
  NginxConfigParser config_parser;
  NginxConfig config;
  config_parser.Parse(config_file, &config);

  try {
    port_num = std::stoi(config.GetConfig("listen"));
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return port_num;
}

int main(int argc, char* argv[]) {
  try {
    if (argc != 2) {
      std::cerr << "Usage: async_tcp_echo_server <path to config file>\n";
      return 1;
    }

    int port_num = get_config_settings(argv[1]);
    if (port_num < 1024 || port_num > 65535) {
      std::cerr << "Invalid port number in config file\n";
      return 2;
    }

    // create and run server
    boost::asio::io_service io_service;

    server s(io_service, port_num);

    io_service.run();
  } catch (std::exception& e) {
    std::cerr << "Exception: " << e.what() << "\n";
  }

  return 0;
}
