#ifndef MARKDOWN_REQUEST_HANDLER_H
#define MARKDOWN_REQUEST_HANDLER_H

#include "request_handler/request_handler.h"
#include <boost/filesystem.hpp>
#include <boost/filesystem/fstream.hpp>
#include <boost/algorithm/string/replace.hpp>

#include <stdlib.h>
#include <ctype.h>

using http::server::reply;

// Request handler for Markdown Requests
class MarkdownHandler : public RequestHandler {
    public:
        static RequestHandler* Init(const NginxConfig& config, const std::string location_path);

        // derived from base class RequestHandler
        std::shared_ptr<http::server::reply> HandleRequest(const request& request_) override; 

        std::string name = "MarkdownHandler"; 
    
    private:
        // Create the initial HTML input form for users
        std::shared_ptr<http::server::reply> createForm(const request& request_);

        // Render the provided markdown into an HTML page and serve to user
        std::shared_ptr<http::server::reply> viewMarkdown(const request& request_);

        void urldecode2(char *dst, const char *src);
};

#endif