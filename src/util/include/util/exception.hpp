#pragma once

#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>
#include <exception>
namespace util {
namespace exception {
using stacktrace = boost::stacktrace::stacktrace;
using traced	 = boost::error_info<struct tag_stacktrace, stacktrace>;
template <class E>
void throw_with_trace(const E &e) {
	throw boost::enable_error_info(e) << traced(stacktrace());
}

void handle_trace_exception(const std::exception &e);

} // namespace exception
} // namespace util