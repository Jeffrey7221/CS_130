# Team WorkFromHome Web Server

## Table of contents
* [Structure of Project](#1.-structure-of-project)
* [Building the Project](#2.-building-the-project)
* [Adding Request Handler](#3.-Adding-Request-Handler)

# 1. Structure of Project

Our web server is written in C++, and the following diagram describes the interaction between header files within the project. A short summary of each header file follows.

```
                                                              |----------------------------|
                                                              |      Parse the Config      |
                                                              |   nginx_config_parser.h    |
                                    server_main.cc ---------->|  nginx_config_statement.h  |
                                          |                   |       nginx_config.h       |
logging throughout                        |                   |----------------------------|
the entire application                    |
                                          |                   |----------------------------|
                                          |                   |    Register all handlers   |
                                       server.h ------------->|        dispatcher.h        |
                                          |                   |      request_handler.h     |
                                          |                   |----------------------------|
                        one session per   |                                      |
                        request received  |                                      | user dispatcher to fetch
                                          |                                      |         handler
                                       session.h ---------------> |--------------------------------|
                                          |                       |   User handler to create reply |
                                          |                       |            reply.h             |
                                          |                  |--> |         echo_handler.h         |
                                          |                  |    |        static_handler.h        |
                                          |                  |    |      bad_request_handler.h     |
                              |-------------------------|    |    |        status_handler.h        |
                              | Create a request object |    |    |--------------------------------|
                              |    request_parser.h     |----|
                              |       request.h         |
                              |-------------------------|
                                       
```

## Config Parser

### nginx_config_parser.h 
- Parses a config file and registers all handler blocks and port number to launch the server on

### nginx_config_statement.h
- A helper class for the config and config parser which keeps tracks of tokens of the config file

### nginx_config.h
- A final, parsed config file represented as a class that our server then utilizes the listen on ports and launch handlers

## HTTP

### reply.h
- A reply object used by the server to send responses back to clients. Generated by request handlers. Follows the Common API

### request.h
- A request object used by the server that contains the parsed request sent by client. This request is used to generate a reply from a request handler.

### request_parser.h
- A request parser that ensures requests sent are in HTTP format. 

## Request Handler

### dispatcher.h
- A dispatcher that given a url, will properly identify which handler is registered to the endpoint and return it back to the session. Also registers handlers based on the parsed configuration file.

### request_handler.h
- A base class for all request handlers. Defines the methods that all request handlers need to implement (such as HandleRequest)

### echo_request_handler.h
- Serves echo requests -> sends back the request in the response

### static_request_handler.h
- Serves static requests -> looks for the requested file on the server and sends it back if possible

### bad_request_handler.h
- The request handler returned when no handler can be found. It is currently configured to "/"

### status_handler.h
- Serves the "/status" endpoint which returns health information about our server including number of requests received, status of requests received, and other important information.

## Logging

### logger/logger.h
- Manages the logging object that writes server events for all files. 

## Server

### server.h
- The server code that launches the dispatcher to start creating handlers and also manages the creation of sessions for each request

### session.h
- Responsible for receiving requests, launching the request parser to create request objects, calling the dispatcher to use the handlers to generate proper reply objects from the request, and finally writing the reply back to the client.


# 2. Building the Project

## Building Locally
1. Build the project in a new directory
```
$ mkdir build
$ cd build
$ cmake ..
$ make
```

2. Running the server from within the build directory
```
$ ./bin/server ../config/local_nginx_config
```

3. Manually running the tests after building
```
$ cd build/
$ make test
$ cd ../tests
```
- Then choose which tests you wish to run
```
$ ../build/bin/request_parser_test
$ ../build/bin/request_handler_test
$ ../build/bin/reply_test
$ ../build/bin/server_test
$ ../build/bin/session_test
$ ../build/bin/nginx_config_test
$ ../build/bin/nginx_config_statement_test
$ ../build/bin/nginx_config_parser_test
$ ../build/bin/logger_test
$ ../build/bin/dispatcher_test
```

## Building on Docker

1. Build and tag a base image
```
docker build -f docker/base.Dockerfile -t wfh-web-server:base .
```

2. Build an image for our webserver
```
docker build -f docker/Dockerfile -t wfh_web_server_image .
```
- To build the container for test coverage
```
docker build -f docker/coverage.Dockerfile -t wfh_web_server_tcoverage .
```

3. Run the image for our webserver, mapping the local port 8080 to the container's port 80
```
docker run --rm -p 8080:80 --name my_run wfh_web_server_image:latest
```

4. To stop the currently running Docker image, use the following command in another terminal window
```
docker container stop my_run
```

# 3. Adding Request Handler
### We'll use status handler as an example


1. First declare a handler in the config file located in ./config folder.

```  
location "/static" StaticHandler {
    root /data;
} 
```

2. Create a new class and header file for your new handler and implement those functions in your .cc file which is shown below
```
/* Status Handler Header*/
#ifndef STATUS_HANDLER_H
#define STATUS_HANDLER_H

#include "request_handler.h"

class StatusRequestHandler : public RequestHandler  {

public:
  StatusRequestHandler(const NginxConfig &config);

  static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

  std::shared_ptr<reply> HandleRequest(const request& request_) override;  

private:
  std::string display_content;
  std::string request_handler_info;
  std::string request_info;
};
#endif
```

```
#include "request_handler/status_handler.h"
#include "request_handler/dispatcher.h"
#include <iostream>
#include "logger/logger.h"


StatusRequestHandler::StatusRequestHandler(const NginxConfig &config):
request_handler_info(""), request_info(""),display_content("") {}

RequestHandler* StatusRequestHandler::Init(const NginxConfig& config, const std::string location_path){
 Implementation here. Please reference request_handler/status_handler.cc for more information
}

std::shared_ptr<reply> StatusRequestHandler::HandleRequest(const request& request_) {
 Impementation here. Please reference request_handler/status_handler.cc for more information
}
```

3. In dispatcher.cc, add the type of of handler which in this case is "status" and "StatusHandler" matches our config file as well as add in the header file ``` #include "request_handler/static_request_handler.h" ``` for our handler class

```
RequestHandlerDispatcher::RequestHandlerDispatcher(const NginxConfig& config): config_(config) {
    Logger& logger = Logger::getInstance();
    logger.log("Creating request handlers based on config files", NORMAL);

    for(int i = 0; i < config_.statements_.size(); i++) {
        if(config_.statements_[i]->tokens_[0] == "server" && config_.statements_[i]->tokens_.size() == 1) {
            for(int j = 0; j < config_.statements_[i]->child_block_->statements_.size(); j++) {
                if (config_.statements_[i]->child_block_->statements_[j]->tokens_.size() > 2) {
                    if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "EchoHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "echo");
                    } else if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "StaticHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "static");
                    }else if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "StatusHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "status");
                    }
                }
            }
            
        }
        
    }
}
```

4. In Cmake, link this class to our request_handler
```
add_library(request_handler_lib src/request_handler/echo_request_handler.cc 
  src/request_handler/static_request_handler.cc src/request_handler/dispatcher.cc 

  # new class linked

  src/request_handler/status_handler.cc)
```

5. Add Unit Tests! (Optional, but advised and recommended to do). Add tests for tests/dispatcher_test.cc and tests/request_handler_test.cc


```
#dispatcher_test.cc

TEST_F(DispatcherTestFix, ProperStatusHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/status";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}
```

```
#request_handler_test.cc
TEST_F(RequestHandlerTestFix, StatusHandler) {

  char incoming_request[1024] = "GET /index.html HTTP/1.1\r\n\r\n";
  char incoming_request_status[1024] = "GET /status HTTP/1.1\r\n\r\n";
  std::string static_path = "/static/";
  std::string static_root = "/tests/static_data/";
  StaticRequestHandler static_handler_(out_config, static_path, static_root);
  StatusRequestHandler status_handler_(out_config);

  std::tie(request_parser_result_, std::ignore) =
  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  reply_ = static_handler_.HandleRequest(request_);

  std::tie(request_parser_result_, std::ignore) =
  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  reply_ = static_handler_.HandleRequest(request_);

  std::tie(request_parser_result_, std::ignore) =
  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  reply_ = static_handler_.HandleRequest(request_);

  std::tie(request_parser_result_, std::ignore) =
  request_parser_.parse(request_, incoming_request_status, incoming_request_status + strlen(incoming_request_status));
  reply_ = status_handler_.HandleRequest(request_);

  EXPECT_EQ(reply_->code_, http::server::reply::ok);
  EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(reply_->body_.size()));
}
```


