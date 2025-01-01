#include "dump_helper.h"
#include <cstdint>
#include <cstdio>

#ifdef WIN32
#include <DbgHelp.h>
#include <corecrt_io.h>
#include <direct.h>
#include <array>
#include <ctime>

#pragma comment(lib, "Dbghelp.lib")
#endif

namespace util::dmp_helper {
#ifdef WIN32
bool GenFilePath(char *buf, uint32_t path_len) noexcept {
	time_t t = 0;
	struct tm local {};
	t = time(nullptr);
	localtime_s(&local, &t);

	if (_access(".\\dmp", 00) == -1) {
		_mkdir(".\\dmp");
	}

	// generate file name string
	sprintf_s(buf, path_len, ".\\dump\\%d%02d%02d%02d%02d%02d.dmp", 1900 + local.tm_year, local.tm_mon + 1, local.tm_mday, local.tm_hour, local.tm_min, local.tm_sec);
	if (strlen(buf) > path_len) {
		return false;
	}
	return true;
}

int GenDump(LPEXCEPTION_POINTERS excep_ptr, const char *file_path) noexcept {
	HANDLE hFile = ::CreateFileA(file_path, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (INVALID_HANDLE_VALUE != hFile) {
		MINIDUMP_EXCEPTION_INFORMATION minidumpExceptionInformation;
		minidumpExceptionInformation.ThreadId = GetCurrentThreadId();
		minidumpExceptionInformation.ExceptionPointers = excep_ptr;
		minidumpExceptionInformation.ClientPointers = TRUE;
		bool isMiniDumpGenerated = MiniDumpWriteDump(GetCurrentProcess(), GetCurrentProcessId(), hFile, MINIDUMP_TYPE::MiniDumpNormal, &minidumpExceptionInformation, nullptr, nullptr);
		CloseHandle(hFile);
		if (!isMiniDumpGenerated) {
		}
	} else {
	}
	return EXCEPTION_EXECUTE_HANDLER;
}

void SnapshotMem() noexcept {
	std::array<char, MAX_PATH> file_path{};
	if (!GenFilePath(&(*file_path.begin()), MAX_PATH)) {
		return;
	};
	__try {
		constexpr auto dwExceptionCode = 0xE0000001;
		// 通过触发异常获取堆栈
		RaiseException(dwExceptionCode, 0, 0, nullptr);
	} __except (GenDump(GetExceptionInformation(), file_path.data())) {
	}
}

LONG WINAPI ExceptionFilter(LPEXCEPTION_POINTERS lpExceptionInfo) noexcept {
	std::array<char, MAX_PATH> file_path{};
	if (!GenFilePath(&(*file_path.begin()), MAX_PATH)) {
		return EXCEPTION_EXECUTE_HANDLER;
	};
	// generate dump file
	return GenDump(lpExceptionInfo, file_path.data());
}
#endif

#ifdef linux
void PrepareCore() noexcept {
	rlimit rlim{ 0 };
	if (prctl(PR_SET_DUMPABLE, 1) < 0) {
		return;
	}
	rlim.rlim_cur = RLIM_INFINITY;
	rlim.rlim_max = RLIM_INFINITY;
	setrlimit(RLIMIT_CORE, &rlim);
}
#endif

} // namespace util::dmp_helper
