﻿#include "app.h"
#include <cstdlib>
#include <exception>

#include <boost/stacktrace.hpp>

#include "util/dump_helper.h"
#include "util/exception.hpp"
#include "util/log.hpp"
#include "util/stacktrace_dumper.h"
#include "util/util.hpp"

static void terminate_handle() {
	try {
		LOG_MSG(error) << "Catch unhandle exception, trace: \n"
					   << boost::stacktrace::stacktrace();
		std::exception_ptr exptr{ std::current_exception() };
		if (exptr) {
			std::rethrow_exception(exptr);
		} else {
			LOG_MSG(error) << "Exiting without exception ...";
		}
	} catch (const std::exception &e) {
		LOG_MSG(error) << "Exception: " << e.what();
	} catch (...) {
		LOG_MSG(error) << "Unkonown exception, exiting ...";
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
	const auto res = util::program_location();
	std::string log_path = "./log/default";
	if (res) {
		log_path = res.value().parent_path().string() + log_path;
	} else {
		std::cout << "fail to get program location" << res.error().message() << std::endl;
	}
	util::Logger::init_default_sink(util::log_level::trace, log_path, true, true);
#ifdef unix
	signal(SIGPIPE, SIG_IGN);
#endif
	util::StacktraceDumper dumper;
	dumper.show_last_dump();
	try {
		LOG_MSG(trace) << "Hello, Boost.log";
		prj_exec1::App{}.run();
	} catch (const std::exception &e) {
		LOG_MSG(error) << "Catch exception: " << e.what();
		const boost::stacktrace::stacktrace *st = boost::get_error_info<util::exception::traced>(e);
		if (st) {
			LOG_MSG(error) << "trace: \n"
						   << *st;
		} else {
			LOG_MSG(error) << "stacktrace:\n"
						   << boost::stacktrace::stacktrace::from_current_exception();
		}
	} catch (...) {
		LOG_MSG(error) << "Catch unknown exception ...";
		LOG_MSG(error) << "stacktrace:\n"
					   << boost::stacktrace::stacktrace::from_current_exception();
	}
	return 0;
}
