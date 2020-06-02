#include <iostream>
#include "request_handler/static_request_handler.h"
#include "logger/logger.h"

#include <memory>
#include <string>

#include "maddy/parser.h"

StaticRequestHandler::StaticRequestHandler(const NginxConfig& config, const std::string location_path, const std::string root) :
    config_(config), location_path_(location_path), root_(root) {}

// Create new static request handler from config file statement
RequestHandler* StaticRequestHandler::Init(const NginxConfig& config, const std::string location_path) {

    // find corresponding root path for static request handler
    std::string root = "/";

    for(int i = 0; i < config.statements_.size(); i++) {
        if(config.statements_[i]->tokens_.size() == 2 && config.statements_[i]->tokens_[0] == "root") {
            root = config.statements_[i]->tokens_[1];
        }
    }
    if (root.back() != '/') {
        root.append("/");
    }

    RequestHandler* handler = new StaticRequestHandler(config, location_path, root);
    handler->name = "StaticHandler";
    return handler; 

     
}

// construct HTTP reply for static requests and serve static file
std::shared_ptr<reply> StaticRequestHandler::HandleRequest(const request& request_) {
    
    Logger& logger = Logger::getInstance();

    // create relative path to requested file 
    std::string new_uri = request_.uri_;
    
    // replace location prefix with root path
    new_uri.replace(0, location_path_.size(), root_);
    new_uri.replace(0, 1, "../");

    logger.log("Looking for static file: " + new_uri, NORMAL);
    
    // if requested file is not found, return a "404 Not Found" error
    boost::filesystem::path boost_path(new_uri);
    if (!boost::filesystem::exists(new_uri) || !boost::filesystem::is_regular_file(new_uri)) {
        std::shared_ptr<reply> rep = std::shared_ptr<reply>(reply::stock_reply(reply::not_found));
        return rep;
    }

    // connect to requested file
    std::ifstream f(new_uri.c_str(), std::ios::in | std::ios::binary);
    if (!f) {
        std::shared_ptr<reply> rep = std::shared_ptr<reply>(reply::stock_reply(reply::not_found));
        return rep;
    }

    // read the contents of the requested file
    std::string content_;
    char c;
    while (f.get(c)) content_ += c;
    f.close();

    logger.log("Static file has been found and read, constructing HTTP reply", NORMAL);

    // choose content type
    std::string content_type = setContentType(new_uri);
    if (content_type == "text/html" || content_type == "text/plain") {
        boost::replace_all(content_, "\n", "\r\n");
        content_.append("\r\n");
    }

    // create HTTP reply with the file contents as the reply body
    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());
    rep->code_ = reply::ok; // set to http 200
    rep->headers_["Content-Length"] = std::to_string(content_.length());
    rep->headers_["Content-Type"] = content_type;
    rep->body_ = content_;

    return rep;
}

// uses extension type of file to set the content type in reply header
// can expand later to support further extensions
std::string StaticRequestHandler::setContentType(std::string path) {
    int dotIndex = path.find_last_of(".");
    std::string extension = path.substr(dotIndex + 1);

    std::string extension_type = "";
    if (extension == "html") {
		extension_type = "text/html";
	} else if (extension == "jpg") {
		extension_type = "image/jpeg";
	} else if (extension == "zip") {
		extension_type = "application/zip";
	} else if (extension == "txt") {
		extension_type = "text/plain";
	} else if (extension == "gif") {
		extension_type = "image/gif";
	} else if (extension == "png") {
		extension_type = "image/png";
	}
    return extension_type;
}