#include <map>
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

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_.method_, http::server::request::GET_REQ);
	EXPECT_EQ(request_.http_version_major, 1);
	EXPECT_EQ(request_.http_version_minor, 1);
	EXPECT_EQ(request_.uri_, "/");
}

// tests Bad http version major
TEST_F(RequestParserTestFix, BadMajor) {

	char incoming_request[1000] = "GET / HTTP/g.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests Bad http version minor
TEST_F(RequestParserTestFix, BadMinor) {

	char incoming_request[1000] = "GET / HTTP/1.f\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}


// tests leading slash
TEST_F(RequestParserTestFix, BadFirstSlash) {

	char incoming_request[1000] = "GET | HTTP/1.f\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests leading slash
TEST_F(RequestParserTestFix, LackingSlash) {

	char incoming_request[1000] = "GET HTTP/1.f\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests second slash
TEST_F(RequestParserTestFix, BadSecondSlash) {

	char incoming_request[1000] = "GET / HTTP|1.f\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests major version double number
TEST_F(RequestParserTestFix, DoubleMajorNumber) {

	char incoming_request[1000] = "GET / HTTP/12.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_.http_version_major, 12);
}

// tests minor version double number
TEST_F(RequestParserTestFix, DoubleMinorNumber) {

	char incoming_request[1000] = "GET / HTTP/1.11\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_.http_version_minor, 11);
}

// tests empty space on input
TEST_F(RequestParserTestFix, EmptyInitial) {

	char incoming_request[1000] = " / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests reset
TEST_F(RequestParserTestFix, testReset) {

	request_parser_.reset(); //should just maintain state and not change result

	char incoming_request[1000] = " / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));

	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests on not HTTP 2nd character
TEST_F(RequestParserTestFix, HltpTest) {

	char incoming_request[1000] = "GET / HlTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests on not HTTP 3rd character
TEST_F(RequestParserTestFix, HtipTest) {

	char incoming_request[1000] = "GET / HTIP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests on not HTTP 4th character
TEST_F(RequestParserTestFix, HttiTest) {

	char incoming_request[1000] = "GET / HTTI/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}


// tests on missing 1 newline
TEST_F(RequestParserTestFix, IncompleteCrlf1n) {

	char incoming_request[1000] = "GET / HTTP/1.1\rwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests on missing 1 r
TEST_F(RequestParserTestFix, IncompleteCrlf1r) {

	char incoming_request[1000] = "GET / HTTP/1.1\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests on missing 2 r
TEST_F(RequestParserTestFix, IncompleteCrlf2r) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests on missing 2 newline
TEST_F(RequestParserTestFix, IncompleteCrlf2n) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
} 
// tests on missing 3 newline
TEST_F(RequestParserTestFix, IncompleteCrlf3n) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}
// tests on missing 3 r
TEST_F(RequestParserTestFix, IncompleteCrlf3r) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}


// test header
TEST_F(RequestParserTestFix, MultiHeader) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\nTrace: index.html\r\n\r\n";
	std::tie(request_parser_result_,std::ignore) = request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
	
	EXPECT_EQ(request_.headers_["Host"], "www.w3.org/pub/WWW/TheProject.html");
	EXPECT_EQ(request_.headers_["Trace"], "index.html");
}

// tests period in between version
TEST_F(RequestParserTestFix, VersionPeriod) {

	char incoming_request[1000] = "GET / HTTP/1,1\r\nwww.w3.org/pub/WWW/TheProject.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests CRLF for second header missing r
TEST_F(RequestParserTestFix, badHeaderClrfr) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\nTrace: index.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}

// tests CRLF for second header missing n
TEST_F(RequestParserTestFix, badHeaderClrfn) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\rTrace: index.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}


// bad header content containing special char
TEST_F(RequestParserTestFix, badHeaderContentSpecial) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n;;;\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}



// tests for missing second Header
TEST_F(RequestParserTestFix, missingSecondHeader) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\nTrace:\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::bad);
}


// tests for extraspace
TEST_F(RequestParserTestFix, extraSpace) {

	char incoming_request[1000] = "GET / HTTP/1.1\r\nHost: www.w3.org/pub/WWW/TheProject.html\r\n Trace:index.html\r\n\r\n";

	std::tie(request_parser_result_,std::ignore) =
		request_parser_.parse(request_,incoming_request, incoming_request + strlen(incoming_request));
		
	EXPECT_EQ(request_parser_result_, http::server::request_parser::good);
}


