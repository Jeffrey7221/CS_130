// server_class file

#include <cstdlib>
#include <iostream>
#include <boost/bind.hpp>
#include <boost/asio.hpp>
#include <boost/log/trivial.hpp>
#include "session.h"
#include "server.h"
#include "logger/logger.h"
#include <thread>         // std::thread
#include <vector>

using boost::asio::ip::tcp;
using namespace std;

void server::run(){ //multithread function which creates threads based on server architecture, runs each thread and then eventually joins at the end
  // we use a vector to store the threads and then iterate through this vector to eventually join
  Logger& logger = Logger::getInstance();
  unsigned int num_threads = std::thread::hardware_concurrency(); //number of threads on machine
  vector<thread*>  thread_vector;
  for(int i = 0; i < num_threads; i++){
    thread_vector.push_back(new thread(boost::bind(&boost::asio::io_service::run, &io_service_)));
    logger.log("New Thread Created with ID: " + std::to_string(i), NOTIFICATION);
  }
  for(int i = 0; i < num_threads; i++){ // join threads
    thread_vector[i]->join();
  }

}

server::server(boost::asio::io_service& io_service, const NginxConfig &config, short port)
  : io_service_(io_service),
  acceptor_(io_service, tcp::endpoint(tcp::v4(), port)),
  dispatcher_(new RequestHandlerDispatcher(config)) {
  start_accept();
}

void server::start_accept() {

  session* new_session = new session(io_service_, dispatcher_);
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