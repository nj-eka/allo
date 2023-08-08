// https://www.boost.org/doc/libs/1_82_0/doc/html/stacktrace/getting_started.html
#pragma once

#include <stdexcept>    // std::logic_error
#include <iostream>     // std::cerr
#include <boost/stacktrace.hpp>
#include <boost/exception/all.hpp>

using traced = boost::error_info<struct tag_stacktrace, boost::stacktrace::stacktrace>;

template <class E>
void throw_with_trace(const E& e) {
    throw boost::enable_error_info(e)
        << traced(boost::stacktrace::stacktrace());
}

// used if BOOST_ENABLE_ASSERT_DEBUG_HANDLER is defined for the whole project
namespace boost {
    inline void assertion_failed_msg(char const* expr, char const* msg, char const* function, char const* /*file*/, long /*line*/) {
        std::cerr << "Expression '" << expr << "' is false in function '" << function << "': " << (msg ? msg : "<...>") << ".\n"
            << "Backtrace:\n" << boost::stacktrace::stacktrace() << '\n';

        std::abort();
    }

    inline void assertion_failed(char const* expr, char const* function, char const* file, long line) {
        ::boost::assertion_failed_msg(expr, 0 /*nullptr*/, function, file, line);
    }
} // namespace boost


inline void my_terminate_handler() {
    try {
        std::cerr << boost::stacktrace::stacktrace();
    } catch (...) {}
    std::abort();
}


