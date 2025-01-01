#include "log.hpp"

#include "boost/log/attributes/mutable_constant.hpp"
#include "boost/log/core/core.hpp"
#include "boost/log/core/record.hpp"
#include "boost/log/expressions/attr.hpp"
#include "boost/log/expressions/formatters/stream.hpp"
#include "boost/log/sinks/text_file_backend.hpp"
#include <string>

namespace util {
namespace expr = boost::log::expressions;
namespace keywords = boost::log::keywords;
namespace sinks = boost::log::sinks;
Logger::Logger(const std::string &file_path) {
	// New attributes that hold filename and line number for trvial logger.
	_slg.add_attribute("File", log_attrs::mutable_constant<std::string>(""));
	_slg.add_attribute("Line", log_attrs::mutable_constant<int>(0));
	_slg.add_attribute("Function", log_attrs::mutable_constant<std::string>(""));
	_slg.add_attribute("SinkName", log_attrs::constant<std::string>(file_path));
}

void Logger::init_default_sink(log_level svl, const std::string &file, bool isConsoleEnabled, bool isFileEnabled) {
	// New attributes that hold filename and line number for trvial logger.
	logging::trivial::logger::get().add_attribute("File",
			log_attrs::mutable_constant<std::string>(""));
	logging::trivial::logger::get().add_attribute("Line", log_attrs::mutable_constant<int>(0));
	logging::trivial::logger::get().add_attribute("Function",
			log_attrs::mutable_constant<std::string>(""));

	/* Add File Log*/
	if (isFileEnabled) {
		logging::trivial::logger::get().add_attribute("SinkName",
				log_attrs::constant<std::string>(file));
		add_file_sink(file, svl);
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
									  << "]" << "[" << expr::attr<std::string>("Function") << "] "
									  << expr::smessage));
	}
	// register to common attributes:
	logging::add_common_attributes();
	// set severity:
	logging::core::get()->set_filter(logging::trivial::severity >= svl);
}

void Logger::add_file_sink(const std::string &file_path, log_level svl) {
	boost::shared_ptr<sinks::text_file_backend> backend =
			boost::make_shared<sinks::text_file_backend>(
					keywords::file_name = file_path + "_%N.log",
					keywords::rotation_size = 5 * 1024 * 1024,
					keywords::open_mode = std::ios_base::out | std::ios_base::app,
					keywords::enable_final_rotation = false);
	backend->auto_flush();
	// wrap backend with the sink front end
	typedef sinks::synchronous_sink<sinks::text_file_backend> SinkT;
	boost::shared_ptr<SinkT> sink_front(new SinkT(backend));
	sink_front->set_formatter(
			expr::stream << expr::format_date_time<boost::posix_time::ptime>("TimeStamp",
									"%Y-%m-%d_%H:%M:%S.%f")
						 << ": <" << boost::log::trivial::severity << "> " << '['
						 << expr::attr<std::string>("File") << ':' << expr::attr<int>("Line") << "]"
						 << "[" << expr::attr<std::string>("Function") << "] " << expr::smessage);
	sink_front->set_filter(expr::attr<std::string>("SinkName") == file_path && logging::trivial::severity >= svl);
	logging::core::get()->add_sink(sink_front);
}

} // namespace util