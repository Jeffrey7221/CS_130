#include <fstream>
#include "gtest/gtest.h"
#include "config_parser/nginx_config.h"
#include "config_parser/nginx_config_parser.h"
#include "config_parser/nginx_config_statement.h"
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
		http::server::reply reply_;

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

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	// DELETE THIS COMMENT: This is the request_handler for echo-ing (creating a new instance)
	// echo_handler should not need the out_config technically, but static_handler will
  echo_handler echo_handler_(out_config);
  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
	// DELETE THIS COMMENT: This is the output from a handler that echos...
	// DELETE THIS COMMENT: THE MAIN FUNCTION BEING TESTED IS HANDLE REQUEST, THIS ONE :)
  echo_handler_.handle_request(request_, &reply_);

	EXPECT_EQ(reply_.status, http::server::reply::ok);
	EXPECT_EQ(reply_.content, "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n");
	EXPECT_EQ(reply_.headers[0].value, std::to_string(reply_.content.size()));
}

// testing basic functionality of an echo_handler, should work even with no nginx_config or location
TEST_F(RequestHandlerTestFix, NoConfigEcho) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  echo_handler echo_handler_(no_location);
  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  echo_handler_.handle_request(request_, &reply_);

	EXPECT_EQ(reply_.status, http::server::reply::ok);
	EXPECT_EQ(reply_.content, "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n");
	EXPECT_EQ(reply_.headers[0].value, std::to_string(reply_.content.size()));
}

// testing basic static_handler, should work given a valid path configuration and file
TEST_F(RequestHandlerTestFix, GoodStaticHandle) {

	char incoming_request[1000] = "GET /static/example_data.txt HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  std::string static_path = "/static";
	// DELETE THIS COMMENT: This is the request_handler for showing static files
	// DELETE THIS COMMENT: Check out example_config to see how 'location' should be formatted
  static_handler static_handler_(out_config, static_path);
  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  static_handler_.handleRequest(request_, &reply_);

	// compare file contents
  file.open("./static_data/example_data.txt");
  while (file.get(chr)) {
		file_body += chr;
	}
  file.close();

	EXPECT_EQ(reply_.status, http::server::reply::ok);
	EXPECT_EQ(reply_.content, file_body);
	EXPECT_EQ(reply_.headers[0].value, std::to_string(file_body.size()));
}

// testing static handler with no root directory configuration
// should default to the true root directory of our application
TEST_F(RequestHandlerTestFix, StaticHandleNoConfig) {

	char incoming_request[1000] = "GET /static/README.md HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  std::string static_path = "/static";
  static_handler static_handler_(no_location, static_path);
  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  static_handler_.handleRequest(request_, &reply_);

	// compare file contents
  file.open("../README.md");
  while (file.get(chr)) {
		file_body += chr;
	}
  file.close();

	EXPECT_EQ(reply_.status, http::server::reply::ok);
	EXPECT_EQ(reply_.content, file_body);
	EXPECT_EQ(reply_.headers[0].value, std::to_string(file_body.size()));
}

// testing static handler with a different route, should have a different base directory
TEST_F(RequestHandlerTestFix, DifferentStaticRoute) {

	char incoming_request[1000] = "GET /static_2/example_config HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  std::string static_path = "/static_2";
  static_handler static_handler_(out_config, static_path);
  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  static_handler_.handleRequest(request_, &reply_);

	// compare file contents
  file.open("./example_configs/example_config");
  while (file.get(chr)) {
		file_body += chr;
	}
  file.close();

	EXPECT_EQ(reply_.status, http::server::reply::ok);
	EXPECT_EQ(reply_.content, file_body);
	EXPECT_EQ(reply_.headers[0].value, std::to_string(file_body.size()));
}

// testing file not found case for static handling
TEST_F(RequestHandlerTestFix, StaticFileNotFound) {

	char incoming_request[1000] = "GET /static/file_does_not_exist.txt HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
  std::string static_path = "/static";
	static_handler static_handler_(out_config, static_path);
  char input[1024] = "GET /static/data/www/data1.data HTTP/1.1\r\nHost: www.example.com\r\nConnection: close\r\n\r\n";
  std::tie(request_parser_result_, std::ignore) =
	  request_parser_.parse(request_, incoming_request, incoming_request + strlen(incoming_request));
  static_handler_.handleRequest(request_, &reply_);

	EXPECT_EQ(reply_.status, http::server::reply::not_found);
	EXPECT_EQ(reply_.content, "<html>"
    "<head><title>Not Found</title></head>"
    "<body><h1>404 Not Found</h1></body>"
    "</html>");
	EXPECT_EQ(reply_.headers[0].value, std::to_string(reply_.content.size()));
}

