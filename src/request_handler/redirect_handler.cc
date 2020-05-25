#include "request_handler/redirect_handler.h"
#include "logger/logger.h"
#include <string>

RedirectHandler::RedirectHandler(const std::string& host) : host_(host) {}

RequestHandler* RedirectHandler::Init(const NginxConfig& config, const std::string location_path) {
    Logger& logger = Logger::getInstance();
    std::string host = "";
    int port = 0;
    for(int i = 0; i < config.statements_.size(); i++) {
        logger.log("Key inside config loop: " + config.statements_[i]->tokens_[0], NOTIFICATION);
        logger.log("Value inside config loop: " + config.statements_[i]->tokens_[1], NOTIFICATION);
        if (config.statements_[i]->tokens_.size() == 2 && config.statements_[i]->tokens_[0] == "host") {
            host = config.statements_[i]->tokens_[1];
            logger.log("Host before replace all: " + host, NOTIFICATION);
            boost::replace_all(host, "/", "");
            logger.log("Host after replace all: " + host, NOTIFICATION);
        } 
    }
    logger.log("Host for redirect is: " + host, NOTIFICATION);
    return new RedirectHandler(host);
}

std::shared_ptr<http::server::reply> RedirectHandler::HandleRequest(const request& request_) {
    Logger& logger = Logger::getInstance();

    logger.log("Host for redirect is: " + host_, NOTIFICATION);
    logger.log("Crafting a reply that redirects to /static/index.html", NOTIFICATION);
    std::shared_ptr<http::server::reply> reply = std::make_shared<http::server::reply>();

    reply->code_ = http::server::reply::moved_temporarily;
    reply->headers_["Location"] = "http://" + host_ + "/static/index.html";
    reply->body_ = "HTTP/1.1 302 Found\r\nLocation: " + reply->headers_["Location"] + "\r\n\r\n";

    return reply;
}