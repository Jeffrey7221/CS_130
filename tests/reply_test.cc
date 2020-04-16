#include "gtest/gtest.h"
#include "http/reply.h"


// mock fixture from discussion
class ReplyTestFix : public ::testing::Test {
	protected:
		http::server::reply rep;
		std::vector<boost::asio::const_buffer> buffers;
};

//test reply for ok response

TEST_F(ReplyTestFix, okReply) {

	rep = http::server::reply::stock_reply(http::server::reply::ok);
	
	const char ok[] = "";


	EXPECT_EQ(rep.status, http::server::reply::ok); //test status
	EXPECT_EQ(rep.content, ok); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}

//test reply for created response

TEST_F(ReplyTestFix, createdReply) {

	rep = http::server::reply::stock_reply(http::server::reply::created);
	
	const char created[] =
	  "<html>"
	  "<head><title>Created</title></head>"
	  "<body><h1>201 Created</h1></body>"
	  "</html>";

	EXPECT_EQ(rep.status, http::server::reply::created); //test status
	EXPECT_EQ(rep.content, created); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}



//test reply for accepted response

TEST_F(ReplyTestFix, acceptedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::accepted);
	
	const char accepted[] =
	  "<html>"
	  "<head><title>Accepted</title></head>"
	  "<body><h1>202 Accepted</h1></body>"
	  "</html>";

	EXPECT_EQ(rep.status, http::server::reply::accepted); //test status
	EXPECT_EQ(rep.content, accepted); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}



//test reply for no_content response

TEST_F(ReplyTestFix, no_contentReply) {

	rep = http::server::reply::stock_reply(http::server::reply::no_content);
	
	const char no_content[] =
	  "<html>"
	  "<head><title>No Content</title></head>"
	  "<body><h1>204 Content</h1></body>"
	  "</html>";

	EXPECT_EQ(rep.status, http::server::reply::no_content); //test status
	EXPECT_EQ(rep.content, no_content); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}

//test reply for mutiple choices response

TEST_F(ReplyTestFix, multiple_choicesReply) {

	rep = http::server::reply::stock_reply(http::server::reply::multiple_choices);
	
	const char multiple_choices[] =
	  "<html>"
	  "<head><title>Multiple Choices</title></head>"
	  "<body><h1>300 Multiple Choices</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::multiple_choices); //test status
	EXPECT_EQ(rep.content, multiple_choices); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}

//test reply for moved perm response

TEST_F(ReplyTestFix, moved_permanentlyReply) {

	rep = http::server::reply::stock_reply(http::server::reply::moved_permanently);
	
	const char moved_permanently[] =
	  "<html>"
	  "<head><title>Moved Permanently</title></head>"
	  "<body><h1>301 Moved Permanently</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::moved_permanently); //test status
	EXPECT_EQ(rep.content, moved_permanently); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}

//test reply for moved temp response

TEST_F(ReplyTestFix, moved_temporarilyReply) {

	rep = http::server::reply::stock_reply(http::server::reply::moved_temporarily);
	
	const char moved_temporarily[] =
	  "<html>"
	  "<head><title>Moved Temporarily</title></head>"
	  "<body><h1>302 Moved Temporarily</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::moved_temporarily); //test status
	EXPECT_EQ(rep.content, moved_temporarily); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}

//test reply for not modified response

TEST_F(ReplyTestFix, not_modifiedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::not_modified);
	
	const char not_modified[] =
	  "<html>"
	  "<head><title>Not Modified</title></head>"
	  "<body><h1>304 Not Modified</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::not_modified); //test status
	EXPECT_EQ(rep.content, not_modified); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}



//test reply for bad request response

TEST_F(ReplyTestFix, bad_requestReply) {

	rep = http::server::reply::stock_reply(http::server::reply::bad_request);
	
	const char bad_request[] =
	  "<html>"
	  "<head><title>Bad Request</title></head>"
	  "<body><h1>400 Bad Request</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::bad_request); //test status
	EXPECT_EQ(rep.content, bad_request); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}


//test reply for unauth response

TEST_F(ReplyTestFix, unauthorizedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::unauthorized);
	
	const char unauthorized[] =
	  "<html>"
	  "<head><title>Unauthorized</title></head>"
	  "<body><h1>401 Unauthorized</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::unauthorized); //test status
	EXPECT_EQ(rep.content, unauthorized); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}



//test reply for forbidden response

TEST_F(ReplyTestFix, forbiddenReply) {

	rep = http::server::reply::stock_reply(http::server::reply::forbidden);
	
	const char forbidden[] =
	  "<html>"
	  "<head><title>Forbidden</title></head>"
	  "<body><h1>403 Forbidden</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::forbidden); //test status
	EXPECT_EQ(rep.content, forbidden); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}






//test reply for not found response

TEST_F(ReplyTestFix, not_foundReply) {

	rep = http::server::reply::stock_reply(http::server::reply::not_found);
	
	const char not_found[] =
	  "<html>"
	  "<head><title>Not Found</title></head>"
	  "<body><h1>404 Not Found</h1></body>"
	  "</html>";
	  
	EXPECT_EQ(rep.status, http::server::reply::not_found); //test status
	EXPECT_EQ(rep.content, not_found); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}



//test reply for internal server response

TEST_F(ReplyTestFix, internal_server_errorReply) {

	rep = http::server::reply::stock_reply(http::server::reply::internal_server_error);
	
	const char internal_server_error[] =
	  "<html>"
	  "<head><title>Internal Server Error</title></head>"
	  "<body><h1>500 Internal Server Error</h1></body>"
	  "</html>";	  
	EXPECT_EQ(rep.status, http::server::reply::internal_server_error); //test status
	EXPECT_EQ(rep.content, internal_server_error); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}


//test reply for not implemented response

TEST_F(ReplyTestFix, not_implementedReply) {

	rep = http::server::reply::stock_reply(http::server::reply::not_implemented);
	
	const char not_implemented[] =
	  "<html>"
	  "<head><title>Not Implemented</title></head>"
	  "<body><h1>501 Not Implemented</h1></body>"
	  "</html>";  

	EXPECT_EQ(rep.status, http::server::reply::not_implemented); //test status
	EXPECT_EQ(rep.content, not_implemented); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}


//test reply for bad gateway response

TEST_F(ReplyTestFix, bad_gatewayReply) {

	rep = http::server::reply::stock_reply(http::server::reply::bad_gateway);
	
	const char bad_gateway[] =
	  "<html>"
	  "<head><title>Bad Gateway</title></head>"
	  "<body><h1>502 Bad Gateway</h1></body>"
	  "</html>";

	EXPECT_EQ(rep.status, http::server::reply::bad_gateway); //test status
	EXPECT_EQ(rep.content, bad_gateway); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}



//test reply for service unavailable response

TEST_F(ReplyTestFix, service_unavailableReply) {

	rep = http::server::reply::stock_reply(http::server::reply::service_unavailable);
	
	const char service_unavailable[] =
	  "<html>"
	  "<head><title>Service Unavailable</title></head>"
	  "<body><h1>503 Service Unavailable</h1></body>"
	  "</html>";

	EXPECT_EQ(rep.status, http::server::reply::service_unavailable); //test status
	EXPECT_EQ(rep.content, service_unavailable); //test content
	EXPECT_EQ(rep.headers.size(), 2); //test size 
	EXPECT_EQ(rep.headers[0].name, "Content-Length"); //test header name
	EXPECT_EQ(rep.headers[0].value, std::to_string(rep.content.size())); //test length of content size
	EXPECT_EQ(rep.headers[1].name, "Content-Type"); //test content type
	EXPECT_EQ(rep.headers[1].value, "text/html"); //test text type

	buffers = rep.to_buffers();
}




