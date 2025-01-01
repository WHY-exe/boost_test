#pragma once

#ifndef WIN32
#ifdef UNICODE
#define TCHAR wchar_t
#else
#define TCHAR char
#endif
#endif

#include <boost/outcome.hpp>
#include <filesystem>
#include <string>

namespace util {

namespace fs = std::filesystem;
using TString = std::basic_string<TCHAR>;

boost::outcome_v2::result<fs::path> program_location() noexcept;

} // namespace util