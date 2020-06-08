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
    logger.log("Received Markdown handler request at path" + request_.uri_, NORMAL);

    std::shared_ptr<reply> rep;
    if(request_.uri_ == "/markdown") {
        rep = createForm(request_);
    } else if (request_.uri_.substr(0, 14) == "/markdown/view") {
        rep = viewMarkdown(request_);
    }
    return rep;
}

std::shared_ptr<reply> MarkdownHandler::createForm(const request& request_) {
    Logger& logger = Logger::getInstance(); 
    logger.log("Creating user input form", NORMAL);

    // FORM CREATION/SUBMISSION
    std::string input_form_path = "../data/input_form.html";

    // find input form's html page
    boost::filesystem::path boost_path(input_form_path);
    if (!boost::filesystem::exists(input_form_path) || !boost::filesystem::is_regular_file(input_form_path)) {
        std::shared_ptr<reply> rep = std::shared_ptr<reply>(reply::stock_reply(reply::internal_server_error));
        return rep;
    }

    // connect to input form's html page
    std::ifstream f(input_form_path.c_str(), std::ios::in | std::ios::binary);
    if (!f) {
        std::shared_ptr<reply> rep = std::shared_ptr<reply>(reply::stock_reply(reply::internal_server_error));
        return rep;
    }

    // read in the input form's html page
    std::string content_;
    char c;
    while (f.get(c)) content_ += c;
    f.close();

    logger.log("Input form has been read in, constructing HTTP reply", NORMAL);

        // create HTTP reply with the file contents as the reply body
    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());
    rep->code_ = reply::ok; // set to http 200
    rep->headers_["Content-Length"] = std::to_string(content_.length());
    rep->headers_["Content-Type"] = "text/html";
    rep->body_ = content_;

    logger.log("HTTP Reply has been constructed", NORMAL);
    return rep;
}

std::shared_ptr<reply> MarkdownHandler::viewMarkdown(const request& request_) {
    Logger& logger = Logger::getInstance(); 
    logger.log("Creating converted Markdown HTML page", NORMAL);

    // Remove the request type from the beginning of the request
    std::string trimmed_uri = request_.body_.substr(request_.body_.find_first_of(" ") + 1);

    // MARKDOWN --> HTML FUNCTION

    // Extract the user input markdown text only
    std::string input_ = trimmed_uri.substr(trimmed_uri.find("=") + 1, trimmed_uri.find_first_of(" ") - (trimmed_uri.find("=") + 1));

    // Decode the input markdown text
    char cleaned_input_char[input_.size()] = "";
    urldecode2(cleaned_input_char, input_.c_str());
    std::string cleaned_input = std::string(cleaned_input_char);

    boost::replace_all(cleaned_input, "\r\n", "\n");


    // DATABASE STORE STRING

    auto parser = std::make_shared<maddy::Parser>();
    std::stringstream markdown(cleaned_input);

    // set actual html from markdown parser to display on server
    const std::string output = parser->Parse(markdown);
    std::string html_markdown = output;

    std::shared_ptr<reply> rep = std::shared_ptr<reply>(new reply());

    rep->code_ = reply::ok;
    rep->body_ = html_markdown;
    rep->headers_["Content-Length"] = std::to_string(html_markdown.size());
    logger.log(std::to_string(html_markdown.size()), NORMAL);
    rep->headers_["Content-Type"] = "text/html";

    return rep;
}

void MarkdownHandler::urldecode2(char *dst, const char *src)
{
    char a, b;
    while (*src) {
        if ((*src == '%') &&
            ((a = src[1]) && (b = src[2])) &&
            (isxdigit(a) && isxdigit(b))) {
                if (a >= 'a')
                        a -= 'a'-'A';
                if (a >= 'A')
                        a -= ('A' - 10);
                else
                        a -= '0';
                if (b >= 'a')
                        b -= 'a'-'A';
                if (b >= 'A')
                        b -= ('A' - 10);
                else
                        b -= '0';
                *dst++ = 16*a+b;
                src+=3;
        } else if (*src == '+') {
                *dst++ = ' ';
                src++;
        } else {
                *dst++ = *src++;
        }
    }
    *dst++ = '\0';
}