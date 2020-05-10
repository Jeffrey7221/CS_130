#ifndef REQUEST_HANDLER_DISPATCHER_H
#define REQUEST_HANDLER_DISPATCHER_H

#include "request_handler/request_handler.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_statement.h"
#include "gtest/gtest.h"

/* Header class for our Request Handler Dispatcher
    Created: in server.cc when config file is read in
    Purpose: Dispatch the correct handlers in response to HTTP requests 
*/

class RequestHandlerDispatcher {
    public:
	    RequestHandlerDispatcher(const NginxConfig& config); 
        virtual std::shared_ptr<RequestHandler> dispatch(request& req);

        // Make our test cases a friend for access to private variables
        friend class DispatcherTestFix_HandlerProperCreation_Test;
    private: 
        void createHandler(const std::shared_ptr<NginxConfigStatement> config_statement_, std::string HandlerType);

        // private variables
        std::map<std::string, std::shared_ptr<RequestHandler>> handlers_;
        NginxConfig config_;
        size_t num_handlers;

};

#endif