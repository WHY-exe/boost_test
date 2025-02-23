#pragma once
#include "type_def.hpp"


namespace util {
outcome<fs::path> program_location() noexcept;
} // namespace util
