#ifndef LOGGER_H
#define LOGGER_H  

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "http/request.h"
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>
#include <mutex>          // std::mutex



namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

using boost::asio::ip::tcp;
using http::server::request;

// We define our own severity levels
enum severity_level
{
    NORMAL,         // Logs typical server activity
    NOTIFICATION,   // Logs events that happen from client interaction with the server
    WARNING,        // Logs potentially bad activity from client or server that will lead to issues
    ERROR,          // Logs bad activity from client that our server can't handle
    CRITICAL        // Logs bad activity from client or server that causes the server to fail
};

class Logger {
    public:

        // standard logging function
        void log(std::string message, severity_level severity);

        // retrieve our Logger singleton
        static Logger& getInstance(){
            static Logger logger;
            return logger;
        }

        // logs details of a client's request including IP address and HTTP version
        void logRequest(request request_, tcp::socket& socket_, severity_level severity);


    protected:
        Logger();

        src::severity_logger<severity_level> slg;

        void init();
};

#endif