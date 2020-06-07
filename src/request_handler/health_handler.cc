#include "request_handler/health_handler.h"
#include "logger/logger.h"

// purpose of health handler is to make sure our server is up 
// For notifications, check GCP for more information to get notified when server is down
RequestHandler* HealthHandler::Init(const NginxConfig& config, const std::string location_path) {
    RequestHandler* handler = new HealthHandler();
    handler->name = "HealthHandler";
    return handler;
}

// creates HTTP reply for Health Ping 
// GCP will check this endpoint to make sure our server is up
std::shared_ptr<reply> HealthHandler::HandleRequest(const request& request_) {

    Logger& logger = Logger::getInstance(); 
    logger.log("Health Ping, Sending 200 OK", NORMAL);

    std::string health_ok = "OK";

    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());

    rep->code_ = reply::ok;
    rep->body_ = health_ok;
    rep->headers_["Content-Length"] = std::to_string(health_ok.size());
    rep->headers_["Content-Type"] = "text/plain";
    return rep;
}
