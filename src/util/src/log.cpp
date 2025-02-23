#include "log.hpp"

#include "boost/log/attributes/mutable_constant.hpp"
#include "boost/log/core/core.hpp"
#include "boost/log/core/record.hpp"
#include "boost/log/expressions/attr.hpp"
#include "boost/log/expressions/formatters/stream.hpp"
#include "boost/log/sinks/text_file_backend.hpp"

#include "error.hpp"
#include "util.hpp"

namespace util {
namespace expr	   = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace sinks	   = boost::log::sinks;
Logger::Logger(const std::string &file_path) {
	// New attributes that hold filename and line number for trvial logger.
	_slg.add_attribute("File", log_attrs::mutable_constant<std::string>(""));
	_slg.add_attribute("Line", log_attrs::mutable_constant<int>(0));
	_slg.add_attribute("Function", log_attrs::mutable_constant<std::string>(""));
	_slg.add_attribute("SinkName", log_attrs::constant<std::string>(file_path));
}

outcome<void> Logger::init_default(log_level svl, bool file_enabled, bool console_enabled) noexcept {
	try {
		std::optional<fs::path> log_path = std::nullopt;
		if (file_enabled) {
#if defined(linux) || defined(WIN32)
			const auto res = util::program_location();
			if (!res) {
				return res.error();
			}
			log_path = res.value().parent_path();
#endif
		}
		return init_default_sink(svl, log_path, console_enabled);
	} catch (...) {
		return std::current_exception();
	}
}
outcome<void> Logger::init_default_sink(log_level svl, const std::optional<fs::path> &log_path,
		bool isConsoleEnabled) {
	try {
		// New attributes that hold filename and line number for trvial logger.
		logging::trivial::logger::get().add_attribute("File",
				log_attrs::mutable_constant<std::string>(""));
		logging::trivial::logger::get().add_attribute("Line", log_attrs::mutable_constant<int>(0));
		logging::trivial::logger::get().add_attribute("Function",
				log_attrs::mutable_constant<std::string>(""));

		/* Add File Log*/
		if (log_path) {
			// std::error_code err;
			std::string file_path = log_path.value().string() + "/log/default";
			logging::trivial::logger::get().add_attribute("SinkName",
					log_attrs::constant<std::string>(file_path));
			outcome<void> res = add_file_sink(file_path, svl);
			if (!res) {
				return res;
			}
		}

		/* Add Console Log*/
		if (isConsoleEnabled) {
			logging::add_console_log(
					std::cout,
					keywords::format =
							(expr::stream << expr::format_date_time<boost::posix_time::ptime>(
													 "TimeStamp", "%Y-%m-%d_%H:%M:%S.%f")
										  << ": <" << boost::log::trivial::severity << "> " << '['
										  << expr::attr<std::string>("File") << ':' << expr::attr<int>("Line")
										  << "]"
										  << "[" << expr::attr<std::string>("Function") << "] "
										  << expr::smessage));
		}
		// register to common attributes:
		logging::add_common_attributes();
		// set severity:
		logging::core::get()->set_filter(logging::trivial::severity >= svl);
	} catch (...) {
		return std::current_exception();
	}
	return boost::outcome_v2::success();
}

outcome<void> Logger::add_file_sink(const std::string &file_path, log_level svl) {
	try {
		boost::shared_ptr<sinks::text_file_backend> backend =
				boost::make_shared<sinks::text_file_backend>(
						keywords::file_name				= file_path + "_%N.log",
						keywords::rotation_size			= 5 * 1024 * 1024,
						keywords::open_mode				= std::ios_base::out | std::ios_base::app,
						keywords::enable_final_rotation = false);
		backend->auto_flush();
		// wrap backend with the sink front end
		using SinkT = sinks::synchronous_sink<sinks::text_file_backend>;
		boost::shared_ptr<SinkT> sink_front(new SinkT(backend));
		sink_front->set_formatter(
				expr::stream << expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
										"%Y-%m-%d_%H:%M:%S.%f")
							 << ": <" << boost::log::trivial::severity << "> " << '['
							 << expr::attr<std::string>("File") << ':' << expr::attr<int>("Line") << "]"
							 << "[" << expr::attr<std::string>("Function") << "] " << expr::smessage);
		sink_front->set_filter(expr::attr<std::string>("SinkName") == file_path && logging::trivial::severity >= svl);
		logging::core::get()->add_sink(sink_front);
	} catch (...) {
		return std::current_exception();
	}
	return boost::outcome_v2::success();
}

} // namespace util