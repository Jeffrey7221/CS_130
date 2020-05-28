#include "request_handler/bad_request_handler.h"
#include "logger/logger.h"


RequestHandler* BadRequestHandler::Init(const NginxConfig& config, const std::string location_path) {
    return new BadRequestHandler();
}

// creates HTTP reply for malformed requests
std::shared_ptr<reply> BadRequestHandler::HandleRequest(const request& request_) {

    Logger& logger = Logger::getInstance(); 
    logger.log("Constructing a 400 bad request HTTP reply for malformed requests", NORMAL);

    std::string bad_request =
        "<html>"
        "<head><title>Bad Request</title></head>"
        "<body><h1>400 Bad Request</h1></body>"
        "</html>";

    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());

    rep->code_ = reply::bad_request; // set to http 400
    rep->body_ = bad_request; // send request in body of response
    rep->headers_["Content-Length"] = std::to_string(bad_request.size());
    rep->headers_["Content-Type"] = "text/html";
    return rep;
}
