#include "request_handler/status_handler.h"
#include "request_handler/dispatcher.h"
#include <iostream>
#include "logger/logger.h"


StatusRequestHandler::StatusRequestHandler(const NginxConfig &config):
request_handler_info(""), request_info(""),display_content("") {}

RequestHandler* StatusRequestHandler::create(const NginxConfig& config, const std::string location_path){
	return new StatusRequestHandler(config);
}

std::shared_ptr<reply> StatusRequestHandler::HandleRequest(const request& request_) {
	Logger& logger = Logger::getInstance();


	// Create HTML for Request Handlers
    logger.log("Looking for Mapping between Request Code and Request URL", NORMAL);

    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply()); //create new reply
    rep->status = reply::ok; 

    
    RequestHandlerDispatcher::request_code_received_[request_.uri].push_back(rep->status); // push this mapping before we iterate


    // Get Mapping of url to response code
    
    for (auto ite = RequestHandlerDispatcher::request_code_received_.begin(); ite != RequestHandlerDispatcher::request_code_received_.end(); ite++){
      for(int j = 0; j <  (ite->second).size(); j++){
        request_info += "<tr><td>" + ite->first +"</td><td>" + std::to_string((ite->second)[j]) + "</td></tr>";
      }
    }

    logger.log("Looking for Mapping between Type of Request Handler and URL Prefix", NORMAL);

    // Get mapping of number of request handlers and their urls
    
    for (auto ite = RequestHandlerDispatcher::request_handler_uri.begin(); ite != RequestHandlerDispatcher::request_handler_uri.end(); ite++){
        request_handler_info += "<tr><td>" + ite->first +"</td><td>" + ite->second+ "</td></tr>";
    }

    logger.log("Constructing Status Handler HTML Page", NORMAL);
    
    std::string display_content = "<html>"
    "<body><h2>Status Handler</h1><div>""</div>"
    "<table>""<tr><th>Total Number of Requests</th></tr>""<tr><td>" + std::to_string(RequestHandlerDispatcher::num_handlers) + "</td></tr>""</table>"
    "<table>""<tr><th>Requests Server Received : </th><th>Response Code</th></tr>" + request_info + "</table>"
    "<table>""<tr><th>Request Handlers Received : </th><th>URL Prefixes</th></tr>" + request_handler_info + "</table>"
    "</body>"
    "</html>";


	

	rep->headers.resize(2);

    rep->content = display_content;
    rep->headers[0].name = "Content-Length";
    rep->headers[0].value = std::to_string((rep->content).length());
    rep->headers[1].name = "Content-Type";
    rep->headers[1].value = "text/html";


	return rep;
}

