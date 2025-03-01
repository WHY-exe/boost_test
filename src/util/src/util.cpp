#include "util.hpp"
#include <spdlog/spdlog.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#ifdef WIN32
#	include <Windows.h>
#endif

namespace util {
void init_log(spdlog::level::level_enum level, bool enable_console, bool enable_file, const std::filesystem::path &file_path) {
	constexpr char const		 *pattern = "[%Y-%m-%d %H:%M:%S.%e][%!:%#][tid %t][%l] %v";
	std::vector<spdlog::sink_ptr> sink_list;
	if (enable_console) {
		auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
		console_sink->set_pattern(pattern);
		console_sink->set_level(level);
		sink_list.push_back(std::move(console_sink));
	}
	if (enable_file) {
		auto file_sink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(file_path.string());
		file_sink->set_pattern(pattern);
		file_sink->set_level(level);
		sink_list.push_back(std::move(file_sink));
	}
	auto default_logger =
			std::make_shared<spdlog::logger>("default", sink_list.begin(), sink_list.end());
	default_logger->set_level(level);
	spdlog::set_default_logger(std::move(default_logger));
	spdlog::flush_every(std::chrono::seconds(3));
}

outcome<fs::path> program_location() noexcept {
	try {
#if defined(WIN32)
		TString ret(MAX_PATH, '\0');
		DWORD	file_path_len = GetModuleFileName(nullptr, &ret[0], MAX_PATH);
		if (file_path_len == 0) {
			return std::error_code(static_cast<int>(GetLastError()), std::system_category());
		}
		ret.resize(file_path_len);
		return ret;
#elif defined(linux)
		constexpr size_t MAXBUFSIZE = 1024;
		std::string		 ret(MAXBUFSIZE, '\0');
		ssize_t			 file_path_len = readlink("/proc/self/exe", &ret[0], MAXBUFSIZE);
		if (file_path_len == -1) {
			return std::error_code(errno, std::generic_category());
		}
		ret.resize(file_path_len);
		return ret;
#else
		return make_error_code(Error::NO_IMPLEMENTED);
#endif
	} catch (...) {
		return std::current_exception();
	}
}
} // namespace util