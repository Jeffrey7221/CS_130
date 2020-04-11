#include "gtest/gtest.h"
#include "http/request_parser.h"
#include "http/request.h"

// mock fixture from discussion
class RequestParserTestFix : public ::testing::Test {
	protected:
		http::server::request_parser request_parser_;
		http::server::request request_;
		http::server::request_parser::result_type request_parser_result_;
};

// test completed http request
TEST_F(RequestParserTestFix, CompleteRequest) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::good);
}

// test intermediate http request
TEST_F(RequestParserTestFix, IndeterminateRequest) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
	
	EXPECT_EQ(request_parser_result_, http::server::request_parser::indeterminate);
}

// test bad http request due to lacking a slash
TEST_F(RequestParserTestFix, LackingSlash) {

	char incoming_request[1000] = "GET USC RULES";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
	
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// test bad http request due to too many slashes
TEST_F(RequestParserTestFix, TooManySlash) {

	char incoming_request[1000] = "GET //// www.google.com";
	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);

}

// tests whether content returned is correct
TEST_F(RequestParserTestFix, BodyTests) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	bool success = false;
	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_.method, "GET");
	EXPECT_EQ(request_.http_version_major, 1);
	EXPECT_EQ(request_.http_version_minor, 1);
	EXPECT_EQ(request_.uri, "/");
}