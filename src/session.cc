// session_class file
// based on
// https://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/echo/async_tcp_echo_server.cpp

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include <boost/algorithm/string/replace.hpp>
#include "session.h"
#include "http/request_parser.h"
#include "http/reply.h"
#include "logger/logger.h"

using boost::asio::ip::tcp;

tcp::socket& session::socket() {
  return socket_;
}


void session::start() {
  
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
    boost::bind(&session::handle_read, this,
    boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred));
  
}

// based on https://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
int session::handle_read(const boost::system::error_code& error, size_t bytes_transferred) {
  Logger& logger = Logger::getInstance();
  if (!error) {
    bool formatted = false;
    request_parser::result_type result;

    if (strcmp(data_+strlen(data_) - 2, "\r\n") == 0) {
      // string is already formatted so don't format it
      std::tie(result, std::ignore) = request_parser_.parse(request_, data_, data_ + bytes_transferred);

    } else {
      // format string
      std::string data = data_;
      boost::replace_all(data, "\n", "\r\n");
      data.append("\r\n");
      strcpy(data_, data.c_str());

      std::tie(result, std::ignore) = request_parser_.parse(request_, data_, data_ + strlen(data_));
    }
    
    std::string d = data_;
    unsigned first = d.find('\r');
    std::string first_line = d.substr(0, first);
    
    try { logger.logRequest(request_, socket_, NOTIFICATION); } catch (...) {}
    logger.log("First line of request: " + first_line, NORMAL);

    if (result == request_parser::good) { // the URL is valid
      logger.log("OK request received.", NORMAL);

      // get request handler
      handler = dispatcher_->dispatch(request_);

      if (handler == NULL) {
        rep = std::shared_ptr<reply>(reply::stock_reply(reply::not_found));
      } else {
        // use request handler to create HTTP reply
        rep = handler->HandleRequest(request_);
      }

      // handle write portion
      boost::asio::async_write(socket_,rep->to_buffers(),
      boost::bind(
        &session::handle_write,this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      ));
      return 0;
    } else if (result == request_parser::bad) { // the URL is invalid
      logger.log("Bad request received.", NORMAL);
      rep = std::shared_ptr<reply>(reply::stock_reply(reply::bad_request));
      // handle write portion
      boost::asio::async_write(socket_,rep->to_buffers(),
      boost::bind(
        &session::handle_write,this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      ));
      return 1;
    } else {
      handle_read(error,bytes_transferred);
    }
  } else {
    logger.log(std::string("Session read error: ") + error.message(), ERROR);
  }
  return -1;
}

int session::handle_write(const boost::system::error_code& error, size_t bytes_transferred) {
  Logger& logger = Logger::getInstance();
  if (!error) {
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both, ignored_ec);
    logger.log("Response sent to connected session. Session now closed.", NORMAL);
    return 0;
  } else {
    logger.log(std::string("Session write error: ") + error.message(), ERROR);
  }

  return -1;
}
