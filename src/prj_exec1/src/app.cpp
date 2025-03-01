#include "app.h"
#include <spdlog/spdlog.h>

namespace prj_exec1 {

void App::run() {
	SPDLOG_INFO("Hello, sdplog.INFO");
	// throw std::logic_error("test exception");
	// abort();
}

} // namespace prj_exec1