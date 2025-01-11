#pragma once
#include "type_def.hpp"

namespace util {
outcome::result<fs::path> program_location() noexcept;

} // namespace util