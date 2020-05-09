// based from 
// https://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio/example/cpp11/http/server/reply.hpp
// reply.hpp
// ~~~~~~~~~
//
// Copyright (c) 2003-2017 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REPLY_H
#define HTTP_REPLY_H

#include <string>
#include <vector>
#include <boost/asio.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "http/header.h"

namespace http {
namespace server {

// a reply to be sent to a client
struct reply {

  // An HTML code indicating success/failure of processing
  enum status_type {
    ok = 200,
    created = 201,
    accepted = 202,
    no_content = 204,
    multiple_choices = 300,
    moved_permanently = 301,
    moved_temporarily = 302,
    not_modified = 304,
    bad_request = 400,
    unauthorized = 401,
    forbidden = 403,
    not_found = 404,
    internal_server_error = 500,
    not_implemented = 501,
    bad_gateway = 502,
    service_unavailable = 503
  } status;

  // A map of header objects (name, value)
  std::vector<header> headers;

  // The content of the response
  std::string content;

  // Convert the reply into a vector of buffers. The buffers do not own the
  // underlying memory blocks, therefore the reply object must remain valid and
  // not be changed until the write operation has completed.
  std::vector<boost::asio::const_buffer> to_buffers();

  // Get a stock reply.
  static reply* stock_reply(status_type status);
};

} // namespace server
} // namespace http

#endif // HTTP_REPLY_H