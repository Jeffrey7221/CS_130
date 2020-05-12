#include "request_handler/bad_request_handler.h"
#include "logger/logger.h"


RequestHandler* BadRequestHandler::Init(const NginxConfig& config, const std::string location_path) {
    return new BadRequestHandler();
}

// creates HTTP reply for all incoming echo requests
std::shared_ptr<reply> BadRequestHandler::HandleRequest(const request& request_) {

    Logger& logger = Logger::getInstance(); 
    logger.log("Constructing a 404 Not Found HTTP reply for bad echo request", NORMAL);
    
    const char not_found[] =
  "<html>"
  "<head><title>Not Found</title></head>"
  "<body><h1>404 Not Found</h1></body>"
  "</html>";

    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());

    std::string data = not_found;
    boost::replace_all(data, "\n", "\r\n");
    data.append("\r\n");
    rep->code_ = reply::not_found; // set to http 404
    rep->body_ = data; // send request in body of response
    rep->headers_["Content-Length"] = std::to_string(data.size());
    rep->headers_["Content-Type"] = "text/plain";
    return rep;
}  
