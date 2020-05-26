#include "gtest/gtest.h"
#include "http/reply.h"


// mock fixture from discussion
class ReplyTestFix : public ::testing::Test {
	protected:
		http::server::reply *rep;
		std::vector<boost::asio::const_buffer> buffers;
};

//test reply for ok response
TEST_F(ReplyTestFix, okReply) {

	rep = http::server::reply::stock_reply(http::server::reply::ok);
	const char ok[] = "\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::ok); //test status
	EXPECT_EQ(rep->body_, ok); //test content
	EXPECT_EQ(rep->headers_.size(), 2); //test size 
	// test header values
	EXPECT_EQ(rep->headers_.size(), 2);
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}

//test reply for created response
TEST_F(ReplyTestFix, createdReply) {

	rep = http::server::reply::stock_reply(http::server::reply::created);
	const char created[] =
		"<html>"
		"<head><title>Created</title></head>"
		"<body><h1>201 Created</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::created); //test status
	EXPECT_EQ(rep->body_, created); //test content
	EXPECT_EQ(rep->headers_.size(), 2); //test size 
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}



//test reply for accepted response
TEST_F(ReplyTestFix, acceptedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::accepted);
	const char accepted[] =
		"<html>"
		"<head><title>Accepted</title></head>"
		"<body><h1>202 Accepted</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::accepted); //test status
	EXPECT_EQ(rep->body_, accepted); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}



//test reply for no_content response
TEST_F(ReplyTestFix, no_contentReply) {

	rep = http::server::reply::stock_reply(http::server::reply::no_content);	
	const char no_content[] =
		"<html>"
		"<head><title>No Content</title></head>"
		"<body><h1>204 Content</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::no_content); //test status
	EXPECT_EQ(rep->body_, no_content); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}

//test reply for mutiple choices response
TEST_F(ReplyTestFix, multiple_choicesReply) {

	rep = http::server::reply::stock_reply(http::server::reply::multiple_choices);
	const char multiple_choices[] =
		"<html>"
		"<head><title>Multiple Choices</title></head>"
		"<body><h1>300 Multiple Choices</h1></body>"
		"</html>"
		"\r\n";
	  
	EXPECT_EQ(rep->code_, http::server::reply::multiple_choices); //test status
	EXPECT_EQ(rep->body_, multiple_choices); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}

//test reply for moved perm response
TEST_F(ReplyTestFix, moved_permanentlyReply) {

	rep = http::server::reply::stock_reply(http::server::reply::moved_permanently);
	const char moved_permanently[] =
		"<html>"
		"<head><title>Moved Permanently</title></head>"
		"<body><h1>301 Moved Permanently</h1></body>"
		"</html>"
		"\r\n";
	  
	EXPECT_EQ(rep->code_, http::server::reply::moved_permanently); //test status
	EXPECT_EQ(rep->body_, moved_permanently); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}

//test reply for moved temp response
TEST_F(ReplyTestFix, moved_temporarilyReply) {

	rep = http::server::reply::stock_reply(http::server::reply::moved_temporarily);
	const char moved_temporarily[] =
		"<html>"
		"<head><title>Moved Temporarily</title></head>"
		"<body><h1>302 Moved Temporarily</h1></body>"
		"</html>"
		"\r\n";
	  
	EXPECT_EQ(rep->code_, http::server::reply::moved_temporarily); //test status
	EXPECT_EQ(rep->body_, moved_temporarily); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}

//test reply for not modified response
TEST_F(ReplyTestFix, not_modifiedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::not_modified);
	const char not_modified[] =
		"<html>"
		"<head><title>Not Modified</title></head>"
		"<body><h1>304 Not Modified</h1></body>"
		"</html>"
		"\r\n";
	  
	EXPECT_EQ(rep->code_, http::server::reply::not_modified); //test status
	EXPECT_EQ(rep->body_, not_modified); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}



//test reply for bad request response
TEST_F(ReplyTestFix, bad_requestReply) {

	rep = http::server::reply::stock_reply(http::server::reply::bad_request);
	const char bad_request[] =
		"<html>"
		"<head><title>Bad Request</title></head>"
		"<body><h1>400 Bad Request</h1></body>"
		"</html>"
		"\r\n";
	  
	EXPECT_EQ(rep->code_, http::server::reply::bad_request); //test status
	EXPECT_EQ(rep->body_, bad_request); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}


//test reply for unauth response
TEST_F(ReplyTestFix, unauthorizedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::unauthorized);
	const char unauthorized[] =
		"<html>"
		"<head><title>Unauthorized</title></head>"
		"<body><h1>401 Unauthorized</h1></body>"
		"</html>"
		"\r\n";
	  
	EXPECT_EQ(rep->code_, http::server::reply::unauthorized); //test status
	EXPECT_EQ(rep->body_, unauthorized); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}



//test reply for forbidden response
TEST_F(ReplyTestFix, forbiddenReply) {

	rep = http::server::reply::stock_reply(http::server::reply::forbidden);
	const char forbidden[] =
		"<html>"
		"<head><title>Forbidden</title></head>"
		"<body><h1>403 Forbidden</h1></body>"
		"</html>"
		"\r\n";
	  
	EXPECT_EQ(rep->code_, http::server::reply::forbidden); //test status
	EXPECT_EQ(rep->body_, forbidden); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}



//test reply for not found response
TEST_F(ReplyTestFix, not_foundReply) {

	rep = http::server::reply::stock_reply(http::server::reply::not_found);
	const char not_found[] =
		"<html>"
		"<head><title>Not Found</title></head>"
		"<body><h1>404 Not Found</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::not_found); //test status
	EXPECT_EQ(rep->body_, not_found); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}



//test reply for internal server response
TEST_F(ReplyTestFix, internal_server_errorReply) {

	rep = http::server::reply::stock_reply(http::server::reply::internal_server_error);
	const char internal_server_error[] =
		"<html>"
		"<head><title>Internal Server Error</title></head>"
		"<body><h1>500 Internal Server Error</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::internal_server_error); //test status
	EXPECT_EQ(rep->body_, internal_server_error); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}


//test reply for not implemented response
TEST_F(ReplyTestFix, not_implementedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::not_implemented);
	const char not_implemented[] =
		"<html>"
		"<head><title>Not Implemented</title></head>"
		"<body><h1>501 Not Implemented</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::not_implemented); //test status
	EXPECT_EQ(rep->body_, not_implemented); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}


//test reply for bad gateway response
TEST_F(ReplyTestFix, bad_gatewayReply) {

	rep = http::server::reply::stock_reply(http::server::reply::bad_gateway);
	const char bad_gateway[] =
		"<html>"
		"<head><title>Bad Gateway</title></head>"
		"<body><h1>502 Bad Gateway</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::bad_gateway); //test status
	EXPECT_EQ(rep->body_, bad_gateway); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}



//test reply for service unavailable response
TEST_F(ReplyTestFix, service_unavailableReply) {

	rep = http::server::reply::stock_reply(http::server::reply::service_unavailable);
	const char service_unavailable[] =
		"<html>"
		"<head><title>Service Unavailable</title></head>"
		"<body><h1>503 Service Unavailable</h1></body>"
		"</html>"
		"\r\n";

	EXPECT_EQ(rep->code_, http::server::reply::service_unavailable); //test status
	EXPECT_EQ(rep->body_, service_unavailable); //test content
	// test header values
	EXPECT_EQ(rep->headers_["Content-Length"], std::to_string(rep->body_.size()));
	EXPECT_EQ(rep->headers_["Content-Type"], "text/html");

	buffers = rep->to_buffers();
}
