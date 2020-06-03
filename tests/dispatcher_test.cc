#include <fstream>
#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"
#include "request_handler/request_handler.h"
#include "request_handler/dispatcher.h"
#include "request_handler/bad_request_handler.h"
#include "request_handler/echo_request_handler.h"
#include "request_handler/health_handler.h"
#include "request_handler/markdown_handler.h"
#include "request_handler/not_found_handler.h"
#include "request_handler/redirect_handler.h"
#include "request_handler/reverse_proxy_handler.h"
#include "request_handler/static_request_handler.h"
#include "request_handler/status_handler.h"
#include "http/request_parser.h"
#include "http/request.h"
#include "http/reply.h"

// mock fixture from discussion
class DispatcherTestFix : public ::testing::Test {
	protected:
		void SetUp() override {
            parser.Parse("./example_configs/example_config", &out_config);
            parser.Parse("./example_configs/example_config2", &no_location);
        }

        http::server::request_parser request_parser_;
        http::server::request request_;
        http::server::request_parser::result_type request_parser_result_;
        std::shared_ptr<http::server::reply> reply_;
        // configuring static root directories
        NginxConfigParser parser;
        NginxConfig out_config;
        NginxConfig no_location;

        // comparing static file contents
        std::ifstream file;
        std::string file_body;
        char chr;
};

// testing dispatcher creation
TEST_F(DispatcherTestFix, HandlerProperCreation) {
	RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    EXPECT_EQ(dispatcher_->handlers_.count("/"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/echo"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/health"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/redirect"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/static"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/static_2"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/status"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/ucla"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/badrequest"), 1);
    EXPECT_EQ(dispatcher_->handlers_.count("/markdown"), 1);
    EXPECT_EQ(dispatcher_->handlers_.size(), 10);
}

// testing for no handlers with empty config
TEST_F(DispatcherTestFix, NoConfigNoHandler) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(no_location);
    request_.uri_ = "/";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler == NULL);
}

// testing dispatcher return for echo handler
TEST_F(DispatcherTestFix, ProperEchoHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/echo";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for static handler
TEST_F(DispatcherTestFix, ProperStaticHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/static";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for status handler
TEST_F(DispatcherTestFix, ProperStatusHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/status";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for not found handler
TEST_F(DispatcherTestFix, ProperNotFoundHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for bad request handler
TEST_F(DispatcherTestFix, ProperBadRequestHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/badrequest";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for health handler
TEST_F(DispatcherTestFix, HealthHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/health";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for health handler
TEST_F(DispatcherTestFix, MarkdownHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/markdown";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for reverse proxy handler
TEST_F(DispatcherTestFix, ProperReverseProxyHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/ucla";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing dispatcher return for redirect handler
TEST_F(DispatcherTestFix, RedirectHandlerReturn) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/redirect";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    EXPECT_TRUE(handler != NULL);
}

// testing 404 not found error request for the not found request handler
TEST_F(DispatcherTestFix, VerifyErrorCode) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/random_bad_path";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    std::shared_ptr<reply> rep = handler->HandleRequest(request_);
    EXPECT_TRUE(rep->code_ == 404);
}

// testing 404 not found error request for the not found request handler
TEST_F(DispatcherTestFix, VerifyErrorCode2) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/static/nonexistentfile.txt";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    std::shared_ptr<reply> rep = handler->HandleRequest(request_);
    EXPECT_TRUE(rep->code_ == 404);
}

// testing 404 not found error request for the not found request handler
TEST_F(DispatcherTestFix, VerifyNoErrorCode) {
    RequestHandlerDispatcher* dispatcher_ = new RequestHandlerDispatcher(out_config);
    request_.uri_ = "/echo";
    std::shared_ptr<RequestHandler> handler = dispatcher_->dispatch(request_);
    std::shared_ptr<reply> rep = handler->HandleRequest(request_);
    EXPECT_TRUE(handler != NULL);
    EXPECT_TRUE(rep->code_ != 404);
}
