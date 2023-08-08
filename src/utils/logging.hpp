#pragma once

#ifndef LOGS_ON

#define TRACE(msg, ...);
#define DEBUG(msg, ...);
#define INFO(msg, ...);
#define WARNING(msg, ...);
#define ERROR(msg, ...);

#else

#include <stdarg.h>
// #include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
// #include <boost/log/expressions.hpp>
#include <boost/log/utility/manipulators/add_value.hpp>

#define TRACE(msg, ...)         Logger::write_log(boost::log::trivial::trace,   __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);
#define DEBUG(msg, ...)         Logger::write_log(boost::log::trivial::debug,   __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);
#define INFO(msg, ...)          Logger::write_log(boost::log::trivial::info,    __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);
#define WARNING(msg, ...)       Logger::write_log(boost::log::trivial::warning, __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);
#define ERROR(msg, ...)         Logger::write_log(boost::log::trivial::error,   __FILE__, __FUNCTION__, __LINE__, msg, ##__VA_ARGS__);

#define LOG_TRIVIAL(lvl)\
    BOOST_LOG_STREAM_WITH_PARAMS(::boost::log::trivial::logger::get(),\
        (::boost::log::keywords::severity = lvl))

namespace Logger{

inline void write_log(boost::log::trivial::severity_level level, const char * file, const char * func, int line, const char *message, ...)
{
    char buffer[1024];
    va_list args;

    // Parse ellipsis and add arguments to message
    va_start(args, message);
    vsnprintf(buffer, sizeof(buffer), message, args);
    va_end(args);

    LOG_TRIVIAL(level) << boost::log::add_value("File", file) << boost::log::add_value("Func", func) << boost::log::add_value("Line", line) << buffer;
    // LOG_TRIVIAL(level) << "File: " << file << " Func: " << func << " Ln#: " << line << buffer;    
}

}

#endif