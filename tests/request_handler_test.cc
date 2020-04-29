#include <fstream>
#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"
#include "request_handler/request_handler.h"
#include "request_handler/echo_request_handler.h"
#include "request_handler/static_request_handler.h"
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
  reply_ = echo_handler_.HandleRequest(request_, incoming_request);

	EXPECT_EQ(reply_->status, http::server::reply::ok);
	EXPECT_EQ(reply_->content, "GET /echo HTTP/1.1\r\n");
	EXPECT_EQ(reply_->headers[0].value, std::to_string(reply_->content.size()));
}

// testing basic functionality of an echo_handler, should work even with a POST request
TEST_F(RequestHandlerTestFix, EchoPost) {

	char incoming_request[1024] = "POST / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  EchoRequestHandler echo_handler_;

  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  reply_ = echo_handler_.HandleRequest(request_, incoming_request);

	EXPECT_EQ(reply_->status, http::server::reply::ok);
	EXPECT_EQ(reply_->content, "POST / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n");
	EXPECT_EQ(reply_->headers[0].value, std::to_string(reply_->content.size()));
}

// testing basic static_handler, should work given a valid path configuration and file
TEST_F(RequestHandlerTestFix, GoodStaticHandle) {

	char incoming_request[1024] = "GET /static/example_data.txt HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  std::string static_path = "/static/";
	std::string static_root = "/tests/static_data/";
  StaticRequestHandler static_handler_(out_config, static_path, static_root);

  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  reply_ = static_handler_.HandleRequest(request_, incoming_request);

	// compare file contents
	file.open("./static_data/example_data.txt");
	while (file.get(chr)) {
		file_body += chr;
	}
	file_body.pop_back();
	file_body += "\r\n\r\n";
  	file.close();

	EXPECT_EQ(reply_->status, http::server::reply::ok);
	EXPECT_EQ(reply_->content, file_body);
	EXPECT_EQ(reply_->headers[0].value, std::to_string(file_body.size()));
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
  reply_ = static_handler_.HandleRequest(request_, incoming_request);

	// compare file contents
  file.open("./static_data/example_data.txt");
  while (file.get(chr)) {
		file_body += chr;
	}
	file_body.pop_back();
	file_body += "\r\n\r\n";  
	file.close();

	EXPECT_EQ(reply_->status, http::server::reply::ok);
	EXPECT_EQ(reply_->content, file_body);
	EXPECT_EQ(reply_->headers[0].value, std::to_string(file_body.size()));
}

// testing static handler with a different route, should have a different base directory
TEST_F(RequestHandlerTestFix, DifferentStaticRoute) {

	char incoming_request[1024] = "GET /static_2/example_config2 HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  std::string static_path = "/static_2/";
	std::string static_root = "/tests/example_configs/";
  StaticRequestHandler static_handler_(out_config, static_path, static_root);

  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  reply_ = static_handler_.HandleRequest(request_, incoming_request);

	// compare file contents
  file.open("./example_configs/example_config2");
  while (file.get(chr)) {
		file_body += chr;
	}
	//file_body += "\r\n";
  file.close();

	EXPECT_EQ(reply_->status, http::server::reply::ok);
	EXPECT_EQ(reply_->content, file_body);
	EXPECT_EQ(reply_->headers[0].value, std::to_string(file_body.size()));
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
  reply_ = static_handler_.HandleRequest(request_, incoming_request);

	EXPECT_EQ(reply_->status, http::server::reply::not_found);
	EXPECT_EQ(reply_->content, "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>"
		"\r\n");
	EXPECT_EQ(reply_->headers[0].value, std::to_string(reply_->content.size()));
}

