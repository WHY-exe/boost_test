#include "util.hpp"
#include "error.hpp"
#ifdef WIN32
#	include <Windows.h>
#endif

namespace util {
outcome::result<fs::path> program_location() noexcept {
	try {
#if defined(WIN32)
		TString ret;
		ret.resize(MAX_PATH, '\0');
		DWORD file_path_len = GetModuleFileName(nullptr, &ret[0], MAX_PATH);
		if (file_path_len == 0) {
			return std::error_code(GetLastError(), std::system_category());
		}
		ret.resize(file_path_len);
		return ret;
#elif defined(linux)
		constexpr size_t MAXBUFSIZE = 1024;
		std::string		 ret;
		ret.resize(MAXBUFSIZE, '\0');
		ssize_t file_path_len = readlink("/proc/self/exe", &ret[0], MAXBUFSIZE);
		if (file_path_len == -1) {
			return std::error_code(errno, std::generic_category());
		}
		ret.resize(file_path_len);
		return ret;
#else
		return make_error_code(Error::NO_IMPLEMENTED);
#endif
	} catch (const std::exception &e) {
		return make_error_code(e);
	} catch (...) {
		return make_error_code(Error::UNKNOWN_EXCEPTION);
	}
}

} // namespace util