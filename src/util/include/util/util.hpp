#pragma once
#include "type_def.hpp"
#include <spdlog/common.h>

namespace util {
void			  init_log(spdlog::level::level_enum level = spdlog::level::level_enum::info, bool enable_console = true, bool enable_file = true,
					 const std::filesystem::path &file_path = "logs/default.log");
outcome<fs::path> program_location() noexcept;
} // namespace util
