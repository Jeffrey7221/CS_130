// https://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio/example/cpp11/http/server/request.hpp
//
// request.hpp
// ~~~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <vector>
#include "http/header.h"

namespace http {
namespace server {

// A request received from a client.
struct request {

  // The HTML method (GET, PUT, POST, etc)
  std::string method;

  // The path of the request
  std::string uri;

  // A map of header objects (name, value)
  std::vector<header> headers;

  // The content of the request
  std::string body_;

  // Entire request recieved
  std::string data_;

  int http_version_major;
  int http_version_minor;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP