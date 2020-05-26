#include <fstream>
#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"
#include "request_handler/bad_request_handler.h"
#include "request_handler/echo_request_handler.h"
#include "request_handler/health_handler.h"
#include "request_handler/redirect_handler.h"
#include "request_handler/reverse_proxy_handler.h"
#include "request_handler/static_request_handler.h"
#include "request_handler/status_handler.h"
#include "http/request_parser.h"
#include "http/request.h"
#include "http/reply.h"

// mock fixture from discussion
class RequestHandlerTestFix : public ::testing::Test {
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

// testing basic functionality of an echo_handler, reply on valid request
TEST_F(RequestHandlerTestFix, BasicEcho) {

	char incoming_request[1024] = "GET /echo HTTP/1.1\r\n\r\n";
  	EchoRequestHandler echo_handler_;

	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = echo_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->body_, "GET /echo HTTP/1.1\r\n");
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(reply_->body_.size()));
}

// testing basic functionality of an echo_handler, should work even with a POST request
TEST_F(RequestHandlerTestFix, EchoPost) {

	char incoming_request[1024] = "POST / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	EchoRequestHandler echo_handler_;

	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = echo_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->body_, "POST / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n");
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(reply_->body_.size()));
}

// testing basic static_handler, should work given a valid path configuration and file
TEST_F(RequestHandlerTestFix, GoodStaticHandle) {

	char incoming_request[1024] = "GET /static/example_data.txt HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  	std::string static_path = "/static/";
	std::string static_root = "/tests/static_data/";
  	StaticRequestHandler static_handler_(out_config, static_path, static_root);

	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = static_handler_.HandleRequest(request_);

	// compare file contents
	file.open("./static_data/example_data.txt");
	while (file.get(chr)) {
		file_body += chr;
	}
	file_body.pop_back();
	file_body += "\r\n\r\n";
  	file.close();

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->body_, file_body);
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(file_body.size()));
}

// testing static handler with no root directory configuration
// should default to the true root directory of our application
TEST_F(RequestHandlerTestFix, StaticHandleNoConfig) {

	char incoming_request[1024] = "GET /static/tests/static_data/example_data.txt HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  	std::string static_path = "/static/";
	std::string static_root = "/";
  	StaticRequestHandler static_handler_(no_location, static_path, static_root);

	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = static_handler_.HandleRequest(request_);

	// compare file contents
	file.open("./static_data/example_data.txt");
	while (file.get(chr)) {
		file_body += chr;
	}
	file_body.pop_back();
	file_body += "\r\n\r\n";  
	file.close();

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->body_, file_body);
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(file_body.size()));
}

// testing static handler with a different route, should have a different base directory
TEST_F(RequestHandlerTestFix, StaticDifferentRoute) {

	char incoming_request[1024] = "GET /static_2/example_config2 HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	std::string static_path = "/static_2/";
	std::string static_root = "/tests/example_configs/";
	StaticRequestHandler static_handler_(out_config, static_path, static_root);

	std::tie(request_parser_result_, std::ignore) =
	request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = static_handler_.HandleRequest(request_);

	// compare file contents
	file.open("./example_configs/example_config2");
	while (file.get(chr)) {
		file_body += chr;
	}
	// file_body += "\r\n";
	file.close();

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->body_, file_body);
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(file_body.size()));
}

// testing file not found case for static handling
TEST_F(RequestHandlerTestFix, StaticFileNotFound) {

	char incoming_request[1024] = "GET /static/file_does_not_exist.txt HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	std::string static_path = "/static/";
	std::string static_root = "/";
	StaticRequestHandler static_handler_(out_config, static_path, static_root);
	char input[1024] = "GET /static/data/www/data1.data HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n";
	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = static_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::not_found);
	EXPECT_EQ(reply_->body_, "<html>"
		"<head><title>Not Found</title></head>"
		"<body><h1>404 Not Found</h1></body>"
		"</html>"
		"\r\n");
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(reply_->body_.size()));
}


TEST_F(RequestHandlerTestFix, StatusHandler) {

	char incoming_request[1024] = "GET /index.html HTTP/1.1\r\n\r\n";
	char incoming_request_status[1024] = "GET /status HTTP/1.1\r\n\r\n";
	std::string static_path = "/static/";
	std::string static_root = "/tests/static_data/";
	StaticRequestHandler static_handler_(out_config, static_path, static_root);
	StatusRequestHandler status_handler_(out_config);

	std::tie(request_parser_result_, std::ignore) =
	request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = static_handler_.HandleRequest(request_);

	std::tie(request_parser_result_, std::ignore) =
	request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = static_handler_.HandleRequest(request_);

	std::tie(request_parser_result_, std::ignore) =
	request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = static_handler_.HandleRequest(request_);

	std::tie(request_parser_result_, std::ignore) =
	request_parser_.parse(request_, incoming_request_status, incoming_request_status + strlen(incoming_request_status));
	reply_ = status_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(reply_->body_.size()));
}

TEST_F(RequestHandlerTestFix, ReverseProxyHandlerSuccess) {
	char incoming_request[1024] = "GET /reverse HTTP/1.1\r\n\r\n";
	int proxy_port = 80;
	std::string proxy_dest = "www.ucla.edu";
	std::string location_path = "/reverse"; 
	ReverseProxyHandler reverse_handler_(location_path, proxy_dest, proxy_port);

  	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = reverse_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->headers_["Content-Type"], "text/html; charset=UTF-8");
	EXPECT_NE(reply_->body_.size(), 0);  // Body should be non-empty
}

TEST_F(RequestHandlerTestFix, ReverseProxyHandlerSuccessImage) {
	char incoming_request[1024] = "GET /reverse/content/hero/cute_puppies_hero.jpg HTTP/1.1\r\n\r\n";
	int proxy_port = 80;
	std::string proxy_dest = "cdn.akc.org";
	std::string location_path = "/reverse"; 
	ReverseProxyHandler reverse_handler_(location_path, proxy_dest, proxy_port);

  	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = reverse_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->headers_["Content-Type"], "image/jpeg");
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(reply_->body_.size()));
}

TEST_F(RequestHandlerTestFix, ReverseProxyHandlerSuccessHEADRequest) {
	char incoming_request[1024] = "HEAD /reverse2 HTTP/1.1\r\n\r\n";
	int proxy_port = 80;
	std::string proxy_dest = "www.ucla.edu";
	std::string location_path = "/reverse2"; 
	ReverseProxyHandler reverse_handler_(location_path, proxy_dest, proxy_port);

  	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	std::cout << "METHOD: " << request_.method_ << std::endl;
	reply_ = reverse_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->headers_["Content-Type"], "text/html; charset=UTF-8");
	EXPECT_EQ(reply_->body_.size(), 0);  // HEAD requests only return headers, not the body
}

TEST_F(RequestHandlerTestFix, ReverseProxyHandlerNotFoundWebpage) {
	char incoming_request[1024] = "GET /reverse/nom HTTP/1.1\r\n\r\n";
	int proxy_port = 80;
	std::string proxy_dest = "www.google.com";
	std::string location_path = "/reverse"; 
	ReverseProxyHandler reverse_handler_(location_path, proxy_dest, proxy_port);

  	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = reverse_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::not_found);
	EXPECT_EQ(reply_->headers_["Content-Type"], "text/html; charset=UTF-8");
	EXPECT_NE(reply_->body_.size(), 0);  // Body should be non-empty
}

TEST_F(RequestHandlerTestFix, ReverseProxyHandlerInvalidAddress) {
	char incoming_request[1024] = "GET /reverse HTTP/1.1\r\n\r\n";
	int proxy_port = 80;
	std::string proxy_dest = "@";
	std::string location_path = "/reverse"; 
	ReverseProxyHandler reverse_handler_(location_path, proxy_dest, proxy_port);

  	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = reverse_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::internal_server_error);
	EXPECT_EQ(reply_->headers_["Content-Type"], "text/html");
	EXPECT_EQ(reply_->headers_["Content-Length"], std::to_string(reply_->body_.size()));
}

TEST_F(RequestHandlerTestFix, ReverseProxyHandlerRedirect) {
	char incoming_request[1024] = "GET /reverse/webhp HTTP/1.1\r\n\r\n";
	int proxy_port = 80;
	std::string proxy_dest = "www.google.com";
	std::string location_path = "/reverse"; 
	ReverseProxyHandler reverse_handler_(location_path, proxy_dest, proxy_port);

  	std::tie(request_parser_result_, std::ignore) =
		request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	reply_ = reverse_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->headers_["Content-Type"], "text/html; charset=ISO-8859-1");
}

TEST_F(RequestHandlerTestFix, RedirectHandlerTest) {
	char incoming_request[1024] = "GET /redirect HTTP/1.1\r\n\r\n";
	std::string host = "localhost";
	int port = 8080;
	RedirectHandler redirect_handler_(host);

	reply_ = redirect_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::moved_temporarily);
	EXPECT_EQ(reply_->headers_["Location"], "http://" + host + "/static/index.html");
	EXPECT_EQ(reply_->body_, "HTTP/1.1 302 Found\r\nLocation: " + reply_->headers_["Location"] + "\r\n\r\n");
}

TEST_F(RequestHandlerTestFix, HealthHandlerTest) {
	char incoming_request[1024] = "GET /health HTTP/1.1\r\n\r\n";
	std::string host = "localhost";
	int port = 8080;
	HealthHandler health_handler_;

	reply_ = health_handler_.HandleRequest(request_);

	EXPECT_EQ(reply_->code_, http::server::reply::ok);
	EXPECT_EQ(reply_->body_, "OK");
}