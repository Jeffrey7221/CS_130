// https://www.boost.org/doc/libs/1_69_0/doc/html/boost_asio/example/cpp11/http/server/request_parser.hpp based off of
//
// request_parser.hpp
// ~~~~~~~~~~~~~~~~~~
//
// Copyright (c) 2003-2018 Christopher M. Kohlhoff (chris at kohlhoff dot com)
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//

#ifndef HTTP_REQUEST_PARSER_H
#define HTTP_REQUEST_PARSER_H

#include <tuple>
#include <map>
#include "request.h"

namespace http {
namespace server {

struct request;

// parser for incoming requests
class request_parser {
  public:
    // construct ready to parse the request method
    request_parser();

    // reset to initial parser state
    void reset();

    // result of parse
    enum result_type { good, bad, indeterminate };

    // Parse some data. The enum return value is good when a complete request has
    // been parsed, bad if the data is invalid, indeterminate when more data is
    // required. The InputIterator return value indicates how much of the input
    // has been consumed.
    template <typename InputIterator>
      std::tuple<result_type, InputIterator> parse(request& req,
      InputIterator begin, InputIterator end) {
      
      req.body_ = begin;
      parse_method(req, begin);

      while (begin != end) {
        result_type result = consume(req, *begin++);
        if (result == good || result == bad) {
          // begin is the remaining body of the request
          return std::make_tuple(result, begin);
        }
      }
      return std::make_tuple(indeterminate, begin);
    }

  private:
    // set the http method of the request
    void parse_method(request& req, char *input);

    // handle the next character of input
    result_type consume(request& req, char input);
    std::string temp_key;
    std::string temp_value;

    // check if a byte is an HTTP character
    static bool is_char(int c);

    // check if a byte is an HTTP control character
    static bool is_ctl(int c);

    // check if a byte is defined as an HTTP tspecial character
    static bool is_tspecial(int c);

    // check if a byte is a digit
    static bool is_digit(int c);

    // the current state of the parser
    enum state {
      method_start,
      method,
      uri,
      http_version_h,
      http_version_t_1,
      http_version_t_2,
      http_version_p,
      http_version_slash,
      http_version_major_start,
      http_version_major,
      http_version_minor_start,
      http_version_minor,
      expecting_newline_1,
      header_line_start,
      header_lws,
      header_name,
      space_before_header_value,
      header_value,
      expecting_newline_2,
      expecting_newline_3
    } state_;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_PARSER_H