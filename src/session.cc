// session_class file
// based on
// https://www.boost.org/doc/libs/1_36_0/doc/html/boost_asio/example/echo/async_tcp_echo_server.cpp

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include "session.h"

#include "http/request_parser.h"
#include "http/reply.h"

#include <boost/algorithm/string/replace.hpp>

using boost::asio::ip::tcp;

tcp::socket& session::socket() {
  return socket_;
}

reply session::echo_response(size_t bytes_transferred) {
  reply rep;
  rep.status = reply::ok; // set to http 200
  rep.content = std::string(data_, bytes_transferred); // send request in body of response
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(bytes_transferred);
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = "text/plain"; // Set the content type to text/plain
  return rep;
}

reply session::echo_bad_response(size_t bytes_transferred) {
  reply rep;
  rep.status = reply::bad_request; // set to http 400
  rep.content = std::string(data_, bytes_transferred); // send request in body of response
  rep.headers.resize(2);
  rep.headers[0].name = "Content-Length";
  rep.headers[0].value = std::to_string(bytes_transferred);
  rep.headers[1].name = "Content-Type";
  rep.headers[1].value = "text/plain"; // set the content type to text/plain
  return rep;
}

void session::start() {
  
  socket_.async_read_some(boost::asio::buffer(data_, max_length),
    boost::bind(&session::handle_read, this,
    boost::asio::placeholders::error,
    boost::asio::placeholders::bytes_transferred));
  

}

// based on https://www.boost.org/doc/libs/1_64_0/doc/html/boost_asio/example/cpp11/http/server/connection.cpp
void session::handle_read(const boost::system::error_code& error,
  size_t bytes_transferred) {
  if (!error) {

    bool formatted = false;
    request_parser::result_type result;

    if ( strcmp(data_+strlen(data_)-2, "\r\n") == 0 ) {
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
    
    if (result == request_parser::good) { // the URL is valid
      
      rep = echo_response(bytes_transferred);

      // handle write portion
      boost::asio::async_write(socket_,rep.to_buffers(),
      boost::bind(
        &session::handle_write,this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      ));
    } else if (result == request_parser::bad) { // the URL is invalid
      
      rep = echo_bad_response(bytes_transferred);

      // handle write portion
      boost::asio::async_write(socket_,rep.to_buffers(),
      boost::bind(
        &session::handle_write,this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred
      ));
    } else {
      handle_read(error,bytes_transferred);
    }
  }
}

void session::handle_write(const boost::system::error_code& error,
  size_t bytes_transferred) {
  if (!error) {
    boost::system::error_code ignored_ec;
    socket_.shutdown(boost::asio::ip::tcp::socket::shutdown_both,
      ignored_ec);
  } 
}
