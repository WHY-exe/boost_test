//! \file util_logger.h
//!
//! \author Dan Yerushalmi
//!
//! \date 20/10/2016
//!
//! \brief Based on
//! http://stackoverflow.com/questions/24750218/boost-log-to-print-source-code-file-name-and-line-number
//!		   I have modified this code to work as thread safe with
//! underlaying boost trvial logger.
//!        Can change filter and output at runtime.
//!
//! \copyright (MIT) please give me credited by keeping my name on header,
//! thanks.
#pragma once

#include <boost/log/attributes/attribute_set.hpp>
#include <boost/log/attributes/mutable_constant.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/support/date_time.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <optional>

#include "type_def.hpp"

namespace util {
namespace logging	= boost::log;
namespace log_attrs = boost::log::attributes;
namespace log_src	= boost::log::sources;
using log_level		= boost::log::trivial::severity_level;
class Logger {
private:
	logging::sources::severity_logger<logging::trivial::severity_level> _slg;

public:
	explicit Logger(const std::string &file_path);

	logging::attribute_set get_attributes() const {
		return _slg.get_attributes();
	}

	inline log_src::severity_logger<log_level> &get() {
		return _slg;
	};

	static outcome<void> init_default(log_level svl, bool file_enabled, bool console_enabled) noexcept;

	static outcome<void> init_default_sink(log_level svl, const std::optional<fs::path> &log_path = std::nullopt,
			bool isConsoleEnabled = false);

	static outcome<void> add_file_sink(const std::string &file_path, log_level svl);

	// Set attribute and return the new value
	template <typename ValueType, class Logger>
	static ValueType set_get_attrib(Logger logger, const char *name, ValueType value) {
		auto attr = logging::attribute_cast<log_attrs::mutable_constant<ValueType>>(
				logger.get_attributes()[name]);
		attr.set(value);
		return attr.get();
	};

	template <class Logger>
	static void set_attrib(const char *name, const char *value);

	// Convert file path to only the filename
	static inline std::string path_to_filename(const std::filesystem::path &path) {
		return path.filename().string();
	};
};

} // namespace util

// New macro that includes severity, filename and line number
#define LOG_ADD_FILE_LG(logger) \
	util::Logger::set_get_attrib(logger, "File", util::Logger::path_to_filename(__FILE__))

#define LOG_ADD_LINE_LG(logger) util::Logger::set_get_attrib(logger, "Line", __LINE__)

#define LOG_ADD_FUNCTION_LG(logger) \
	util::Logger::set_get_attrib(logger, "Function", std::string(__FUNCTION__))

#define LOG_SETUP__(logger)  \
	LOG_ADD_FILE_LG(logger); \
	LOG_ADD_LINE_LG(logger); \
	LOG_ADD_FUNCTION_LG(logger);

#define LOG_LG__(sev, logger)            \
	LOG_SETUP__(logger)                  \
	BOOST_LOG_STREAM_WITH_PARAMS(logger, \
			(::boost::log::keywords::severity = ::boost::log::trivial::sev))

#define LOG_MSG_LG_(sev, logger) LOG_LG__(sev, logger)

#define LOG_MSG(sev) LOG_MSG_LG_(sev, boost::log::trivial::logger::get())

#define LOG_MSG_LG(sev, logger) LOG_MSG_LG_(sev, (logger).get())