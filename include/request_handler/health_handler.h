#ifndef HEALTH_HANDLER_H
#define HEALTH_HANDLER_H

#include "request_handler/request_handler.h"

using http::server::reply;

// Return 200 OK as Health Ping
class HealthHandler : public RequestHandler {
    public:
        static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

        // derived from base class RequestHandler
        std::shared_ptr<http::server::reply> HandleRequest(const request& request_) override;  
};

#endif