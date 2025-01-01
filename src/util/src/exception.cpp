#include "exception.hpp"
#include "boost/stacktrace/stacktrace.hpp"
#include "boost/stacktrace/stacktrace_fwd.hpp"
#include "log.hpp"

#include <boost/stacktrace.hpp>

namespace util {
namespace exception {
void handle_trace_exception(const std::exception &e) {
	const boost::stacktrace::stacktrace *st = boost::get_error_info<traced>(e);
	if (st) {
		LOG_MSG(error) << "trace: \n"
					   << *st;
	} else {
		LOG_MSG(error) << "trace: \n"
					   << boost::stacktrace::stacktrace();
	}
}
} // namespace exception
} // namespace util