#include "stacktrace_dumper.h"

#include "dump_helper.h"
#include <spdlog/spdlog.h>
#include "util.hpp"
#include <boost/stacktrace.hpp>
#include <csignal>
#include <filesystem>
#include <fstream>

namespace util {
void StacktraceDumper::_signal_handler(int signum) {
	::signal(signum, SIG_DFL);
#if defined(WIN32)
	util::dmp_helper::SnapshotMem();
#elif defined(linux)
	const std::filesystem::path file_path = StacktraceDumper::_get_dump_path();
	// create parent path recursivly
	if (!std::filesystem::exists(file_path.parent_path())) {
		std::filesystem::create_directories(file_path.parent_path());
	}
	boost::stacktrace::safe_dump_to(file_path.string().c_str());
#endif
	::raise(SIGABRT);
}

std::filesystem::path StacktraceDumper::_get_dump_path() {
	const auto current_program_path = util::program_location();
	if (current_program_path) {
		std::stringstream ss;
		ss << current_program_path.value().parent_path().string() << "/dump/"
		   << current_program_path.value().filename().string() << ".dump";
		return ss.str();
	}
	return "./dump/default.dump";
}

StacktraceDumper::StacktraceDumper() {
	::signal(SIGSEGV, &StacktraceDumper::_signal_handler);
	::signal(SIGABRT, &StacktraceDumper::_signal_handler);
}

void StacktraceDumper::show_last_dump() {
	const std::string filename = StacktraceDumper::_get_dump_path().string();
	if (std::filesystem::exists(filename)) {
		std::ifstream ifs(filename);
		std::stringstream ss;
		ss << "last dump detect:\n" << boost::stacktrace::stacktrace::from_dump(ifs);
		SPDLOG_ERROR(ss.str());
		ifs.close();
		std::filesystem::remove(filename);
	}
}

}; //namespace util