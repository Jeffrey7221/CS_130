// server_class file

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "session.h"
#include "server.h"
#include "logger/logger.h"

using boost::asio::ip::tcp;

server::server(boost::asio::io_service& io_service, short port)
  : io_service_(io_service),
  acceptor_(io_service, tcp::endpoint(tcp::v4(), port)) {
  start_accept();
}

void server::start_accept() {

  session* new_session = new session(io_service_);
  acceptor_.async_accept(new_session->socket(),
    boost::bind(&server::handle_accept, this, new_session,
    boost::asio::placeholders::error));
}

void server::handle_accept(session* new_session,
  const boost::system::error_code& error) {
  Logger& logger = Logger::getInstance();
  if (!error) {
    logger.log("New session connected to server.", NOTIFICATION);
    new_session->start();
  } else {
    logger.log(std::string("Server could not accept session: ") + error.message(), ERROR);
  }
  
  start_accept();
}