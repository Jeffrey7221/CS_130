#include "request_handler/dispatcher.h"
#include "request_handler/bad_request_handler.h"
#include "request_handler/echo_request_handler.h"
#include "request_handler/static_request_handler.h"
#include "request_handler/status_handler.h"
#include "request_handler/reverse_proxy_handler.h"
#include "logger/logger.h"

// Constructor for RequestHandlerDispatcher
// Read through config file and create any echo handlers and static handlers
RequestHandlerDispatcher::RequestHandlerDispatcher(const NginxConfig& config): config_(config) {
    Logger& logger = Logger::getInstance();
    logger.log("Creating request handlers based on config files", NORMAL);

    for(int i = 0; i < config_.statements_.size(); i++) {
        if(config_.statements_[i]->tokens_[0] == "server" && config_.statements_[i]->tokens_.size() == 1) {
            for(int j = 0; j < config_.statements_[i]->child_block_->statements_.size(); j++) {
                if (config_.statements_[i]->child_block_->statements_[j]->tokens_.size() > 2) {
                    if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "EchoHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "echo");
                    } else if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "StaticHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "static");
                    } else if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "StatusHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "status");
                    } else if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "BadRequestHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "bad");
                    } else if(config_.statements_[i]->child_block_->statements_[j]->tokens_[2] == "ReverseProxyHandler") {
                        createHandler(config_.statements_[i]->child_block_->statements_[j], "reverse");
                    }
                }
            }
            
        }
        
    }
}

// For a given request req, dispatch any request handler that matches the request uri
std::shared_ptr<RequestHandler> RequestHandlerDispatcher::dispatch(request& req) {
    Logger& logger = Logger::getInstance();
    logger.log("Dispatching request handler", NORMAL);

    // remove any trailing slashes, except for root '/'
    std::string path_uri = req.uri_;
    while(path_uri.length() > 1 && path_uri.back() == '/')
        path_uri.pop_back();

    // longest prefix matching for uri to find the appropriate request handler
    std::shared_ptr<RequestHandler> req_handler = NULL;
    int longest_match = 0;
    for (auto iter = handlers_.begin(); iter != handlers_.end(); iter++) {
        if (iter->first == path_uri.substr(0, iter->first.length())) {
            if (iter->first.length() > longest_match) {
                req_handler = iter->second;
                longest_match = iter->first.length();
            }
        }
    }

    return req_handler;
}

// For each request handler statement in config file, create the proper request handler
void RequestHandlerDispatcher::createHandler(const std::shared_ptr<NginxConfigStatement> config_statement_, std::string HandlerType) {
    Logger& logger = Logger::getInstance();
    
    if(config_statement_->tokens_[0] != "location") {
        return;
    }

    // remove all trailing slashes
    std::string path_uri = config_statement_->tokens_[1];

    if(path_uri.front() == '"' && path_uri.back() == '"') {
        path_uri.erase(0, 1);
        path_uri.pop_back();
    } else {
        return;
    }

    while(path_uri.length() > 1 && path_uri.back() == '/')
        path_uri.pop_back();
    
    // skip if there already exists a request handler with the same path uri
    if (handlers_.find(path_uri) != handlers_.end()) {
        return;
    }
    
    // create the echo and static handlers, and increment the handlers counter
    if(HandlerType == "echo") {
        logger.log("Adding an echo handler at path: " + path_uri, NORMAL);
        handlers_[path_uri] = std::shared_ptr<RequestHandler>(EchoRequestHandler::Init(*(config_statement_->child_block_), path_uri));
        request_handler_uri[path_uri] = "echo handler";
        num_handlers++;
    } else if (HandlerType == "static") {
        logger.log("Adding an static handler at path: " + path_uri, NORMAL);
        handlers_[path_uri] = std::shared_ptr<RequestHandler>(StaticRequestHandler::Init(*(config_statement_->child_block_), path_uri));
        request_handler_uri[path_uri] = "static handler";
        num_handlers++;
    } else if (HandlerType == "status") {
        logger.log("Adding an status handler at path: " + path_uri, NORMAL);
        handlers_[path_uri] = std::shared_ptr<RequestHandler>(StatusRequestHandler::Init(*(config_statement_->child_block_), path_uri));
        request_handler_uri[path_uri] = "status handler";
        num_handlers++;
    } else if (HandlerType == "bad") {
        logger.log("Adding a bad request handler at path: " + path_uri, NORMAL);
        handlers_["/"] = std::shared_ptr<RequestHandler>(BadRequestHandler::Init(*(config_statement_->child_block_), "/"));
        request_handler_uri["/"] = "bad handler";
        num_handlers++;
    } else if (HandlerType == "reverse") {
        logger.log("Adding a reverse proxy request handler at path: " + path_uri, NORMAL);
        handlers_[path_uri] = std::shared_ptr<RequestHandler>(ReverseProxyHandler::Init(*(config_statement_->child_block_), path_uri));
        request_handler_uri[path_uri] = "reverse proxy request handler";
        num_handlers++;
    } else { 
        return;
    }
}


size_t RequestHandlerDispatcher::num_handlers = 0;
size_t RequestHandlerDispatcher::num_requests = 0;
std::map<std::string, std::vector<int>> RequestHandlerDispatcher::request_code_received_;
std::map<std::string, std::string> RequestHandlerDispatcher::request_handler_uri;
