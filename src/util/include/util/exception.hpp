#pragma once

#include <boost/exception/all.hpp>
#include <boost/stacktrace.hpp>
namespace util {
namespace exception {
using stacktrace = boost::stacktrace::stacktrace;
using traced	 = boost::error_info<struct tag_stacktrace, stacktrace>;
template <class E>
void throw_with_trace(const E &e) {
	throw boost::enable_error_info(e) << traced(stacktrace());
}

} // namespace exception
} // namespace util