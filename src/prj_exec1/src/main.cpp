#include "app.h"
#include <cstdlib>
#include <exception>

#include <boost/stacktrace.hpp>

#include "util/dump_helper.h"
#include "util/exception.hpp"
#include "util/error.hpp"
#include "util/stacktrace_dumper.h"
#include "util/util.hpp"

#include <openssl/ssl.h>
#include <spdlog/spdlog.h>

static void terminate_handle() {
	try {
		std::stringstream ss;
		ss << boost::stacktrace::stacktrace();
		SPDLOG_ERROR("Catch unhandle exception, trace: \n {}", ss.str());
		std::exception_ptr exptr{ std::current_exception() };
		if (exptr) {
			std::rethrow_exception(exptr);
		} else {
			SPDLOG_ERROR("Exiting without exception ...");
		}
	} catch (const std::exception &e) {
		SPDLOG_ERROR("Exception: {}", e.what());
	} catch (...) {
		SPDLOG_ERROR("Unkonown exception, exiting ...");
	}
	std::exit(EXIT_FAILURE);
}

int main() {
	std::set_terminate(terminate_handle);
#if defined(WIN32)
	SetUnhandledExceptionFilter(util::dmp_helper::ExceptionFilter);
#elif defined(linux)
	// for linux generating might still need to do
	// bash -c "echo core > /proc/sys/kernel/core_pattern"
	util::dmp_helper::PrepareCore();
#endif
#ifdef unix
	signal(SIGPIPE, SIG_IGN);
#endif
	try {
		util::init_log(spdlog::level::info, false);
		util::StacktraceDumper dumper;
		dumper.show_last_dump();
		SPDLOG_INFO("Hello, spdlog");
		prj_exec1::App{}.run();
	} catch (const std::exception &e) {
		const boost::stacktrace::stacktrace *st = boost::get_error_info<util::exception::traced>(e);
		std::stringstream ss;
		ss << (st ? *st : boost::stacktrace::stacktrace::from_current_exception());
		SPDLOG_ERROR("Catch exception: {}, trace: \n {}", e.what(), ss.str());
	} catch (...) {
		std::stringstream ss;
		ss << boost::stacktrace::stacktrace::from_current_exception();
		SPDLOG_ERROR("Catch unknown exception ... trace: {}", ss.str());
	}
	return 0;
}
