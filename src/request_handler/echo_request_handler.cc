#include "request_handler/echo_request_handler.h"
#include "logger/logger.h"

RequestHandler* EchoRequestHandler::create(const NginxConfig& config, const std::string location_path) {
    return new EchoRequestHandler();
}

// creates HTTP reply for all incoming echo requests
std::shared_ptr<reply> EchoRequestHandler::HandleRequest(const request& request_, char data_[1024]) {
    Logger& logger = Logger::getInstance();

    size_t content_size = strlen(data_) - 2; // size of request content
    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());
    rep->status = reply::ok; // set to http 200
    rep->content = std::string(data_, content_size); // send request in body of response
    rep->headers.resize(2);
    rep->headers[0].name = "Content-Length";
    rep->headers[0].value = std::to_string(content_size);
    rep->headers[1].name = "Content-Type";
    rep->headers[1].value = "text/plain"; // set the content type to text/plain

    return rep;
}  

