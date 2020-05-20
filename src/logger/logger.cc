#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include "logger/logger.h"
#include "http/request.h"
#include <boost/asio.hpp>
#include <boost/asio/signal_set.hpp>

namespace logging = boost::log;
namespace sinks = boost::log::sinks;
namespace src = boost::log::sources;
namespace expr = boost::log::expressions;
namespace attrs = boost::log::attributes;
namespace keywords = boost::log::keywords;

using boost::asio::ip::tcp;
using http::server::request;

std::mutex g_pages_mutex;


Logger::Logger () {
    init();
    logging::add_common_attributes();
    logging::add_console_log(std::cout, boost::log::keywords::format = ">> %Message%");
}

void Logger::init()
{
    logging::add_file_log
    (
        keywords::file_name = "../logs/server_logs_%N.log",
        keywords::rotation_size = 10 * 1024 * 1024,
        keywords::time_based_rotation = sinks::file::rotation_at_time_point(0, 0, 0),
        keywords::format = "[%TimeStamp%]: (%ThreadID%): %Message%",
        keywords::auto_flush = true
    );
}

void Logger::log(std::string message, severity_level severity) 
{
    std::lock_guard<std::mutex> guard(g_pages_mutex); // for critical section
    switch(severity) {
        case NORMAL:
            BOOST_LOG_SEV(slg, NORMAL) << "<Normal>: " << message;
            break;
        case NOTIFICATION:
            BOOST_LOG_SEV(slg, NOTIFICATION) << "<Notification>: " << message;
            break;
        case WARNING:
            BOOST_LOG_SEV(slg, WARNING) << "<Warning>: " << message;
            break;
        case ERROR:
            BOOST_LOG_SEV(slg, ERROR) << "<Error>: " << message;
            break;
        case CRITICAL:
            BOOST_LOG_SEV(slg, CRITICAL) << "<Critical>: " << message;
            break;
    }
}

void Logger::logRequest(request request_, tcp::socket& socket_, severity_level severity)
{
    std::lock_guard<std::mutex> guard(g_pages_mutex); //for critical section
    std::string msg = "Request received. Sender IP: " + socket_.remote_endpoint().address().to_string() 
        + " via HTTP: " + std::to_string(request_.http_version_major) + "." + std::to_string(request_.http_version_minor);
    switch(severity) {
        case NOTIFICATION:
            BOOST_LOG_SEV(slg, NOTIFICATION) << "<Notification>: " << msg;
            break;
        case ERROR:
            BOOST_LOG_SEV(slg, ERROR) << "<Error>: " << msg;
            break;
        default:
            // we should never log a request that is neither a notification nor an error, so this is a safeguard.
            BOOST_LOG_SEV(slg, NORMAL) << "<Normal>: " << "Odd request received..." << msg;
            break;
    }

}


