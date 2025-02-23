#define BOOST_TEST_MODULE test1
#include <boost/test/included/unit_test.hpp>
#include "util/error.hpp"


util::outcome<int> test_exception() {
    try {
        throw std::runtime_error("test runtime error");
    } catch (...) {
        return std::current_exception();
    }
}

util::outcome<int> test_error_code() {
    try {
        return util::make_error_code(util::Error::NO_IMPLEMENTED);
    } catch (...) {
        return std::current_exception();
    }
}

BOOST_AUTO_TEST_CASE(first_test) {
	int i = 1;
	BOOST_TEST(i);
	// BOOST_TEST(i == 2);
}

BOOST_AUTO_TEST_CASE(test_error)
{
    auto excep_error = test_exception();
    if (!excep_error) {
        BOOST_TEST(fmt::format("{}", excep_error) == "test runtime error");
    }
    auto code_error = test_error_code();
    if (!code_error) {
        BOOST_TEST(fmt::format("{}", code_error) ==  "no implemented");
    }
}