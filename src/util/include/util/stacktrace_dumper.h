#pragma once
#include <boost/core/noncopyable.hpp>
#include <filesystem>

namespace util {
class StacktraceDumper : public boost::noncopyable {
private:
	static void _signal_handler(int signum);
	static std::filesystem::path _get_dump_path();

public:
	StacktraceDumper();
	void show_last_dump();
};
} //namespace util