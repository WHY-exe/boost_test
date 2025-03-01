#include <boost/asio.hpp>
#include <boost/stacktrace.hpp>

#include "util/asio_as_result.hpp"
#include "util/asio_timeout.hpp"
#include "util/exception.hpp"
#include "spdlog/spdlog.h"
#include "util/util.hpp"

using tcp = boost::asio::ip::tcp;

boost::asio::awaitable<void> async_connect(tcp::endpoint ep) {
	try {
		auto executor = co_await boost::asio::this_coro::executor;

		tcp::socket socket(executor);
		const auto	conn_time = std::chrono::system_clock::now();

		timeout_provider timeout{ executor, std::chrono::seconds(3) };
		auto conn_res = co_await socket.async_connect(ep, timeout(CO_RESULT_TOKEN));

		const auto now = std::chrono::system_clock::now();
		if (conn_res) {
			SPDLOG_INFO("connection to {}:{} takes {}ms", ep.address().to_string(), ep.port(),
					std::chrono::duration_cast<std::chrono::milliseconds>(now - conn_time).count());
			co_return;
		}

		SPDLOG_ERROR("error connected to {}:{}: {}", ep.address().to_string(), ep.port(),
				conn_res.error().message());
	} catch (const std::exception &e) {
		SPDLOG_INFO("catch exception: {}", e.what());
	} catch (...) {
		SPDLOG_INFO("Catch unknown exception ...");
	}
}

int main() {
	try {
		util::init_log(spdlog::level::trace);
		boost::asio::io_context		   io_ctx;
		boost::asio::ip::tcp::resolver resolver(io_ctx);
		const std::string_view		   sv[] = {
			"www.baidu.com",
			"182.61.200.6",
			"103.235.47.188",
			"182.61.200.7",
			"103.235.47.96",
			"31.13.73.9",
		};
		std::vector<tcp::endpoint> eps;
		for (const auto i : sv) {
			const auto eps_in = resolver.resolve(i, "http");
			std::copy(eps_in.begin(), eps_in.end(), std::back_inserter(eps));
		}

		for (const auto &ep : eps) {
			boost::asio::co_spawn(io_ctx, async_connect(ep), boost::asio::detached);
		}
		io_ctx.run();
	} catch (const std::exception &e) {
		const boost::stacktrace::stacktrace *st = boost::get_error_info<util::exception::traced>(e);
		std::stringstream ss;
		ss << (st ? *st : boost::stacktrace::stacktrace::from_current_exception());
		SPDLOG_ERROR("Catch exception: {}, trace: \n {}", e.what(), ss.str());
	} catch (...) {
		std::stringstream ss;
		ss << boost::stacktrace::stacktrace::from_current_exception();
		SPDLOG_ERROR("Catch unknown exception ... trace: {}", ss.str());
	}
	return 0;
}