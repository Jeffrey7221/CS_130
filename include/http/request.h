#ifndef HTTP_REQUEST_H
#define HTTP_REQUEST_H

#include <string>
#include <map>

namespace http {
namespace server {

// A request received from a client.
struct request {

  // The HTML method (GET, PUT, POST, etc)
  enum method_type {
    GET_REQ = 0,
    HEAD_REQ = 1,
    POST_REQ = 2,
    PUT_REQ = 3,
    DELETE_REQ = 4,
    CONNECT_REQ = 5,
    OPTIONS_REQ = 6,
    TRACE_REQ = 7,
    PATCH_REQ = 8
  } method_;

  // The path of the request
  std::string uri_;

  // A map of header, for convenient lookup ("Content-Type", "Cookie", etc)
  std::map<std::string, std::string> headers_;
  
  // The content of the request
  std::string body_;

  int http_version_major;
  int http_version_minor;
};

} // namespace server
} // namespace http

#endif // HTTP_REQUEST_HPP