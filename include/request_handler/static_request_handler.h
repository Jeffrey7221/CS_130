#ifndef STATIC_REQUEST_HANDLER_H
#define STATIC_REQUEST_HANDLER_H

#include "request_handler.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/replace.hpp>

using http::server::reply;

// Request handler for static file requests
class StaticRequestHandler : public RequestHandler {
    public:
        StaticRequestHandler(const NginxConfig& config, const std::string location_path, const std::string root);
        static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

        // derived from base class RequestHandler
        std::shared_ptr<reply> HandleRequest(const request& request_) override;  
    
    private:
        // uses the extension of file to set the content type in response header
        std::string setContentType(std::string extension);
        
        NginxConfig config_;
        std::string location_path_;
        std::string root_;
};

#endif