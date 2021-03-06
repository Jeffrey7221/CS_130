#include "request_handler/not_found_handler.h"
#include "logger/logger.h"


RequestHandler* NotFoundRequestHandler::Init(const NginxConfig& config, const std::string location_path) {
    RequestHandler* handler = new NotFoundRequestHandler();
    handler->name = "NotFoundHandler";
    return handler;
}

// creates HTTP reply for 404 requests
std::shared_ptr<reply> NotFoundRequestHandler::HandleRequest(const request& request_) {

    Logger& logger = Logger::getInstance(); 
    logger.log("Constructing a 404 Not Found HTTP reply for bad echo request", NORMAL);

    std::string not_found =
        "<html>"
        "<head><title>Not Found</title></head>"
        "<body><h1>404 Not Found</h1></body>"
        "</html>";

    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());

    rep->code_ = reply::not_found; // set to http 404
    rep->body_ = not_found; // send request in body of response
    rep->headers_["Content-Length"] = std::to_string(not_found.size());
    rep->headers_["Content-Type"] = "text/html";
    return rep;
}
