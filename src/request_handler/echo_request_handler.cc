#include "request_handler/echo_request_handler.h"
#include "logger/logger.h"

RequestHandler* EchoRequestHandler::Init(const NginxConfig& config) {
    return new EchoRequestHandler();
}

// creates HTTP reply for all incoming echo requests
std::shared_ptr<reply> EchoRequestHandler::HandleRequest(const request& request_) {

    Logger& logger = Logger::getInstance(); 
    logger.log("Constructing HTTP reply for an echo request", NORMAL);

    size_t content_size = request_.data_.length() - 2; // size of request content
    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());
    rep->status = reply::ok; // set to http 200
    rep->content = request_.data_.substr(0, content_size); // send request in body of response
    rep->headers.resize(2);
    rep->headers[0].name = "Content-Length";
    rep->headers[0].value = std::to_string(content_size);
    rep->headers[1].name = "Content-Type";
    rep->headers[1].value = "text/plain";

    return rep;
}  

