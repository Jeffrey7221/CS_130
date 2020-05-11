#ifndef ECHO_REQUEST_HANDLER_H
#define ECHO_REQUEST_HANDLER_H

#include "request_handler/request_handler.h"

using http::server::reply;

// Request handler for echo requests

class EchoRequestHandler : public RequestHandler {
    public:
        static RequestHandler* Init(const NginxConfig& config);

            // derived from base class RequestHandler
        std::shared_ptr<http::server::reply> HandleRequest(const request& request_) override;  
};

#endif