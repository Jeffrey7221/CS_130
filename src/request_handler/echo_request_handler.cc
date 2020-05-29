#include "request_handler/echo_request_handler.h"
#include "logger/logger.h"

RequestHandler* EchoRequestHandler::Init(const NginxConfig& config, const std::string location_path) {
    RequestHandler* handler = new EchoRequestHandler();
    handler->name = "EchoHandler";
    return handler;

}

// creates HTTP reply for all incoming echo requests
std::shared_ptr<reply> EchoRequestHandler::HandleRequest(const request& request_) {

    Logger& logger = Logger::getInstance(); 
    logger.log("Constructing HTTP reply for an echo request", NORMAL);

    size_t content_size = request_.body_.length() - 2; // size of request content
    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());
    rep->code_ = reply::ok; // set to http 200
    rep->body_ = request_.body_.substr(0, content_size); // send request in body of response
    rep->headers_["Content-Length"] = std::to_string(content_size);
    rep->headers_["Content-Type"] = "text/plain";

    return rep;
}  
