#include "request_handler/status_handler.h"
#include "request_handler/dispatcher.h"
#include <iostream>
#include "logger/logger.h"


StatusRequestHandler::StatusRequestHandler(const NginxConfig &config):
request_handler_info(""), request_info(""),display_content("") {}

RequestHandler* StatusRequestHandler::Init(const NginxConfig& config, const std::string location_path){
    return new StatusRequestHandler(config);
}

std::shared_ptr<reply> StatusRequestHandler::HandleRequest(const request& request_) {
    Logger& logger = Logger::getInstance();

    // Create HTML for Request Handlers
    logger.log("Looking for Mapping between Request Code and Request URL", NORMAL);

    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply()); //create new reply
    rep->code_ = reply::ok; 

    // map this url to a response code for status Request Handler
    RequestHandlerDispatcher::request_code_received_[request_.uri_].push_back(rep->code_);

    request_info = ""; // need to clear this variable due to being a static variable

    // Get Mapping of url to response code
    for (auto ite = RequestHandlerDispatcher::request_code_received_.begin(); ite != RequestHandlerDispatcher::request_code_received_.end(); ite++){
        for(int j = 0; j <  (ite->second).size(); j++){
            request_info += "<tr><td>" + ite->first +"</td><td>" + std::to_string((ite->second)[j]) + "</td></tr>";
        }
    }

    logger.log("Looking for Mapping between Type of Request Handler and URL Prefix", NORMAL);

    // Get mapping of number of request handlers and their urls
    request_handler_info = ""; // need to clear this variable due to being a static variable
    for (auto ite = RequestHandlerDispatcher::request_handler_uri.begin(); ite != RequestHandlerDispatcher::request_handler_uri.end(); ite++){
        request_handler_info += "<tr><td>" + ite->first +"</td><td>" + ite->second+ "</td></tr>";
    }

    logger.log("Constructing Status Handler HTML Page", NORMAL);

    std::string display_content = "<html>"
        "<body><h2>Status Handler</h1><div>""</div>"
        "<table>""<tr><th>Total Number of Requests</th></tr>""<tr><td>" + std::to_string(RequestHandlerDispatcher::num_requests) + "</td></tr>""</table>"
        "<table>""<tr><th>Requests Server Received : </th><th>Response Code</th></tr>" + request_info + "</table>"
        "<table>""<tr><th>URL Prefixes : </th><th>Request Handlers Received</th></tr>" + request_handler_info + "</table>"
        "</body>"
        "</html>";

    rep->body_ = display_content;
    rep->headers_["Content-Length"] = std::to_string((rep->body_).length());
    rep->headers_["Content-Type"] = "text/html";

    RequestHandlerDispatcher::request_code_received_[request_.uri_].pop_back(); //push and pop for the sake of the HTML display

    return rep;
}
