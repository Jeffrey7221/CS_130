#ifndef NOT_FOUND_REQUEST_HANDLER_H
#define NOT_FOUND_REQUEST_HANDLER_H

#include "request_handler/request_handler.h"

using http::server::reply;

// Request handler for 404 Not Found
class NotFoundRequestHandler : public RequestHandler {
    public:
        static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

        // derived from base class RequestHandler
        std::shared_ptr<http::server::reply> HandleRequest(const request& request_) override;  
};

#endif