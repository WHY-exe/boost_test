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
namespace outcome = boost::outcome_v2;

using TString = std::basic_string<TCHAR>;
} //namespace util