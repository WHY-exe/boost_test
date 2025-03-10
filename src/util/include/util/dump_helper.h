#pragma once
#if defined(WIN32)
#	include <Windows.h>
#	include <cstdint>
#elif defined(linux)
#	include <sys/prctl.h>
#	include <sys/resource.h>
#endif

namespace util::dmp_helper {
#ifdef WIN32
void		SnapshotMem() noexcept;
bool		GetDmpFilePath(char *buf, uint32_t path_len) noexcept;
int			GenDump(LPEXCEPTION_POINTERS lpExceptionInfo, const char *file_path) noexcept;
LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo) noexcept;
#endif
#ifdef linux
void PrepareCore() noexcept;
#endif
} // namespace util::dmp_helper
