#ifndef REDIRECT_HANDLER_H
#define REDIRECT_HANDLER_H

#include "request_handler.h"

using http::server::reply;

// Request handler for redirect requests
class RedirectHandler : public RequestHandler {
    public:
        RedirectHandler(const std::string& host);
        static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

        // derived from base class RequestHandler
        std::shared_ptr<http::server::reply> HandleRequest(const request& request_) override;
    
    private:
        std::string host_;
};

#endif