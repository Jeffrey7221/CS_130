#include "gtest/gtest.h"
#include "http/request_parser.h"
#include "http/request.h"

// mock fixture from discussion
class RequestParserTestFix : public ::testing::Test
{
	protected:
		http::server::request_parser request_parser_;
		http::server::request request_;
		http::server::request_parser::result_type request_parser_result_;
};

TEST_F(RequestParserTestFix, CompleteRequest){ //completed http request
	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	bool success = false;
	if(request_parser_result_ == http::server::request_parser::good){ 
		success = true;//test case passes
		
	}else{
		success = false; //test case did not pass
	}
	EXPECT_TRUE(success); 

}

TEST_F(RequestParserTestFix, IndeterminateRequest){ //Intermediate http request
	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
	bool success = false;
	if(request_parser_result_ == http::server::request_parser::indeterminate){ 
		success = true;//test case passes
		
	}else{
		success = false; //test case did not pass
	}
	EXPECT_TRUE(success); 

}

TEST_F(RequestParserTestFix, LackingSlash){ //Bad http request due to lacking a slash
	char incoming_request[1000] = "GET USC RULES";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
	bool success = false;
	if(request_parser_result_ == http::server::request_parser::bad){ 
		success = true;//test case passes
		
	}else{
		success = false; //test case did not pass
	}
	EXPECT_TRUE(success); 

}

TEST_F(RequestParserTestFix, TooManySlash){ //Bad http request due to too many slashes
	char incoming_request[1000] = "GET //// www.google.com";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
	bool success = false;
	if(request_parser_result_ == http::server::request_parser::bad){ 
		success = true;//test case passes
		
	}else{
		success = false; //test case did not pass
	}
	EXPECT_TRUE(success); 

}

TEST_F(RequestParserTestFix, BodyTests) {// tests whether content returned is correct

	char incoming_request[1000] = "GET / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";
	bool success = false;
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_.method , "GET");
	EXPECT_EQ(request_.http_version_major , 1);
	EXPECT_EQ(request_.http_version_minor , 1 );
	EXPECT_EQ(request_.uri , "/");

  
}
