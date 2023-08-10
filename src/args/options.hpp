#include <fstream>
#include <iostream>
#include <string>
#include <tuple>

#include <boost/program_options.hpp>
#include <boost/log/trivial.hpp>

#include <info/version.h>

namespace args {

namespace opt = boost::program_options;

constexpr const size_t COUNTS = 1000;

inline auto parse(int argc, char const *argv[]) {
  opt::options_description desc("Usage");
  // clang-format off
  desc.add_options()
    ("help,h",      "Print this help message")
    ("version,v",   "Print version")
    ("info",        "Print project info")
#ifdef ENABLE_LOGGING   
    ("logging-level,l",   opt::value<std::string>()->default_value(to_string(boost::log::trivial::info)), "Logging level") // opt::value<boost::log::trivial::severity_level>...  ошибка: недостаточно контекстной информации для определения типа
    ("logs",              opt::value<std::string>(), "Logging output file name [default:clog]")
#endif
  ;
  // clang-format on
  // opt::positional_options_description pos_desc;
  // pos_desc.add("input", -1);
  opt::variables_map vm;
  // opt::store(opt::command_line_parser(argc, argv).options(desc).positional(pos_desc).run(), vm);
  opt::store(opt::command_line_parser(argc, argv).options(desc).run(), vm);
  opt::notify(vm);
  return std::make_tuple(std::move(desc), std::move(vm));
}

inline bool info_requested(const opt::variables_map &vm, const opt::options_description &desc) {
  if (vm.count("help")) {
    std::cout << desc << std::endl;
    return true;
  }
  if (vm.count("version")) {
    std::cout << info::info_project_version() << std::endl;
    return true;
  }
  if (vm.count("info")) {
    std::cout << info::info_project_full() << std::endl;
    return true;
  }
  return false;
}

} // namespace args
