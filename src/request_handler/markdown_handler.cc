#include "request_handler/markdown_handler.h"
#include "logger/logger.h"

// dependencies for maddy markdown
#include <memory>
#include <string>
#include "maddy/parser.h"

// purpose of markdown handler is to draw traffic to our website and educate users
RequestHandler* MarkdownHandler::Init(const NginxConfig& config, const std::string location_path) {
    RequestHandler* handler = new MarkdownHandler();
    handler->name = "MarkdownHandler";
    return handler;
}

// creates HTTP reply for 404 requests
std::shared_ptr<reply> MarkdownHandler::HandleRequest(const request& request_) {

    Logger& logger = Logger::getInstance(); 
    logger.log("Constructing a Markdown Handler HTTP reply", NORMAL);

    // FORM CREATION/SUBMISSION


    // DATABASE GET/STORE STRING


    // MARKDOWN --> HTML FUNCTION

    std::string input_ = "PLEASE EDIT ONCE DONE"; // THIS IS WHERE WE GET THE ACTUAL STRING FROM THE MARKDOWN FILE JEFF PLS EDIT


    auto parser = std::make_shared<maddy::Parser>();
    std::stringstream markdown(input_);


    // set actual html from markdown parser to display on server
    const std::string output = parser->Parse(markdown);
    std::string html_markdown = output;


    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());

    rep->code_ = reply::ok;
    rep->body_ = html_markdown;
    rep->headers_["Content-Length"] = std::to_string(html_markdown.size());
    rep->headers_["Content-Type"] = "text/html";
    return rep;
}
