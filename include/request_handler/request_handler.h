#ifndef REQUEST_HANDLER_H
#define REQUEST_HANDLER_H

#include "../http/request.h"
#include "../http/reply.h"
#include "../config_parser/nginx_config.h"
#include "../config_parser/nginx_config_statement.h"


using http::server::request;
using http::server::reply;

// Create an abstract Request Handler class 
//  Used for EchoRequestHandler and StaticRequestHandler
class RequestHandler {
    public:
        // Pure virtual function that will be defined in derived classes
        virtual std::shared_ptr<reply> HandleRequest(const request& request_) = 0;
};

#endif