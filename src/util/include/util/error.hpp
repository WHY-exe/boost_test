#pragma once
#include <system_error> // bring in std::error_code et al
namespace util {
enum class Error {
	Success,
	NO_IMPLEMENTED,
	STD_EXCEPTION,
	UNKNOWN_EXCEPTION
};

// Define a custom error code category derived from std::error_category
class ErrorCategory : public std::error_category {
public:
	ErrorCategory() noexcept = default;

	explicit ErrorCategory(const char *what);
	// Return a short descriptive name for the category
	const char *name() const noexcept final;
	// Return what each enum means in text
	std::string message(int c) const final;
	// OPTIONAL: Allow generic error conditions to be compared to me
	std::error_condition default_error_condition(int c) const noexcept final;

private:
	std::string _exception_what;
};

std::error_code make_error_code(Error e);
std::error_code make_error_code(const std::exception &e);
} //namespace util
