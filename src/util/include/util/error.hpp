#pragma once
#include <system_error> // bring in std::error_code et al
#include <spdlog/fmt/fmt.h>
#include "type_def.hpp"
namespace util {
enum class Error {
	SUCCESSED,
	NO_IMPLEMENTED,
	STD_EXCEPTION,
	UNKNOWN_EXCEPTION
};

// Define a custom error code category derived from std::error_category
class ErrorCategory : public std::error_category {
public:
	ErrorCategory() noexcept = default;
	// Return a short descriptive name for the category
	const char *name() const noexcept final;
	// Return what each enum means in text
	std::string message(int c) const final;
	// OPTIONAL: Allow generic error conditions to be compared to me
	std::error_condition default_error_condition(int c) const noexcept final;

	static ErrorCategory &get();
};

template <typename T>
std::string retrive_error_msg(const outcome<T> &outcome) {
	try {
		if (outcome.has_value()) {
			return "no error";
		} else if (outcome.has_error()) {
			return outcome.error().message();
		} else {
			boost::rethrow_exception(outcome.exception());
		}
	} catch (const std::exception &e) {
		return fmt::format("{}", e.what());
	} catch (...) {
		return "unknown exception";
	}
}

std::error_code make_error_code(Error e);
} //namespace util


template <typename T> class fmt::formatter<util::outcome<T>> {
public:
    constexpr auto parse(auto& context) {
        auto       iter{ context.begin() };
        const auto end{ context.end() };
        if (iter != end && *iter != '}') {
            throw fmt::format_error{ "Invalid gl::Error format specifier." };
        }
        return iter;
    };

    auto format(const util::outcome<T>& error, auto& context) const {
        return fmt::format_to(context.out(), "{}", util::retrive_error_msg(error));
    };
};


template <typename T>
std::ostream& operator<<(std::ostream& ostream, const util::outcome<T>& outcome) {
	ostream << util::retrive_error_msg(outcome);
	return ostream;
}