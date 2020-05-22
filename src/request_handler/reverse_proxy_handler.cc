#include "request_handler/reverse_proxy_handler.h"
#include "logger/logger.h"
#include <string>
#include <boost/asio.hpp>
#include <boost/algorithm/string/erase.hpp>
#include <boost/algorithm/string.hpp>

ReverseProxyHandler::ReverseProxyHandler(const std::string& location_path, const std::string& proxy_dest, int proxy_port)
    : location_path_(location_path), proxy_dest_(proxy_dest), proxy_port_(proxy_port) {}

// Create new reverse proxy request handler from config file statement
RequestHandler* ReverseProxyHandler::Init(const NginxConfig& config, const std::string location_path) {

    // Obtain the proxy port number and address of the proxy destination.
    int proxy_port = 0;
    std::string proxy_dest = "";

    Logger& logger = Logger::getInstance();
    for(int i = 0; i < config.statements_.size(); i++) {
        if (config.statements_[i]->tokens_.size() == 2 && config.statements_[i]->tokens_[0] == "proxy_port") {
            try {
                proxy_port = std::stoi(config.statements_[i]->tokens_[1]);
            } catch (std::exception& e) {
                logger.log(std::string("Exception: ") + e.what() + " (proxy port was not a number)", CRITICAL);
                return nullptr;
            }
        }
        else if (config.statements_[i]->tokens_.size() == 2 && config.statements_[i]->tokens_[0] == "proxy_dest") {
            proxy_dest = config.statements_[i]->tokens_[1];
        }
    }

    return new ReverseProxyHandler(location_path, proxy_dest, proxy_port);
}

std::shared_ptr<http::server::reply> ReverseProxyHandler::HandleRequest(const request& request_) {
    Logger& logger = Logger::getInstance();
    boost::asio::io_service io_service;
    // Resolve DNS name for proxy host name
    boost::asio::ip::tcp::resolver::query query(proxy_dest_, std::to_string(proxy_port_));
    boost::asio::ip::tcp::resolver resolver(io_service);
    boost::system::error_code resolver_error;

    boost::asio::ip::tcp::resolver::iterator it = resolver.resolve(query, resolver_error);
    if (resolver_error) {
        logger.log("Failed to make DNS request for: " + proxy_dest_ + ":" + std::to_string(proxy_port_)
            + " with error: " + resolver_error.message(), ERROR);
        return std::shared_ptr<reply>(reply::stock_reply(reply::internal_server_error));
    }
    boost::asio::ip::tcp::resolver::iterator it_end;
    boost::asio::ip::tcp::endpoint proxy_endpoint;
    for(; it != it_end; it++) {
        // Only accept IPv4 addresses, otherwise an error may occur when connecting 
        if (it->endpoint().address().is_v4())
            proxy_endpoint = it->endpoint();
    }
    logger.log("Successful DNS retrieval for proxy site: " + proxy_dest_ + " with ip address: " + proxy_endpoint.address().to_string(), NORMAL);

    // Connect to proxy website and pass down the request from this server's client
    // Use url with proxy destination but keep the non-location path.
    std::string uri = boost::erase_first_copy(request_.uri_, location_path_);
    if (uri == "") {
        // the uri was just the location path, so it should just point to /
        uri = "/";
    }

    // Write request string to send to proxy server
    std::string http_request = "";
    // Use the same type of request as the original request
    std::string request_type = "";
    if (request_.method_ == http::server::request::GET_REQ) {
        request_type = "GET";
    } else if (request_.method_ == http::server::request::HEAD_REQ) {
        request_type = "HEAD";
    } else if (request_.method_ == http::server::request::POST_REQ) {
        request_type = "POST";
    } else if (request_.method_ == http::server::request::PUT_REQ) {
        request_type = "PUT";
    } else if (request_.method_ == http::server::request::DELETE_REQ) {
        request_type = "DELETE";
    } else if (request_.method_ == http::server::request::CONNECT_REQ) {
        request_type = "CONNECT";
    } else if (request_.method_ == http::server::request::OPTIONS_REQ) {
        request_type = "OPTIONS";
    } else if (request_.method_ == http::server::request::TRACE_REQ) {
        request_type = "TRACE";
    } else if (request_.method_ == http::server::request::PATCH_REQ) {
        request_type = "PATCH";
    }
    http_request += request_type + " " + uri + " HTTP/1.1\r\n"; // Request line
    http_request += "Host: " + proxy_dest_ + "\r\nConnection: close\r\n";
    // Include headers
    for (auto header : request_.headers_) {
        if (header.first != "Host")  // This is overwritten with the proxy destination
            http_request += header.first + ": " + header.second + "\r\n";
    }
    http_request += "\r\n";

    tcp::socket socket(io_service);
    socket.connect(proxy_endpoint);
    boost::system::error_code proxy_write_error;
    logger.log("Sending request: " + http_request + " to: " + proxy_dest_ , NOTIFICATION);
    boost::asio::write(socket, boost::asio::buffer(http_request), proxy_write_error);
    if (proxy_write_error) {
        logger.log("Reverse proxy handler pointing to " + proxy_dest_ + ":" + std::to_string(proxy_port_)
            + " failed proxy request write with error: " + proxy_write_error.message(), ERROR);
        return std::shared_ptr<reply>(reply::stock_reply(reply::internal_server_error));
    }
    boost::asio::streambuf receive_buffer;
    boost::system::error_code proxy_read_error;
    logger.log("Reading response from: " + proxy_dest_ + "...", NOTIFICATION);
    boost::asio::read(socket, receive_buffer, boost::asio::transfer_all(), proxy_read_error);
    if (proxy_read_error && proxy_read_error != boost::asio::error::eof) {
        logger.log("Reverse proxy handler pointing to " + proxy_dest_ + ":" + std::to_string(proxy_port_)
            + " failed proxy response read with error: " + proxy_read_error.message(), ERROR);
        return std::shared_ptr<reply>(reply::stock_reply(reply::internal_server_error));
    }
    logger.log("Succesfully obtained resposne from: " + proxy_dest_ + "!", NOTIFICATION);

    // Parse the reply
    http::server::reply reply;
    const char* data = boost::asio::buffer_cast<const char*>(receive_buffer.data());
    const std::string response_str(data, receive_buffer.size());
    const std::string delim = "\r\n";
    size_t header_end_pos = response_str.find("\r\n\r\n");
    std::string request_line = response_str.substr(0, response_str.find(delim));
    // Obtain the reply code
    int num_counter = 0; // Count consecutive amount of numbers (all status codes have 3 consecutive digits)
    for (size_t request_pos = 0; request_pos < request_line.size(); request_pos++) {
        if (isdigit(request_line[request_pos])) {
            num_counter++;
        } else {
            num_counter = 0;
        }
        if (num_counter == 3) {
            reply.code_ = static_cast<http::server::reply::status_code>(std::stoi(request_line.substr(request_pos - 2, 3)));
        }
    }
    // From the start to the end of headers (before the carriage return separating headers to body and after carriage return for request line)
    std::string headers = response_str.substr(request_line.size() + delim.size(), header_end_pos - request_line.size());

    // From the beginning of the body (after the carriage return separating headers and body) to the end of the response
    reply.body_ = response_str.substr(header_end_pos + 2 * delim.size());

    // Add the headers by parsing and consuming them one per line
    while (!headers.empty()) {
        size_t current_pos = 0;
        std::string tmp_buf = "", header_key = "";
        bool ignore_whitespace = true;
        bool is_header_key = true;
        // Obtain the header key and value pair
        while(headers[current_pos] != '\r') {
            if (ignore_whitespace) {
                if (headers[current_pos] != ' ') {
                    ignore_whitespace = false;
                    tmp_buf += headers[current_pos];
                }
            } else if (is_header_key && headers[current_pos] == ':') {
                // Done reading header key
                header_key = tmp_buf;
                ignore_whitespace = true;
                is_header_key = false;
                tmp_buf = "";
            } else {
                tmp_buf += headers[current_pos];
            }
            current_pos++;
        }
        reply.headers_[boost::trim_copy(header_key)] = boost::trim_copy(tmp_buf);
        headers = headers.substr(current_pos + delim.size());
    }
    // TODO(justinj151): Handle relative paths inside the html body and 302 redirect

    return std::make_shared<http::server::reply>(reply);
}