#include "error.hpp"

namespace util {
ErrorCategory::ErrorCategory(const char *what) :
		_exception_what(what) {
}

const char *ErrorCategory::name() const noexcept {
	return "UtilError";
}

std::string ErrorCategory::message(int c) const {
	switch (static_cast<Error>(c)) {
		case Error::Success:
			return "conversion successful";
		case Error::NO_IMPLEMENTED:
			return "no implemented";
		case Error::STD_EXCEPTION:
			return "std exception:" + _exception_what;
		case Error::UNKNOWN_EXCEPTION:
			return "catching unknown exception";
		default:
			return "unknown";
	}
}

std::error_condition ErrorCategory::default_error_condition(int c) const noexcept {
	switch (static_cast<Error>(c)) {
		case Error::NO_IMPLEMENTED:
			return make_error_condition(std::errc::not_supported);
		default:
			// I have no mapping for this code
			return std::error_condition(c, *this);
	}
}

std::error_code make_error_code(Error e) {
	return { static_cast<int>(e), ErrorCategory() };
}

std::error_code make_error_code(const std::exception &e) {
	return { static_cast<int>(Error::STD_EXCEPTION), ErrorCategory(e.what()) };
}
} //namespace util
