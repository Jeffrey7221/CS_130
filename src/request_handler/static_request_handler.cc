#include "request_handler/static_request_handler.h"
#include "logger/logger.h"

#include <iostream>

StaticRequestHandler::StaticRequestHandler(const NginxConfig& config, const std::string location_path, const std::string root) :
            config_(config), location_path_(location_path), root_(root) {}

// Create new static request handler from config file statement
RequestHandler* StaticRequestHandler::create(const NginxConfig& config, const std::string location_path) {

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
    return new StaticRequestHandler(config, location_path, root);
}

// construct HTTP reply for static requests and serve static file
std::shared_ptr<reply> StaticRequestHandler::HandleRequest(const request& request_, char data_[1024]) {
    
    Logger& logger = Logger::getInstance();

        // create relative path to requested file 
    std::string new_uri = request_.uri;
    // replace location prefix with root path
    new_uri.replace(0, location_path_.size(), root_);
    new_uri.replace(0, 1, "../");

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

    // formatting new lines properly
    boost::replace_all(content_, "\n", "\r\n");
    content_.append("\r\n");

    // create HTTP reply with the file contents as the reply body
    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());
    rep->status = reply::ok; // set to http 200
    rep->content = content_;
    rep->headers.resize(2);
    rep->headers[0].name = "Content-Length";
    rep->headers[0].value = std::to_string(content_.length());
    rep->headers[1].name = "Content-Type";
    rep->headers[1].value = setContentType(new_uri); 

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