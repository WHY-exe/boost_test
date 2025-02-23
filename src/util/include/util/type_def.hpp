// include this header file at the end of include list
#pragma once
#ifndef WIN32
#	ifdef UNICODE
#		define TCHAR wchar_t
#	else
#		define TCHAR char
#	endif
#else
#	include <tchar.h>
#endif
#include <boost/outcome.hpp>
#include <filesystem>
#include <string>

namespace util {
namespace fs	  = std::filesystem;

template <typename T>
using outcome = boost::outcome_v2::outcome<T>;

template <typename T>
using result = boost::outcome_v2::result<T>;

using TString = std::basic_string<TCHAR>;
} //namespace util