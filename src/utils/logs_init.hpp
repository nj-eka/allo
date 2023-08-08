#pragma once

#include <string>
#include <sstream>
#include <iomanip>

#include <boost/program_options.hpp>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>

#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/support/date_time.hpp>

namespace Logger{

inline void init(const boost::program_options::variables_map &vm){
  namespace logging = boost::log;
  // namespace src = logging::sources;
  // namespace sinks = logging::sinks;
  namespace keywords = logging::keywords;
  namespace expres = logging::expressions;

  logging::add_common_attributes();
  // static const std::string LOG_MSG_FMT("[%TimeStamp%][%Severity%]:  %Message%");
  logging::register_simple_filter_factory<logging::trivial::severity_level, char>("Severity");
  logging::register_simple_formatter_factory<logging::trivial::severity_level, char>("Severity");
  // logging::core::get()->add_global_attribute("File", logging::attributes::mutable_constant<std::string>(""));
  // logging::core::get()->add_global_attribute("Func", logging::attributes::mutable_constant<std::string>(""));
  // logging::core::get()->add_global_attribute("Line", logging::attributes::mutable_constant<int>(5));  
  auto syslog_format(
    expres::stream <<
      expres::format_date_time< boost::posix_time::ptime >("TimeStamp", "%Y-%m-%d_%H:%M:%S.%f") <<
      " " << expres::attr<logging::attributes::current_thread_id::value_type>("ThreadID") << 
      " " << std::left << std::setw(7) << std::setfill(' ') << logging::trivial::severity <<
      " ~ " << expres::smessage <<
      " "  << expres::attr<std::string>("File") <<
      ":"   << expres::attr<std::string>("Func") <<
      ":"   << expres::attr<int>("Line") <<
      ""
  );

  // parse config file https://stackoverflow.com/questions/30293400/how-do-i-set-the-boost-logging-severity-level-from-config
  if (vm.count("logs")) {
    logging::add_file_log
    (
        keywords::file_name = vm["logs"].as<std::string>(), // "sample_%N.log"        /*< file name pattern >*/
        keywords::rotation_size = 1 * 1024 * 1024,                                    /*< rotate files every 1 MiB... >*/
        keywords::format = syslog_format // "[%TimeStamp%]: %Message%"                  /*< log record format >*/
        //keywords::auto_flush = true
    );
  } else {
    logging::add_console_log(
        std::clog,
        keywords::format = syslog_format,
        keywords::auto_flush = true
    );
  }

  logging::trivial::severity_level logSeverity; // = vm["logging-level"].as<logging::trivial::severity_level>();
  std::istringstream{vm["logging-level"].as<std::string>()} >> logSeverity;
  logging::core::get()->set_filter
  (
      logging::trivial::severity >= logSeverity
  );
                     
}

}

#define LOGS_ON