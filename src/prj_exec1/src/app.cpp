#include "app.h"

#include "util/log.hpp"

namespace prj_exec1 {

void App::run() {
	LOG_MSG(info) << "Hello, Boost.log INFO";
	// throw std::logic_error("test exception");
	// abort();
}

} // namespace prj_exec1