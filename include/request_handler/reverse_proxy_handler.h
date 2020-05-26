#ifndef REVERSE_PROXY_HANDLER_H
#define REVERSE_PROXY_HANDLER_H

#include "request_handler.h"

using http::server::reply;

// Request handler for reverse proxy requests
class ReverseProxyHandler : public RequestHandler {
    public:
        ReverseProxyHandler(const std::string& location_path, const std::string& proxy_dest, int proxy_port);
        static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

        // derived from base class RequestHandler
        std::shared_ptr<http::server::reply> HandleRequest(const request& request_) override;
    
    private:
        std::string uri_replace(const std::string& uri, const std::string& proxy_host, const std::string& local_path);
        void append_relative_URI(std::string& body, const std::string& append);
        std::string location_path_;
        std::string proxy_dest_;  // Address for proxy destination
        int proxy_port_;  // Port number for connecting to the proxy destination
};

#endif